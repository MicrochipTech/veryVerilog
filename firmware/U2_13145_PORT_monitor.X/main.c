 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.2
 *
 * @version Package Version: 3.1.2
*/

/*
© [2026] Microchip Technology Inc. and its subsidiaries.

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
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include "mcc_generated_files/system/system.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 0

#pragma config IDLOC0 = 1
#pragma config IDLOC1 = 0

/*
    Main application
*/

void print8pins(char* prefix, uint8_t portValue) {
    int i;
    for(i=0; i<8; ++i) {
        printf("%s%d == %c\n", prefix, i, (portValue & (1<<i)) ? '1' : '0');
    }
}

void compare8pins(char* prefixSame, char* prefixDifferent, uint8_t portValueNow, uint8_t portValueOther) {
    int i;
    bool same;
    for(i=0; i<8; ++i) {
        same = (portValueNow & (1<<i)) == (portValueOther & (1<<i));
        printf("%s(%c)\n", same ? prefixSame : prefixDifferent, (portValueNow & (1<<i)) ? '1' : '0');
    }
}

#define ___post(x, post) x##post
#define _post(x, post) ___post(x, post)  
#define _pre(pre, x) ___post(x, pre)   
#define ___prepost(pre, x, post) pre##x##post      
#define _prepost(pre, x, post) ___prepost(pre, x, post) 
#define measureAndPrint(chname) do {ADC_SampleCapacitorDischarge(); printf("\x1b[22G%s == %d\x1b[K\n", "AN"#chname, ADC_ChannelSelectAndConvert(_post(ADC_CHANNEL_AN, chname))); } while(0)

int main(void)
{
    uint8_t PApu, PBpu, PCpu;
    int counter = 31;
    SYSTEM_Initialize();
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts 
    // Use the following macros to: 

    // Enable the Global Interrupts 
    //INTERRUPT_GlobalInterruptEnable(); 

    // Disable the Global Interrupts 
    //INTERRUPT_GlobalInterruptDisable(); 

    // Enable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptEnable(); 

    // Disable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptDisable(); 
    __delay_ms(100);
    printf("\x1b[8;33;40t""\x1b[?25l\x1b[2J\x1b]2;miniFPGA\x07");
    while(1)
    {
        //Pull-ups enabled
        printf("\x1b[1;%d""m\x1b[H\x1b[2G**** PCB inputs test FW v"___mkstr(VERSION_MAJOR)"."___mkstr(VERSION_MINOR)" ****\n\n", counter);
        if (++counter > 37) {
            counter = 31;
        }
        WPUA = 0xFF;
        ANSELA = 0;
        PApu = PORTA;
        printf("\x1b[0mPORTA: \x1b[1;33m(noPullUp)\x1b[0m\n");
        print8pins(" RA", PApu);
        WPUB = 0xFF;
        ANSELB = 0;
        PBpu = PORTB;
        printf("\n\x1b[0mPORTB: \x1b[1;33m(noPullUp)\x1b[0m\n");
        print8pins(" RB", PBpu);
        WPUC = 0xFF;
        ANSELC = 0;
        PCpu = PORTC;
        printf("\n\x1b[0mPORTC: \x1b[1;33m(noPullUp)\x1b[0m\n");
        print8pins(" RC", PCpu);
        
        //Pull-ups disabled
        printf("\x1b[0m\x1b[H\n\n\n");
        WPUA = 0;
        ANSELA = 0;
        compare8pins("\x1b[12G\x1b[1;33m","\x1b[12G\x1b[1;31m", PORTA, PApu);
        WPUB = 0;
        ANSELB = 0;
        printf("\n\n");
        compare8pins("\x1b[12G\x1b[1;33m","\x1b[12G\x1b[1;31m", PORTB, PBpu);
        WPUC = 0;
        ANSELC = 0;
        printf("\n\n");
        compare8pins("\x1b[12G\x1b[1;33m","\x1b[12G\x1b[1;31m", PORTC, PCpu);
        
        //Analog no pull-up
        printf("\x1b[H\x1b[1;36m");
        ANSELA = 0x17;
        printf("\n\n\n");
        measureAndPrint(A0);
        measureAndPrint(A1);
        measureAndPrint(A2);
        printf("\n");
        measureAndPrint(A4);
        printf("\n");
        printf("\n");
        printf("\n");

        ANSELB = 0xF0;
        printf("\n\n");
        printf("\n");
        printf("\n");
        printf("\n");
        printf("\n");
        measureAndPrint(B4);
        measureAndPrint(B5);
        measureAndPrint(B6);
        measureAndPrint(B7);

        ANSELC = 0xFE;
        printf("\n\n\n");
        measureAndPrint(C1);
        measureAndPrint(C2);
        measureAndPrint(C3);
        measureAndPrint(C4);
        measureAndPrint(C5);
        measureAndPrint(C6);
        measureAndPrint(C7);
    }    
}