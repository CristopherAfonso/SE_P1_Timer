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
 * @file test_timer_sleep.c
 * @brief Ejemplo de uso de las funciones sleep de la libreria de
 * temporizacion para el contador global en microsegundos y milisegundos
 *
 * @bug No hay bugs conocidos
 * 
 * @version 0.1
 * @copyright Copyright (c) 2023
 * @brief Historial de Revisiones 
 * 17/11/23 - Creacion del archivo:
 *            Se ha creado el archivo y se ha creado la estructura del
 *            programa para probar las funciones sleep en microsegundos
 *            y milisegundos.
 * 21/11/23 - Programa rediseñado y documentado, mostramos mensaje de
 *            inicio del programa y probamos las funciones sleep, además
 *            de mostrar las diferencias de los valores obtenidos
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
  
  /// Encendemos led  
  _io_ports[M6812_DDRG] |= M6812B_PG7;
  _io_ports[M6812_PORTG] &= ~M6812B_PG7; ///< bit a cero
  
  /// Desconectamos el flag OM1 del comparador 1 
  _io_ports[M6812_TCTL2] &= ~(M6812B_OM0);
  
  /// Conectamos el flag OL1 del comparador 1 
  _io_ports[M6812_TCTL2] |= (M6812B_OL0);
  
  /// Declaramos las variables a usar
  //uint32_t mseg_ini = 0;
  //uint32_t mseg_fin = 0;
  uint32_t useg_ini = 0;
  uint32_t useg_fin = 0;
  //uint32_t cuenta = 0;

  /// Imprimimos mensaje de inicio
  serial_print("\nPrograma test_timer_sleep:\n");
  serial_print("Probamos las funciones timer_sleep_milis y "); 
  serial_print("timer_sleep_micros\n");

  /// Cuenta cada segundo 
  while (1) {
	  /*
	  /// Obtenemos los valores de los contadores en milisegundos
    mseg_ini = timer_milis();
    timer_sleep_milis(500);
    mseg_fin = timer_milis();*/

    /// Obtenemos los valores de los contadores en microsegundos
    useg_ini = timer_micros();
    _io_ports[M6812_PORTG] ^= M6812B_PG7;
    timer_sleep_micros(5000);
    _io_ports[M6812_PORTG] ^= M6812B_PG7;
    timer_sleep_micros(5000);
    useg_fin = timer_micros();
    
    /*
    cuenta += 2; ///< pasan 2 segundos
    
    /// Imprimimos los segundos
    serial_print("\n\nSegundos: ");
    serial_printdeclong(cuenta); 

    /// Imprimimos los valores obtenidos en milisegundos
    serial_print("\nMiliSeg ini: "); 
    serial_printdeclong(mseg_ini);
    serial_print("; MiliSeg fin: ");
    serial_printdeclong(mseg_fin);
    serial_print("; Diferencia MiliSeg: ");
    serial_printdeclong(mseg_fin - mseg_ini);
    
    /// Imprimimos los valores obtenidos en microsegundos
    serial_print("\nMicroSeg ini: ");
    serial_printdeclong(useg_ini);
    serial_print("; MicroSeg fin: ");
    serial_printdeclong(useg_fin);
    serial_print("; Diferencia MicroSeg: ");
    serial_printdeclong(useg_fin - useg_ini);
    */
  }

  return 0;
}
