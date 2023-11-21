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
 * @date 17/11/2023
 * 
 * @file test_timer_getters.c
 * @brief Ejemplo de uso de los getters de la libreria de temporizacion
 * para el contador global en microsegundos y milisegundos
 *
 * @bug No hay bugs conocidos
 * 
 * @version 0.1
 * @copyright Copyright (c) 2023
 * @brief Historial de Revisiones 
 * 17/11/23 - Creacion del archivo:
 *            Se ha creado el archivo y se ha creado la estructura del
 *            programa para probar los getters en milisegundos.
 * 21/11/23 - Terminado programa, documentado y testeado, además se ha
 *            rediseñado el archivo para probar los getters en
 *            microsegundos, se ha añadido un mensaje de cabecera del
 *            programa.
 */

#include <sys/interrupts.h>
#include <sys/locks.h>
#include <sys/param.h>
#include <sys/sio.h>
#include <types.h>
#include <timer.h>

int main() {
  serial_init(); ///< Inicializamos el puerto serie
  timer_init(3); ///< Inicializamos el temporizador con factor de 3
  
  /// Inicializamos las variables que almacenaran los valores a mostrar
  uint32_t mseg_begin = 0;
  uint32_t mseg_end = 0;
  uint32_t useg_begin = 0;
  uint32_t useg_end = 0;

  /// Imprimimos mensaje de inicio
  serial_print("\nPrograma test_timer_getters:\n");
  serial_print("Probamos la obtencion de el contador global ");
  serial_print("en microsegundos y milisegundos\n");

  while (1) {
    /// Obtenemos los valores de los contadores
    mseg_begin = timer_milis();
    mseg_end = timer_milis();
    useg_begin = timer_micros();
    useg_end = timer_micros();

    /// Imprimimos los valores obtenidos en milisegundos
    serial_print("\n\nInicial Milis: ");
    serial_printdeclong(mseg_begin);
    serial_print(" ; Final Milis: ");
    serial_printdeclong(mseg_end);
    serial_print(" ; Diferencia Milis: ");
    serial_printdeclong(mseg_end - mseg_begin);

    /// Imprimimos los valores obtenidos en microsegundos
    serial_print("\nInicial Micros: ");
    serial_printdeclong(useg_begin);
    serial_print(" ; Final Micros: ");
    serial_printdeclong(useg_end);
    serial_print(" ; Diferencia Micros: ");
    serial_printdeclong(useg_end - useg_begin);

    /// Imprimimos el valor del contador global
    serial_print("\nContador: ");
    serial_printdecword((uint16_t)(timer_micros() >> 16));
  }

  return 0;
}
