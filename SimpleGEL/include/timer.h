/**
 * Universidad de La Laguna (ULL)
 * Escuela Superior de Ingenieria y Tecnologia (ESIT)
 * Grado en Ingenieria Informatica
 * Asignatura: Sistemas Empotrados
 * Curso: 4º
 * Itinerario 2: Ingeniería de Computadores
 * Practica 1: Librerías
 * @author Cristopher Manuel Afonso Mora (alu0101402031@ull.edu.es)
 * @author Francesco La Spina (alu0101435022@ull.edu.es)
 * @date 16/11/2023
 *
 * @file timer.h
 * @brief Donde se aloja la declaración de las funciones de la librería
 * timer, que se encarga de gestionar el temporizador de forma más
 * asequible en el microcontrolador 68HC12
 *
 * @bug No hay bugs conocidos
 *
 * @version 0.1
 * @copyright Copyright (c) 2023
 * @brief Historial de Revisiones
 * 16/11/23 - Creacion del archivo:
 *            Se han creado todas las definiciones de cada función que
 *            usará esta librería, cada una documentada
 * 17/11/23 - A la función timer_init se le ha añadido un parametro
 *            para especificar el factor de escala del timer.
 * 21/11/23 - Archivo terminado, documentación finalizada
 */

#ifndef _TIMER_H
#define _TIMER_H

#include <types.h>

/*! @defgroup timer Timer Functions
 *
 * Conjunto de funciones que facilitan la interacción con el temporizador
 * del microcontrolador 68HC12
 *
 */
/*@{*/

/*! Inicializa el temporizador.
 * @brief Funcion que se necesita llamar nada mas iniciar el sistema,
 * para que el timer funcione
 *
 * @param tcm_factor especifica el factor de escala del timer, debe
 * tener un valor entre 1, 2, 4, 8, 16, 32, 64, 128
 */
void timer_init(uint8_t tcm_factor);

/*! Obtiene el tiempo en milisegundos desde que se inicio el sistema
 * @brief Obtiene el tiempo en milisegundos desde que se inicio el sistema
 * @warning El tiempo en desbordar dependerá del factor del timer, pero
 * para un factor de 3, el tiempo en desborde es de 16 segundos
 *
 * @return uint32_t Tiempo del contador global en milisegundos
 */
uint32_t timer_milis(void);

/*! Obtiene el tiempo en microsecundos desde que se inicio el sistema
 * @brief Obtiene el tiempo en microsecundos desde que se inicio el sistema
 * @warning El tiempo en desbordar dependerá del factor del timer, pero
 * para un factor de 3, el tiempo en desborde es de 16 segundos
 *
 * @return uint32_t Tiempo del contador global en microsecundos
 */
uint32_t timer_micros(void);

/*! El programa se queda esperando el tiempo indicado en milisegundos
 * @brief El programa se queda esperando el tiempo indicado en milisegundos
 * @warning Las interrupciones no se desactivan durante la espera
 * @warning No se pueden poner tiempos superiores a 16 segundos (2^24/1000
 * milisegundos)
 *
 * @param milis El tiempo de espera en milisegundos
 */
void timer_sleep_milis(uint32_t milis);

/*! El programa se queda esperando el tiempo indicado en microsecundos
 * @brief El programa se queda esperando el tiempo indicado en microsecundos
 * @warning Las interrupciones no se desactivan durante la espera
 * @warning No se pueden poner tiempos superiores a 16 segundos (2^24/1000
 * milisegundos)
 *
 * @param micros El tiempo de espera en microsecundos
 */
void timer_sleep_micros(uint32_t micros);

/*! Registra una tarea para que se ejecute cuando pase el tiempo dado. Solo se
 * pueden registrar 8 tareas.
 * @brief Registra una tarea para que se ejecute cuando pase el tiempo
 * indicado desde que se inicio el sistema
 * @warning La tarea se ejecutara en una interupcion, por lo que no se
 * puede llamar a funciones que no sean seguras en interrupciones y ejecutar e
 * un periodo relativamente rapido
 *
 * @param task funcion que se ejecutara, debe ser void task(void * params),
 * en donde params es un puntero a los parametros que se le pasaran a la
 * funcion
 * @param params puntero a los parametros que se le pasaran a la funcion
 * @param when tiempo en microsegundos desde que se inicio el sistema
 * @return uint8_t id de la tarea
 */
uint8_t timer_add_task(void (*task)(void* params), void* params, uint32_t when);

/*! Registra una tarea para que se ejecute periodicamente. Solo se
 * pueden registrar 8 tareas.
 * @brief Registra una tarea para que se ejecute periodicamente cada
 * cierto tiempo
 * @warning La tarea se ejecutara en una interupcion, por lo que no se
 * puede llamar a funciones que no sean seguras en interrupciones y ejecutar e
 * un periodo relativamente rapido
 *
 * @param task funcion que se ejecutara, debe ser void task(void * params),
 * en donde params es un puntero a los parametros que se le pasaran a la
 * funcion
 * @param params puntero a los parametros que se le pasaran a la funcion
 * @param period periodo en microsegundos, se recomienda que este sea mayor a 1
 * milisegundo
 * @return uint8_t id de la tarea
 */
uint8_t timer_add_periodic_task(void (*task)(void* params), void* params,
                                uint32_t period);

/*! Elimina una tarea registrada
 * @brief Elimina una tarea
 *
 * @param id id de la tarea a eliminar
 */
void timer_remove_task(uint8_t id);

/*@}*/

#endif /* _TIMER_H */
