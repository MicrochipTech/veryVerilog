/**
 * CLB1 Generated Driver API Source File
 *
 * @file clb1.c
 *
 * @ingroup clb1
 *
 * @brief This file contains the implementation of public and private functions for the CLB1 driver.
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

/**
 * Section: Included files
 */
#include "../../system/system.h"


extern uint16_t start_clb_config;
extern uint16_t end_clb_config;

/**
  Section: CLB1 APIs
*/
void CLB1_Initialize(void)
{
    /* Disable CLB */
    CLBCONbits.CLBEN = 0;

    //Load the bitstream
    CLB1_Configure((uint16_t) &start_clb_config);

    /* CLK TMR0_Overflow; */
    CLBCLK = 0xC;

    /* OESEL0 0; OESEL1 0; */
    CLBPPSCON1 = 0x0;

    /* OESEL2 0; OESEL3 0; */
    CLBPPSCON2 = 0x0;

    /* OESEL4 0; OESEL5 0; */
    CLBPPSCON3 = 0x0;

    /* OESEL6 0; OESEL7 0; */
    CLBPPSCON4 = 0x0;

    // Clearing CLB1I0 IF flag.
    PIR7bits.CLB1IF0 = 0;
    // Disabled CLB1I0 CLB1 interrupt
    PIE7bits.CLB1IE0 = 0;

    // Clearing CLB1I1 IF flag.
    PIR7bits.CLB1IF1 = 0;
    // Disabled CLB1I1 CLB1 interrupt
    PIE7bits.CLB1IE1 = 0;

    // Clearing CLB1I2 IF flag.
    PIR7bits.CLB1IF2 = 0;
    // Disabled CLB1I2 CLB1 interrupt
    PIE7bits.CLB1IE2 = 0;

    // Clearing CLB1I3 IF flag.
    PIR7bits.CLB1IF3 = 0;
    // Disabled CLB1I3 CLB1 interrupt
    PIE7bits.CLB1IE3 = 0;

    /* EN enabled; */
    CLBCON = 0x80;

}

void CLB1_Enable(void)
{
    CLBCONbits.CLBEN = 1;
}

void CLB1_Disable(void)
{
    CLBCONbits.CLBEN = 0;
}

inline bool CLB1_IsCLBSWINBusy(void)
{
    return(CLBCONbits.BUSY);
}

void CLB1_Configure(uint16_t start_address)
{

    uint16_t end_address;

    end_address = start_address + BITSTREAM_SIZE;

    // Set the bitstream address
    CRC_SetScannerAddressLimit(start_address, end_address);

    // Start CLB bitstream load
    CRC_StartNvmScanner();

    // Wait to complete
    while (CRC_IsScannerBusy());

    // Switch back to the CRC peripheral
    CRC_StopNvmScanner();
}

void CLB1_SWIN_Write8(uint8_t data)
{
    //wait for CLBSWIN register to be synchronized
    while (CLB1_IsCLBSWINBusy());

    CLBSWINL = data;
}

void CLB1_SWIN_Write16 (uint16_t data)
{
    //wait for CLBSWIN register to be synchronized
    while (CLB1_IsCLBSWINBusy());

	CLBSWINM = (uint8_t)((data >> 8) & 0xFF);

	//Write to the CLBSWINL register trigger load of CLBSWINM into CLB
	CLBSWINL = (uint8_t)(data & 0xFF);

}

void CLB1_SWIN_Write32 (uint32_t data)
{
   //wait for CLBSWIN register to be synchronized
    while (CLB1_IsCLBSWINBusy());
    
	CLBSWINU = (uint8_t)((data >> 24) & 0xFF);
	CLBSWINH = (uint8_t)((data >> 16) & 0xFF);
	CLBSWINM = (uint8_t)((data >> 8) & 0xFF);
	
	//Write to the CLBSWINL register trigger load of upper registers into CLB
	CLBSWINL = (uint8_t)(data & 0xFF);

}

uint8_t CLB1_SWIN_Read8(void)
{
    uint8_t result = 0x00;

    result = (uint8_t)CLBSWINL;

	return result;
}

uint16_t CLB1_SWIN_Read16(void)
{
    uint16_t result = 0x00;

    result = (uint16_t)CLBSWINL;
    result = result | ((uint16_t)CLBSWINM << 8);

	return result;
}

uint32_t CLB1_SWIN_Read32(void)
{
    uint32_t result = 0x00;

    result = (uint32_t)CLBSWINL;
    result = result | ((uint32_t)CLBSWINM << 8);
    result = result | ((uint32_t)CLBSWINH << 16);
    result = result | ((uint32_t)CLBSWINU << 24);

	return result;
}

