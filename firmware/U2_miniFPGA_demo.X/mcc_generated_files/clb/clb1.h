
/**
 * CLB1 Generated Driver API Header File
 * 
 * @file clb1.h
 *
 * @defgroup  clb1 CLB1
 *
 * @brief This file contains the prototypes and other data types for the CLB1 driver
 *
 * @version CLB1 Driver Version 1.1.0
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

#ifndef CLB1_H
#define CLB1_H

#define BITSTREAM_SIZE 102UL

/**
  Section: Included Files
*/

#include <stdbool.h>
#include <stdint.h>

/**
  Section: CLB1 APIs
*/

/**
 * @ingroup clb1
 * @brief  Initializes the clb1 and it must be called before any other routine is called.
 * @param None.
 * @return None.
 */ 
void CLB1_Initialize(void);

/**
 * @ingroup clb1
 * @brief Enables CLB1.
 * @pre CLB1_Initialize() is already called.
 * @param None.
 * @return None.
 */
void CLB1_Enable(void);

/**
 * @ingroup clb1
 * @brief Disables CLB1.
 * @param None.
 * @return None.
 */
void CLB1_Disable(void);

/**
 * @ingroup clb1
 * @brief Configures CLB1.
 * @param uint16_t start_address.
 * @return None.
 */
void CLB1_Configure(uint16_t start_address);

/**
 * @ingroup clb1
 * @brief Writes the data into the CLB Data registers.
 * @param uint8_t data - CLB input data.
 * @return None.
 */
void CLB1_SWIN_Write8(uint8_t data);

/**
 * @ingroup clb1
 * @brief Writes the data into the CLB Data registers.
 * @param uint16_t data - CLB input data.
 * @return None.
 */
void CLB1_SWIN_Write16 (uint16_t data);

/**
 * @ingroup clb1
 * @brief Writes the data into the CLB Data registers.
 * @param uint32_t data - CLB input data.
 * @return None.
 */
void CLB1_SWIN_Write32 (uint32_t data);

/**
 * @ingroup clb1
 * @brief Reads the data from the CLB Data registers.
 * @param None.
 * @return uint8_t data - CLB output data.
 */
uint8_t CLB1_SWIN_Read8(void);

/**
 * @ingroup clb1
 * @brief Reads the data from the CLB Data registers.
 * @param None.
 * @return uint16_t data - CLB output data.
 */
uint16_t CLB1_SWIN_Read16(void);

/**
 * @ingroup clb1
 * @brief Reads the data from the CLB Data registers.
 * @param None.
 * @return uint32_t data - CLB output data.
 */
uint32_t CLB1_SWIN_Read32(void);


#endif // CLB1_H
/**
 End of File
*/
