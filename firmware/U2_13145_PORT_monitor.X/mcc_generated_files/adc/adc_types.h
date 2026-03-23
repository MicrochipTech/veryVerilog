/**
 * ADC Type Definitions Header File
 *
 * @file adc_types.h
 *
 * @defgroup adc ADC
 *
 * @brief This file provides type definitions for the ADC module.
 *
 * @version ADC Driver Version 2.0.1
 * 
 * @version ADC Package Version 2.0.2
*/

/*
© [2026] Microchip Technology Inc. and its subsidiaries.

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

#ifndef ADC_TYPES_H
#define	ADC_TYPES_H

// Section: Macros

/**
 @ingroup adc
 @brief ADC resolution value.
*/
#define ADC_RESOLUTION 10U

/**
 @ingroup adc
 @brief ADC bit set value.
*/
#define ADC_BIT_SET 1U

/**
 @ingroup adc
 @brief ADC bit clear value.
*/
#define ADC_BIT_CLEAR 0U

// Section: Types

/**
 @ingroup adc
 @typedef adc_result_t
 @brief ADC conversion result type.
*/
typedef int16_t adc_result_t;

/**
 @ingroup adc
 @typedef adc_accumulate_t
 @brief ADC conversion accumulator type.
*/
typedef int32_t adc_accumulate_t;

/**
 @ingroup adc
 @typedef adc_threshold_t
 @brief ADC conversion threshold type.
*/
typedef int16_t adc_threshold_t;

/**
 @ingroup adc
 @typedef adc_repeat_count_t
 @brief ADC conversion repeat count type.
*/
typedef uint8_t adc_repeat_count_t;

// Section: Enumerations

/**
 * @ingroup adc
 * @enum adc_channel_t
 * @brief Enumeration that describes the available ADC channels.
*/
typedef enum
{
    ADC_CHANNEL_ADCG1 =  0x3, /**< ADCG1*/ 
    ADC_CHANNEL_VSS =  0x3a, /**< VSS*/ 
    ADC_CHANNEL_TEMP =  0x3b, /**< Temp*/ 
    ADC_CHANNEL_DAC1 =  0x3c, /**< DAC1*/ 
    ADC_CHANNEL_FVR_BUF1 =  0x3e, /**< FVR_buf1*/ 
    ADC_CHANNEL_FVR_BUF2 =  0x3f, /**< FVR_buf2*/ 
    ADC_CHANNEL_ANA0 =  0x0, /**< IO_RA0: RA0*/ 
    ADC_CHANNEL_ANA1 =  0x1, /**< IO_RA1: RA1*/ 
    ADC_CHANNEL_ANA2 =  0x2, /**< IO_RA2: RA2*/ 
    ADC_CHANNEL_ANA4 =  0x4, /**< IO_RA4: RA4*/ 
    ADC_CHANNEL_ANB4 =  0xc, /**< IO_RB4: RB4*/ 
    ADC_CHANNEL_ANB5 =  0xd, /**< IO_RB5: RB5*/ 
    ADC_CHANNEL_ANB6 =  0xe, /**< IO_RB6: RB6*/ 
    ADC_CHANNEL_ANB7 =  0xf, /**< IO_RB7: RB7*/ 
    ADC_CHANNEL_ANC1 =  0x11, /**< IO_RC1: RC1*/ 
    ADC_CHANNEL_ANC2 =  0x12, /**< IO_RC2: RC2*/ 
    ADC_CHANNEL_ANC3 =  0x13, /**< IO_RC3: RC3*/ 
    ADC_CHANNEL_ANC4 =  0x14, /**< IO_RC4: RC4*/ 
    ADC_CHANNEL_ANC5 =  0x15, /**< IO_RC5: RC5*/ 
    ADC_CHANNEL_ANC6 =  0x16, /**< IO_RC6: RC6*/ 
    ADC_CHANNEL_ANC7 =  0x17 /**< IO_RC7: RC7*/ 
} adc_channel_t;

/**
 * @ingroup adc
 * @typedef adc_computation_mode_t
 * @brief Enumeration for the ADC computation modes.
 */
typedef enum
{
    ADC_BASIC = 0x0, /**< Basic_mode*/  
    ADC_SERIES_ACCUMULATE = 0x1, /**< Accumulate_mode*/  
    ADC_SERIES_AVERAGE = 0x2, /**< Average_mode*/  
    ADC_BURST_AVERAGE = 0x3, /**< Burst_average_mode*/  
    ADC_LOW_PASS_FILTER = 0x4 /**< Low_pass_filter_mode*/  
} adc_computation_mode_t;

/**
 * @ingroup adc
 * @typedef adc_calculation_mode_t
 * @brief Enumeration for the ADC error calculation modes.
 */
typedef enum
{
    ADC_FIRST_DERIVATIVE_OF_SINGLE_MEASUREMENT = 0x0, /**< First Derivative Of Single Measurement*/  
    ADC_ACTUAL_RESULT_VS_SETPOINT = 0x1, /**< Actual Result Vs Setpoint*/  
    ADC_ACTUAL_RESULT_VS_FILTERED_VALUE = 0x2, /**< Actual Result Vs Filtered Value*/  
    ADC_FIRST_DERIVATIVE_OF_FILTERED_VALUE = 0x4, /**< First Derivative Of Filtered Value*/  
    ADC_FILTERED_VALUE_VS_SETPOINT = 0x5 /**< Filtered Value Vs Setpoint*/  
} adc_calculation_mode_t;

/**
 * @ingroup adc
 * @typedef adc_threshold_mode_t 
 * @brief Enumeration for the ADC threshold modes.
 */
typedef enum
{
    ADC_NEVER_INTERRUPT = 0x0, /**< Disabled*/  
    ADC_BELOW_LOWER_THRESHOLD = 0x1, /**< ADERR < ADLTH*/  
    ADC_ABOVE_LOWER_THRESHOLD = 0x2, /**< ADERR >= ADLTH*/  
    ADC_INSIDE_LOWER_AND_UPPER_THRESHOLD = 0x3, /**< ADERR > ADLTH And ADERR < ADUTH*/  
    ADC_OUTSIDE_LOWER_AND_UPPER_THRESHOLD = 0x4, /**< ADERR < ADLTH Or ADERR > ADUTH*/  
    ADC_BELOW_UPPER_THRESHOLD = 0x5, /**< ADERR <= ADUTH*/  
    ADC_ABOVE_UPPER_THRESHOLD = 0x6, /**< ADERR > ADUTH*/  
    ADC_ALWAYS_INTERRUPT = 0x7 /**< Enabled*/  
} adc_threshold_mode_t ;

/**
 * @ingroup adc
 * @typedef adc_trigger_source_t 
 * @brief Enumeration for the ADC auto-trigger sources.
 */
typedef enum
{
    ADC_TRIGGER_SOURCE_DISABLED = 0x0, /**< Disabled*/
    ADC_TRIGGER_SOURCE_ADACTPPS = 0x1, /**< ADACTPPS*/
    ADC_TRIGGER_SOURCE_TMR0 = 0x2, /**< TMR0*/
    ADC_TRIGGER_SOURCE_TMR1 = 0x3, /**< TMR1*/
    ADC_TRIGGER_SOURCE_TMR2 = 0x4, /**< TMR2*/
    ADC_TRIGGER_SOURCE_CCP1 = 0x8, /**< CCP1*/
    ADC_TRIGGER_SOURCE_CCP2 = 0x9, /**< CCP2*/
    ADC_TRIGGER_SOURCE_C1OUT = 0x13, /**< C1OUT*/
    ADC_TRIGGER_SOURCE_C2OUT = 0x14, /**< C2OUT*/
    ADC_TRIGGER_SOURCE_IOC = 0x15, /**< IOC*/
    ADC_TRIGGER_SOURCE_CLC1 = 0x16, /**< CLC1*/
    ADC_TRIGGER_SOURCE_CLC2 = 0x17, /**< CLC2*/
    ADC_TRIGGER_SOURCE_CLC3 = 0x18, /**< CLC3*/
    ADC_TRIGGER_SOURCE_CLC4 = 0x19, /**< CLC4*/
    ADC_TRIGGER_SOURCE_ADERRH = 0x1d, /**< ADERRH*/
    ADC_TRIGGER_SOURCE_ADRESH = 0x1e, /**< ADRESH*/
    ADC_TRIGGER_SOURCE_ADPCH = 0x1f, /**< ADPCH*/
    ADC_TRIGGER_SOURCE_PWM1OUT = 0x20, /**< PWM1OUT*/
    ADC_TRIGGER_SOURCE_PWM2OUT = 0x21, /**< PWM2OUT*/
    ADC_TRIGGER_SOURCE_CLB_BLE0 = 0x22, /**< CLB_BLE[0]*/
    ADC_TRIGGER_SOURCE_CLB_BLE1 = 0x23, /**< CLB_BLE[1]*/
    ADC_TRIGGER_SOURCE_CLB_BLE2 = 0x24, /**< CLB_BLE[2]*/
    ADC_TRIGGER_SOURCE_CLB_BLE3 = 0x25, /**< CLB_BLE[3]*/
    ADC_TRIGGER_SOURCE_CLB_BLE4 = 0x26, /**< CLB_BLE[4]*/
    ADC_TRIGGER_SOURCE_CLB_BLE5 = 0x27, /**< CLB_BLE[5]*/
    ADC_TRIGGER_SOURCE_CLB_BLE6 = 0x28, /**< CLB_BLE[6]*/
    ADC_TRIGGER_SOURCE_CLB_BLE7 = 0x29 /**< CLB_BLE[7]*/
} adc_trigger_source_t ;

/**
 * @ingroup adc
 * @typedef adc_conversion_stage_t
 * @brief Enumeration for the ADC conversion stage statuses.
 */
typedef enum
{
    ADC_NOT_CONVERTING = 0x0, /**< Not Converting*/  
    ADC_1ST_PRECHARGE = 0x1, /**< 1st Precharge*/  
    ADC_1ST_ACQUISITION = 0x2, /**< 1st Acquisition*/  
    ADC_1ST_CONVERSION = 0x3, /**< 1st Conversion*/  
    ADC_COMPUTATION_SUSPENDED_BETWEEN_1ST_AND_2ND_SAMPLE = 0x4, /**< Computation Suspended Between 1st And 2nd Sample*/  
    ADC_2ND_PRECHARGE = 0x5, /**< 2nd Precharge*/  
    ADC_2ND_ACQUISITION = 0x6, /**< 2nd Acquisition*/  
    ADC_2ND_CONVERSION = 0x7 /**< 2nd Conversion*/  
} adc_conversion_stage_t;

#endif // ADC_TYPES_H