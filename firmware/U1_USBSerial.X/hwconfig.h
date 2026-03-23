/* 
 * File:   hwconfig.h
 * Author: M91541
 *
 * Created on May 27, 2024, 3:19 PM
 */

#ifndef HWCONFIG_H
#define	HWCONFIG_H

#include "mcc_generated_files/device_config.h"
#include "mcc_generated_files/pin_manager.h"
#include "gpio_x.h"


#ifdef	__cplusplus
extern "C" {
#endif

#define PIN_nMCLR_High() do {IO_nMCLR_TRIS = INPUT;} while(0)
#define PIN_nMCLR_Low() do {IO_nMCLR_LAT = LOW; IO_nMCLR_TRIS = OUTPUT;} while(0)

#define PIN_LedTX_On() do {IO_LED_TX_LAT = LOW; IO_LED_TX_TRIS = OUTPUT; } while (0)
#define PIN_LedTX_Off() do {IO_LED_TX_TRIS = INPUT;} while (0)

#define PIN_LedRX_On() do {IO_LED_RX_LAT = LOW; IO_LED_RX_TRIS = OUTPUT; } while (0)
#define PIN_LedRX_Off() do {IO_LED_RX_TRIS = INPUT;} while (0)


#ifdef	__cplusplus
}
#endif

#endif	/* HWCONFIG_H */

