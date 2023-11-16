/*
 Distribucion de memoria para Adpat912DT60
 con programa en memoria RAM externa utilizando
el CargaRam 2.7 que permite activar ram externa pero
sólo quita bloque bajo de la flash => hay memoria RAM de
 0x1000 a 0x7fff

Los seudovectores están en 0x07c2

Los registros de control están en la dirección 0x0800

Dejamos pila bajo los vectores.

*/

vectors_addr = 0x07c2;
_stack = vectors_addr - 1;
_io_ports = 0x0800;

/*No parece posible usar variables en las siguientes expresiones,
pero se pueden poner las expersiones con constantes
*/

MEMORY
{
  page0 (rwx) : ORIGIN = 0x0, LENGTH = 256
  data        : ORIGIN = 0x1000, LENGTH = 0x3000
  text  (rx)  : ORIGIN = 0x4000, LENGTH = 0x7fff - 0x4000
  eeprom      : ORIGIN = 0x0C00, LENGTH = 1024
}

PROVIDE(vi_default = _start);

PROVIDE(vi_osc = vi_default);
PROVIDE(vi_cantx = vi_default);
PROVIDE(vi_canrx = vi_default);
PROVIDE(vi_canerr = vi_default);
PROVIDE(vi_pabov = vi_default);
PROVIDE(vi_cmuv = vi_default);
PROVIDE(vi_kwgh = vi_default);
PROVIDE(vi_canwu = vi_default);
PROVIDE(vi_atd = vi_default);
PROVIDE(vi_sci1 = vi_default);
PROVIDE(vi_sci0 = vi_default);
PROVIDE(vi_spi = vi_default);
PROVIDE(vi_pai = vi_default);
PROVIDE(vi_paov = vi_default);
PROVIDE(vi_tov = vi_default);
PROVIDE(vi_ioc7 = vi_default);
PROVIDE(vi_ioc6 = vi_default);
PROVIDE(vi_ioc5 = vi_default);
PROVIDE(vi_ioc4 = vi_default);
PROVIDE(vi_ioc3 = vi_default);
PROVIDE(vi_ioc2 = vi_default);
PROVIDE(vi_ioc1 = vi_default);
PROVIDE(vi_ioc0 = vi_default);
PROVIDE(vi_rti = vi_default);
PROVIDE(vi_irq = vi_default);
PROVIDE(vi_xirq = vi_default);
PROVIDE(vi_swi = vi_default);
PROVIDE(vi_trap = vi_default);
PROVIDE(vi_copreset = vi_default);
PROVIDE(vi_clkreset = vi_default);

/*
 me baso en http://www.scoberlin.de/content/media/http/informatik/gcc_docs/ld_3.html
*/

SECTIONS {
  .vectors vectors_addr : {
    SHORT(vi_osc);
    SHORT(vi_cantx);
    SHORT(vi_canrx);
    SHORT(vi_canerr);
    SHORT(vi_pabov);
    SHORT(vi_cmuv);
    SHORT(vi_kwgh);
    SHORT(vi_canwu);
    SHORT(vi_atd);
    SHORT(vi_sci1);
    SHORT(vi_sci0);
    SHORT(vi_spi);
    SHORT(vi_pai);
    SHORT(vi_paov);
    SHORT(vi_tov);
    SHORT(vi_ioc7);
    SHORT(vi_ioc6);
    SHORT(vi_ioc5);
    SHORT(vi_ioc4);
    SHORT(vi_ioc3);
    SHORT(vi_ioc2);
    SHORT(vi_ioc1);
    SHORT(vi_ioc0);
    SHORT(vi_rti);
    SHORT(vi_irq);
    SHORT(vi_xirq);
    SHORT(vi_swi);
    SHORT(vi_trap);
    SHORT(vi_copreset);
    SHORT(vi_clkreset);
    SHORT(_start);
  }
}
