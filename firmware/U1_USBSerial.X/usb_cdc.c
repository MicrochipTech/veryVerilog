#include <stdint.h>
#include <string.h>
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/usb/usb.h"
#include "hwconfig.h"
#include "helper_funcs.h"
#include "usbcdc_app.h"
#include "version.h"

uint8_t usb_txbuffer[CDC_DATA_OUT_EP_SIZE * 2]; // Coming from UART, moving to PC
uint_fast8_t usb_txBufferNumBytes;

uint8_t usb_rxbuffer[CDC_DATA_IN_EP_SIZE]; // Coming from USB, leaving UART
uint_fast8_t usb_rxBufferNumBytes;
uint8_t usb_rxbuffer_sendPointer;

#define LED_DELAY_TIMER_TIME (20) //LED timer overflow rate [msec]
#define TX_LED_MINON_TIME (100) // TX LED minimum on time [msec]
#define RX_LED_MINON_TIME (TX_LED_MINON_TIME) // TX LED minimum on time [msec]

#define TX_LED_MINON_TICKS (TX_LED_MINON_TIME / LED_DELAY_TIMER_TIME)  // Ontime converted to ticks
#define RX_LED_MINON_TICKS (RX_LED_MINON_TIME / LED_DELAY_TIMER_TIME)  // Ontime converted to ticks


uint8_t rxLED_counter = 0;
uint8_t txLED_counter = 0;
uint8_t nRESET_counter = 0;
uint8_t whoAmI_counter = 0;
bool USBCDC_serialBridgeMode = true;

char * USBCDC_whoami_str(char* dst_buffer) {
    strcpy(dst_buffer, "\n{\"app\":\"miniFPGA board\",\"USBFW\":\"");
    dst_buffer += strlen(dst_buffer);
    *dst_buffer++= ((USB_FW_VERSION >> 12) & 0xF) + '0';
    *dst_buffer++= ((USB_FW_VERSION >> 8) & 0xF) + '0';
    *dst_buffer++='.';
    *dst_buffer++= ((USB_FW_VERSION >> 4) & 0xF) + '0';
    *dst_buffer++= (USB_FW_VERSION & 0xF) + '0';
    strcpy(dst_buffer, "\",\"meta_cmd\":");
    dst_buffer = u8toazpad(ICSP_meta_command_id, 1, dst_buffer + strlen(dst_buffer));
    strcpy(dst_buffer, "}\n");
    return dst_buffer;
}

void USBCDC_task(void) {
    if (TMR2IF) {
        /* 20ms expired */
        TMR2IF = 0;
        if (txLED_counter) {
            --txLED_counter;
        } else {
            PIN_LedTX_Off();
        }
        if (rxLED_counter) {
            --rxLED_counter;
        } else {
            PIN_LedRX_Off();
        }
    }

    if ((USBGetDeviceState() < CONFIGURED_STATE) || (USBIsDeviceSuspended() == true)) {
        /* USB communication inactive state */
        txLED_counter = 0;
        rxLED_counter = 0;
        PIN_LedRX_Off(); // RX LED off
        PIN_LedTX_Off(); // TX LED off
        usb_txBufferNumBytes = 0; /* Clear buffers */
        usb_rxBufferNumBytes = 0;
        /* dump incoming data */
        if (PIR1bits.RCIF) {
            if (1 == RCSTAbits.OERR) {
                // EUSART error - restart

                RCSTAbits.CREN = 0;
                RCSTAbits.CREN = 1;
            }

            usb_txbuffer[usb_txBufferNumBytes] = RCREG;
        }
        return;
    }

    // USB communication alive
    if (PIR1bits.RCIF) {
        /* New character received from UART */
        if (1 == RCSTAbits.OERR) {
            // EUSART FIFO overrun error - restart
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }

        if (USBCDC_isHWbridgeEnabled() && (usb_txBufferNumBytes < sizeof (usb_txbuffer))) {
            /* Store incoming UART character to the USB output buffer */
            usb_txbuffer[usb_txBufferNumBytes++] = RCREG;
            txLED_counter = TX_LED_MINON_TICKS;
            PIN_LedTX_On(); // Make sure TX led is on

        } else {
            /* Ignore incoming bytes if the local USB tx buffer is full */
            static volatile uint8_t rxdump;
            rxdump = RCREG;
        }
    }

    if (usb_txBufferNumBytes && (USBUSARTIsTxTrfReady() == true)) {
        /* USB ready for accepting data, and there is data to upload */
        putUSBUSART(usb_txbuffer, usb_txBufferNumBytes);
        usb_txBufferNumBytes = 0;
    }

    /* When USART TX is idle, read USB and transmit when USB data waiting */
    if (!usb_rxBufferNumBytes) {
        /* UART local TX buffer is empty */
        usb_rxBufferNumBytes = getsUSBUSART(usb_rxbuffer, sizeof (usb_rxbuffer));
        usb_rxbuffer_sendPointer = 0;
    }

    if (USBCDC_isHWbridgeEnabled()) {
        // UART <-> USB Gateway mode
        if (PIR1bits.TXIF && usb_rxBufferNumBytes) {
            /* UART ready for transmit and there are bytes to send */
            TXREG = usb_rxbuffer[usb_rxbuffer_sendPointer++];
            --usb_rxBufferNumBytes;
            PIN_LedRX_On(); // Make sure RX led is on
            rxLED_counter = RX_LED_MINON_TICKS;
        }
    } else {
        // Not gateway mode
        PIN_LedRX_Off(); // Make sure RX led is off
        PIN_LedRX_Off(); // Make sure RX led is off
        if (whoAmI_counter && !usb_txBufferNumBytes) {
            if (USBUSARTIsTxTrfReady() == true) {
                USBCDC_whoami_str((char*)usb_txbuffer);
                usb_txBufferNumBytes = (uint8_t)strlen((char*)usb_txbuffer);
                --whoAmI_counter;
            }
        }
    }
}

CTRL_TRF_RETURN USBCDC_baudRateChangeRequest(CTRL_TRF_PARAMS) {
    if (cdc_notice.SetLineCoding.bDataBits > 7) {
        CDCSetBaudRate(cdc_notice.SetLineCoding.dwDTERate);
        usb_txBufferNumBytes = 0; // Empty incoming Queue on baud-rate changes
        usb_rxBufferNumBytes = 0;
        if (cdc_notice.SetLineCoding.dwDTERate < 1201) {
            //1200 baud or below will engage the bootload line
            whoAmI_counter = 1;
            USBCDC_setHWbridgeEnable(false);
        } else {
            whoAmI_counter = 0;
            USBCDC_setHWbridgeEnable(true);
        }
    }
}

void USBCDC_setHwBaudRate(uint24_t requestedBaud) {
    uint8_t rshift = 6;  // DIV 64
    if (BRG16 || BRGH) {
        rshift = 4;      // DIV 16
    }
    if (BRG16 && BRGH) {
        rshift = 2;      // DIV 4
    }
    SPBRG = (uint16_t)((_XTAL_FREQ >> rshift) / requestedBaud - 1);
}

uint24_t USBCDC_getHwBaudRate(void) {
    uint8_t lshift = 6;  // DIV 64
    if (BRG16 || BRGH) {
        lshift = 4;      // DIV 16
    }
    if (BRG16 && BRGH) {
        lshift = 2;      // DIV 4
    }
    return (uint24_t)((uint32_t)_XTAL_FREQ / ((uint32_t)(SPBRG + 1) << lshift));
}

bool USBCDC_putc(char ch) {
    if (usb_txBufferNumBytes < sizeof (usb_txbuffer)) {
        /* Store incoming UART character to the USB output buffer */
        usb_txbuffer[usb_txBufferNumBytes++] = ch;
        return true;
    }
    return false;
}