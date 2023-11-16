/* ****************************************
* Programa de manejo de SPI
*

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


int main() {
  /* Deshabilitamos interrupciones */
  lock();

  /*Encendemos led*/
  _io_ports[M6812_DDRG] |= M6812B_PG7;
  _io_ports[M6812_PORTG] |= M6812B_PG7;


  serial_init();
  serial_print("\r\n" __FILE__ " ==========\r\n");

  /* Inicializamos el periferico SPI*/
  _io_ports[M6812_SP0BR] = 7; //Mínima velocidad

  /* Todos los pines intervinientes como salida para que funcionen bien*/
  _io_ports[M6812_DDRS] = (M6812B_DDS4 | M6812B_DDS5 | M6812B_DDS6
      |M6812B_DDS7);

  /* Comfiguració principal */
  _io_ports[M6812_SP0CR1] = ( 0
    //| SPIE  // habilitación de interrupciones
    | M6812B_SPE  // habilitación del sistema SPI
    //| M6812B_SWOM // selección del modo o-cableada
    //| M6812B_MSTR  // selección maestro (=1) ó esclavo (=0)
    //| M6812B_CPOL  // selección de polaridad del reloj
    //| M6812B_CPHA  // selección de la fase del reloj
    | M6812B_SSOE  // selección de la salida SS. 1 => sigue transmisión
    //| M6812B_LSBF  // transmisión del bit menos significativo primero
  );

  _io_ports[M6812_SP0CR2] = 0; // SPSWAI y SPC0 a 0

  unlock(); /* habilitamos interrupciones */

  serial_print("\r\nTerminada inicialización\r\n");

  while(1) {
    serial_print("\r\n\nElegir modo Master o Slave (M/S) ");
    char c;
    while( (c = (serial_recv() | 0x20))!= 's' && c != 'm');
    serial_send(c);  // devolvemos el aceptado
    if (c =='m') {
      serial_print("\r\n\nPasamos a modo Maestro");
      // Pasamos a modo maestro
      _io_ports[M6812_SP0CR1] |= M6812B_MSTR;

      serial_print("\r\nVelocidad (0 maxima a 7 minima)");
      while( (c = serial_recv())> '7' && c < '0');
      serial_send(c);  // devolvemos el aceptado

      // Fijamos la velocidad
      _io_ports[M6812_SP0BR] = c - '0';

      while (1) {
        uint16_t dato = 0x55;

        serial_print("\r\nIntroduce hexadecimal a enviar (> 0xFF sale maestro):");
        dato = serial_gethexword();
        if (dato > 0xFF)
          break;

        // Enviamos el dato
        _io_ports[M6812_SP0DR] = (uint8_t)(dato & 0xFF);

        // Esperamos a que termine la transmisión
        while(!(_io_ports[M6812_SP0SR] & M6812B_SPIF))
          serial_send('+');

        serial_print("\r\nTerminado envío");
        serial_print("\r\nRecibido dato: 0x");
        serial_printhexbyte(_io_ports[M6812_SP0DR]);
      }

    } else {
      serial_print("\r\n\nPasamos a modo Esclavo");
      // Pasamos al modo esclavo
      _io_ports[M6812_SP0CR1] &= ~M6812B_MSTR;

      while(1) {
        serial_print("\r\nEsperando recepción (pulsa para salir)");
        // Esperamos a que termine la transmisión o pulse tecla
        while(!(_io_ports[M6812_SP0SR] & M6812B_SPIF)
          && !serial_receive_pending());

        if (serial_receive_pending()) {
          serial_recv();  //quitamos caracter pulsado
          break;
        }

        serial_print("\r\nRecibido dato: 0x");
        serial_printhexbyte(_io_ports[M6812_SP0DR]);
      }
    }

  } // del while(1) bucle del programa principal
}

