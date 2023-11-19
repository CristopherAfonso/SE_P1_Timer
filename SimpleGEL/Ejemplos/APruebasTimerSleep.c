/**
 * @file Aprueba.c
 * @author Francesco La Spina (alu0101435022@ull.edu.es)
 * @author CRISTOPHER MANUEL AFONSO MORA (alu0101402031@ull.edu.es)
 * @brief Ejemplo de uso de la libreria de temporizacion con el sleep
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

int main() {
  serial_init();
  timer_init(3);
  uint32_t cuenta = 0;

  // Cuenta cada segundo
  while (1) {
    serial_printdeclong(cuenta);
    serial_print("\n");
    timer_sleep_milis(1000);
    cuenta++;
  }
}