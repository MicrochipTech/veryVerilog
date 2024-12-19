/**
 * Generated Driver File
 * 
 * @file pins.c
 * 
 * @ingroup  pinsdriver
 * 
 * @brief This is generated driver implementation for pins. 
 *        This file provides implementations for pin APIs for all pins selected in the GUI.
 *
 * @version Driver Version 3.0.0
*/

/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include "../pins.h"


void PIN_MANAGER_Initialize(void)
{
   /**
    LATx registers
    */
    LATA = 0x0;
    LATB = 0x0;
    LATC = 0x0;

    /**
    TRISx registers
    */
    TRISA = 0x37;
    TRISB = 0xE0;
    TRISC = 0xF8;

    /**
    ANSELx registers
    */
    ANSELA = 0x7;
    ANSELB = 0xE0;
    ANSELC = 0xC0;

    /**
    WPUx registers
    */
    WPUA = 0x10;
    WPUB = 0x0;
    WPUC = 0x38;
  
    /**
    ODx registers
    */
   
    ODCONA = 0x0;
    ODCONB = 0x0;
    ODCONC = 0x0;
    /**
    SLRCONx registers
    */
    SLRCONA = 0x27;
    SLRCONB = 0xE0;
    SLRCONC = 0xC1;
    /**
    INLVLx registers
    */
    INLVLA = 0xF;
    INLVLB = 0xE0;
    INLVLC = 0xC0;

    /**
    PPS registers
    */
    CLBIN0PPS = 0x14; //RC4->CLB1:CLBIN0;
    CLBIN1PPS = 0x15; //RC5->CLB1:CLBIN1;
    CLBIN2PPS = 0x4; //RA4->CLB1:CLBIN2;
    CLBIN3PPS = 0x13; //RC3->CLB1:CLBIN3;
    RX1PPS = 0x5; //RA5->EUSART1:RX1;
    RC1PPS = 0x24;  //RC1->CLB1:CLBPPSOUT0;
    RC2PPS = 0x25;  //RC2->CLB1:CLBPPSOUT1;
    RB4PPS = 0x26;  //RB4->CLB1:CLBPPSOUT2;
    RC0PPS = 0x13;  //RC0->EUSART1:TX1;

    /**
    APFCON registers
    */

   /**
    IOCx registers 
    */
    IOCAP = 0x0;
    IOCAN = 0x0;
    IOCAF = 0x0;
    IOCBP = 0x0;
    IOCBN = 0x0;
    IOCBF = 0x0;
    IOCCP = 0x0;
    IOCCN = 0x0;
    IOCCF = 0x0;


}
  
void PIN_MANAGER_IOC(void)
{
}
/**
 End of File
*/