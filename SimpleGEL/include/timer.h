#include <types.h>

/**
 * @brief Funcion que se necesita llamar nada mas iniciar el sistema, para que
 * el timer funcione
 *
 *
 */
void timer_init(uint8_t tcm_factor);

/**
 * @brief Obtiene el tiempo en milisegundos desde que se inicio el sistema
 * @warning A los 71 minutos de funcionamiento el contador se reinicia, y el
 * tiempo de encendido se reiniciara
 *
 * @return uint32_t Tiempo en milisegundos
 */
uint32_t timer_milis(void);

/**
 * @brief Obtiene el tiempo en microsecundos desde que se inicio el sistema
 * @warning A los 71 minutos de funcionamiento el contador se reinicia, y el
 * tiempo de encendido se reiniciara
 *
 * @return uint32_t Tiempo en microsecundos
 */
uint32_t timer_micros(void);

/**
 * @brief El programa se queda esperando el tiempo indicado en milisegundos
 * @warning Las interrupciones no se desactivan durante la espera
 * @warning No se pueden poner tiempos superiores a 71 minutos (2^32/1000
 * milisegundos)
 *
 * @param milis El tiempo de espera en milisegundos
 */
void timer_sleep_milis(uint32_t milis);

/**
 * @brief El programa se queda esperando el tiempo indicado en microsecundos
 * @warning Las interrupciones no se desactivan durante la espera
 * @warning No se pueden poner tiempos superiores a 71 minutos (2^32/1000
 * milisegundos)
 *
 * @param micros El tiempo de espera en microsecundos
 */
void timer_sleep_micros(uint32_t micros);

/**
 * @brief Registra una tarea para que se ejecute cuando pase el tiempo indicado
 * desde que se inicio el sistema
 * @warning La tarea se ejecutara en una interupcion, por lo que no se
 * puede llamar a funciones que no sean seguras en interrupciones. Si se
 * registran tareas muy seguidas, puede colisionar con la anterior, generando un
 * estado inestable.
 *
 * @param task funcion que se ejecutara, debe ser void task(void * params), en
 * donde params es un puntero a los parametros que se le pasaran a la funcion
 * @param params puntero a los parametros que se le pasaran a la funcion
 * @param when tiempo en microsegundos desde que se inicio el sistema, se
 * recomienda que sea mayor a 1000 del actual
 * @return uint8_t id de la tarea
 */
uint8_t timer_add_task(void (*task)(void* params), void* params, uint32_t when);

/**
 * @brief Registra una tarea para que se ejecute periodicamente cada cierto
 * tiempo
 * @warning La tarea se ejecutara en una interupcion, por lo que no se
 * puede llamar a funciones que no sean seguras en interrupciones. Si se
 * registran tareas muy seguidas, puede colisionar con la anterior, generando un
 * estado inestable.
 *
 * @param task funcion que se ejecutara, debe ser void task(void * params), en
 * donde params es un puntero a los parametros que se le pasaran a la funcion
 * @param params puntero a los parametros que se le pasaran a la funcion
 * @param period periodo en microsegundos, se recomienda que sea mayor a 1000
 * @return uint8_t id de la tarea
 */
uint8_t timer_add_periodic_task(void (*task)(void* params), void* params,
                                uint32_t period);

/**
 * @brief Elimina una tarea
 *
 * @param id id de la tarea
 */
void timer_remove_task(uint8_t id);
