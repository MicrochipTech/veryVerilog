# veryVerilog

veryVerilog enables an easy way to program PICs from family **PIC16F180XY** and **PIC16F131XY** over a Web Browser without the need to install any software using a PIC as a USB HiD device. 

The project is based on 3 parts:
- USB-HID PCB (hardware)
- USB-HID Hex File (firmware)
- Web Page to flash Hex files a PIC device attached to the hardware

The PCB looks like this and is based on a [**MCP2221A**](https://www.microchip.com/en-us/product/mcp2221a)

![PCB](/doc/images/pcb.png)

The IC on the left-hand side is the MCP2221 and IC on the right-hand side is PIC to be programmed.

# Program a PIC in 5 Steps

## 1. Insert the Hardware

Plug the USB dongle on your computer.

## 2. Web Page

Open the Web Page [here](http://versatek.com.br/web_icsp)
You will see the veryVerilog Web Page:
![Web1](/doc/images/web1.png)

## 3. Connect to Programmer

Click the button "Connect to Programmer" and select the miniFPGA device:
![Web2](/doc/images/web2.png)

If you have multiple programmers connected, click "identify Programmer" and the LEDs of the connected board will blink for about 2 seconds.

The device is recognized and the Web Page displays information about the connected PIC and the identification of the program already flashed on the device (User Id)

![Web3](/doc/images/web3.png)

## 4. Drag and Drop

Drag a file on the *"Drag and Drop"* area and the program will be transfered automatically to the PIC.

![Web4](/doc/images/web4.png)

Have fun!

