/* 
 * File:   gpio_x.h
 * Author: M91541
 *
 * Created on 23. September 2024, 12:32
 */

#ifndef GPIO_X_H
#define	GPIO_X_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct {
        union {
            uint8_t TRIS_alt;
            struct {
                unsigned bit0                  :1;
                unsigned bit1                  :1;
                unsigned bit2                  :1;
                unsigned bit3                  :1;
                unsigned bit4                  :1;
                unsigned bit5                  :1;
                unsigned bit6                  :1;
                unsigned bit7                  :1;
            } TRIS_alt_bits;
        };
        union {
            uint8_t LAT_alt;
            struct {
                unsigned bit0                  :1;
                unsigned bit1                  :1;
                unsigned bit2                  :1;
                unsigned bit3                  :1;
                unsigned bit4                  :1;
                unsigned bit5                  :1;
                unsigned bit6                  :1;
                unsigned bit7                  :1;
            } LAT_alt_bits;;
        };
        union {
            uint8_t TRIS_default;
            struct {
                unsigned bit0                  :1;
                unsigned bit1                  :1;
                unsigned bit2                  :1;
                unsigned bit3                  :1;
                unsigned bit4                  :1;
                unsigned bit5                  :1;
                unsigned bit6                  :1;
                unsigned bit7                  :1;
            }  TRIS_default_bits;
        };
        union {
            uint8_t LAT_default;
            struct {
                unsigned bit0                  :1;
                unsigned bit1                  :1;
                unsigned bit2                  :1;
                unsigned bit3                  :1;
                unsigned bit4                  :1;
                unsigned bit5                  :1;
                unsigned bit6                  :1;
                unsigned bit7                  :1;
            } LAT_default_bits;
        };
        union {
            uint8_t is_PinAlternative;
            struct {
                unsigned bit0                  :1;
                unsigned bit1                  :1;
                unsigned bit2                  :1;
                unsigned bit3                  :1;
                unsigned bit4                  :1;
                unsigned bit5                  :1;
                unsigned bit6                  :1;
                unsigned bit7                  :1;
            } is_PinAlternative_bits;
        };
    } GPIOx_t;

    extern GPIOx_t PORTA_x;
    extern GPIOx_t PORTC_x;
    void gpiox_update_ports(void);
    void gpiox_load_all_from_ports(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_X_H */

