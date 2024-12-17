#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "mcc_generated_files/usb/usb.h"
#include "icsp_app.h"
#include "hwconfig.h"
#include "usbcdc_app.h"

#define ICSP_DEFAULT_META_COMMAND       255
#define ICSP_DEFAULT_COMMAND_BITS       8
#define ICSP_DEFAULT_DATA_BITS          24

uint8_t ICSP_meta_command_id = ICSP_DEFAULT_META_COMMAND;
uint8_t ICSP_numBitsCmd = ICSP_DEFAULT_COMMAND_BITS;
uint8_t ICSP_numBitsData = ICSP_DEFAULT_DATA_BITS;
bool ICSP_HIDInitDone = false;
bool ICSP_needSendUSBdata = false;
volatile USB_HANDLE USBOutHandle;
volatile USB_HANDLE USBInHandle;

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

    if ((USBGetDeviceState() < CONFIGURED_STATE) || (USBIsDeviceSuspended() == true)) {
        ICSP_HIDInitDone = false;
        if (USBIsDeviceSuspended() == true) {
            // Keep target in RESET during USB suspend
            ICSP_MCLR_keepLow();
        }
        return;
    }

    if (ICSP_HIDInitDone) {
        if (HIDRxHandleBusy(USBOutHandle) == false) {
            //We just received a packet of data from the USB host.
            //Check the first uint8_t of the packet to see what command the host
            //application software wants us to fulfill.
            unsigned i;
            for (i = 0; i<sizeof (HID_ToSendDataBuffer) / sizeof (HID_ToSendDataBuffer[0]); ++i) {
                HID_ToSendDataBuffer[i].asMetaCommand.command_ID = ICSP_meta_command_id;
                HID_ToSendDataBuffer[i].asMetaCommand.meta_command = ICSP_MCMD_NOP;
            }
            for (i = 0; i<sizeof (HID_ReceivedDataBuffer) / sizeof (HID_ReceivedDataBuffer[0]); ++i) {
                HID_ToSendDataBuffer[i].asUINT32 = ICSP_executeCommand(HID_ReceivedDataBuffer[i]);
            }
            ICSP_needSendUSBdata = true;
            // Listen to next USB packet
            USBOutHandle = HIDRxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*) HID_ReceivedDataBuffer, HID_INT_OUT_EP_SIZE);
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
                if ((cmd.asMetaCommand.meta_parameter > 1) && (cmd.asMetaCommand.meta_parameter < 9)) {
                    ICSP_numBitsCmd = cmd.asMetaCommand.meta_parameterBytes[0];
                }
                cmd.asMetaCommand.meta_parameter = ICSP_numBitsCmd;
                break;
            case ICSP_MCMD_SET_DATA_BITS:
                if ((cmd.asMetaCommand.meta_parameter > 1) && (cmd.asMetaCommand.meta_parameter < 25)) {
                    ICSP_numBitsData = cmd.asMetaCommand.meta_parameterBytes[0];
                }
                cmd.asMetaCommand.meta_parameter = ICSP_numBitsData;
                break;
            case ICSP_MCMD_LVP_ENTER:
                ICSP_lvpEnter();
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
        }
    } else {
        // It is a generic ICSP command
        ICSP_sendCmd(cmd.asICSPCommand.icsp_command);
        ICSP_sendData(cmd.asICSPCommand.icsp_data);
    }
    return cmd.asUINT32;
}

void ICSP_lvpEnter(void) {

    ICSP_lvpExit(); // Make sure we are not in the middle of ICSP cycle
    uint8_t tmpCmdBits = ICSP_numBitsCmd;
    __delay_ms(10);
    PIN_nMCLR_Low(); // MCLR low
    IO_ICSP_DT_SetDigitalInput();
    IO_ICSP_CK_SetLow();
    IO_ICSP_CK_SetDigitalOutput();

    __delay_ms(10);
    ICSP_numBitsCmd = 8;
    ICSP_sendCmd('M');
    ICSP_sendCmd('C');
    ICSP_sendCmd('H');
    ICSP_sendCmd('P');
    __delay_ms(5);
    ICSP_numBitsCmd = tmpCmdBits;
}

void ICSP_lvpExit(void) {
    IO_ICSP_CK_SetDigitalInput(); // Release ICSP pins
    IO_ICSP_DT_SetDigitalInput();
    PIN_nMCLR_High(); // Release target from reset
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
