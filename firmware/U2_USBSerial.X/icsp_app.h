/* 
 * File:   icsp_app.h
 * Author: M91541
 *
 * Created on 28. Mai 2024, 09:20
 */

#ifndef ICSP_APP_H
#define	ICSP_APP_H

#include "hwconfig.h"

#if defined(__XC8)
    #define PACKED
#else
    #define PACKED __attribute__((packed))
#endif

#ifdef	__cplusplus
extern "C" {
#endif
    typedef enum PACKED {
        ICSP_MCMD_NOP = 0, // No operation. Used for filling the buffer
        ICSP_MCMD_SET_HWUART_BAUD = 1, // Change the HW UART baud rate to parameter*100 [2..1152] (Example 3 = 300, 12 = 1200, 96 = 9600, 1152 = 115200 )
        ICSP_MCMD_SET_METACMD = 2, // Change the ICSP meta command to the (param) - used if ICSP has the default (ICSP_DEFAULT_META_COMMAND) as valid command
        ICSP_MCMD_SET_CMD_BITS = 3, // Set number of ICSP command bits length to <param>
        ICSP_MCMD_SET_DATA_BITS = 4, // Set number of ICSP data bits length to <param>
        ICSP_MCMD_LVP_ENTER = 5, // Enter ICSP LVP mode
        ICSP_MCMD_LVP_EXIT = 6, // Exit ICSP LVP mode
        ICSP_MCMD_DELAY_us = 7, // Delay for (param) microseconds
        ICSP_MCMD_READ_CMD = 8, // Send command and read dataunits defined by <param_byte0 = ICSP_ReadCommand> 1 dataunit is ICSP_MCMD_SET_DATA_BITS bits. Number of RX and TX bits determined by <ICSP_MCMD_SET_CMD_BITS> <ICSP_MCMD_SET_DATA_BITS>
        ICSP_MCMD_GET_HWUART_BAUD = 9, // Get the HW UART baud rate to parameter
        ICSP_MCMD_HWUART_ENABLE = 10, // Enable or disable the HW UART bridging to USB CDC. If parameter is 1 -> UART enabled, parameter 0 -> UART bridge disabled
        ICSP_MCMD_USBCDC_SEND = 11, // Mirror a single byte to the USB CDC interface (parameter low byte). Useful for verifying if the right COM port is selected by user. Returns the byte sent and zero on fail. (returns LB=data, HB=0 = fail, HB nonzero for success)
    } ICSP_metacommands_e;

    typedef union PACKED {
        uint32_t asUINT32;
        uint8_t asBytes[4];
        struct PACKED {
            uint8_t command_ID;
            uint8_t parameters[3];
        } asGenericCommand;
        struct PACKED {
            uint8_t command_ID;
            uint8_t meta_command;
            union PACKED {
                uint16_t meta_parameter;
                uint8_t meta_parameterBytes[2];
            };
        } asMetaCommand;
        struct PACKED {
            uint8_t icsp_command;
            uint24_t icsp_data;
        } asICSPCommand;
    } ICSP_hid_cmd_t;

    extern uint8_t ICSP_meta_command_id;
    
#define ICSP_MCLR_keepLow() do { TMR1ON = 0; TMR1IF = 0; PIN_nMCLR_Low(); } while(0)
#define ICSP_MCLR_softRelease() do { TMR1 = 0x15A0; TMR1IF = 0; TMR1ON = 1; } while(0)
#define ICSP_MCLR_resetCycle() do { ICSP_MCLR_keepLow(); ICSP_MCLR_softRelease(); } while(0)
#define ICSP_MCLR_keepHigh() do { TMR1ON = 0; TMR1IF = 0; ICSP_lvpExit(); } while(0)
    
    void ICSP_task(void);
    uint32_t ICSP_executeCommand(ICSP_hid_cmd_t cmd);
    void ICSP_lvpEnter(void);
    void ICSP_lvpExit(void);
    void ICSP_sendCmd(uint8_t b);
    void ICSP_sendData(uint24_t w);
    uint24_t ICSP_getData(void);
    uint8_t ICSP_getTargetState(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* ICSP_APP_H */

