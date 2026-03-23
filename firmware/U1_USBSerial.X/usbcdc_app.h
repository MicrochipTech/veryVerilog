/* 
 * File:   usbcdc_app.h
 * Author: M91541
 *
 * Created on 28. Mai 2024, 16:59
 */

#ifndef USBCDC_APP_H
#define	USBCDC_APP_H

#ifdef	__cplusplus
extern "C" {
#endif

#define USBCDC_setHWbridgeEnable(x) do { USBCDC_serialBridgeMode = x; } while (0)
#define USBCDC_isHWbridgeEnabled() (USBCDC_serialBridgeMode)
    
    extern bool USBCDC_serialBridgeMode;
    
    void USBCDC_task(void);
    void USBCDC_setHwBaudRate(uint24_t requestedBaud);
    uint24_t USBCDC_getHwBaudRate(void);
    bool USBCDC_putc(char ch);

#ifdef	__cplusplus
}
#endif

#endif	/* USBCDC_APP_H */

