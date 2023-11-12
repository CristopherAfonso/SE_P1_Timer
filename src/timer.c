#include <types.h>

void timer_init(void) {}

uint32_t timer_milis(void) {}

uint32_t timer_micros(void) {}

void timer_sleep_milis(uint32_t milis) {}

void timer_sleep_micros(uint32_t micros) {}

uint8_t timer_add_task(void (*task)(void* params), void* params,
                       uint32_t when) {}

uint8_t timer_add_periodic_task(void (*task)(void* params), void* params,
                                uint32_t period) {}

void timer_remove_task(uint8_t id) {}