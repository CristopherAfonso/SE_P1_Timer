/*! \file
  Fichero de inclusión que define la tabla de vectores.
  Se definen todos los vectores a _start salvo en aquellos para los
  que exista una etiqueta vi_nombre, en cuyo caso se usará esa etiqueta.

  Copyright (C) Alberto F. Hamilton Castro
  Dpto. de Ingeniería Informática de Sistemas
  Universidad de La Laguna

  Licencia: GPLv3

*/

#ifndef _SYS_INTERRUPTS_H
#define _SYS_INTERRUPTS_H

/*

Todos los vectores de interrupción quedan apuntando
  a la etiqueta 'vi_default', salvo el vector de reset de
  encendido que queda apuntando a la etiqueta '_start'.
  A su vez, si no se define, 'vi_default'  apuntará a '_start'.


  Para modificar algún vector de interrupción es necesario
    definir una función con el nombre correspondiente.
  El nombre de las funciones es el que aparece en la siguiente lista:

  * vi_osc   Interrupción del sistema oscilador
  * vi_cantx   Transmisión en el subsistema CAN
  * vi_canrx   Recepción en el subsistema CAN
  * vi_canerr   Errores en el subsistema CAN
  * vi_pabov   Desbordamiento del acumulador de pulsos B
  * vi_cmuv   Cuenta final del contador de módulo
  * vi_kwgh   Activación de los puestos G ó H
  * vi_canwu   Activación del subsistema CAN
  * vi_atd   Conversores analógicos
  * vi_sci1   Interrupción del sistema SCI 1
  * vi_sci0   Interrupción del sistema SCI 0
  * vi_spi   Interrupción del sistema SPI
  * vi_pai   Flanco del acumulador de pulsos
  * vi_paov   Desbordamiento del acumulador de pulsosA
  * vi_tov   Desbordamiento del temporizador
  * vi_ioc7   Canal 7 del temporizador
  * vi_ioc6   Canal 6 del temporizador
  * vi_ioc5   Canal 5 del temporizador
  * vi_ioc4   Canal 4 del temporizador
  * vi_ioc3   Canal 3 del temporizador
  * vi_ioc2   Canal 2 del temporizador
  * vi_ioc1   Canal 1 del temporizador
  * vi_ioc0   Canal 0 del temporizador
  * vi_rti   Interrupción de tiempo real
  * vi_irq   Interrupción línea IRQ
  * vi_xirq   Interrupción línea XIRQ
  * vi_swi   Instrucción SWI
  * vi_trap   Instrucción Ilegal
  * vi_copreset   Reset por temporizador COP
  * vi_clkreset   Reset por fallo del reloj
  * vi_poreset   Reset externo o reset de encendido

  Las funciones deben tener el prototipo:
    void  __attribute__((interrupt))    vi_XXX(void)

  Es el linker, según la definición en config/memory.x el que se encarga
  de rellenar la tablas de vectores de interrupción.

*/

#endif /* _SYS_INTERRUPTS_H */
