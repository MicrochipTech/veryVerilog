#include <xc.h>
#include "gpio_x.h"

GPIOx_t PORTA_x;
GPIOx_t PORTC_x;

void gpiox_load_all_from_ports(void) {
    PORTA_x.LAT_alt = PORTA_x.LAT_default = LATA;
    PORTA_x.TRIS_alt = PORTA_x.TRIS_default = TRISA;
    PORTC_x.LAT_alt = PORTC_x.LAT_default = LATC;
    PORTC_x.TRIS_alt = PORTC_x.TRIS_default = TRISC;
}

void gpiox_update_ports(void) {
    // https://graphics.stanford.edu/~seander/bithacks.html#MaskedMerge
    LATA = PORTA_x.LAT_default ^ ((PORTA_x.LAT_default ^ PORTA_x.LAT_alt) & PORTA_x.is_PinAlternative); 
    TRISA = PORTA_x.TRIS_default ^ ((PORTA_x.TRIS_default ^ PORTA_x.TRIS_alt) & PORTA_x.is_PinAlternative); 
    LATC = PORTC_x.LAT_default ^ ((PORTC_x.LAT_default ^ PORTC_x.LAT_alt) & PORTC_x.is_PinAlternative); 
    TRISC = PORTC_x.TRIS_default ^ ((PORTC_x.TRIS_default ^ PORTC_x.TRIS_alt) & PORTC_x.is_PinAlternative); 
}
