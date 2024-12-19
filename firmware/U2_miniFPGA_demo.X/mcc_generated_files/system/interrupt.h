/**
 * Interrupt Manager Generated Driver API Header File
 * 
 * @file interrupt.h
 * 
 * @defgroup interrupt INTERRUPT
 * 
 * @brief This file contains API prototypes and other data types for the Interrupt Manager driver.
 *
 * @version Interrupt Manager Driver Version 2.0.6
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

#ifndef INTERRUPT_H
#define INTERRUPT_H


/**
 * @ingroup interrupt
 * @brief Enables global interrupts.
 * @param None.
 * @return None.
 */
#define INTERRUPT_GlobalInterruptEnable() (INTCONbits.GIE = 1)

/**
 * @ingroup interrupt
 * @brief Disables global interrupts.
 * @param None.
 * @return None.
 */
#define INTERRUPT_GlobalInterruptDisable() (INTCONbits.GIE = 0)

/**
 * @ingroup interrupt
 * @brief Returns the Global Interrupt Enable bit status.
 * @param None.
 * @retval 0 - Global interrupt disabled.
 * @retval 1 - Global interrupt enabled.
 */
#define INTERRUPT_GlobalInterruptStatus() (INTCONbits.GIE)

/**
 * @ingroup interrupt
 * @brief Enables peripheral interrupts.
 * @param None.
 * @return None.
 */
#define INTERRUPT_PeripheralInterruptEnable() (INTCONbits.PEIE = 1)

/**
 * @ingroup interrupt
 * @brief Disables peripheral interrupts.
 * @param None.
 * @return None.
 */
#define INTERRUPT_PeripheralInterruptDisable() (INTCONbits.PEIE = 0)

/**
 * @ingroup interrupt
 * @brief Initializes peripheral interrupt priorities, enables or disables priority vectors and initializes the external interrupt.
 * @param None.
 * @return None.
 */
void INTERRUPT_Initialize (void);


/**
 * @ingroup interrupt
 * @brief Clears the Interrupt flag for the external interrupt, INT.
 * @param None.
 * @return None.
 */
#define EXT_INT_InterruptFlagClear()       (PIR0bits.INTF = 0)

/**
 * @ingroup interrupt
 * @brief Clears the interrupt enable for the external interrupt, INT. This way, the external interrupts on this pin will not be serviced by the interrupt handler.
 * @param None.
 * @return None.
 */
#define EXT_INT_InterruptDisable()     (PIE0bits.INTE = 0)

/**
 * @ingroup interrupt
 * @brief Sets the interrupt enable for the external interrupt, INT. This way, the external interrupts on this pin will be serviced by the interrupt handler.
 * @param None.
 * @return None.
 */
#define EXT_INT_InterruptEnable()       (PIE0bits.INTE = 1)

/**
 * @ingroup interrupt
 * @brief Sets the edge detect of the external interrupt to positive edge. This way, the Interrupt flag will be set when the external interrupt pin level transitions from low to high.
 * @param None.
 * @return None.
 */
#define EXT_INT_risingEdgeSet()          (INTCONbits.INTEDG = 1)

/**
 * @ingroup interrupt
 * @brief Sets the edge detect of the external interrupt to negative edge. This way, the Interrupt flag will be set when the external interrupt pin level transitions from high to low.
 * @param None.
 * @return None.
 */
#define EXT_INT_fallingEdgeSet()          (INTCONbits.INTEDG = 0)

/**
   Section: External Interrupt Handlers
 */

/**
 * @ingroup interrupt
 * @brief Executes the ISR whenever the signal on the INT pin transitions to the preconfigured state.
 * @pre Interrupt Manager is initialized.
 * @param None.
 * @return None.
 */
void INT_ISR(void);

/**
 * @ingroup interrupt
 * @brief Allows for a specific callback function to be called in the INT ISR and for a nonspecific interrupt handler to be called at run time.
 * @pre Interrupt Manager is initialized.
 * @param None.
 * @return None.
 */
void INT_CallBack(void);

/**
 * @ingroup interrupt
 * @brief Allows selecting an interrupt handler for EXT_INT - INT at application run time.
 * @pre Interrupt Manager is initialized.
 * @param (*InterruptHandler)(void) - InterruptHandler function pointer.
 * @return None.
 */
void INT_SetInterruptHandler(void (* InterruptHandler)(void));

/**
 * @ingroup interrupt
 * @brief Dynamic interrupt handler to be called every time the INT ISR is executed. It allows any function to be registered at run time.
 * @pre Interrupt Manager is initialized.
 * @param None.
 * @return None.
 */
extern void (*INT_InterruptHandler)(void);

/**
 * @ingroup interrupt
 * @brief Default interrupt handler to be called every time the INT ISR is executed. It allows any function to be registered at run time.
 * @pre Interrupt Manager is initialized.
 * @param None.
 * @return None.
 */
void INT_DefaultInterruptHandler(void);

#endif  // INTERRUPT_H
/**
 End of File
*/