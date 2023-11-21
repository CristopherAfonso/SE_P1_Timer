/**
 * @file Aprueba.c
 * @author Francesco La Spina (alu0101435022@ull.edu.es)
 * @author CRISTOPHER MANUEL AFONSO MORA (alu0101402031@ull.edu.es)
 * @brief Ejemplo de uso de la libreria de temporizacion con la obtencion de el contador global
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
  uint32_t begin;
  uint32_t end;


  while (1) {
    begin = timer_milis();
    end = timer_milis();
    serial_print("\nInicial Micros: ");
    serial_printdeclong(begin);
    serial_print(" ; Final Micros: ");
    serial_printdeclong(end);
    serial_print("\nContador: ");
    serial_printdecword((uint16_t)(timer_micros() >> 16));
  }
  return 0;
}
