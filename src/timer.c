#include <sys/sio.h>
#include <types.h>

#define TCM_FACTOR (3)  // La potencia de 2 a aplicar al factor

uint16_t timer_ticks_msb;

void timer_init(void) {
  // La primera vez que iniciamos el temporizador, reseteamos la variable global
  timer_ticks_msb = 0;

  // Especificamos que el contador global estará en microsegundos
  _io_ports[M6812_TMSK2] = TCM_FACTOR;

  // Habilitamos las interrupciones de desbordamiento del temporizador
  _io_ports[M6812_TMSK2] |= M6812B_TOI;

  // Habilitamos el temporizador global
  _io_ports[M6812_TSCR] |= M6812B_TEN;
}

uint32_t timer_milis(void) {
  // Cogemos el tiempo en microsegundos y lo pasamos a milisegundos
  return timer_micros() / 1000;
}

uint32_t timer_micros(void) {
  // Ponemos la variable global en los 16 bits más grandes del resultado
  uint32_t result = timer_ticks_msb << 16;
  
  // Le sumamos el valor actual del contador
  result += _io_ports[M6812_TCNT];
  
  return result;
}

void timer_sleep_milis(uint32_t milis) {}

void timer_sleep_micros(uint32_t micros) {}

struct timer_task {
  uint8_t id;
  uint8_t periodic;
  uint32_t when;
  void (*task)(void* params);
  void* params;
};

struct timer_task timer_tasks[254];  // 254 tareas como máximo, pues el id es
                                     // uint8_t y el 0 no se usa

uint8_t timer_add_task(void (*task)(void* params), void* params,
                       uint32_t when) {
  // Buscamos un hueco libre en el array de tareas
  for (uint8_t i = 0; i < 255; i++) {
    // Si el id es 0, es que está libre
    if (timer_tasks[i].id == 0) {
      // Llenamos los campos de la tarea
      timer_tasks[i].id =
          i + 1;  // El id es el índice + 1, pues el id 0 es para tareas libres
      timer_tasks[i].periodic = 0;     // No es periódica
      timer_tasks[i].when = when;      // Cuando se ejecutará
      timer_tasks[i].task = task;      // La función a ejecutar
      timer_tasks[i].params = params;  // Los parámetros de la función
      return i + 1;                    // Devolvemos el id
    }
  }

  // Si no hay hueco libre, devolvemos 0
  return 0;
}

uint8_t timer_add_periodic_task(void (*task)(void* params), void* params,
                                uint32_t period) {
  // Buscamos un hueco libre en el array de tareas
  for (uint8_t i = 0; i < 255; i++) {
    // Si el id es 0, es que está libre
    if (timer_tasks[i].id == 0) {
      // Llenamos los campos de la tarea
      timer_tasks[i].id =
          i + 1;  // El id es el índice + 1, pues el id 0 es para tareas libres
      timer_tasks[i].periodic = 1;     // Es periódica
      timer_tasks[i].when = period;    // Cuando se ejecutará
      timer_tasks[i].task = task;      // La función a ejecutar
      timer_tasks[i].params = params;  // Los parámetros de la función
      return i + 1;                    // Devolvemos el id
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
  _io_ports[M6812_TFLG2] = M6812B_TOF;

  // Incrementamos el contador de ticks MSB
  timer_ticks_msb++;
}