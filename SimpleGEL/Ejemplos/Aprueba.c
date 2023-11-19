/**
 * @file Aprueba.c
 * @author Francesco La Spina (alu0101435022@ull.edu.es)
 * @author CRISTOPHER MANUEL AFONSO MORA (alu0101402031@ull.edu.es)
 * @brief Ejemplo de uso de la libreria de temporizacion con una tarea periodica
 * y un sleep
 * @version 0.1
 * @date 2023-11-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <sys/interrupts.h>
#include <sys/sio.h>
#include <timer.h>
#include <types.h>

void test_task(void *params) {
  uint8_t *param = (uint8_t *)params;
  serial_print("Task executed\n");
  serial_print("Param: ");
  serial_printdecbyte(*param);
  param++;
}

int main() {
  serial_init();
  timer_init(3);

  uint8_t param = 5;
  uint8_t id = timer_add_periodic_task(test_task, &param, 3000000);
  serial_print("id: ");
  serial_printdecbyte(id);
  serial_print("\n");

  while (1) {
    serial_print("\inicial ");
    serial_printdeclong(timer_milis());
    timer_sleep_milis(1000);
    serial_print(" Final ");
    serial_printdeclong(timer_milis());
    serial_print("\n");
    serial_print("\nContador: ");
    serial_printdecword((uint16_t)(timer_micros() >> 16));
  }
  return 0;
}
