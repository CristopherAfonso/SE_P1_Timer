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
 * @file test_timer_task.c
 * @brief Ejemplo de uso de las funciones de gestión de tareas de la
 * libreria de temporizacion 
 *
 * @bug No hay bugs conocidos
 * 
 * @version 0.1
 * @copyright Copyright (c) 2023
 * @brief Historial de Revisiones 
 * 17/11/23 - Creacion del archivo:
 *            Se ha creado el archivo y se ha creado la estructura del
 *            programa para probar las funciones de gestion de tareas
 *            de la librería de temporizacion, se crean 4 tareas, 2 de
 *            ellas periódicas y 2 de ellas no periódicas
 * 21/11/23 - Programa documentado
 */

#include <sys/interrupts.h>
#include <sys/locks.h>
#include <sys/param.h>
#include <sys/sio.h>
#include <types.h>
#include <timer.h>

/**
 * @brief Función que se ejecuta cada 3 segundos
 * 
 * @param params Puntero a los parametros de la tarea
 */
void period_task_1(void *params) {
  _io_ports[M6812_PORTG] ^= M6812B_PG7;
}

int main() {
  serial_init(); ///< Inicializamos el puerto serie
  timer_init(3); ///< Inicializamos el temporizador con factor de 3
  
  /// Encendemos led  
  _io_ports[M6812_DDRG] |= M6812B_PG7;
  _io_ports[M6812_PORTG] |= M6812B_PG7;
  
  /// Desconectamos el flag OM1 del comparador 1
  _io_ports[M6812_TCTL2] &= ~(M6812B_OM0);
  
  /// Conectamos el flag OL1 del comparador 1
  _io_ports[M6812_TCTL2] |= (M6812B_OL0);

  /// Declaramos las variables a usar
  uint8_t param = 0;
  uint8_t id_1 = timer_add_periodic_task(period_task_1, &param, 250); 
  

  /// Imprimimos mensaje de inicio
  serial_print("\nPrograma test_timer_task: "); 
  serial_print("Probamos las funciones de gestión de tareas de la ");
  serial_print("librería de temporizacion");
  serial_print("\n  1. timer_add_task"); 
  serial_print("\n  2. timer_add_periodic_task");
  serial_print("\n  3. timer_remove_task");

  /// Imprimimos el id de la tarea
  serial_print(" ID1: ");
  serial_printdecword(id_1);
  
  /// Entramos en un bucle infinito a esperar a que se ejecuten las tareas
  while (1) __asm__ __volatile__("wai");

  return 0;
}
