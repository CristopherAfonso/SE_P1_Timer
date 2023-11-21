/**
 * @file timer.h
 * @author Francesco La Spina (alu0101435022@ull.edu.es)
 * @author CRISTOPHER MANUEL AFONSO MORA (alu0101402031@ull.edu.es)
 * @brief Libreria de temporizacion
 * @version 0.1
 * @date 2023-11-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <sys/param.h>
#include <sys/sio.h>
#include <timer.h>
#include <types.h>

//#define DEBUG

#define TCM_FREQ(tcm_factor) (M6812_CPU_E_CLOCK / (1 << tcm_factor))
// Pasamos de ticks a microsegundos según el factor de escala del temporizador
#define TICKS_2_MICROS(ticks, tcm_factor) \
  ((ticks) / (TCM_FREQ(tcm_factor) / 1000000L))
// Pasamos de microsegundos a ticks según el factor de escala del temporizador
#define MICROS_2_TICKS(micros, tcm_factor) \
  ((micros) * (TCM_FREQ(tcm_factor) / 1000000L))

uint16_t timer_ticks_msb;
uint8_t timer_tcm_factor;  // Guardamos el factor de escala del temporizador

void timer_init(uint8_t tcm_factor) {
  // La primera vez que iniciamos el temporizador, reseteamos la variable global
  timer_ticks_msb = 0;

  // Especificamos que el contador global estará en microsegundos
  _io_ports[M6812_TMSK2] = tcm_factor;
  timer_tcm_factor = tcm_factor;

  // Habilitamos las interrupciones de desbordamiento del temporizador
  _io_ports[M6812_TMSK2] |= M6812B_TOI;

  // Habilitamos el temporizador global
  _io_ports[M6812_TSCR] |= M6812B_TEN;
}

uint32_t timer_milis(void) {
  // Ponemos la variable global en los 16 bits más grandes del resultado
  uint32_t result = (uint32_t)timer_ticks_msb << 16;

  // Le sumamos el valor actual del contador
  result += _IO_PORTS_W(M6812_TCNT);

  // Cogemos el tiempo en microsegundos y lo pasamos a milisegundos
  return TICKS_2_MICROS(result, timer_tcm_factor) / 1000;
}

uint32_t timer_micros(void) {
  // Ponemos la variable global en los 16 bits más grandes del resultado
  uint32_t result = (uint32_t)timer_ticks_msb << 16;

  // Le sumamos el valor actual del contador
  result += _IO_PORTS_W(M6812_TCNT);

  return TICKS_2_MICROS(result, timer_tcm_factor);
}

void timer_sleep_milis(uint32_t milis) {
  uint32_t timer = timer_micros();
  while (timer_micros() < timer + milis * (uint32_t)1000)
    ;
}

void timer_sleep_micros(uint32_t micros) {
  uint32_t timer = timer_micros();
  while (timer_micros() < timer + micros)
    ;
}

struct timer_task {
  uint8_t id;
  uint32_t periodic;
  uint32_t when;
  void (*task)(void* params);
  void* params;
};

// tamaño timer task: 1(id) + 1(periodic) + 4(when) + 2(task) + 2(params) = 10
// bytes 10 * 254 = 2540 bytes = 2.48 KB
struct timer_task timer_tasks[8];  // 8 tareas como máximo, pues el id es
                                   // uint8_t y el 0 no se usa

void timer_arm_task(uint8_t id) {
  struct timer_task* task = &timer_tasks[id - 1];
#ifdef DEBUG
  serial_print("\n Rearming ");
  serial_printdecbyte(task->id);
  serial_print(" ");
  serial_printdecword(task->when);
  serial_print("\n");
#endif  // DEBUG

  // Ponemos en que valor del contador global se activará el comparador id-1
  _IO_PORTS_W(M6812_TC0 + (id - 1) * 2) =
      MICROS_2_TICKS(task->when, timer_tcm_factor);
  // Encendemos el comparador id-1
  _io_ports[M6812_TIOS] |= M6812B_IOS0 << (id - 1);
  // Ponemos el flag del comparador id-1 a 0
  _io_ports[M6812_TFLG1] |= M6812B_IOS0 << (id - 1);
  // Habilitamos las interrupciones del comparador id-1
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

void timer_execute_task(uint8_t id) {
#ifdef DEBUG
  serial_print("\n int task ");
  serial_printdecbyte(id);
  serial_print("\n");
#endif  // DEBUG
  struct timer_task* task = &timer_tasks[id - 1];

  // Comprobamos si efectivamente es el momento de ejcutar la tarea 1
  if (task->id != 0 && task->when <= timer_micros()) {
    // Si la tarea está activa y es hora de ejecutarla, la ejecutamos
    task->task(task->params);

    // Si la tarea es periódica, la rearmamos
    if (task->periodic != 0) {
      task->when += task->periodic;
      timer_arm_task(task->id);
    } else {
      // Si no es periódica, la eliminamos
      timer_remove_task(task->id);
      // Desarmamos la interupcion
      _io_ports[M6812_TMSK1] &= ~(M6812B_IOS0 << (id - 1));
    }
  }
}

uint8_t timer_add_task(void (*task)(void* params), void* params,
                       uint32_t when) {
  // Buscamos un hueco libre en el array de tareas
  for (uint8_t i = 0; i < 8; i++) {
    // Si el id es 0, es que está libre
    if (timer_tasks[i].id == 0) {
      // Llenamos los campos de la tarea
      timer_tasks[i].id =
          i + 1;  // El id es el índice + 1, pues el id 0 es para tareas libres
      timer_tasks[i].periodic = 0;     // No es periódica
      timer_tasks[i].when = when;      // Cuando se ejecutará
      timer_tasks[i].task = task;      // La función a ejecutar
      timer_tasks[i].params = params;  // Los parámetros de la función

      // Armamos la interupcion de la tarea
      timer_arm_task(i + 1);

      return i + 1;  // Devolvemos el id
    }
  }

  // Si no hay hueco libre, devolvemos 0
  return 0;
}

uint8_t timer_add_periodic_task(void (*task)(void* params), void* params,
                                uint32_t period) {
  // Buscamos un hueco libre en el array de tareas
  for (uint8_t i = 0; i < 8; i++) {
    // Si el id es 0, es que está libre
    if (timer_tasks[i].id == 0) {
      // Llenamos los campos de la tarea
      timer_tasks[i].id =
          i + 1;  // El id es el índice + 1, pues el id 0 es para tareas libres
      timer_tasks[i].periodic = period;  // Cada cuanto se tiene que ejecutar
      timer_tasks[i].when =
          timer_micros() + period;     // Cuando se ejecutará a continuacion
      timer_tasks[i].task = task;      // La función a ejecutar
      timer_tasks[i].params = params;  // Los parámetros de la función

      // Armamos la interupcion de la tarea
      timer_arm_task(i + 1);

      return i + 1;  // Devolvemos el id
    }
  }

  // Si no hay hueco libre, devolvemos 0
  return 0;
}

void timer_remove_task(uint8_t id) {
  // Comprobamos que el id sea válido
  if (id > 0 && id < 255) {
    timer_tasks[id - 1].id = 0;
  }
}

/**
 * @brief Interupción de desbordamiento del temporizador
 *
 */
void __attribute__((interrupt)) vi_tov(void) {
  // Ponemos el flag de desbordamiento a 0
  _io_ports[M6812_TFLG2] |= M6812B_TOF;

#ifdef DEBUG
  //serial_print("\n int overflow \n");
#endif  // DEBUG

  // Incrementamos el contador de ticks MSB
  timer_ticks_msb++;
}

/**
 * @brief Interupciones de las tareas
 *
 */

void __attribute__((interrupt)) vi_ioc0(void) {
  // Ponemos el flag del comparador 1 a 0
  _io_ports[M6812_TFLG1] |= M6812B_IOS1;

  timer_execute_task(1);
}

void __attribute__((interrupt)) vi_ioc1(void) {
  // Ponemos el flag del comparador 1 a 0
  _io_ports[M6812_TFLG1] |= M6812B_IOS1;

  timer_execute_task(2);
}

void __attribute__((interrupt)) vi_ioc2(void) {
  // Ponemos el flag del comparador 2 a 0
  _io_ports[M6812_TFLG1] |= M6812B_IOS2;

  timer_execute_task(3);
}

void __attribute__((interrupt)) vi_ioc3(void) {
  // Ponemos el flag del comparador 3 a 0
  _io_ports[M6812_TFLG1] |= M6812B_IOS3;

  timer_execute_task(4);
}

void __attribute__((interrupt)) vi_ioc4(void) {
  // Ponemos el flag del comparador 4 a 0
  _io_ports[M6812_TFLG1] |= M6812B_IOS4;

  timer_execute_task(4);
}

void __attribute__((interrupt)) vi_ioc5(void) {
  // Ponemos el flag del comparador 5 a 0
  _io_ports[M6812_TFLG1] |= M6812B_IOS5;

  timer_execute_task(6);
}

void __attribute__((interrupt)) vi_ioc6(void) {
  // Ponemos el flag del comparador 6 a 0
  _io_ports[M6812_TFLG1] |= M6812B_IOS6;

  timer_execute_task(7);
}
