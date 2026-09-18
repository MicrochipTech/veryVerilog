# veryVerilog

veryVerilog enables an easy way to program PICs from family 
- **PIC16F131xy**
- **PIC16F132xy**
- **PIC16F180xy**
- **PIC18FxyQ35**

over a Web Browser without the need to install any software using a PIC as a USB HiD device. 

The project is based on 3 parts:
- USB-HID PCB (hardware)
- USB-HID Hex File (firmware)
- Web Page to flash Hex files a PIC device attached to the hardware

The PCB looks like this and is based on a [**PIC16F1455**](https://www.microchip.com/en-us/product/pic16f1455) .

The [**PIC16F1455**](https://www.microchip.com/en-us/product/pic16f1455) needs a [**custom firmware**](/firmware/hex/U1_PIC16F1455_v1.20.hex) to perform as ICSP programmer.

![PCB](/doc/images/pcb.png)

The IC on the left-hand side is the [**PIC16F1455**](https://www.microchip.com/en-us/product/pic16f1455)-I/P and IC on the right-hand side is the miniFPGA ([**PIC16F13145**](https://www.microchip.com/en-us/product/pic16f13145)-I/P) to be programmed.

# Program a PIC in 4 Steps

It is that easy:

## 1. Insert the Hardware

Plug the miniFPGA dongle in your computer.

## 2. Click the Web Page

Open the Web Page: [https://microchiptech.github.io/veryVerilog](https://microchiptech.github.io/veryVerilog/).

You will see the veryVerilog Web Page:

![Web1](/doc/images/web1.png)

## 3. Connect to Programmer

Click the button "Connect to Programmer" and select the miniFPGA device:

![Web2](/doc/images/web2.png)

If you have multiple programmers connected, click "identify Programmer" and the LEDs of the connected board will blink for about 2 seconds.

The device is recognized and the Web Page displays information about the connected PIC and the identification of the program already flashed on the device (User Id)

![Web3](/doc/images/web3.png)

## 4. Drag and Drop

Drag a HEX file on the *"Drag and Drop"* area and the program will be transfered automatically to the PIC.

![Web4](/doc/images/web4.png)

Alternatively, you can click the **"Browse for HEX file"** button to select a HEX file from your file system. This is particularly useful when working with IDEs where you can copy and paste file paths.

Have fun!

## Additional Features

### Reset Target Device

After connecting to the programmer, a **"Reset Target"** button becomes available. Clicking this button will reset the target PIC.

### Device Detection

When you connect to a programmer:
- If the device is **recognized**, the web page displays the PIC model name (e.g., "PIC16F13145") and its User ID
- If the device is **not recognized** or reads an invalid ID (like 0x0000), the page will display: **"Unknown PIC with DEVID 0x[hex_value]"** along with the actual device ID that was read

This helps in debugging connectivity issues or identifying when an unsupported PIC is connected.

**Tip:** Click on the **UserId** display to open a detailed information window showing:
- **MCU Parameters** - Device ID, Revision ID, Memory sizes (ERSIZ, WLSIZ, URSIZ, EESIZ), Program Counter size (PCNT), and other device-specific information
- **Programmer Parameters** - Hardware UART baud rate and other programmer settings

### Reading Memory Regions

After connecting to the programmer, you can read the entire contents of the target PIC device:

1. Click the **Settings** dropdown (split button next to "Identify Programmer")
2. Select **"Read Device"** from the menu
3. The device memory will be read and automatically displayed in a modal window

The memory viewer organizes data into collapsible sections:
- **Program Flash** - The main program memory (displayed in rows of 16 words)
- **EEPROM** - Data EEPROM memory (if available on the device, displayed in rows of 8 bytes)
- **UserId** - User ID locations (displayed in rows of 4 words)
- **Config Words** - Configuration bits

You can also access previously read memory using the **"Show Memory"** option without reading the device again. This is useful for comparing data or reviewing what was last read from the device.

### Programming Settings

The **Settings** dropdown provides options to customize which memory regions are programmed and whether verification is performed:

**Default Behavior:**
- All memory regions are programmed: **Program** (Flash), **EEPROM**, **UserId**, and **Config Bits**
- After programming, the code is automatically **verified** to ensure it was written correctly

**Customizing Memory Regions:**

Click the **Settings** dropdown to access programming options. You can selectively choose which memory regions to program by checking/unchecking:
- **Program** - Main program flash memory
- **EEPROM** - Data EEPROM memory (if available on the device)
- **UserId** - User ID locations
- **Config Bits** - Configuration bits

**Verification:**
- The **Verify** option (enabled by default) checks that the programmed memory matches the expected values
- This ensures programming integrity but may increase programming time slightly
- You can disable verification if faster programming is needed, though this is not recommended

### Serial Terminal

The veryVerilog web page includes a built-in **Serial Terminal** for communicating directly with the target PIC over a serial (UART) connection.

#### Opening the Serial Terminal

Click the **"Serial Terminal"** tab to open the terminal window:

![Serial Terminal](/doc/images/web5.png)

#### Connecting to a Serial Port

Click the **"Connect"** button and a browser popup will appear listing the available serial ports on your computer. Select the desired port (e.g., the miniFPGA board) and click **Connect**:

![Serial Port Selection](/doc/images/web6.png)

Once connected, the status indicator turns green and the terminal displays a confirmation message with a timestamp:

![Serial Terminal Connected](/doc/images/web7.png)

#### Sending Data

Type a command in the input field at the bottom of the terminal and press **Enter** or click the **"Send"** button. Use the **line ending** dropdown (default: **CR+LF**) to choose the line termination characters appended to each message.

#### Terminal Controls

The toolbar at the top of the terminal provides the following controls:

| Button | Description |
|--------|-------------|
| **Connect** / **Disconnect** | Connect to or disconnect from a serial port. The button label changes depending on the connection state. |
| **Pause** | Pause the terminal output. Incoming data is still received but the display stops scrolling. Click again to resume. |
| **Graph** | Open a graphing view to plot numeric data received from the serial port in real time (see [Graph Mode](#graph-mode) below). |
| **X** (Clear Terminal) | Clear all text currently displayed in the terminal window. |
| **Hide Terminal** | Collapse the serial terminal panel to free up screen space. Click the **"Serial Terminal"** tab again to reopen it. |

#### Graph Mode

Click the **"Graph"** button to switch the terminal into a real-time plotting view. The graph plots numeric data received over the serial port in **CSV format** — each line of text represents one data point in time, with comma-separated values mapping to individual channels.

**Expected data format:**

The PIC should send an optional **header line** followed by **numeric data lines**, all in CSV format terminated by a newline:

```
header1,header2,header3\n   ← optional: names for each channel
value1,value2,value3\n      ← numeric data (one sample per line)
value1,value2,value3\n
...
```

The first non-numeric line received is used as the **channel names** displayed in the graph legend. If no header is sent, channels are automatically named `Ch1`, `Ch2`, etc.

> **Tip:** Always send a header line at PIC startup to get meaningful trace names. Without it, UART noise or partial bytes at connection time may produce garbled labels.

**Example — single channel** (e.g., ADC reading):
```
ADC
512
523
531
548
```

**Example — multiple channels** (e.g., temperature and humidity sensors):
```
Temperature,Humidity
23.5,65.2
23.6,64.8
23.7,65.0
23.8,64.5
```

**Example C code** to send plottable data from a PIC:
```c
// Send header once at startup
printf("ADC,Temperature\r\n");

// Then send data periodically
while (1) {
    printf("%d,%d\r\n", adc_value, temperature);
    __delay_ms(100);
}
```

Each comma-separated value is plotted as a separate trace on the graph, allowing you to monitor multiple signals simultaneously. The example below shows two channels — one counting up from 0 to 10 and the other counting down from 10 to 0 — plotted in real time:

![Graph Mode](/doc/images/web8.png)

