

 
/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

/********************************************************************
-usb_descriptors.c-
-------------------------------------------------------------------
Filling in the descriptor values in the usb_descriptors.c file:
-------------------------------------------------------------------

[Device Descriptors]
The device descriptor is defined as a USB_DEVICE_DESCRIPTOR type.  
This type is defined in usb_ch9.h  Each entry into this structure
needs to be the correct length for the data type of the entry.

[Configuration Descriptors]
The configuration descriptor was changed in v2.x from a structure
to a uint8_t array.  Given that the configuration is now a byte array
each byte of multi-byte fields must be listed individually.  This
means that for fields like the total size of the configuration where
the field is a 16-bit value "64,0," is the correct entry for a
configuration that is only 64 bytes long and not "64," which is one
too few bytes.

The configuration attribute must always have the _DEFAULT
definition at the minimum. Additional options can be ORed
to the _DEFAULT attribute. Available options are _SELF and _RWU.
These definitions are defined in the usb_device.h file. The
_SELF tells the USB host that this device is self-powered. The
_RWU tells the USB host that this device supports Remote Wakeup.

[Endpoint Descriptors]
Like the configuration descriptor, the endpoint descriptors were 
changed in v2.x of the stack from a structure to a uint8_t array.  As
endpoint descriptors also has a field that are multi-byte entities,
please be sure to specify both bytes of the field.  For example, for
the endpoint size an endpoint that is 64 bytes needs to have the size
defined as "64,0," instead of "64,"

Take the following example:
    // Endpoint Descriptor //
    0x07,                       //the size of this descriptor //
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    _EP02_IN,                   //EndpointAddress
    _INT,                       //Attributes
    0x08,0x00,                  //size (note: 2 bytes)
    0x02,                       //Interval

The first two parameters are self-explanatory. They specify the
length of this endpoint descriptor (7) and the descriptor type.
The next parameter identifies the endpoint, the definitions are
defined in usb_device.h and has the following naming
convention:
_EP<##>_<dir>
where ## is the endpoint number and dir is the direction of
transfer. The dir has the value of either 'OUT' or 'IN'.
The next parameter identifies the type of the endpoint. Available
options are _BULK, _INT, _ISO, and _CTRL. The _CTRL is not
typically used because the default control transfer endpoint is
not defined in the USB descriptors. When _ISO option is used,
addition options can be ORed to _ISO. Example:
_ISO|_AD|_FE
This describes the endpoint as an isochronous pipe with adaptive
and feedback attributes. See usb_device.h and the USB
specification for details. The next parameter defines the size of
the endpoint. The last parameter in the polling interval.

-------------------------------------------------------------------
Adding a USB String
-------------------------------------------------------------------
A string descriptor array should have the following format:

rom struct{byte bLength;byte bDscType;word string[size];}sdxxx={
sizeof(sdxxx),DSC_STR,<text>};

The above structure provides a means for the C compiler to
calculate the length of string descriptor sdxxx, where xxx is the
index number. The first two bytes of the descriptor are descriptor
length and type. The rest <text> are string texts which must be
in the unicode format. The unicode format is achieved by declaring
each character as a word type. The whole text string is declared
as a word array with the number of characters equals to <size>.
<size> has to be manually counted and entered into the array
declaration. Let's study this through an example:
if the string is "USB" , then the string descriptor should be:
(Using index 02)
rom struct{byte bLength;byte bDscType;word string[3];}sd002={
sizeof(sd002),DSC_STR,'U','S','B'};

A USB project may have multiple strings and the firmware supports
the management of multiple strings through a look-up table.
The look-up table is defined as:
rom const unsigned char *rom USB_SD_Ptr[]={&sd000,&sd001,&sd002};

The above declaration has 3 strings, sd000, sd001, and sd002.
Strings can be removed or added. sd000 is a specialized string
descriptor. It defines the language code, usually this is
US English (0x0409). The index of the string must match the index
position of the USB_SD_Ptr array, &sd000 must be in position
USB_SD_Ptr[0], &sd001 must be in position USB_SD_Ptr[1] and so on.
The look-up table USB_SD_Ptr is used by the get string handler
function.

-------------------------------------------------------------------

The look-up table scheme also applies to the configuration
descriptor. A USB device may have multiple configuration
descriptors, i.e. CFG01, CFG02, etc. To add a configuration
descriptor, user must implement a structure similar to CFG01.
The next step is to add the configuration descriptor name, i.e.
cfg01, cfg02,.., to the look-up table USB_CD_Ptr. USB_CD_Ptr[0]
is a dummy place holder since configuration 0 is the un-configured
state according to the definition in the USB specification.

********************************************************************/
 
/*********************************************************************
 * Descriptor specific type definitions are defined in:
 * usb_device.h
 *
 * Configuration options are defined in:
 * usb_device_config.h
 ********************************************************************/
#ifndef __USB_DESCRIPTORS_C
#define __USB_DESCRIPTORS_C
 
/** INCLUDES *******************************************************/
#include "usb.h"
#include "usb_device_cdc.h"
#include "usb_device_hid.h"
#include "../../version.h"

/** CONSTANTS ******************************************************/
#if defined(__18CXX)
#pragma romdata
#endif

#define LOB(x)  (x & 0xFF)
#define HIB(x)  ((x >> 8) & 0xFF)
#define WORDVAL(x) LOB(x), HIB(x)

/* Device Descriptor */
const USB_DEVICE_DESCRIPTOR device_dsc=
{
    sizeof(USB_DEVICE_DESCRIPTOR),// Size of this descriptor in bytes
    USB_DESCRIPTOR_DEVICE,  // DEVICE descriptor type
    0x0200,                 // USB Spec Release Number in BCD format
    0xEF,//CDC_DEVICE,             // Class Code
    0x02,                   // Subclass code
    0x01,                   // Protocol code
    USB_EP0_BUFF_SIZE,      // Max packet size for EP0, see usb_device_config.h
    0x04D8,                 // Vendor ID
    0xE594,                 // Product ID
    USB_FW_VERSION,         // Device release number in BCD format
    0x01,                   // Manufacturer string index
    0x02,                   // Product string index
    0x00,                   // Device serial number string index
    0x01                    // Number of possible configurations
};

/* Configuration 1 Descriptor */
const uint8_t configDescriptor1[]={
    /* Configuration Descriptor */
    sizeof(USB_CONFIGURATION_DESCRIPTOR),// Size of this descriptor in bytes
    USB_DESCRIPTOR_CONFIGURATION,  // CONFIGURATION descriptor type
    8+67+32,0,                     // Total length of data for this cfg
    2+1,                           // Number of interfaces in this cfg
    1,                             // Index value of this configuration
    0,                             // Configuration string index
    _DEFAULT,                      // Attributes, see usb_device.h
    50,                            // Max power consumption (2X mA)
    
	// <editor-fold defaultstate="collapsed" desc="CDC interface descriptors">						
	// <editor-fold defaultstate="collapsed" desc="Interface Association Descriptor">						
    sizeof(USB_IAD_DESCRIPTOR),     // Size of this descriptor in bytes = 9
    USB_DESCRIPTOR_IAD,         // INTERFACE ASSOCIATION descriptor (IAD) type
    0,                          // Interface 0
    0x02,                       // 2 interfaces
    0x02,                       // Communications and CDC control
    0x02,                       // Function Subclass 2
    0x01,                       // Function protocol 1
    0x00,                       // No function string descriptor
    
    // </editor-fold>
	// <editor-fold defaultstate="collapsed" desc="CDC control interface">						
    /* CDC Comm control Interface Descriptor */
    sizeof(USB_INTERFACE_DESCRIPTOR),// Size of this descriptor in bytes = 9
    USB_DESCRIPTOR_INTERFACE,   // INTERFACE descriptor type
    CDC_COMM_INTF_ID,           // Interface Number
    0,                          // Alternate Setting Number
    1,                          // Number of endpoints in this intf
    COMM_INTF,                  // Class code
    ABSTRACT_CONTROL_MODEL,     // Subclass code
    V25TER,                     // Protocol code
    0,                          // Interface string index

    /* CDC Class-Specific Descriptors */
    sizeof(USB_CDC_HEADER_FN_DSC),// Size of this descriptor in bytes = 6
    CS_INTERFACE,
    DSC_FN_HEADER,
    0x10,0x01,

    sizeof(USB_CDC_ACM_FN_DSC), // Size of this descriptor in bytes = 4
    CS_INTERFACE,
    DSC_FN_ACM,
    USB_CDC_ACM_FN_DSC_VAL,

    sizeof(USB_CDC_UNION_FN_DSC),// Size of this descriptor in bytes = 5
    CS_INTERFACE,
    DSC_FN_UNION,
    CDC_COMM_INTF_ID,
    CDC_DATA_INTF_ID,

    sizeof(USB_CDC_CALL_MGT_FN_DSC),// Size of this descriptor in bytes = 5
    CS_INTERFACE,
    DSC_FN_CALL_MGT,
    0x00,
    CDC_DATA_INTF_ID,

    /* Endpoint Descriptor */
    sizeof(USB_ENDPOINT_DESCRIPTOR),// Size of this descriptor in bytes = 7
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor
    CDC_COMM_EP | _EP_IN,       // EndpointAddress
    _INTERRUPT,                 // Attributes
    WORDVAL(CDC_COMM_IN_EP_SIZE),// size
    0x02,                       // Interval
	
    // </editor-fold>
    // <editor-fold defaultstate="collapsed" desc="CDC data interface">						
    /* CDC Data Interface Descriptor */
    sizeof(USB_INTERFACE_DESCRIPTOR),// Size of this descriptor in bytes = 9
    USB_DESCRIPTOR_INTERFACE,   // INTERFACE descriptor type
    CDC_DATA_INTF_ID,           // Interface Number
    0,                          // Alternate Setting Number
    2,                          // Number of endpoints in this intf
    DATA_INTF,                  // Class code
    0,                          // Subclass code
    NO_PROTOCOL,                // Protocol code
    0,                          // Interface string index
    
    /* Endpoint Descriptor */
    sizeof(USB_ENDPOINT_DESCRIPTOR),// Size of this descriptor in bytes = 7
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor
    CDC_DATA_EP | _EP_OUT,      // EndpointAddress
    _BULK,                      // Attributes
    WORDVAL(CDC_DATA_OUT_EP_SIZE),// size
    0x00,                       // Interval

    /* Endpoint Descriptor */
    sizeof(USB_ENDPOINT_DESCRIPTOR),// Size of this descriptor in bytes = 7
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor
    CDC_DATA_EP | _EP_IN,       // EndpointAddress
    _BULK,                      // Attributes
    WORDVAL(CDC_DATA_IN_EP_SIZE),// size
    0x00,                       // Interval
    // </editor-fold>   
    // </editor-fold>
	// <editor-fold defaultstate="collapsed" desc="HID interface descriptors">
    /* HID interface descriptor */
    sizeof(USB_INTERFACE_DESCRIPTOR),// Size of this descriptor in bytes = 9
    USB_DESCRIPTOR_INTERFACE,   // INTERFACE descriptor type
    HID_INTF_ID,                // Interface Number
    0,                          // Alternate Setting Number
    2,                          // Number of endpoints in this intf
    HID_INTF,                   // Class code
    HID_NO_SUBCLASS,            // Subclass code
    HID_PROTOCOL_NONE,          // Protocol code
    0,                          // Interface string index

    /* HID Class-Specific Descriptor */
    sizeof(USB_HID_DSC)+sizeof(USB_HID_DSC_HEADER)*HID_NUM_OF_DSC,// Size of this descriptor in bytes = 6+3*HID_NUM_OF_DSC
    DSC_HID,                    // HID descriptor type
    0x11,0x01,                  // HID Spec Release Number in BCD format (1.11)
    0x00,                       // Country Code (0x00 for Not supported)
    HID_NUM_OF_DSC,             // Number of class descriptors, see usb_device_config.h
    DSC_RPT,                    // Report descriptor type
    /* Report descriptor list */
    WORDVAL(HID_RPT01_SIZE),    // Size of the report descriptor#1
    
    /* Endpoint Descriptor */
    sizeof(USB_ENDPOINT_DESCRIPTOR),// Size of this descriptor in bytes = 7
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    CUSTOM_DEVICE_HID_EP | _EP_IN,//EndpointAddress
    _INTERRUPT,                 //Attributes
    WORDVAL(HID_INT_IN_EP_SIZE),//size in bytes
    0x01,                       //Interval

    /* Endpoint Descriptor */
    sizeof(USB_ENDPOINT_DESCRIPTOR),// Size of this descriptor in bytes = 7
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    CUSTOM_DEVICE_HID_EP | _EP_OUT,                   //EndpointAddress
    _INTERRUPT,                 //Attributes
    WORDVAL(HID_INT_OUT_EP_SIZE),//size in bytes
    0x01,                       //Interval
    // </editor-fold>    
};

//Language code string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[1];}sd000={
sizeof(sd000),USB_DESCRIPTOR_STRING,{0x0409}};

//Manufacturer string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[25];}sd001={
sizeof(sd001),USB_DESCRIPTOR_STRING,
{'M','i','c','r','o','c','h','i','p',' ','T','e','c','h','n','o','l','o','g','y',' ','I','n','c','.'}
};

//Product string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[14];}sd002={
sizeof(sd002),USB_DESCRIPTOR_STRING,
{'m','i','n','i','F','P','G','A',' ','b','o','a','r','d'}
};

//Array of configuration descriptors
const uint8_t *const USB_CD_Ptr[]=
{
    (const uint8_t *const)&configDescriptor1
};

//Array of string descriptors
const uint8_t *const USB_SD_Ptr[USB_NUM_STRING_DESCRIPTORS]=
{
    (const uint8_t *const)&sd000,
    (const uint8_t *const)&sd001,
    (const uint8_t *const)&sd002
};

//Class specific descriptor - HID 
const struct{uint8_t report[HID_RPT01_SIZE];}hid_rpt01={
{
    0x06, 0x00, 0xFF,       // Usage Page = 0xFF00 (Vendor Defined Page 1)
    0x09, 0x01,             // Usage (Vendor Usage 1)
    0xA1, 0x01,             // Collection (Application)
    0x15, 0x00,             //      Logical Minimum (data bytes in the report may have minimum value = 0x00)
    0x26, 0xFF, 0x00,       //      Logical Maximum (data bytes in the report may have maximum value = 0x00FF = unsigned 255)
    0x75, 0x08,             //      Report Size: 8-bit field size
    0x95, 0x40,             //      Report Count: Make sixty-four 8-bit fields (the next time the parser hits an "Input", "Output", or "Feature" item)
    0x19, 0x01,             //      Usage Minimum 
    0x29, 0x40,             //      Usage Maximum   //64 input usages total (0x01 to 0x40)
    0x81, 0x00,             //      Input (Data, Array, Abs): Instantiates input packet fields based on the above report size, count, logical min/max, and usage.
    0x19, 0x01,             //      Usage Minimum 
    0x29, 0x40,             //      Usage Maximum 	//64 output usages total (0x01 to 0x40)
    0x91, 0x00,             //      Output (Data, Array, Abs): Instantiates output packet fields.  Uses same report size and count as "Input" fields, since nothing new/different was specified to the parser since the "Input" item.
    0x95, USB_EP0_BUFF_SIZE,//      Report Count:  	- Must fit into EP0
    0x19, 0x01,             //      Usage Minimum 
    0x29, USB_EP0_BUFF_SIZE,//      Usage Maximum 	- Must fit into EP0
    0xB1, 0x00,             //      Feature (Data, Array, Abs): Instantiates feature packet fields.  Uses same report size and count as "Input" fields, since nothing new/different was specified to the parser since the "Input" item.
    0xC0}                   // End Collection
};                  

#if defined(__18CXX)
    #pragma code
#endif

#endif
/** EOF usb_descriptors.c ****************************************************/
