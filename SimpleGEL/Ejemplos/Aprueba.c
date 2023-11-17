#include <sys/interrupts.h>
#include <sys/sio.h>
#include <timer.h>
#include <types.h>

void test_task(void *params) {
  uint8_t *param = (uint8_t *)params;
  serial_print("Task executed\n");
  serial_print("Param: ");
  serial_printdecbyte(*param);
  param++;
}

int main() {
  serial_init();
  timer_init(3);
  
  uint8_t param = 5;
  uint8_t id = timer_add_periodic_task(test_task, &param, 3000000);
  serial_print("id: ");
  serial_printdecbyte(id);
  
  while (1) {
	serial_print("\inicial ");
	serial_printdeclong(timer_milis());
    timer_sleep_milis(1000);
    serial_print(" Final ");
    serial_printdeclong(timer_milis());
    serial_print("\n");
    serial_print("\nContador: ");
    serial_printdecword((uint16_t)(timer_micros() >> 16));
  }
  return 0;
}
