#include <types.h>
#include <sys/param.h>
#include <sys/interrupts.h>
#include <sys/sio.h>
#include <sys/locks.h>
#include <timer.h>

int main() {
  serial_init();
  timer_init(3);
  uint32_t begin;
  uint32_t end;
  
  while (1) {
	begin = timer_milis();
	timer_sleep_milis(1000);
	end = timer_milis();
	serial_print("\n\nInicial Milis: ");
	serial_printdeclong(begin);
    serial_print(" ; Final Milis: ");
    serial_printdeclong(end);
    
    begin = timer_micros();
    timer_sleep_milis(1000);
    end = timer_micros();
    serial_print("\nInicial Micros: ");
	serial_printdeclong(begin);
    serial_print(" ; Final Micros: ");
    serial_printdeclong(end);
    serial_print("\nContador: ");
    serial_printdecword((uint16_t)(timer_micros() >> 16));
  }
  return 0;
}
