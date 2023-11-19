/**
 * @file Aprueba.c
 * @author Francesco La Spina (alu0101435022@ull.edu.es)
 * @author CRISTOPHER MANUEL AFONSO MORA (alu0101402031@ull.edu.es)
 * @brief Ejemplo de uso de la libreria de temporizacion con dos funciones periodicas y una que se ejecuta a un segundo
 * y un sleep
 * @version 0.1
 * @date 2023-11-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <sys/interrupts.h>
#include <sys/locks.h>
#include <sys/param.h>
#include <sys/sio.h>
#include <timer.h>
#include <types.h>

void initial_task(void *params) {
  serial_print("Task executed after 1 second before reset\n");
}

void period_task_1(void *params) {
  serial_print("Periodic task 1 executed\n");
}

void period_task_2(void *params) {
  serial_print("Periodic task 2 executed\n");
}

int main() {
  serial_init();
  timer_init(3);

  uint8_t param = 0;
  uint8_t id_0 = timer_add_periodic_task(initial_task, &param, timer_micros() + 1000000);
  uint8_t id_1 = timer_add_periodic_task(period_task_1, &param, 3000000);
  uint8_t id_2 = timer_add_periodic_task(period_task_2, &param, 6500000);

  while (1) {
    __asm__ __volatile__("wai");
  }
}