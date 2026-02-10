/**
 * ADC Generated Driver File
 * 
 * @file adc.c
 * 
 * @ingroup  adc
 * 
 * @brief This file contains the API implementations for the ADC driver.
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

#include <xc.h>
#include "../adc.h"
#include "../adc_types.h"


static void (*ADC_ConversionDoneCallback)(void);
static void (*ADC_ThresholdCallback)(void);

void ADC_Initialize(void)
{
    ADCON0bits.ADON = ADC_BIT_CLEAR;    
    ADCON0 = (uint8_t)(1U << _ADCON0_ADON_POSITION)	/* ADON enabled(1) */
			|(0U << _ADCON0_ADCONT_POSITION)	/* ADCONT disabled(0) */
			|(1U << _ADCON0_ADCS_POSITION)	/* ADCS ADCRC(1) */
			|(1U << _ADCON0_ADFM_POSITION)	/* ADFM right(1) */
			|(0U << _ADCON0_GO_nDONE_POSITION);	/* GO_nDONE stop(0) */
    ADCON1 = (uint8_t)(1U << _ADCON1_ADDSEN_POSITION)	/* ADDSEN enabled(1) */
			|(0U << _ADCON1_PCSC_POSITION)	/* PCSC internal sampling capacitor and external i/o pin(0) */
			|(0U << _ADCON1_ADGPOL_POSITION)	/* ADGPOL digital_low(0) */
			|(0U << _ADCON1_ADIPEN_POSITION)	/* ADIPEN disabled(0) */
			|(0U << _ADCON1_ADPPOL_POSITION);	/* ADPPOL VSS(0) */
    ADCON2 = (uint8_t)(0U << _ADCON2_ADMD_POSITION)	/* ADMD Basic_mode(0) */
			|(1U << _ADCON2_ADACLR_POSITION)	/* ADACLR enabled(1) */
			|(1U << _ADCON2_ADCRS_POSITION)	/* ADCRS 0x1(1) */
			|(0U << _ADCON2_ADPSIS_POSITION);	/* ADPSIS ADRES(0) */
    ADCON3 = (uint8_t)(0U << _ADCON3_ADTMD_POSITION)	/* ADTMD disabled(0) */
			|(0U << _ADCON3_ADSOI_POSITION)	/* ADSOI ADGO not cleared(0) */
			|(1U << _ADCON3_ADCALC_POSITION);	/* ADCALC Actual result vs setpoint(1) */
    ADCLK = (uint8_t)(15U << _ADCLK_ADCS_POSITION);	/* ADCS FOSC/32(15) */
    ADCG1A = (uint8_t)(0U << _ADCG1A_CGA0_POSITION)	/* CGA0 disabled(0) */
			|(0U << _ADCG1A_CGA1_POSITION)	/* CGA1 disabled(0) */
			|(0U << _ADCG1A_CGA2_POSITION)	/* CGA2 disabled(0) */
			|(0U << _ADCG1A_CGA4_POSITION)	/* CGA4 disabled(0) */
			|(0U << _ADCG1A_CGA5_POSITION);	/* CGA5 disabled(0) */
    ADCG1B = (uint8_t)(0U << _ADCG1B_CGB4_POSITION)	/* CGB4 disabled(0) */
			|(0U << _ADCG1B_CGB5_POSITION)	/* CGB5 disabled(0) */
			|(0U << _ADCG1B_CGB6_POSITION)	/* CGB6 disabled(0) */
			|(0U << _ADCG1B_CGB7_POSITION);	/* CGB7 disabled(0) */
    ADCG1C = (uint8_t)(0U << _ADCG1C_CGC0_POSITION)	/* CGC0 disabled(0) */
			|(0U << _ADCG1C_CGC1_POSITION)	/* CGC1 disabled(0) */
			|(0U << _ADCG1C_CGC2_POSITION)	/* CGC2 disabled(0) */
			|(0U << _ADCG1C_CGC3_POSITION)	/* CGC3 disabled(0) */
			|(0U << _ADCG1C_CGC4_POSITION)	/* CGC4 disabled(0) */
			|(0U << _ADCG1C_CGC5_POSITION)	/* CGC5 disabled(0) */
			|(0U << _ADCG1C_CGC6_POSITION);	/* CGC6 disabled(0) */
    ADLTHL = (uint8_t)(0U << _ADLTHL_LTH_POSITION);	/* LTH 0x0(0) */
    ADLTHH = (uint8_t)(0U << _ADLTHH_LTH_POSITION);	/* LTH 0x0(0) */
    ADUTHL = (uint8_t)(0U << _ADUTHL_UTH_POSITION);	/* UTH 0x0(0) */
    ADUTHH = (uint8_t)(0U << _ADUTHH_UTH_POSITION);	/* UTH 0x0(0) */
    ADSTPTL = (uint8_t)(0U << _ADSTPTL_STPT_POSITION);	/* STPT 0x0(0) */
    ADSTPTH = (uint8_t)(0U << _ADSTPTH_STPT_POSITION);	/* STPT 0x0(0) */
    ADACCU = (uint8_t)(0U << _ADACCU_ACC_POSITION);	/* ACC 0x0(0) */
    ADACCH = (uint8_t)(0U << _ADACCH_ACC_POSITION);	/* ACC 0x0(0) */
    ADACCL = (uint8_t)(0U << _ADACCL_ACC_POSITION);	/* ACC 0x0(0) */
    ADRESH = (uint8_t)(0U << _ADRESH_ADRES_POSITION);	/* ADRES 0x0(0) */
    ADRESL = (uint8_t)(0U << _ADRESL_ADRES_POSITION);	/* ADRES 0x0(0) */
    ADCNT = (uint8_t)(0U << _ADCNT_ADCNT_POSITION);	/* ADCNT 0x0(0) */ 
    ADRPT = (uint8_t)(0U << _ADRPT_ADRPT_POSITION);	/* ADRPT 0x0(0) */       
    ADCAP = (uint8_t)(0U << _ADCAP_CAP_POSITION);	/* CAP Additional uC disabled(0) */
    ADPCH = (uint8_t)(0U << _ADPCH_PCH_POSITION);	/* PCH ANA0(0) */ 
    ADACQL = (uint8_t)(0U << _ADACQL_ACQ_POSITION);	/* ACQ 0x0(0) */  
    ADACQH = (uint8_t)(0U << _ADACQH_ACQ_POSITION);	/* ACQ 0x0(0) */ 
    ADPREL = (uint8_t)(0U << _ADPREL_PRE_POSITION);	/* PRE 0x0(0) */ 
    ADPREH = (uint8_t)(0U << _ADPREH_PRE_POSITION);	/* PRE 0x0(0) */
    ADREF = (uint8_t)(0U << _ADREF_ADPREF_POSITION);	/* ADPREF VDD(0) */    
    ADACT = (uint8_t)(0U << _ADACT_ADACT_POSITION);	/* ADACT disabled(0) */ 
    ADSTAT =(uint8_t)(0U << _ADSTAT_ADMATH_POSITION);	/* ADMATH registers not updated(0) */   
    
    ADC_ConversionDoneCallback = NULL;
    ADC_ThresholdCallback = NULL;
    
    PIR6bits.ADIF = ADC_BIT_CLEAR;
    PIE6bits.ADIE = ADC_BIT_CLEAR;
    PIR6bits.ADTIF = ADC_BIT_CLEAR;
    PIE6bits.ADTIE = ADC_BIT_CLEAR;
    ADCON0 = (uint8_t)(1U << _ADCON0_ADON_POSITION)	/* ADON enabled(1) */
			|(0U << _ADCON0_ADCONT_POSITION)	/* ADCONT disabled(0) */
			|(1U << _ADCON0_ADCS_POSITION)	/* ADCS ADCRC(1) */
			|(1U << _ADCON0_ADFM_POSITION)	/* ADFM right(1) */
			|(0U << _ADCON0_GO_nDONE_POSITION);	/* GO_nDONE stop(0) */
}

void ADC_Deinitialize(void)
{
    ADCON0 = (uint8_t)0x0;
    PIR6bits.ADIF = ADC_BIT_CLEAR;
    PIE6bits.ADIE = ADC_BIT_CLEAR;
    PIR6bits.ADTIF = ADC_BIT_CLEAR;
    PIE6bits.ADTIE = ADC_BIT_CLEAR;
    ADCON1 = (uint8_t)0x0;
    ADCON2 = (uint8_t)0x0;
    ADCON3 = (uint8_t)0x0;
    ADCLK = (uint8_t)0x0;
    ADCG1A = (uint8_t)0x0;
    ADCG1B = (uint8_t)0x0;    
    ADCG1C = (uint8_t)0x0;
    ADLTHL = (uint8_t)0x0;
    ADLTHH = (uint8_t)0x0;
    ADUTHL = (uint8_t)0x0;
    ADUTHH = (uint8_t)0x0;
    ADSTPTL = (uint8_t)0x0;
    ADSTPTH = (uint8_t)0x0;
    ADACCU = (uint8_t)0x0;
    ADACCH = (uint8_t)0x0;
    ADACCL = (uint8_t)0x0;
    ADRESH = (uint8_t)0x0;
    ADRESL = (uint8_t)0x0;
    ADCNT = (uint8_t)0x0;
    ADRPT = (uint8_t)0x0;
    ADCAP = (uint8_t)0x0;
    ADPCH = (uint8_t)0x0;
    ADACQL = (uint8_t)0x0;
    ADACQH = (uint8_t)0x0;
    ADPREL = (uint8_t)0x0;
    ADPREH = (uint8_t)0x0;
    ADREF = (uint8_t)0x0;
    ADACT = (uint8_t)0x0;
    ADSTAT = (uint8_t)0x0;
}

void ADC_Enable(void)
{
    ADCON0bits.ADON = ADC_BIT_SET;
}

void ADC_Disable(void)
{
    ADCON0bits.ADON = ADC_BIT_CLEAR;
}

void ADC_ChannelSelect(adc_channel_t channel)
{
   ADPCH = (unsigned char)channel;  
}

void ADC_ConversionStart(void)
{
    ADCON0bits.GO_nDONE = ADC_BIT_SET;
}

bool ADC_IsConversionDone(void)
{
    return ((bool)(!ADCON0bits.GO_nDONE));
}

void ADC_ConversionStop(void)
{
    ADCON0bits.GO_nDONE = ADC_BIT_CLEAR;
}

adc_result_t ADC_ConversionResultGet(void)
{
    uint16_t readValLow = (uint16_t)ADRESL;
    uint16_t readValHigh = (uint16_t)((uint16_t)ADRESH << 8U);
    uint16_t conversionResult = readValHigh | readValLow;
    return (adc_result_t)conversionResult;
}

void ADC_ComputationModeSet(adc_computation_mode_t computationMode)
{
    ADCON2bits.ADMD = (unsigned char)computationMode;
}

void ADC_ThresholdModeSet(adc_threshold_mode_t thresholdMode)
{
    ADCON3bits.ADTMD = (unsigned char)thresholdMode;
}

void ADC_SampleRepeatCountSet(adc_repeat_count_t repeatCount)
{
    ADRPT = repeatCount;
}

void ADC_UpperThresholdSet(adc_threshold_t upperThreshold)
{
    ADUTHH = (uint8_t) ((uint16_t)upperThreshold >> 8U);
    ADUTHL = (uint8_t) upperThreshold;
}

void ADC_LowerThresholdSet(adc_threshold_t lowerThreshold)
{
    ADLTHH = (uint8_t) ((uint16_t)lowerThreshold >> 8U);
    ADLTHL = (uint8_t) lowerThreshold;
}

adc_accumulate_t ADC_AccumulatedResultGet(void)
{
    return (adc_accumulate_t)(((uint32_t)ADACCU << 16U)+((uint32_t)ADACCH << 8U) + ADACCL);
}

adc_result_t ADC_ChannelSelectAndConvert(adc_channel_t channel)
{
    ADPCH = (unsigned char)channel;  
    uint16_t readValLow;
    uint16_t readValHigh;
    uint16_t conversionResult;
    ADCON0bits.ADCONT = ADC_BIT_CLEAR;

    ADCON0bits.GO_nDONE = ADC_BIT_SET;

    while (1U == ADCON0bits.GO_nDONE)
    {
    }

    readValLow = (uint16_t)ADRESL;
    readValHigh = (uint16_t)((uint16_t)ADRESH << 8U);
    conversionResult = readValHigh | readValLow;
    return (adc_result_t)conversionResult;
}

void ADC_StopOnInterruptEnable(void)
{
    ADCON3bits.ADSOI = ADC_BIT_SET;
}

void ADC_StopOnInterruptDisable(void)
{
    ADCON3bits.ADSOI = ADC_BIT_CLEAR;
}

void ADC_SampleCapacitorDischarge(void)
{
     ADPCH = (unsigned char)ADC_CHANNEL_VSS;
}

void ADC_AcquisitionTimeSet(uint16_t acquisitionValue)
{
    ADACQH = (uint8_t)(acquisitionValue >> 8U);
    ADACQL = (uint8_t)(acquisitionValue);
}

void ADC_PrechargeTimeSet(uint16_t prechargeTime)
{
    ADPREH = (uint8_t)(prechargeTime >> 8U);
    ADPREL = (uint8_t)(prechargeTime);
}

adc_repeat_count_t ADC_CurrentConversionCountGet(void)
{
    return ((adc_repeat_count_t)ADCNT);
}

void ADC_AccumulatorClear(void)
{
    ADCON2bits.ADACLR = ADC_BIT_SET;
}

bool ADC_IsAccumulatorClearComplete(void)
{
    return ((bool)(!ADCON2bits.ADACLR));
}

bool ADC_HasAccumulatorOverflowed(void)
{
    return ADSTATbits.ADAOV;
}

adc_result_t ADC_FilterValueGet(void)
{
    uint16_t readValLow = (uint16_t)ADFLTRL;
    uint16_t readValHigh = (uint16_t)((uint16_t)ADFLTRH << 8U);
    uint16_t filterValue = readValHigh | readValLow;
    return (adc_result_t)filterValue;
}

adc_result_t ADC_PreviousResultGet(void)
{
    uint16_t readValLow = (uint16_t)ADPREVL;
    uint16_t readValHigh = (uint16_t)((uint16_t)ADPREVH << 8U);
    uint16_t previousResultValue = readValHigh | readValLow;
    return (adc_result_t)previousResultValue;
}

void ADC_SetPointDefine(adc_threshold_t setPoint)
{
    ADSTPTH = (uint8_t) ((uint16_t)setPoint >> 8U);
    ADSTPTL = (uint8_t) setPoint;
}

adc_result_t ADC_ErrorCalculationGet(void)
{
    uint16_t readValLow = (uint16_t)ADERRL;
    uint16_t readValHigh = (uint16_t)((uint16_t)ADERRH << 8U);
    uint16_t errorValue = readValHigh | readValLow;
    return (adc_result_t)errorValue;
}

void ADC_DoubleSamplingEnable(void)
{
    ADCON1bits.ADDSEN = ADC_BIT_SET;
}

void ADC_DoubleSamplingDisable(void)
{
    ADCON1bits.ADDSEN = ADC_BIT_CLEAR;
}

void ADC_ContinuousConversionEnable(void)
{
    ADCON0bits.ADCONT = ADC_BIT_SET;
}

void ADC_ContinuousConversionDisable(void)
{
    ADCON0bits.ADCONT = ADC_BIT_CLEAR;
}

bool ADC_IsErrorGreaterThanUpperThreshold(void)
{
    return ADSTATbits.ADUTHR;
}

bool ADC_IsErrorLesserThanLowerThreshold(void)
{
    return ADSTATbits.ADLTHR;
}

adc_conversion_stage_t ADC_ConversionStageStatusGet(void)
{
    return (adc_conversion_stage_t)ADSTATbits.ADSTAT;
}

void ADC_ErrorCalculationModeSet(adc_calculation_mode_t errorCalculationMode)
{
    ADCON3bits.ADCALC = (unsigned char)errorCalculationMode;    
}  

void ADC_CalculationRightShiftSet(uint8_t rightShiftValue)
{
    ADCON2bits.ADCRS = rightShiftValue;
}

void ADC_ThresholdInterruptFlagClear(void)
{
    PIR6bits.ADTIF = ADC_BIT_CLEAR;
}

bool ADC_IsThresholdInterruptFlagSet(void)
{
    return PIR6bits.ADTIF;
}

void ADC_AutoTriggerSourceSet(adc_trigger_source_t triggerSource)
{
    ADACT = (unsigned char)triggerSource;
}

uint8_t ADC_ResolutionGet(void)
{
    return ADC_RESOLUTION;
}

void ADC_ConversionDoneInterruptFlagClear(void)
{
    PIR6bits.ADIF = ADC_BIT_CLEAR;
}

bool ADC_IsConversionDoneInterruptFlagSet(void)
{
    return PIR6bits.ADIF; 
}

void ADC_ConversionDoneCallbackRegister(void (*callback)(void))
{
    ADC_ConversionDoneCallback = callback;
}

void ADC_ThresholdCallbackRegister(void (*callback)(void))
{
    ADC_ThresholdCallback = callback;
}

void ADC_ConversionDoneInterruptEnable(void)
{
    PIE6bits.ADIE = ADC_BIT_SET;
}

void ADC_ConversionDoneInterruptDisable(void)
{
    PIE6bits.ADIE = ADC_BIT_CLEAR;
}

void ADC_ThresholdInterruptEnable(void)
{
    PIE6bits.ADTIE = ADC_BIT_SET;    
}

void ADC_ThresholdInterruptDisable(void)
{
    PIE6bits.ADTIE = ADC_BIT_CLEAR;    
}

void ADC_ISR(void)
{
    PIR6bits.ADIF = ADC_BIT_CLEAR;

    if (NULL != ADC_ConversionDoneCallback)
    {
        ADC_ConversionDoneCallback();
    }
    else
    {
        // Do nothing
    }
}
void ADC_ThresholdISR(void)
{
    PIR6bits.ADTIF = ADC_BIT_CLEAR;

    if (NULL != ADC_ThresholdCallback)
    {
        ADC_ThresholdCallback();
    }
    else
    {
        // Do nothing
    }
}
