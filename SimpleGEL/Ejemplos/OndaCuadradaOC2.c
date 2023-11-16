/* ****************************************
* Salida onda cuadrada por el OC2
* Se especifica periondo en variable.

   Copyright (C) Alberto F. Hamilton Castro
   Dpto. de Ingeniería Informática de Sistemas
   Universidad de La Laguna

  Licencia: GPLv3

  *************************************** */

#include <types.h>
#include <sys/param.h>
#include <sys/interrupts.h>
#include <sys/sio.h>
#include <sys/locks.h>

#define  TCM_FACTOR (3)  /*La potencia de 2 a aplicar al factor*/
#define  TCM_FREQ (M6812_CPU_E_CLOCK / (1 << TCM_FACTOR))
/*Pasa de microsegundos a ticks*/
#define  USG_2_TICKS(us)  ((us) * (TCM_FREQ / 1000000L))
/*Pasa de milisegundos a ticks*/
#define  MSG_2_TICKS(ms)  ((ms) * (TCM_FREQ / 1000L))

/* Iniciamos periodo según microsegundos que queremos que dure */
/* se hace en tiempo de COMPILACION */
uint16_t Periodo = USG_2_TICKS(5000); /* Ticks del temporizador que dura el periodo */

uint16_t cuenta_irqs  = 0; /* Se incremente en cada interrupción */


int main () {

  /* Deshabilitamos interrupciones */
  lock ();

  /*Encendemos led*/
  _io_ports[M6812_DDRG] |= M6812B_PG7;
  _io_ports[M6812_PORTG] |= M6812B_PG7;


  serial_init();
  serial_print("\r\n" __FILE__ " ==========\r\n");

  serial_print("\r\n usg del periodo: ");
  serial_printdecword(Periodo/USG_2_TICKS(1));

  /*Inicialización del Temporizador*/
  _io_ports[M6812_TMSK2] = TCM_FACTOR;

  /* OC2 Invierte el pin en cada disparo */
  _io_ports[M6812_TCTL2] &= ~M6812B_OM1;
  _io_ports[M6812_TCTL2] |= M6812B_OL1;

  /*preparamos disparo*/
  _IO_PORTS_W(M6812_TC1) = _IO_PORTS_W(M6812_TCNT) + Periodo;


  /*configuramos canal 2 como comparador salida*/
  _io_ports[M6812_TIOS] |= M6812B_IOS1;


  _io_ports[M6812_TFLG1] = M6812B_IOS1; /*Bajamos el banderín de OC2 */
  _io_ports[M6812_TMSK1] |= M6812B_IOS1; /*habilitamos sus interrupciones*/
  _io_ports[M6812_TSCR] = M6812B_TEN; /*Habilitamos temporizador*/

  unlock(); /* habilitamos interrupciones */

  serial_print("\r\nTerminada inicialización");

  while(1) {
    char c;
    c = serial_recv();
    //Hacemos eco del caracter recibido
    serial_send(c);

    serial_print("\r\n\n Cuenta irqs: ");
    serial_printdecword(cuenta_irqs);

    if(c == '+') Periodo += 10;
    if(c == '-') Periodo -= 10;

    serial_print("\r\n Periodo: ");
    serial_printdecword(Periodo);
    serial_print("\tusg del periodo: ");
    serial_printdecword(Periodo / USG_2_TICKS(1));

    /*Invertimos el led*/
    _io_ports[M6812_PORTG] ^= M6812B_PG7;

  }
}


/* Manejador interrupciones del OC2  */
void __attribute__((interrupt)) vi_ioc1 (void) {
  _io_ports[M6812_TFLG1] = M6812B_IOS1; /*Bajamos el banderín de OC2 */

  /*preparamos siguiente disparo*/
  _IO_PORTS_W(M6812_TC1) = _IO_PORTS_W(M6812_TC1) + Periodo;
  cuenta_irqs++;

}

