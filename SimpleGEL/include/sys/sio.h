/* sys/sio.h -- Utility methods to read/write the SIO
   Copyright 2000 Free Software Foundation, Inc.
   Written by Stephane Carrez (stcarrez@worldnet.fr)

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _SYS_SIO_H
#define _SYS_SIO_H

#include <types.h>
#include <sys/param.h>
#include <sys/ports.h>

#if M6812_STDOUT_PORT == 0
# define M6812_SCBD   M6812_SC0BDH
# define M6812_SCCR1  M6812_SC0CR1
# define M6812_SCCR2  M6812_SC0CR2
# define M6812_SCSR1  M6812_SC0SR1
# define M6812_SCDR   M6812_SC0DRL
#elif M6812_STDOUT_PORT == 1
# define M6812_SCBD   M6812_SC1BDH
# define M6812_SCCR1  M6812_SC1CR1
# define M6812_SCCR2  M6812_SC1CR2
# define M6812_SCSR1  M6812_SC1SR1
# define M6812_SCDR   M6812_SC1DRL
#else
# error "M6812_STDOUT_PORT must be defined to either 0 or 1"
#endif


/*! @defgroup sio Serial Input Output

  Conjunto de funciones que facilitan la interacción a través de la serial.
  Evintan el uso del `printf()` y `scanf()` definiendo funciones específicas
  para la lectura y escritura de datos de tipos enteros concretos.

 */
/*@{*/

/*! Initialize the SCI.

    This function configures the SCI to send at `M6811_DEF_BAUD` baud
    rate.  It must be called before any other serial operation unless
    the program is started from another one (such as a monitor or
    in bootstrap mode).

    @see serial_print, M6811_DEF_BAUD
*/
extern inline void serial_init (void)
{
  ((unsigned volatile short*) &_io_ports[M6812_SCBD])[0] = M6812_DEF_BAUD;

  /* Setup character format 1 start, 8-bits, 1 stop.  */
  _io_ports[M6812_SCCR1] = 0;

  /* Enable reciever and transmitter.  */
  _io_ports[M6812_SCCR2] = 0xc;
}


/*! Test for pending character on the reception.

    Return != 0 if there is something to read on the serial line.

    @return 1 if there is something to read on the serial line.
*/
extern inline uint8_t serial_receive_pending (void)
{
  return _io_ports[M6812_SCSR1] & M6812B_RDRF;
}


/*! Wait until the SIO has finished to send the character.

    This function waits for the transmission of the current character.
    While waiting, the COP is reset using \c cop_optional_reset.

    @see cop_optional_reset, serial_init, serial_send
*/
extern inline void serial_flush (void)
{
  while (!(_io_ports[M6812_SCSR1] & M6812B_TDRE))
    cop_optional_reset ();
}


/*! Send the character on the serial line.

    This function sends the character \a c on the serial line.
    Before sending, it calls \c serial_flush to make sure the
    transmitter is ready.  Once the function returns, the
    character is in the SCI queue and it may not be sent completely
    over the serial line.

    @param c character to send on the serial line.

    @see serial_init, serial_flush
*/
extern inline void serial_send (char c)
{
  serial_flush ();
  _io_ports[M6812_SCDR] = c;
  _io_ports[M6812_SCCR2] |= M6812B_TE;
}

/*! Wait for a character on the serial line and return it.

    This function waits for a character to be received by the SCI.
    While waiting, it calls \c cop_optional_reset to probe the COP
    regularly.

    @return the character received from the SCI.

    @see serial_init, cop_optional_reset
*/
extern inline uint8_t serial_recv (void)
{
  while (!(_io_ports[M6812_SCSR1] & M6812B_RDRF))
    cop_optional_reset ();

  return _io_ports[M6812_SCDR];
}

/** Write the string on the serial line.

    @param msg null terminated string to write.

    @see serial_init, serial_send
*/
extern void serial_print (const char *msg);

/** Wait for a string from serial line.

    @param buf buffer that will hold the string.

    @see serial_init, serial_recv
*/
extern void serial_getline (char *buf);

/** Espera 8 bits (0s o 1s) por la serial para formar un byte.
    Permite borrar y termina con \\n

    @return byte con el número binario recibido por la serial
*/
extern uint8_t serial_getbinbyte();

/** Saca por la serial la configuración de 8 bits (0s y 1s) del byte pasado

    @param bt byte a sacar
*/
extern void serial_printbinbyte(uint8_t bt);

/** Saca por la serial la configuración de 16 bits (0s y 1s) del doble byte pasado

    @param sa doble byte a sacar
*/
extern void serial_printbinword(uint16_t sa);

/** Espera 2 dígitos hexadecimales para formar byte. Permite borrar
    y termina con \\n

    @return devuelve el byte hexadecimal tecleado por el usuario.

*/
extern uint8_t serial_gethexbyte();

/** Espera 4 dígitos hexadecimales para formar doble byte. Permite borrar
    y termina con \\n

    @return devuelve el word hexadecimal tecleado por el usuario.

*/
extern uint16_t serial_gethexword();

/** Saca por la serial en hexadecimal el byte pasado

    @param bt byte a sacar

*/
extern void serial_printhexbyte(uint8_t bt);

/** Saca por la serial en hexadecimal el doble byte pasado

    @param sa word a sacar
*/
extern void serial_printhexword(uint16_t sa);

/** Espera por la serial digitos decimales para formar byte sin signo.
    Permite borrar y termina con \\n. Controla desboradmiento.

    @return byte con el número decimal recibido por la serial
*/
extern uint8_t serial_getdecbyte();

/** Espera por la serial digitos decimales para formar doble byte sin signo.
    Permite borrar y termina con \\n. Controla desboradmiento.

    @return devuelve el byte decimal tecleado por el usuario.

*/
extern uint16_t serial_getdecword();

/** Espera por la serial digitos decimales para formar un long
    (4 bytes) sin signo.
    Permite borrar y termina con \\n. Controla desboradmiento.

    @return devuelve número decimal tecleado por el usuario.

*/
extern uint32_t serial_getdeclong();


/** Saca por la serial en decimal doble byte pasado.

    @param sa word a mostar por la serial

*/
extern void serial_printdecword(uint16_t sa);

/** Saca por la serial en decimal el long (4 bytes) pasado.

    @param sa word a mostar por la serial

*/
void serial_printdeclong(uint32_t sa);


/** Saca por la serial en decimal byte pasado.

    @param ba byte a mostar por la serial
*/
extern void serial_printdecbyte(uint8_t ba);

/*@}*/

#endif /* _SYS_SIO_H */
