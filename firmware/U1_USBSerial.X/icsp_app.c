#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "mcc_generated_files/usb/usb.h"
#include "icsp_app.h"
#include "hwconfig.h"
#include "gpio_x.h"
#include "usbcdc_app.h"

#define ICSP_DEFAULT_META_COMMAND       255
#define ICSP_DEFAULT_COMMAND_BITS       8
#define ICSP_DEFAULT_DATA_BITS          24

uint8_t ICSP_meta_command_id = ICSP_DEFAULT_META_COMMAND;
uint8_t ICSP_numBitsCmd = ICSP_DEFAULT_COMMAND_BITS;
uint8_t ICSP_numBitsData = ICSP_DEFAULT_DATA_BITS;
bool ICSP_HIDInitDone = false;
bool ICSP_needSendUSBdata = false;
bool ICSP_active = false;
volatile USB_HANDLE USBOutHandle;
volatile USB_HANDLE USBInHandle;

uint16_t ICSP_ms_delay_downcounter = 0;
ICSP_hid_cmd_t ICSP_HID_ReceviedCommands[HID_INT_OUT_EP_SIZE/sizeof(ICSP_hid_cmd_t)];
unsigned ICSP_command_index = 255;

#if defined(FIXED_ADDRESS_MEMORY)
#if defined(__XC8)
        ICSP_hid_cmd_t HID_ReceivedDataBuffer[HID_INT_OUT_EP_SIZE/sizeof(ICSP_hid_cmd_t)] HID_CUSTOM_OUT_DATA_BUFFER_ADDRESS;
        ICSP_hid_cmd_t HID_ToSendDataBuffer[HID_INT_IN_EP_SIZE/sizeof(ICSP_hid_cmd_t)] HID_CUSTOM_IN_DATA_BUFFER_ADDRESS;
unsigned char hid_feature_data[USB_EP0_BUFF_SIZE] HID_CUSTOM_FEATURE_DATA_BUFFER_ADDRESS;
#endif
#else
unsigned char HID_ReceivedDataBuffer[HID_INT_OUT_EP_SIZE];
unsigned char HID_ToSendDataBuffer[HID_INT_IN_EP_SIZE];
unsigned char hid_feature_data[USB_EP0_BUFF_SIZE];
#endif

void HIDInitEP(void) {
    USBInHandle = 0;
    //enable the HID endpoint
    USBEnableEndpoint(CUSTOM_DEVICE_HID_EP, USB_IN_ENABLED | USB_OUT_ENABLED | USB_HANDSHAKE_ENABLED | USB_DISALLOW_SETUP);

    //Re-arm the OUT endpoint for the next packet
    USBOutHandle = (volatile USB_HANDLE)HIDRxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)HID_ReceivedDataBuffer, HID_INT_OUT_EP_SIZE);
    ICSP_MCLR_softRelease();
    ICSP_HIDInitDone = true;
}

static void USBHID_CBSetFeatureReportComplete(void) {
    uint8_t command_ID = SetupPkt.W_Value.byte.LB;
    uint8_t *cmd_params = hid_feature_data;
    if (!command_ID) {
        command_ID = hid_feature_data[0];
        cmd_params = hid_feature_data + 1;
    }
    uint16_t *cmd_params_16 = (uint16_t *)cmd_params;
    // SET_FEATURE data phase completed
    switch (command_ID) {
        case ICSP_MCMD_SET_METACMD: // Change the ICSP meta command to parameter
            ICSP_meta_command_id = *cmd_params;
            break;
        case ICSP_MCMD_SET_HWUART_BAUD:
            if (*cmd_params_16 >= 2 && (*cmd_params_16 <= 1152)) {
                // 200 <= Desired baud rate  <= 115200
                USBCDC_setHwBaudRate(*cmd_params_16 * 100);
            }
            break;
        case ICSP_MCMD_HWUART_ENABLE:
            USBCDC_setHWbridgeEnable(*cmd_params_16 ? true : false);
            break;
        case ICSP_MCMD_USBCDC_SEND:
            USBCDC_putc(*cmd_params);
            break;
    }
}

void USBHID_UserSetReportHandler(void) {
    //Set report request initiated
    if (SetupPkt.W_Value.byte.HB == HID_REPORT_TYPE_FEATURE) {
        // Only respond to FEATURE requests
        USBEP0Receive((uint8_t*) hid_feature_data, (SetupPkt.wLength < USB_EP0_BUFF_SIZE) ? SetupPkt.wLength : USB_EP0_BUFF_SIZE, USBHID_CBSetFeatureReportComplete);
    }
}

void USBHID_UserGetReportHandler(void) {
    // GET_FEATURE data phase
    if (SetupPkt.W_Value.byte.HB == HID_REPORT_TYPE_FEATURE) {
        // Only respond to FEATURE requests
        switch (SetupPkt.W_Value.byte.LB) { // Feature report ID
            case 0: // Only 1 report ID exists
                hid_feature_data[0] = ICSP_meta_command_id;
                hid_feature_data[1] = ICSP_getTargetState(); // 0 : Target running, 1: stopped, running soon, 2: Stopped
                uint24_t hw_uart_baud = USBCDC_getHwBaudRate();
                hid_feature_data[2] = (hw_uart_baud & 0xFF);
                hid_feature_data[3] = ((hw_uart_baud >> 8) & 0xFF);
                hid_feature_data[4] = ((hw_uart_baud >> 16) & 0xFF);
                hid_feature_data[5] = PORTA;
                hid_feature_data[6] = 0;
                hid_feature_data[7] = PORTC;
                break;
            default:
                return;
        }
        USBEP0SendRAMPtr((uint8_t*) & hid_feature_data, USB_EP0_BUFF_SIZE, USB_EP0_NO_OPTIONS);
    }
}

void ICSP_task(void) {
    if (TMR1IF) {
        /* Reset time timeout reached, release Reset */
        ICSP_MCLR_keepHigh();
    }

    if (USBGetDeviceState() < CONFIGURED_STATE) {
        ICSP_HIDInitDone = false;
        if (ICSP_active == true) {
            ICSP_MCLR_softRelease(); // Exit programming if in ICSP mode
        }
        // Reset ICSP task to idle state
        ICSP_ms_delay_downcounter = 0;
        ICSP_command_index = 255;
        ICSP_needSendUSBdata = false;
        return;
    }

    if (ICSP_HIDInitDone) {
        if (HIDRxHandleBusy(USBOutHandle) == false) {
            //We just received a packet of data from the USB host.
            //Check the first uint8_t of the packet to see what command the host
            //application software wants us to fulfill.
            memcpy(ICSP_HID_ReceviedCommands, HID_ReceivedDataBuffer, sizeof(ICSP_HID_ReceviedCommands)); // Cache commands
            ICSP_command_index = ICSP_ms_delay_downcounter = 0;
            // Listen to next USB packet
            USBOutHandle = HIDRxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*) HID_ReceivedDataBuffer, HID_INT_OUT_EP_SIZE); // Prime USB HID handle for next receive
        }
        if (ICSP_ms_delay_downcounter) {
            //Long delay in progress
            if (INTCONbits.TMR0IF) {
                // Timer0 overflow -> 1ms elapsed
                INTCONbits.TMR0IF = 0; // Ack overflow handled
                TMR0 += 69; // Overflow in 1ms
                --ICSP_ms_delay_downcounter;
            }
        } else {
            if (ICSP_command_index == (sizeof (ICSP_HID_ReceviedCommands) / sizeof (ICSP_HID_ReceviedCommands[0]))) {
                // Send data if all commands processed
                // We can be sure here, no long delay is in progress
                ICSP_needSendUSBdata = true;
                ++ICSP_command_index; // Send only once
            } else {
                for (; ICSP_command_index<sizeof (ICSP_HID_ReceviedCommands) / sizeof (ICSP_HID_ReceviedCommands[0]); ++ICSP_command_index) {
                    // Process  batch of received commands, if commands are pending
                    HID_ToSendDataBuffer[ICSP_command_index].asUINT32 = ICSP_executeCommand(ICSP_HID_ReceviedCommands[ICSP_command_index]);
                    if (ICSP_ms_delay_downcounter) {
                        // Long delay needed, exit command processing loop
                        // Restart ms_timer (TMR0)
                        // TMR0 69; ~1ms
                        TMR0 = 69;
                        INTCONbits.TMR0IF = 0;
                        ++ICSP_command_index; // Next command, when timing completed
                        break;
                    }
                }
            }
        }
        if (ICSP_needSendUSBdata && !HIDTxHandleBusy(USBInHandle)) {
            //Prepare the USB module to send the data packet to the host
            USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*) HID_ToSendDataBuffer, HID_INT_IN_EP_SIZE);
            ICSP_needSendUSBdata = false;
        }
    } else {
        // HID init needed
        HIDInitEP();
    }
}

uint32_t ICSP_executeCommand(ICSP_hid_cmd_t cmd) {
    uint16_t tmpCtr;
    if (cmd.asGenericCommand.command_ID == ICSP_meta_command_id) {
        // It is a meta command
        switch (cmd.asMetaCommand.meta_command) {
            case ICSP_MCMD_NOP:
                break;
            case ICSP_MCMD_SET_HWUART_BAUD:
                if (cmd.asMetaCommand.meta_parameter >= 2 && (cmd.asMetaCommand.meta_parameter <= 1152)) {
                    // 200 <= Desired baud rate  <= 115200
                    USBCDC_setHwBaudRate((uint24_t)cmd.asMetaCommand.meta_parameter * 100);
                }
                break;
            case ICSP_MCMD_SET_METACMD:
                if (cmd.asMetaCommand.meta_parameter < 256) {
                    ICSP_meta_command_id = cmd.asMetaCommand.meta_parameterBytes[0];
                }
                cmd.asMetaCommand.meta_parameter = ICSP_meta_command_id;
                break;
            case ICSP_MCMD_SET_CMD_BITS:
                if (cmd.asMetaCommand.meta_parameter < 9) {
                    ICSP_numBitsCmd = cmd.asMetaCommand.meta_parameterBytes[0];
                }
                cmd.asMetaCommand.meta_parameter = ICSP_numBitsCmd;
                break;
            case ICSP_MCMD_SET_DATA_BITS:
                if (cmd.asMetaCommand.meta_parameter < 25) {
                    ICSP_numBitsData = cmd.asMetaCommand.meta_parameterBytes[0];
                }
                cmd.asMetaCommand.meta_parameter = ICSP_numBitsData;
                break;
            case ICSP_MCMD_LVP_ENTER:
                ICSP_lvpEnter(cmd.asMetaCommand.meta_parameter == 0xFE01); // parameter == 0xFE01 means enter LVP with LSB first method
                break;
            case ICSP_MCMD_LVP_EXIT:
                ICSP_lvpExit();
                break;
            case ICSP_MCMD_DELAY_us:
                for(tmpCtr = cmd.asMetaCommand.meta_parameter; tmpCtr; --tmpCtr) {
                    __delay_us(1);
                }
                break;
            case ICSP_MCMD_READ_CMD:
                ICSP_sendCmd(cmd.asMetaCommand.meta_parameterBytes[0]);
                cmd.asICSPCommand.icsp_command = cmd.asMetaCommand.meta_parameterBytes[0];
                cmd.asICSPCommand.icsp_data = ICSP_getData();
                break;
            case ICSP_MCMD_GET_HWUART_BAUD:
                cmd.asICSPCommand.icsp_data = USBCDC_getHwBaudRate();
                break;
            case ICSP_MCMD_HWUART_ENABLE:
                USBCDC_setHWbridgeEnable(cmd.asMetaCommand.meta_parameter ? true : false);
                cmd.asMetaCommand.meta_parameter = USBCDC_isHWbridgeEnabled();
                break;
            case ICSP_MCMD_USBCDC_SEND:
                cmd.asMetaCommand.meta_parameterBytes[1] = USBCDC_putc(cmd.asMetaCommand.meta_parameterBytes[0]);
                break;
            case ICSP_MCMD_GPIO:
                // Parameter[0] : Primary or secondary function (0=Primary; 1=Secondary)
                // Parameter[1] : Secondary function LOW/HIGH (0=Low; 1=High)
                // Bit | Pin# | PORT | Primary f.   | Alternate
                //   7 |   2  | RA5  | /LED RX      | Output
                //   6 |   3  | RA4  | /LED TX      | Output
                //   5 |   5  | RC5  | RX(data In) *| Input
                //   4 |   6  | RC4  | TX(data Out)*| Output
                //   3 |   7  | RC3  | ICSP DATA    | Output
                //   2 |   8  | RC2  | ICSP CLK     | Output
                //   1 |   9  | RC1  | /ICSP MCLR   | Output
                //   0 |  10  | RC0  | GP Input     | Output
                
                PORTA_x.TRIS_alt = 0b11001111;
                PORTC_x.TRIS_alt = 0b11100000;
                
                // Handle alternative function pins
                PORTA_x.is_PinAlternative_bits.bit5 = (cmd.asMetaCommand.meta_parameterBytes[0] & 0x80) ? 1 : 0;
                PORTA_x.is_PinAlternative_bits.bit4 = (cmd.asMetaCommand.meta_parameterBytes[0] & 0x40) ? 1 : 0;
                if (cmd.asMetaCommand.meta_parameterBytes[0] & (0x20 | 0x10)) {
                    //Either of UART pins are controlled by PC
                    USBCDC_setHWbridgeEnable(false);
                    RCSTAbits.SPEN = 0;
                    cmd.asMetaCommand.meta_parameterBytes[0] |= 0x20 | 0x10; // Take control of both UART RX/TX pins
                } else {
                    TRISC4 = 0; // Output
                    TRISC5 = 1; // Input
                    RCSTAbits.SPEN = 1;
                    USBCDC_setHWbridgeEnable(true);
                }
                PORTC_x.is_PinAlternative = cmd.asMetaCommand.meta_parameterBytes[0] & 0b00111111;

                // Set alternative outputs high/low
                PORTA_x.LAT_alt_bits.bit5 = (cmd.asMetaCommand.meta_parameterBytes[1] & 0x80) ? 1 : 0;
                PORTA_x.LAT_alt_bits.bit4 = (cmd.asMetaCommand.meta_parameterBytes[1] & 0x40) ? 1 : 0;
                PORTC_x.LAT_alt &= 0b00111111;;
                PORTC_x.LAT_alt |= cmd.asMetaCommand.meta_parameterBytes[1] & 0b00111111;
                
                //Async update!
                gpiox_update_ports();
                
                //Read inputs
                cmd.asMetaCommand.meta_parameterBytes[1] = PORTC & 0b00111111;
                cmd.asMetaCommand.meta_parameterBytes[1] |= IO_LED_RX_GetValue() ? 0x80 : 0x00;
                cmd.asMetaCommand.meta_parameterBytes[1] |= IO_LED_TX_GetValue() ? 0x40 : 0x00;
                break;
            case ICSP_MCMD_DELAY_ms:
                ICSP_ms_delay_downcounter = cmd.asMetaCommand.meta_parameter;
                break;
            default:
                cmd.asMetaCommand.meta_command = ICSP_MCMD_NOP; // Unknown command executes a NOP
                cmd.asMetaCommand.meta_parameter = 0; // Unknown command executes a NOP
                break;
        }
    } else {
        // It is a generic ICSP command
        ICSP_sendCmd(cmd.asICSPCommand.icsp_command);
        ICSP_sendData(cmd.asICSPCommand.icsp_data);
    }
    return cmd.asUINT32;
}

void ICSP_lvpEnter(bool isLSB_first) {

    ICSP_lvpExit(); // Make sure we are not in the middle of ICSP cycle
    ICSP_active = true;
    uint8_t tmpCmdBits = ICSP_numBitsCmd;
    __delay_ms(10);
    PIN_nMCLR_Low(); // MCLR low
    IO_ICSP_DT_SetDigitalInput();
    IO_ICSP_CK_SetLow();
    IO_ICSP_CK_SetDigitalOutput();

    __delay_ms(10);
    ICSP_numBitsCmd = 8;
    if (isLSB_first) {
        //Bit reversed of 'MCHP' -> 0x0a 12 c2 b2
        ICSP_sendCmd(0x0A);
        ICSP_sendCmd(0x12);
        ICSP_sendCmd(0xC2);
        ICSP_sendCmd(0xB2);
    } else {
        ICSP_sendCmd('M');
        ICSP_sendCmd('C');
        ICSP_sendCmd('H');
        ICSP_sendCmd('P');
    }
    __delay_ms(5);
    ICSP_numBitsCmd = tmpCmdBits;
}

void ICSP_lvpExit(void) {
    IO_ICSP_CK_SetDigitalInput(); // Release ICSP pins
    PORTC_x.TRIS_default_bits.bit2 = INPUT;
    IO_ICSP_DT_SetDigitalInput();
    PORTC_x.TRIS_default_bits.bit3 = INPUT;
    PIN_nMCLR_High(); // Release target from reset
    PORTC_x.TRIS_default_bits.bit1 = INPUT;
    ICSP_active = false;
}

void ICSP_sendCmd(uint8_t b) {
    uint8_t i;
    IO_ICSP_DT_SetDigitalOutput();
    for (i = 0; i < ICSP_numBitsCmd; i++) { // 8-bit commands
        if ((b & 0x80) > 0) // Msb first
            IO_ICSP_DT_SetHigh();

        else
            IO_ICSP_DT_SetLow();
        IO_ICSP_CK_SetHigh();
        b <<= 1; // shift left
        __delay_us(1);
        IO_ICSP_CK_SetLow();
        __delay_us(1);
    }
    __delay_us(1);
}

void ICSP_sendData(uint24_t w) {
    uint8_t i;

    IO_ICSP_DT_SetDigitalOutput();
    for (i = 0; i < ICSP_numBitsData; i++) {
        if ((w & 0x800000) > 0) // Msb first
            IO_ICSP_DT_SetHigh();

        else
            IO_ICSP_DT_SetLow();
        IO_ICSP_CK_SetHigh();
        w <<= 1; // shift left
        __delay_us(1);
        IO_ICSP_CK_SetLow();
        __delay_us(1);
    }
}

uint24_t ICSP_getData(void) {
    uint8_t i;
    uint24_t w = 0;
    IO_ICSP_DT_SetDigitalInput();
    for (i = 0; i < ICSP_numBitsData; i++) { // 24 bit
        IO_ICSP_CK_SetHigh();
        w <<= 1; // shift left
        __delay_us(1);
        w |= IO_ICSP_DT_GetValue(); // read port, msb first (loose top byte)
        IO_ICSP_CK_SetLow();
        __delay_us(1);
    }
    return w;
}

uint8_t ICSP_getTargetState(void) {
    if (IO_nMCLR_GetValue()) {
        // Target is running
        return 0;
    }
    if (TMR1ON) {
        // Target in reset, but released to run soon
        return 1;
    }
    // Target held in reset
    return 2;
}
