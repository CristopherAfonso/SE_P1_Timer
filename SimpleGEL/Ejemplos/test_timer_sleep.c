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
  uint32_t miliseg_ini = 0;
  uint32_t miliseg_fin = 0;
  uint32_t microseg = 0;

  // Cuenta cada segundo
  while (1) {
	miliseg_ini = timer_milis();
    timer_sleep_milis(1000);
    cuenta++;
    miliseg_fin = timer_milis();
    microseg = timer_micros();
    serial_print("\nSegundos: ");
    serial_printdeclong(cuenta);
    serial_print("; Milisegundos ini: ");
    serial_printdeclong(miliseg_ini);
    serial_print("; Milisegundos fin: ");
    serial_printdeclong(miliseg_fin);
    serial_print("; Microsegundos: ");
    serial_printdeclong(microseg);
  }
}
