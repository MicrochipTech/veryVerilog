/**
 * CRC Generated Driver File.
 * 
 * @file crc.h
 * 
 * @defgroup  crc CRC
 * 
 * @brief This file contains the API prototypes for the CRC driver.
 *
 * @version CRC Driver Version 1.0.0
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

#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @ingroup crc
 * @brief Initializes the CRC module. This is called only once before calling other CRC APIs.
 * @param None.
 * @return None.
 */
void CRC_Initialize(void);

/**
 * @ingroup crc
 * @brief Starts the CRC serial shifter.
 * @param None.
 * @return None.
 */
inline void CRC_StartCrc(void);

/**
 * @ingroup crc
 * @brief Writes the data into the CRC Data registers.
 * @param data - CRC input data.
 * @retval True - Data write was successful.
 * @retval False - Data write was not successful.
 */
bool CRC_WriteData(uint32_t data);

/**
 * @ingroup crc
 * @brief Returns the calculated CRC output value.
 * @param reverse - Determines if the normal (0) or reverse (1) value of the CRC output will be calculated.
 * @param xorValue - Value which is XOR-ed with CRC result.
 * @return CRC calculated result.
 */
uint32_t CRC_GetCalculatedResult(bool reverse, uint32_t xorValue);

/**
 * @ingroup crc
 * @brief Returns the status of the CRCBUSY bit value to check if the shifting is in progress or not.
 * @param None.
 * @retval True - CRC bit shifting is in progress or pending.
 * @retval False - CRC bit shifting is complete or not pending.
 */
inline bool CRC_IsCrcBusy(void);

/**
 * @ingroup crc
 * @brief Starts the scanning process.
 * @param None.
 * @return None.
 */
inline void CRC_StartCrcScanner(void);

/**
 * @ingroup crc
 * @brief Stops the scanning process.
 * @param None.
 * @return None.
 */
inline void CRC_StopCrcScanner(void);

/**
 * @ingroup crc
 * @brief Starts the CLB load process.
 * @param None.
 * @return None.
 */
inline void CRC_StartNvmScanner(void);

/**
 * @ingroup crc
 * @brief Stopsthe CLB load process and switch back to CRC
 * @param None.
 * @return None.
 */
inline void CRC_StopNvmScanner(void);

/**
 * @ingroup crc
 * @brief Sets the memory address limit for the scanner.
 * @param startAddr - Start address of the memory block.
 * @param endAddr - End address of the memory block.
 * @return None.
 */
void CRC_SetScannerAddressLimit(uint16_t startAddr, uint16_t endAddr);

/**
 * @ingroup crc
 * @brief Checks if the scanner cycle is in progress or not.
 * @param None.
 * @retval True - Scanner cycle is in progress.
 * @retval False - Scanner cycle is complete (or not yet started).
 */
inline bool CRC_IsScannerBusy(void);


#endif //CRC_H