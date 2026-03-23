/**
 * Configuration Bits Generated Driver Source File
 * 
 * @file config_bits.c
 * 
 * @ingroup config_bitsdriver
 * 
 * @brief This file contains the API Implementation for the Device Configuration Bits driver.
 *
 * @version Driver Version 2.0.2
 *
 * @version Package Version 4.1.4
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

// Configuration bits: selected in the GUI

//CONFIG1
#pragma config FEXTOSC = OFF     // External Oscillator mode selection bits->Oscillator not enabled
#pragma config RSTOSC = HFINTOSC_32MHz     // Power-up default value for COSC bits->HFINTOSC (32 MHz)
#pragma config CLKOUTEN = OFF     // Clock Out Enable bit->CLKOUT function is disabled; i/o or oscillator function on OSC2
#pragma config CSWEN = ON     // Clock Switch Enable bit->Writing to NOSC and NDIV is allowed
#pragma config VDDAR = HI     // VDD Range Analog Calibration Selection bit->Internal analog systems are calibrated for operation between VDD = 2.3 - 5.5V
#pragma config FCMEN = OFF     // Fail-Safe Clock Monitor Enable bit->Fail-Safe Clock Monitor disabled

//CONFIG2
#pragma config MCLRE = EXTMCLR     // Master Clear Enable bit->If LVP = 0, MCLR pin is MCLR; If LVP = 1, RA3 pin function is MCLR
#pragma config PWRTS = PWRT_64     // Power-up Timer Selection bits->PWRT set at 64 ms
#pragma config DACAUTOEN = OFF     // DAC Buffer Automatic Range Select Enable bit->DAC Buffer reference range is determined by the REFRNG bit
#pragma config LPBOREN = OFF     // Low-Power BOR enable bit->ULPBOR disabled
#pragma config BOREN = ON     // Brown-out reset enable bits->Brown-out Reset enabled, SBOREN bit is ignored
#pragma config BORV = HI     // Brown-out Reset Voltage Selection->Brown-out Reset Voltage (VBOR) is set to 2.85V
#pragma config PPS1WAY = OFF     // Peripheral Pin Select one-way control->The PPSLOCKED bit can be set and cleared as needed (unlocking sequence is required)
#pragma config STVREN = ON     // Stack Overflow/Underflow Reset Enable bit->Stack Overflow or Underflow will cause a reset

//CONFIG3
#pragma config WDTCPS = WDTCPS_31     // WDT Period Select bits->Divider ratio 1:65536; software control of WDTPS
#pragma config WDTE = OFF     // WDT operating mode->WDT Disabled, SEN is ignored
#pragma config WDTCWS = WDTCWS_7     // WDT Window Select bits->window always open (100%); software control; keyed access not required
#pragma config WDTCCS = SC     // WDT input clock selector->Software Control

//CONFIG4
#pragma config BBSIZE = BB512     // Boot Block Size Selection bits->512 words boot block size
#pragma config BBEN = OFF     // Boot Block Enable bit->Boot Block disabled
#pragma config SAFEN = OFF     // Storage Area Flash (SAF) Enable bit->SAF disabled
#pragma config WRTAPP = OFF     // Application Block Write Protection bit->Application Block is NOT write-protected
#pragma config WRTB = OFF     // Boot Block Write Protection bit->Boot Block is NOT write-protected
#pragma config WRTC = OFF     // Configuration Register Write Protection bit->Configuration Register is NOT write-protected
#pragma config WRTSAF = OFF     // Storage Area Flash (SAF) Write Protection bit->SAF is NOT write-protected
#pragma config LVP = ON     // Low Voltage Programming Enable bit->Low Voltage programming enabled. MCLR/Vpp pin function is MCLR. MCLRE Configuration bit is ignored

//CONFIG5
#pragma config CP = OFF     // UserNVM Program memory code protection bit->Program Flash Memory code protection is disabled

/**
 End of File
*/