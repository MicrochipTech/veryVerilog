/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.6
        Device            :  PIC16F1455
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.30 and above
        MPLAB 	          :  MPLAB X 5.40	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set RA0 procedures
#define RA0_GetValue()              PORTAbits.RA0

// get/set RA1 procedures
#define RA1_GetValue()              PORTAbits.RA1

// get/set IO_LED_TX aliases
#define IO_LED_TX_TRIS                 TRISAbits.TRISA4
#define IO_LED_TX_LAT                  LATAbits.LATA4
#define IO_LED_TX_PORT                 PORTAbits.RA4
#define IO_LED_TX_WPU                  WPUAbits.WPUA4
#define IO_LED_TX_ANS                  ANSELAbits.ANSA4
#define IO_LED_TX_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define IO_LED_TX_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define IO_LED_TX_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define IO_LED_TX_GetValue()           PORTAbits.RA4
#define IO_LED_TX_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define IO_LED_TX_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define IO_LED_TX_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define IO_LED_TX_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define IO_LED_TX_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define IO_LED_TX_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set IO_LED_RX aliases
#define IO_LED_RX_TRIS                 TRISAbits.TRISA5
#define IO_LED_RX_LAT                  LATAbits.LATA5
#define IO_LED_RX_PORT                 PORTAbits.RA5
#define IO_LED_RX_WPU                  WPUAbits.WPUA5
#define IO_LED_RX_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define IO_LED_RX_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define IO_LED_RX_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define IO_LED_RX_GetValue()           PORTAbits.RA5
#define IO_LED_RX_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define IO_LED_RX_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define IO_LED_RX_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define IO_LED_RX_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)

// get/set IO_RC0 aliases
#define IO_RC0_TRIS                 TRISCbits.TRISC0
#define IO_RC0_LAT                  LATCbits.LATC0
#define IO_RC0_PORT                 PORTCbits.RC0
#define IO_RC0_ANS                  ANSELCbits.ANSC0
#define IO_RC0_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define IO_RC0_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define IO_RC0_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define IO_RC0_GetValue()           PORTCbits.RC0
#define IO_RC0_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define IO_RC0_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define IO_RC0_SetAnalogMode()      do { ANSELCbits.ANSC0 = 1; } while(0)
#define IO_RC0_SetDigitalMode()     do { ANSELCbits.ANSC0 = 0; } while(0)

// get/set IO_nMCLR aliases
#define IO_nMCLR_TRIS                 TRISCbits.TRISC1
#define IO_nMCLR_LAT                  LATCbits.LATC1
#define IO_nMCLR_PORT                 PORTCbits.RC1
#define IO_nMCLR_ANS                  ANSELCbits.ANSC1
#define IO_nMCLR_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define IO_nMCLR_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define IO_nMCLR_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define IO_nMCLR_GetValue()           PORTCbits.RC1
#define IO_nMCLR_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define IO_nMCLR_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define IO_nMCLR_SetAnalogMode()      do { ANSELCbits.ANSC1 = 1; } while(0)
#define IO_nMCLR_SetDigitalMode()     do { ANSELCbits.ANSC1 = 0; } while(0)

// get/set IO_ICSP_CK aliases
#define IO_ICSP_CK_TRIS                 TRISCbits.TRISC2
#define IO_ICSP_CK_LAT                  LATCbits.LATC2
#define IO_ICSP_CK_PORT                 PORTCbits.RC2
#define IO_ICSP_CK_ANS                  ANSELCbits.ANSC2
#define IO_ICSP_CK_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define IO_ICSP_CK_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define IO_ICSP_CK_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define IO_ICSP_CK_GetValue()           PORTCbits.RC2
#define IO_ICSP_CK_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define IO_ICSP_CK_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define IO_ICSP_CK_SetAnalogMode()      do { ANSELCbits.ANSC2 = 1; } while(0)
#define IO_ICSP_CK_SetDigitalMode()     do { ANSELCbits.ANSC2 = 0; } while(0)

// get/set IO_ICSP_DT aliases
#define IO_ICSP_DT_TRIS                 TRISCbits.TRISC3
#define IO_ICSP_DT_LAT                  LATCbits.LATC3
#define IO_ICSP_DT_PORT                 PORTCbits.RC3
#define IO_ICSP_DT_ANS                  ANSELCbits.ANSC3
#define IO_ICSP_DT_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define IO_ICSP_DT_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define IO_ICSP_DT_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define IO_ICSP_DT_GetValue()           PORTCbits.RC3
#define IO_ICSP_DT_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define IO_ICSP_DT_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define IO_ICSP_DT_SetAnalogMode()      do { ANSELCbits.ANSC3 = 1; } while(0)
#define IO_ICSP_DT_SetDigitalMode()     do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()              PORTCbits.RC4
#define RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)

// get/set RC5 procedures
#define RC5_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define RC5_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define RC5_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define RC5_GetValue()              PORTCbits.RC5
#define RC5_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define RC5_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/