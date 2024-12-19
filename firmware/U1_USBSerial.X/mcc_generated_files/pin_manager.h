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
#define IO_LED_TX_TRIS                 PORTA_x.TRIS_default_bits.bit4 //TRISAbits.TRISA4
#define IO_LED_TX_LAT                  PORTA_x.LAT_default_bits.bit4 //LATAbits.LATA4
#define IO_LED_TX_PORT                 PORTAbits.RA4
#define IO_LED_TX_WPU                  WPUAbits.WPUA4
#define IO_LED_TX_ANS                  ANSELAbits.ANSA4
#define IO_LED_TX_SetHigh()            do { IO_LED_TX_LAT = HIGH; } while(0)
#define IO_LED_TX_SetLow()             do { IO_LED_TX_LAT = LOW; } while(0)
#define IO_LED_TX_Toggle()             do { IO_LED_TX_LAT = ~IO_LED_TX_LAT; } while(0)
#define IO_LED_TX_GetValue()           IO_LED_TX_PORT
#define IO_LED_TX_SetDigitalInput()    do { IO_LED_TX_TRIS = INPUT; } while(0)
#define IO_LED_TX_SetDigitalOutput()   do { IO_LED_TX_TRIS = OUTPUT; } while(0)
#define IO_LED_TX_SetPullup()          do { IO_LED_TX_WPU = PULL_UP_ENABLED; } while(0)
#define IO_LED_TX_ResetPullup()        do { IO_LED_TX_WPU = PULL_UP_DISABLED; } while(0)
#define IO_LED_TX_SetAnalogMode()      do { IO_LED_TX_ANS = ANALOG; } while(0)
#define IO_LED_TX_SetDigitalMode()     do { IO_LED_TX_ANS = DIGITAL; } while(0)

// get/set IO_LED_RX aliases
#define IO_LED_RX_TRIS                 PORTA_x.TRIS_default_bits.bit5 //TRISAbits.TRISA5
#define IO_LED_RX_LAT                  PORTA_x.LAT_default_bits.bit5 //LATAbits.LATA5
#define IO_LED_RX_PORT                 PORTAbits.RA5
#define IO_LED_RX_WPU                  WPUAbits.WPUA5
#define IO_LED_RX_SetHigh()            do { IO_LED_RX_LAT = HIGH; } while(0)
#define IO_LED_RX_SetLow()             do { IO_LED_RX_LAT = LOW; } while(0)
#define IO_LED_RX_Toggle()             do { IO_LED_RX_LAT = ~IO_LED_RX_LAT; } while(0)
#define IO_LED_RX_GetValue()           IO_LED_RX_PORT
#define IO_LED_RX_SetDigitalInput()    do { IO_LED_RX_TRIS = INPUT; } while(0)
#define IO_LED_RX_SetDigitalOutput()   do { IO_LED_RX_TRIS = OUTPUT; } while(0)
#define IO_LED_RX_SetPullup()          do { IO_LED_RX_WPU = PULL_UP_ENABLED; } while(0)
#define IO_LED_RX_ResetPullup()        do { IO_LED_RX_WPU = PULL_UP_DISABLED; } while(0)

// get/set IO_RC0 aliases
#define IO_RC0_TRIS                 TRISCbits.TRISC0
#define IO_RC0_LAT                  LATCbits.LATC0
#define IO_RC0_PORT                 PORTCbits.RC0
#define IO_RC0_ANS                  ANSELCbits.ANSC0
#define IO_RC0_SetHigh()            do { IO_RC0_LAT = HIGH; } while(0)
#define IO_RC0_SetLow()             do { IO_RC0_LAT = LOW; } while(0)
#define IO_RC0_Toggle()             do { IO_RC0_LAT = ~IO_RC0_LAT; } while(0)
#define IO_RC0_GetValue()           IO_RC0_PORT
#define IO_RC0_SetDigitalInput()    do { IO_RC0_TRIS = INPUT; } while(0)
#define IO_RC0_SetDigitalOutput()   do { IO_RC0_TRIS = OUTPUT; } while(0)
#define IO_RC0_SetAnalogMode()      do { IO_RC0_ANS = ANALOG; } while(0)
#define IO_RC0_SetDigitalMode()     do { IO_RC0_ANS = DIGITAL; } while(0)

// get/set IO_nMCLR aliases
#define IO_nMCLR_TRIS                 TRISCbits.TRISC1
#define IO_nMCLR_LAT                  LATCbits.LATC1
#define IO_nMCLR_PORT                 PORTCbits.RC1
#define IO_nMCLR_ANS                  ANSELCbits.ANSC1
#define IO_nMCLR_SetHigh()            do { IO_nMCLR_LAT = HIGH; } while(0)
#define IO_nMCLR_SetLow()             do { IO_nMCLR_LAT = LOW; } while(0)
#define IO_nMCLR_Toggle()             do { IO_nMCLR_LAT = ~IO_nMCLR_LAT; } while(0)
#define IO_nMCLR_GetValue()           IO_nMCLR_PORT
#define IO_nMCLR_SetDigitalInput()    do { IO_nMCLR_TRIS = INPUT; } while(0)
#define IO_nMCLR_SetDigitalOutput()   do { IO_nMCLR_TRIS = OUTPUT; } while(0)
#define IO_nMCLR_SetAnalogMode()      do { IO_nMCLR_ANS = ANALOG; } while(0)
#define IO_nMCLR_SetDigitalMode()     do { IO_nMCLR_ANS = DIGITAL; } while(0)

// get/set IO_ICSP_CK aliases
#define IO_ICSP_CK_TRIS                 TRISCbits.TRISC2
#define IO_ICSP_CK_LAT                  LATCbits.LATC2
#define IO_ICSP_CK_PORT                 PORTCbits.RC2
#define IO_ICSP_CK_ANS                  ANSELCbits.ANSC2
#define IO_ICSP_CK_SetHigh()            do { IO_ICSP_CK_LAT = HIGH; } while(0)
#define IO_ICSP_CK_SetLow()             do { IO_ICSP_CK_LAT = LOW; } while(0)
#define IO_ICSP_CK_Toggle()             do { IO_ICSP_CK_LAT = ~IO_ICSP_CK_LAT; } while(0)
#define IO_ICSP_CK_GetValue()           IO_ICSP_CK_PORT
#define IO_ICSP_CK_SetDigitalInput()    do { IO_ICSP_CK_TRIS = INPUT; } while(0)
#define IO_ICSP_CK_SetDigitalOutput()   do { IO_ICSP_CK_TRIS = OUTPUT; } while(0)
#define IO_ICSP_CK_SetAnalogMode()      do { IO_ICSP_CK_ANS = ANALOG; } while(0)
#define IO_ICSP_CK_SetDigitalMode()     do { IO_ICSP_CK_ANS = DIGITAL; } while(0)

// get/set IO_ICSP_DT aliases
#define IO_ICSP_DT_TRIS                 TRISCbits.TRISC3
#define IO_ICSP_DT_LAT                  LATCbits.LATC3
#define IO_ICSP_DT_PORT                 PORTCbits.RC3
#define IO_ICSP_DT_ANS                  ANSELCbits.ANSC3
#define IO_ICSP_DT_SetHigh()            do { IO_ICSP_DT_LAT = HIGH; } while(0)
#define IO_ICSP_DT_SetLow()             do { IO_ICSP_DT_LAT = LOW; } while(0)
#define IO_ICSP_DT_Toggle()             do { IO_ICSP_DT_LAT = ~IO_ICSP_DT_LAT; } while(0)
#define IO_ICSP_DT_GetValue()           IO_ICSP_DT_PORT
#define IO_ICSP_DT_SetDigitalInput()    do { IO_ICSP_DT_TRIS = INPUT; } while(0)
#define IO_ICSP_DT_SetDigitalOutput()   do { IO_ICSP_DT_TRIS = OUTPUT; } while(0)
#define IO_ICSP_DT_SetAnalogMode()      do { IO_ICSP_DT_ANS = ANALOG; } while(0)
#define IO_ICSP_DT_SetDigitalMode()     do { IO_ICSP_DT_ANS = DIGITAL; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()            do { LATCbits.LATC4 = HIGH; } while(0)
#define RC4_SetLow()             do { LATCbits.LATC4 = LOW; } while(0)
#define RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()              PORTCbits.RC4
#define RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = INPUT; } while(0)
#define RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = OUTPUT; } while(0)

// get/set RC5 procedures
#define RC5_SetHigh()            do { LATCbits.LATC5 = HIGH; } while(0)
#define RC5_SetLow()             do { LATCbits.LATC5 = LOW; } while(0)
#define RC5_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define RC5_GetValue()              PORTCbits.RC5
#define RC5_SetDigitalInput()    do { TRISCbits.TRISC5 = INPUT; } while(0)
#define RC5_SetDigitalOutput()   do { TRISCbits.TRISC5 = OUTPUT; } while(0)

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