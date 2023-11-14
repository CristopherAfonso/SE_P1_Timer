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
  uint32_t periodic;
  uint32_t when;
  void (*task)(void* params);
  void* params;
};

// tamaño timer task: 1(id) + 1(periodic) + 4(when) + 2(task) + 2(params) = 10
// bytes 10 * 254 = 2540 bytes = 2.48 KB
struct timer_task timer_tasks[254];  // 254 tareas como máximo, pues el id es
                                     // uint8_t y el 0 no se usa
uint8_t timer_tasker_armed = 0;

void timer_rearm_tasker() {
  // Recorremos todas las tareas, en busca de la proxima a ejecutar
  struct timer_task* next_task = 0;
  for (uint8_t i = 0; i < 255; i++) {
    if (timer_tasks[i].id != 0 && timer_tasks[i].when > timer_micros()) {
      // Si la tarea está activa, comprobamos si es la mas próxima a ejecutar
      if (next_task == 0 || timer_tasks[i].when < next_task->when) {
        // Si es la mas próxima a ejecutar, la guardamos
        next_task = &timer_tasks[i];
      }
    }

    // Si next_task no esta vacio, rearmamos el tasker (comparador 0)
    if (next_task != 0) {
      _io_ports[M6812_TMSK1] |= M6812B_C0I;
      _io_ports[M6812_TC0] = next_task->when;
      timer_tasker_armed = 1;
    }
  }
}

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

      // Comprobamos si hay que rearmar el tasker
      if (timer_tasker_armed == 0) {
        timer_rearm_tasker();
      }

      return i + 1;  // Devolvemos el id
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
      timer_tasks[i].periodic = period;  // Cada cuanto se tiene que ejecutar
      timer_tasks[i].when =
          timer_micros() + period;     // Cuando se ejecutará a continuacion
      timer_tasks[i].task = task;      // La función a ejecutar
      timer_tasks[i].params = params;  // Los parámetros de la función

      // Comprobamos si hay que rearmar el tasker
      if (timer_tasker_armed == 0) {
        timer_rearm_tasker();
      }

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
  _io_ports[M6812_TFLG2] = M6812B_TOF;

  // Incrementamos el contador de ticks MSB
  timer_ticks_msb++;
}

/**
 * @brief Interupción del tasker
 *
 */
void __attribute__((interrupt)) vi_ioc0(void) {
  // Ponemos el flag del comparador 0 a 0
  _io_ports[M6812_TFLG1] = M6812B_C0F;

  // Recorremos todas las tareas, en busca de las que tengan un when menor o
  // igual al actual
  for (uint8_t i = 0; i < 255; i++) {
    if (timer_tasks[i].id != 0 && timer_tasks[i].when <= timer_micros()) {
      // Si la tarea está activa y es hora de ejecutarla, la ejecutamos
      timer_tasks[i].task(timer_tasks[i].params);

      // Si la tarea es periódica, la rearmamos
      if (timer_tasks[i].periodic != 0) {
        timer_tasks[i].when += timer_tasks[i].periodic;
      } else {
        // Si no es periódica, la eliminamos
        timer_remove_task(timer_tasks[i].id);
      }
    }
  }

  // Rearmamos el tasker
  timer_rearm_tasker();
}