/**
 * TMR2 Generated Driver File
 *
 * @file tmr2.c
 * 
 * @ingroup  tmr2
 * 
 * @brief API implementations for the TMR2 module.
 *
 * @version TMR2 Driver Version 3.0.4
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

/**
  Section: Included Files
*/

#include <xc.h>
#include "../tmr2.h"

const struct TMR_INTERFACE Timer2 = {
    .Initialize = TMR2_Initialize,
    .Start = TMR2_Start,
    .Stop = TMR2_Stop,
    .PeriodCountSet = TMR2_PeriodCountSet,
    .TimeoutCallbackRegister = TMR2_OverflowCallbackRegister,
    .Tasks = TMR2_Tasks
};

static void (*TMR2_OverflowCallback)(void);
static void TMR2_DefaultOverflowCallback(void);

/**
  Section: TMR2 APIs
*/

void TMR2_Initialize(void){

    // Set TMR2 to the options selected in the User Interface
    // TCS HFINTOSC; 
    T2CLKCON = 0x3;
    // TMODE Software control; TCKSYNC Not Synchronized; TCKPOL Rising Edge; TPSYNC Not Synchronized; 
    T2HLT = 0x0;
    // TRSEL T2INPPS pin; 
    T2RST = 0x0;
    // PR 255; 
    T2PR = 0xFF;
    // TMR 0x0; 
    T2TMR = 0x0;

    // Set default overflow callback
    TMR2_OverflowCallbackRegister(TMR2_DefaultOverflowCallback);

    // Clearing IF flag.
    PIR2bits.TMR2IF = 0;
    // TCKPS 1:1; TMRON on; TOUTPS 1:1; 
    T2CON = 0x80;
}

void TMR2_ModeSet(TMR2_HLT_MODE mode)
{
   // Configure different types HLT mode
    T2HLTbits.T2MODE = mode;
}

void TMR2_ExtResetSourceSet(TMR2_HLT_EXT_RESET_SOURCE reset)
{
    //Configure different types of HLT external reset source
    T2RSTbits.T2RSEL = reset;
}

void TMR2_Start(void)
{
    // Start the Timer by writing to TMRxON bit
    T2CONbits.TMR2ON = 1;
}

void TMR2_Stop(void)
{
    // Stop the Timer by writing to TMRxON bit
    T2CONbits.TMR2ON = 0;
}

uint8_t TMR2_Read(void)
{
    uint8_t readVal;
    readVal = TMR2;
    return readVal;
}

void TMR2_Write(uint8_t timerVal)
{
    // Write to the Timer2 register
    TMR2 = timerVal;;
}

void TMR2_PeriodCountSet(size_t periodVal)
{
   PR2 = (uint8_t) periodVal;
}

void TMR2_OverflowCallbackRegister(void (* InterruptHandler)(void))
{
   TMR2_OverflowCallback = InterruptHandler;
}

static void TMR2_DefaultOverflowCallback(void)
{
    // add your TMR2 interrupt custom code
    // or set custom function using TMR2_OverflowCallbackRegister()
}

void TMR2_Tasks(void)
{
    if(PIR2bits.TMR2IF)
    {
        // Clearing IF flag.
        PIR2bits.TMR2IF = 0;
       TMR2_OverflowCallback();
    }
}

