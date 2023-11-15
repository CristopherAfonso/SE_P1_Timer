#include <sys/interrupts.h>
#include <sys/sio.h>
#include <timer.h>
#include <types.h>

void test_task(void *params) {
  uint8_t *param = (uint8_t *)params;
  sio_printf("Task executed\n");
  sio_printf("Param: %d\n", *param);
  param++;
}

int main() {
  serial_init();
  timer_init();

  uint8_t param = 5;
  uint8_t id = timer_add_periodic_task(test_task, &param, 1000000);
  serial_printf("id: %d\n", id);
  while (1) {
    timer_sleep_milis(1000);
    sio_printf("Main loop\n");
  }
  return 0;
}