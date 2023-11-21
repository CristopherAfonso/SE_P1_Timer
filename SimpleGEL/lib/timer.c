/** 
 * Universidad de La Laguna (ULL)
 * Escuela Superior de Ingenierıa y Tecnologıa (ESIT)
 * Grado en Ingenierıa Informatica
 * Asignatura: Sistemas Empotrados
 * Curso: 4º
 * Itinerario 2: Ingenierıa de Computadores
 * Practica 1: Librerías
 * @author Cristopher Manuel Afonso Mora (alu0101402031@ull.edu.es)
 * @author Francesco La Spina (alu0101435022@ull.edu.es)
 * @date 16/11/2023
 * 
 * @file timer.c
 * @brief Donde se aloja el código de las funciones de la librería
 * timer, que se encarga de gestionar el temporizador de forma más
 * asequible en el microcontrolador 68HC12
 *
 * @bug No hay bugs conocidos
 * 
 * @version 0.1
 * @copyright Copyright (c) 2023
 * @brief Historial de Revisiones 
 * 16/11/23 - Creacion del archivo:
 *            Se han creado todos los cuerpos de cada función que
 *            usará esta librería, cada una documentada, y se han
 *            empezado timer_init, timer_milis y timer_micros
 * 17/11/23 - A la función timer_init se le ha añadido un parametro
 *            para especificar el factor de escala del timer, además
 *            se ha empezado a implementar timer_sleep_milis, 
 *            timer_sleep_micros, timer_add_task, timer_remove_task y
 *            timer_periodic_task
 * 21/11/23 - Archivo terminado, funciones terminadas, bugs arreglados
 *            y documentación finalizada
 */

#include <sys/param.h>
#include <sys/sio.h>
#include <types.h>
#include <timer.h>

//#define DEBUG

/// Ajustamos el factor de escala del temporizador
#define TCM_FREQ(tcm_factor) (M6812_CPU_E_CLOCK / (1 << tcm_factor))

/// Pasamos de ticks a microsegundos según el factor de escala del temporizador
#define TICKS_2_MICROS(ticks, tcm_factor) \
  ((ticks) / (TCM_FREQ(tcm_factor) / 1000000L))

/// Pasamos de microsegundos a ticks según el factor de escala del temporizador
#define MICROS_2_TICKS(micros, tcm_factor) \
  ((micros) * (TCM_FREQ(tcm_factor) / 1000000L))

/// Guardamos los 16 bits más grandes del contador global
uint16_t timer_ticks_msb;

/// Guardamos el factor de escala del temporizador
uint8_t timer_tcm_factor;

/**
 * @brief Función que inicializa el temporizador global
 * 
 * @param tcm_factor especifica el factor de escala del temporizador,
 * debe tener un valor entre 0 y 7, o sino se usará el valor 3 por defecto
 */
void timer_init(uint8_t tcm_factor) {
  /// La primera vez que iniciamos el temporizador, reseteamos la variable global
  timer_ticks_msb = 0;

  /// Especificamos la escala del contador global
  timer_tcm_factor = ((tcm_factor <= 7) ? tcm_factor : 3);
  _io_ports[M6812_TMSK2] = timer_tcm_factor;

  /// Habilitamos las interrupciones de desbordamiento del temporizador
  _io_ports[M6812_TMSK2] |= M6812B_TOI;

  /// Habilitamos el temporizador global
  _io_ports[M6812_TSCR] |= M6812B_TEN;
}

/**
 * @brief Función que devuelve el tiempo en milisegundos desde que se
 * inició el temporizador
 * 
 * @return uint32_t Tiempo en milisegundos desde que se inició el temporizador
 */
uint32_t timer_milis(void) {
  /// Ponemos la variable global en los 16 bits más grandes del resultado
  uint32_t result = (uint32_t)timer_ticks_msb << 16;
  
  result += _IO_PORTS_W(M6812_TCNT); ///< Le sumamos el valor del contador

  /// Cogemos el tiempo en microsegundos y lo pasamos a milisegundos
  return TICKS_2_MICROS(result, timer_tcm_factor) / 1000;
}

/**
 * @brief Función que devuelve el tiempo en microsegundos desde que se
 * inició el temporizador
 * 
 * @return uint32_t Tiempo en microsegundos desde que se inició el temporizador
 */
uint32_t timer_micros(void) {
  /// Ponemos la variable global en los 16 bits más grandes del resultado
  uint32_t result = (uint32_t)timer_ticks_msb << 16;

  result += _IO_PORTS_W(M6812_TCNT); /// Le sumamos el valor del contador

  /// Retornamos el resultado en microsegundos
  return TICKS_2_MICROS(result, timer_tcm_factor);
}

/**
 * @brief Función que hace que el programa se quede esperando el tiempo
 * indicado en milisegundos
 * 
 * @param milis El tiempo de espera en milisegundos
 */
void timer_sleep_milis(uint32_t milis) {
  uint32_t timer = timer_micros(); ///< Guardamos el tiempo actual

  /// Esperamos hasta que el tiempo actual sea mayor o igual que el
  /// tiempo que guardamos más el tiempo de espera
  while (timer_micros() < timer + milis * (uint32_t)1000);
}

/**
 * @brief Función que hace que el programa se quede esperando el tiempo
 * indicado en microsegundos
 * 
 * @param micros El tiempo de espera en microsegundos
 */
void timer_sleep_micros(uint32_t micros) {
  uint32_t timer = timer_micros(); ///< Guardamos el tiempo actual

  /// Esperamos hasta que el tiempo actual sea mayor o igual que el
  /// tiempo que guardamos más el tiempo de espera
  while (timer_micros() < timer + micros);
}

/**
 * @brief Estructura que guarda los datos de una tarea del temporizador
 * 
 */
struct timer_task {
  uint8_t id; ///< Identificador de la tarea
  uint32_t periodic; ///< Período de ejecución de la tarea
  uint32_t when; ///< Momento en el que se debe ejecutar la tarea
  void (*task)(void* params); ///< Puntero a la función de la tarea
  void* params; ///< Parámetros de la tarea
};

/// tamaño timer task: 1(id) + 1(periodic) + 4(when) + 2(task) + 2(params) = 10
/// bytes 10 * 254 = 2540 bytes = 2.48 KB
/// 8 tareas como máximo, pues el id es uint8_t y el 0 no se usa
struct timer_task timer_tasks[8];

/**
 * @brief Función que arma la interupción de una tarea
 * 
 * @param id Identificador de la tarea a armar
 */
void timer_arm_task(uint8_t id) {
  struct timer_task* task = &timer_tasks[id - 1];
  
  #ifdef DEBUG
    serial_print("\n Rearming ");
    serial_printdecbyte(task->id);
    serial_print(" ");
    serial_printdecword(task->when);
    serial_print("\n");
  #endif  // DEBUG

  /// Ponemos en que valor del contador global se activará el comparador id-1
  _IO_PORTS_W(M6812_TC0 + (id - 1) * 2) =
      MICROS_2_TICKS(task->when, timer_tcm_factor);

  /// Encendemos el comparador id-1
  _io_ports[M6812_TIOS] |= M6812B_IOS0 << (id - 1);

  /// Ponemos el flag del comparador id-1 a 0
  _io_ports[M6812_TFLG1] |= M6812B_IOS0 << (id - 1);
  
  /// Habilitamos las interrupciones del comparador id-1
  _io_ports[M6812_TMSK1] |= M6812B_IOS0 << (id - 1);
  
  #ifdef DEBUG
    serial_print("\n TIOS ");
    serial_printbinbyte(_io_ports[M6812_TIOS]);
    serial_print(" TFLG1 ");
    serial_printbinword(_io_ports[M6812_TFLG1]);
    serial_print(" TMSK1 ");
    serial_printbinword(_io_ports[M6812_TMSK1]);
    serial_print("\n");
  #endif  // DEBUG
}

/**
 * @brief Función que ejecuta una tarea del temporizador
 * 
 * @param id Identificador de la tarea a ejecutar
 */
void timer_execute_task(uint8_t id) {
  #ifdef DEBUG
    serial_print("\n int task ");
    serial_printdecbyte(id);
    serial_print("\n");
  #endif  // DEBUG

  /// Obtenemos la tarea a ejecutar
  struct timer_task* task = &timer_tasks[id - 1];

  /// Comprobamos si efectivamente es el momento de ejcutar la tarea 1
  if (task->id != 0 && task->when <= timer_micros()) {

    /// Si la tarea está activa y es hora de ejecutarla, la ejecutamos
    task->task(task->params);

    /// Si la tarea es periódica, la rearmamos
    if (task->periodic != 0) {
      task->when += task->periodic;
      timer_arm_task(task->id);
    } else {
      /// Si no es periódica, la eliminamos
      timer_remove_task(task->id);

      /// Desarmamos la interupcion
      _io_ports[M6812_TMSK1] &= ~(M6812B_IOS0 << (id - 1));
    }
  }
}

/**
 * @brief Función que registra una tarea para que se ejecute cuando pase
 * el tiempo indicado desde que se inició el sistema
 * 
 * @param task Función que se ejecutará, debe ser void task(void * params),
 * @param params Puntero a los parámetros que se le pasarán a la función
 * @param when Tiempo en microsegundos desde que se inició el sistema, se
 * @return uint8_t Id de la tarea
 */
uint8_t timer_add_task(void (*task)(void* params), void* params,
                       uint32_t when) {

  /// Buscamos un hueco libre en el array de tareas
  for (uint8_t i = 0; i < 8; i++) {

    /// Si el id es 0, es que está libre
    if (timer_tasks[i].id == 0) {
    
      /// El id es el índice + 1, pues el id 0 es para tareas libres
      timer_tasks[i].id = i + 1;

      timer_tasks[i].periodic = 0;     ///< No es periódica
      timer_tasks[i].when = when;      ///< Cuando se ejecutará
      timer_tasks[i].task = task;      ///< La función a ejecutar
      timer_tasks[i].params = params;  ///< Los parámetros de la función
      
      timer_arm_task(i + 1); ///< Armamos la interupcion de la tarea

      return i + 1;  ///< Devolvemos el id
    }
  }

  return 0; ///< Si no hay hueco libre, devolvemos 0
}

/**
 * @brief Función que registra una tarea para que se ejecute periodicamente
 * 
 * @param task Función que se ejecutará, debe ser void task(void * params),
 * @param params Puntero a los parámetros que se le pasarán a la función
 * @param period Periodo en microsegundos, se recomienda que sea mayor a 1000
 * @return uint8_t Id de la tarea
 */
uint8_t timer_add_periodic_task(void (*task)(void* params), void* params,
                                uint32_t period) {

  /// Buscamos un hueco libre en el array de tareas
  for (uint8_t i = 0; i < 8; i++) {

    /// Si el id es 0, es que está libre
    if (timer_tasks[i].id == 0) {

      /// El id es el índice + 1, pues el id 0 es para tareas libres
      timer_tasks[i].id = i + 1;

      timer_tasks[i].periodic = period; ///< Cada cuanto se tiene que ejecutar
      timer_tasks[i].when =
          timer_micros() + period;     ///< Cuando se ejecutará a continuacion
      timer_tasks[i].task = task;      ///< La función a ejecutar
      timer_tasks[i].params = params;  ///< Los parámetros de la función

      timer_arm_task(i + 1); ///< Armamos la interupcion de la tarea

      return i + 1;  ///< Devolvemos el id
    }
  }

  return 0; ///< Si no hay hueco libre, devolvemos 0
}

/**
 * @brief Función que elimina una tarea del temporizador
 * 
 * @param id Identificador de la tarea a eliminar
 */
void timer_remove_task(uint8_t id) {
  /// Comprobamos que el id sea válido
  if (id > 0 && id < 255) {
    timer_tasks[id - 1].id = 0;
  }
}

/**
 * @brief Interupción de desbordamiento del temporizador
 *
 */
void __attribute__((interrupt)) vi_tov(void) {
  _io_ports[M6812_TFLG2] |= M6812B_TOF; ///< flag de desbordamiento a 0
  timer_ticks_msb++;

  #ifdef DEBUG
    //serial_print("\n int overflow \n");
  #endif  // DEBUG
}

/**
 * @brief Interupción del comparador del temporizador 0
 *
 */
void __attribute__((interrupt)) vi_ioc0(void) {
  _io_ports[M6812_TFLG1] |= M6812B_IOS1; ///< flag del comparador 0 = 0
  timer_execute_task(1);
}

/**
 * @brief Interupción del comparador del temporizador 1
 * 
 */
void __attribute__((interrupt)) vi_ioc1(void) {
  _io_ports[M6812_TFLG1] |= M6812B_IOS1; ///< flag del comparador 1 = 0
  timer_execute_task(2);
}

/**
 * @brief Interupción del comparador del temporizador 2
 * 
 */
void __attribute__((interrupt)) vi_ioc2(void) {
  _io_ports[M6812_TFLG1] |= M6812B_IOS2; ///< flag del comparador 2 = 0
  timer_execute_task(3);
}

/**
 * @brief Interupción del comparador del temporizador 3
 * 
 */
void __attribute__((interrupt)) vi_ioc3(void) {
  _io_ports[M6812_TFLG1] |= M6812B_IOS3; ///< flag del comparador 3 = 0
  timer_execute_task(4);
}

/**
 * @brief Interupción del comparador del temporizador 4
 * 
 */
void __attribute__((interrupt)) vi_ioc4(void) {
  _io_ports[M6812_TFLG1] |= M6812B_IOS4; ///< flag del comparador 4 = 0
  timer_execute_task(4);
}

/**
 * @brief Interupción del comparador del temporizador 5
 * 
 */
void __attribute__((interrupt)) vi_ioc5(void) {
  // Ponemos el flag del comparador 5 a 0
  _io_ports[M6812_TFLG1] |= M6812B_IOS5; ///< flag del comparador 5 = 0
  timer_execute_task(6);
}

/**
 * @brief Interupción del comparador del temporizador 6
 * 
 */
void __attribute__((interrupt)) vi_ioc6(void) {
  _io_ports[M6812_TFLG1] |= M6812B_IOS6; ///< flag del comparador 6 = 0
  timer_execute_task(7);
}
