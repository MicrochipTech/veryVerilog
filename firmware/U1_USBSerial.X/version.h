/* 
 * File:   version.h
 * Author: M91541
 *
 * Created on 17. Juli 2024, 16:34
 */

#ifndef VERSION_H
#define	VERSION_H

#ifdef	__cplusplus
extern "C" {
#endif

#define USB_FW_VERSION      (0x0120)                 // Firmware release number in BCD format
#pragma config IDLOC0 = 0x01
#pragma config IDLOC1 = 0x20

#ifdef	__cplusplus
}
#endif

#endif	/* VERSION_H */

