/**
  ******************************************************************************
  * @file    analog.c
  * @author  WI6LABS
  * @version V1.0.0
  * @date    01-August-2016
  * @brief   provide analog services (ADC + PWM)
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 CHicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of CHicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32_def.h"
#include "analog.h"
#include "timer.h"
#include "PinAF_CH32V3xx.h"
#include "ch32v30x_adc.h"

/* Private_Variables */
#if defined(HAL_ADC_MODULE_ENABLED) || defined(HAL_DAC_MODULE_ENABLED) ||\
  defined(HAL_TIM_MODULE_ENABLED)
static PinName g_current_pin = NC;
#endif

/* Private_Defines */
#ifdef HAL_ADC_MODULE_ENABLED

#ifndef ADC_SAMPLINGTIME
#if defined(ADC_SAMPLETIME_8CYCLES_5)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_8CYCLES_5;
#elif defined(ADC_SAMPLETIME_12CYCLES_5)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_12CYCLES_5;
#elif defined(ADC_SAMPLETIME_13CYCLES_5)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_13CYCLES_5;
#elif defined(ADC_SAMPLETIME_15CYCLES)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_15CYCLES;
#elif defined(ADC_SAMPLETIME_16CYCLES)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_16CYCLES;
#elif defined(ADC_SAMPLETIME_19CYCLES_5)
#define ADC_SAMPLINGTIME        ADC_SAMPLETIME_19CYCLES_5;
#endif
#endif /* !ADC_SAMPLINGTIME */

/*
 * Minimum ADC sampling time is required when reading
 * internal channels so set it to max possible value.
 * It can be defined more precisely by defining:
 * ADC_SAMPLINGTIME_INTERNAL
 * to the desired ADC sample time.
 */
#ifndef ADC_SAMPLINGTIME_INTERNAL
#if defined(ADC_SAMPLETIME_480CYCLES)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_480CYCLES
#elif defined(ADC_SAMPLETIME_384CYCLES)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_384CYCLES
#elif defined(ADC_SAMPLETIME_810CYCLES_5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_810CYCLES_5
#elif defined(ADC_SAMPLETIME_640CYCLES_5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_640CYCLES_5
#elif defined(ADC_SAMPLETIME_601CYCLES_5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_601CYCLES_5
#elif defined(ADC_SAMPLETIME_247CYCLES_5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_247CYCLES_5
#elif defined(ADC_SAMPLETIME_239CYCLES_5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_239CYCLES_5
#elif defined(ADC_SAMPLETIME_160CYCLES_5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SAMPLETIME_160CYCLES_5
#else
#error "ADC sampling time could not be defined for internal channels!"
#endif
#endif /* !ADC_SAMPLINGTIME_INTERNAL */

#ifndef ADC_CLOCK_DIV
#ifdef ADC_CLOCK_SYNC_PCLK_DIV4
#define ADC_CLOCK_DIV       ADC_CLOCK_SYNC_PCLK_DIV4
#elif ADC_CLOCK_SYNC_PCLK_DIV2
#define ADC_CLOCK_DIV       ADC_CLOCK_SYNC_PCLK_DIV2
#elif defined(ADC_CLOCK_ASYNC_DIV1)
#define ADC_CLOCK_DIV       ADC_CLOCK_ASYNC_DIV1
#endif
#endif /* !ADC_CLOCK_DIV */

#ifndef ADC_REGULAR_RANK_1
#define ADC_REGULAR_RANK_1  1
#endif

/* Private Functions */
static uint32_t get_adc_channel(PinName pin)
{
  uint32_t function = pinmap_function(pin, PinMap_ADC);
  uint32_t channel = 0;
  switch (CH_PIN_CHANNEL(function)) {
#ifdef ADC_CHANNEL_0
    case 0:
      channel = ADC_CHANNEL_0;
      break;
#endif
    case 1:
      channel = ADC_CHANNEL_1;
      break;
    case 2:
      channel = ADC_CHANNEL_2;
      break;
    case 3:
      channel = ADC_CHANNEL_3;
      break;
    case 4:
      channel = ADC_CHANNEL_4;
      break;
    case 5:
      channel = ADC_CHANNEL_5;
      break;
    case 6:
      channel = ADC_CHANNEL_6;
      break;
    case 7:
      channel = ADC_CHANNEL_7;
      break;
    case 8:
      channel = ADC_CHANNEL_8;
      break;
    case 9:
      channel = ADC_CHANNEL_9;
      break;
    case 10:
      channel = ADC_CHANNEL_10;
      break;
    case 11:
      channel = ADC_CHANNEL_11;
      break;
    case 12:
      channel = ADC_CHANNEL_12;
      break;
    case 13:
      channel = ADC_CHANNEL_13;
      break;
    case 14:
      channel = ADC_CHANNEL_14;
      break;
    case 15:
      channel = ADC_CHANNEL_15;
      break;
#ifdef ADC_CHANNEL_16
    case 16:
      channel = ADC_CHANNEL_16;
      break;
#endif
    case 17:
      channel = ADC_CHANNEL_17;
      break;
#ifdef ADC_CHANNEL_18
    case 18:
      channel = ADC_CHANNEL_18;
      break;
#endif
#ifdef ADC_CHANNEL_19
    case 19:
      channel = ADC_CHANNEL_19;
      break;
#endif
#ifdef ADC_CHANNEL_20
    case 20:
      channel = ADC_CHANNEL_20;
      break;
    case 21:
      channel = ADC_CHANNEL_21;
      break;
    case 22:
      channel = ADC_CHANNEL_22;
      break;
    case 23:
      channel = ADC_CHANNEL_23;
      break;
    case 24:
      channel = ADC_CHANNEL_24;
      break;
    case 25:
      channel = ADC_CHANNEL_25;
      break;
    case 26:
      channel = ADC_CHANNEL_26;
      break;
#ifdef ADC_CHANNEL_27
    case 27:
      channel = ADC_CHANNEL_27;
      break;
    case 28:
      channel = ADC_CHANNEL_28;
      break;
    case 29:
      channel = ADC_CHANNEL_29;
      break;
    case 30:
      channel = ADC_CHANNEL_30;
      break;
    case 31:
      channel = ADC_CHANNEL_31;
      break;
#endif
#endif
    default:
      channel = 0;
      break;
  }
  return channel;
}

static uint32_t get_adc_internal_channel(PinName pin)
{
  uint32_t channel = 0;
  switch (pin) {
#ifdef ADC_CHANNEL_TEMPSENSOR
    case PADC_TEMP:
      channel = ADC_CHANNEL_TEMPSENSOR;
      break;
#endif
#ifdef ADC_CHANNEL_VREFINT
    case PADC_VREF:
      channel = ADC_CHANNEL_VREFINT;
      break;
#endif
#ifdef ADC_CHANNEL_VBAT
    case PADC_VBAT:
      channel = ADC_CHANNEL_VBAT;
      break;
#endif
    default:
      channel = 0;
      break;
  }
  return channel;
}
#endif /* HAL_ADC_MODULE_ENABLED */

#ifdef HAL_TIM_MODULE_ENABLED
static uint32_t get_pwm_channel(PinName pin)
{
  uint32_t function = pinmap_function(pin, PinMap_PWM);
  uint32_t channel = 0;
  switch (CH_PIN_CHANNEL(function)) {
    case 1:
      channel = TIM_CHANNEL_1;
      break;
    case 2:
      channel = TIM_CHANNEL_2;
      break;
    case 3:
      channel = TIM_CHANNEL_3;
      break;
    case 4:
      channel = TIM_CHANNEL_4;
      break;
    default:
      channel = 0;
      break;
  }
  return channel;
}
#endif /* HAL_TIM_MODULE_ENABLED */

#ifdef HAL_DAC_MODULE_ENABLED
static uint32_t get_dac_channel(PinName pin)
{
  uint32_t function = pinmap_function(pin, PinMap_DAC);
  uint32_t channel = 0;
  switch (CH_PIN_CHANNEL(function)) {
#ifdef DAC_CHANNEL_0
    case 0:
      channel = DAC_CHANNEL_0;
      break;
#endif
    case 1:
      channel = DAC_CHANNEL_1;
      break;
#ifdef DAC_CHANNEL_2
    case 2:
      channel = DAC_CHANNEL_2;
      break;
#endif
    default:
      channel = 0;
      break;
  }
  return channel;
}

////////////////////////// DAC INTERFACE FUNCTIONS /////////////////////////////

/**
  * @brief DAC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hdac: DAC handle pointer
  * @retval None
  */
void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
  UNUSED(hdac);

  /* DAC Periph clock enable */
#ifdef __HAL_RCC_DAC1_CLK_ENABLE
  __HAL_RCC_DAC1_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_DAC_CLK_ENABLE
  __HAL_RCC_DAC_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_DAC12_CLK_ENABLE
  __HAL_RCC_DAC12_CLK_ENABLE();
#endif
  /* Configure DAC GPIO pins */
  pinmap_pinout(g_current_pin, PinMap_DAC);
}


/**
  * @brief  This function will set the DAC to the required value
  * @param  port : the gpio port to use
  * @param  pin : the gpio pin to use
  * @param  value : the value to push on the adc output
  * @param  do_init : if set to 1 the initialization of the adc is done
  * @retval None
  */
void dac_write_value(PinName pin, uint32_t value, uint8_t do_init)
{
  DAC_HandleTypeDef DacHandle = {};
  DAC_ChannelConfTypeDef dacChannelConf = {};
  uint32_t dacChannel;

  DacHandle.Instance = pinmap_peripheral(pin, PinMap_DAC);
  if (DacHandle.Instance == NP) {
    return;
  }
  dacChannel = get_dac_channel(pin);
  if (!IS_DAC_CHANNEL(dacChannel)) {
    return;
  }
  if (do_init == 1) {

    if (HAL_DAC_DeInit(&DacHandle) != HAL_OK) {
      /* DeInitialization Error */
      return;
    }

    /*##-1- Configure the DAC peripheral #######################################*/
    g_current_pin = pin;
    if (HAL_DAC_Init(&DacHandle) != HAL_OK) {
      /* Initialization Error */
      return;
    }

    dacChannelConf.DAC_Trigger = DAC_TRIGGER_NONE;
    dacChannelConf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    /*##-2- Configure DAC channel1 #############################################*/
    if (HAL_DAC_ConfigChannel(&DacHandle, &dacChannelConf, dacChannel) != HAL_OK) {
      /* Channel configuration Error */
      return;
    }
  }

  /*##-3- Set DAC Channel1 DHR register ######################################*/
  if (HAL_DAC_SetValue(&DacHandle, dacChannel, DAC_ALIGN_12B_R, value) != HAL_OK) {
    /* Setting value Error */
    return;
  }

  /*##-4- Enable DAC Channel1 ################################################*/
  HAL_DAC_Start(&DacHandle, dacChannel);
}

/**
  * @brief  DeInitialize the DAC MSP.
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *         the configuration information for the specified DAC.
  * @retval None
  */
void HAL_DAC_MspDeInit(DAC_HandleTypeDef *hdac)
{
  UNUSED(hdac);
  /* DAC Periph clock disable */
#ifdef __HAL_RCC_DAC1_CLK_DISABLE
  __HAL_RCC_DAC1_CLK_DISABLE();
#endif
#ifdef __HAL_RCC_DAC_CLK_DISABLE
  __HAL_RCC_DAC_CLK_DISABLE();
#endif
}

/**
  * @brief  This function will stop the DAC
  * @param  port : the gpio port to use
  * @param  pin : the gpio pin to use
  * @retval None
  */
void dac_stop(PinName pin)
{
  DAC_HandleTypeDef DacHandle;
  uint32_t dacChannel;

  DacHandle.Instance = pinmap_peripheral(pin, PinMap_DAC);
  if (DacHandle.Instance == NP) {
    return;
  }
  dacChannel = get_dac_channel(pin);
  if (!IS_DAC_CHANNEL(dacChannel)) {
    return;
  }

  HAL_DAC_Stop(&DacHandle, dacChannel);

  if (HAL_DAC_DeInit(&DacHandle) != HAL_OK) {
    /* DeInitialization Error */
    return;
  }
}
#endif //HAL_DAC_MODULE_ENABLED

#ifdef HAL_ADC_MODULE_ENABLED
////////////////////////// ADC INTERFACE FUNCTIONS /////////////////////////////

/**
  * @brief ADC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* ADC Periph clock enable */
  if (hadc->Instance == ADC1) {
#ifdef __HAL_RCC_ADC1_CLK_ENABLE
    __HAL_RCC_ADC1_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_ADC12_CLK_ENABLE
    __HAL_RCC_ADC12_CLK_ENABLE();
#endif
  }
#ifdef ADC2
  else if (hadc->Instance == ADC2) {
#ifdef __HAL_RCC_ADC2_CLK_ENABLE
    __HAL_RCC_ADC2_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_ADC12_CLK_ENABLE
    __HAL_RCC_ADC12_CLK_ENABLE();
#endif
  }
#endif
#ifdef ADC3
  else if (hadc->Instance == ADC3) {
#ifdef __HAL_RCC_ADC3_CLK_ENABLE
    __HAL_RCC_ADC3_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_ADC34_CLK_ENABLE
    __HAL_RCC_ADC34_CLK_ENABLE();
#endif
  }
#endif
#ifdef ADC4
  else if (hadc->Instance == ADC4) {
    __HAL_RCC_ADC34_CLK_ENABLE();
  }
#endif

#ifdef __HAL_RCC_ADC_CLK_ENABLE
  __HAL_RCC_ADC_CLK_ENABLE();
#endif
  /* For CH32F1xx and CH32H7xx, ADC prescaler is configured in
     SystemClock_Config (variant.cpp) */
#if defined(__HAL_RCC_ADC_CONFIG) && !defined(CH32F1xx) && !defined(CH32H7xx)
  /* ADC Periph interface clock configuration */
  __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_SYSCLK);
#endif

  /* Configure ADC GPIO pin */
  if (!(g_current_pin & PADC_BASE)) {
    pinmap_pinout(g_current_pin, PinMap_ADC);
  }
}

/**
  * @brief  DeInitializes the ADC MSP.
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
#ifdef __HAL_RCC_ADC_FORCE_RESET
  __HAL_RCC_ADC_FORCE_RESET();
#endif
#ifdef __HAL_RCC_ADC_RELEASE_RESET
  __HAL_RCC_ADC_RELEASE_RESET();
#endif

  if (hadc->Instance == ADC1) {
#ifdef __HAL_RCC_ADC1_FORCE_RESET
    __HAL_RCC_ADC1_FORCE_RESET();
#endif
#ifdef __HAL_RCC_ADC1_RELEASE_RESET
    __HAL_RCC_ADC1_RELEASE_RESET();
#endif
#ifdef __HAL_RCC_ADC12_FORCE_RESET
    __HAL_RCC_ADC12_FORCE_RESET();
#endif
#ifdef __HAL_RCC_ADC12_RELEASE_RESET
    __HAL_RCC_ADC12_RELEASE_RESET();
#endif
#ifdef __HAL_RCC_ADC1_CLK_DISABLE
    __HAL_RCC_ADC1_CLK_DISABLE();
#endif
#ifdef __HAL_RCC_ADC12_CLK_DISABLE
    __HAL_RCC_ADC12_CLK_DISABLE();
#endif
  }
#ifdef ADC2
  else if (hadc->Instance == ADC2) {
#ifdef __HAL_RCC_ADC2_FORCE_RESET
    __HAL_RCC_ADC2_FORCE_RESET();
#endif
#ifdef __HAL_RCC_ADC2_RELEASE_RESET
    __HAL_RCC_ADC2_RELEASE_RESET();
#endif
#ifdef __HAL_RCC_ADC12_FORCE_RESET
    __HAL_RCC_ADC12_FORCE_RESET();
#endif
#ifdef __HAL_RCC_ADC12_RELEASE_RESET
    __HAL_RCC_ADC12_RELEASE_RESET();
#endif
#ifdef __HAL_RCC_ADC2_CLK_DISABLE
    __HAL_RCC_ADC2_CLK_DISABLE();
#endif
#ifdef __HAL_RCC_ADC2_CLK_DISABLE
    __HAL_RCC_ADC2_CLK_DISABLE();
#endif
  }
#endif
#ifdef ADC3
  else if (hadc->Instance == ADC3) {
#ifdef __HAL_RCC_ADC3_FORCE_RESET
    __HAL_RCC_ADC3_FORCE_RESET();
#endif
#ifdef __HAL_RCC_ADC3_RELEASE_RESET
    __HAL_RCC_ADC3_RELEASE_RESET();
#endif
#ifdef __HAL_RCC_ADC34_FORCE_RESET
    __HAL_RCC_ADC34_FORCE_RESET();
#endif
#ifdef __HAL_RCC_ADC34_RELEASE_RESET
    __HAL_RCC_ADC34_RELEASE_RESET();
#endif
#ifdef __HAL_RCC_ADC3_CLK_DISABLE
    __HAL_RCC_ADC3_CLK_DISABLE();
#endif
#ifdef __HAL_RCC_ADC34_CLK_DISABLE
    __HAL_RCC_ADC34_CLK_DISABLE();
#endif
  }
#endif
#ifdef ADC4
  else if (hadc->Instance == ADC4) {
    __HAL_RCC_ADC34_FORCE_RESET();
    __HAL_RCC_ADC34_RELEASE_RESET();
    __HAL_RCC_ADC34_CLK_DISABLE();
  }
#endif
#ifdef __HAL_RCC_ADC_CLK_DISABLE
  __HAL_RCC_ADC_FORCE_RESET();
  __HAL_RCC_ADC_RELEASE_RESET();
  __HAL_RCC_ADC_CLK_DISABLE();
#endif
}

/**
  * @brief  This function will set the ADC to the required value
  * @param  pin : the pin to use
  * @retval the value of the adc
  */
uint16_t adc_read_value(PinName pin)
{
  ADC_HandleTypeDef AdcHandle = {};
  ADC_ChannelConfTypeDef  AdcChannelConf = {};
  __IO uint16_t uhADCxConvertedValue = 0;
  uint32_t samplingTime = ADC_SAMPLINGTIME;
  uint32_t channel = 0;

  if (pin & PADC_BASE) {
#if defined(CH32H7xx)
    AdcHandle.Instance = ADC3;
#else
    AdcHandle.Instance = ADC1;
#endif
    channel = get_adc_internal_channel(pin);
    samplingTime = ADC_SAMPLINGTIME_INTERNAL;
  } else {
    AdcHandle.Instance = pinmap_peripheral(pin, PinMap_ADC);
    channel = get_adc_channel(pin);
  }

  if (AdcHandle.Instance == NP) {
    return 0;
  }

#ifdef ADC_CLOCK_DIV
  AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_DIV;                 /* (A)synchronous clock mode, input ADC clock divided */
#endif
#ifdef ADC_RESOLUTION_12B
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;            /* 12-bit resolution for converted data */
#endif
#ifdef ADC_DATAALIGN_RIGHT
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;           /* Right-alignment for converted data */
#endif
  AdcHandle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
#ifdef ADC_EOC_SINGLE_CONV
  AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;           /* EOC flag picked-up to indicate conversion end */
#endif
#if !defined(CH32F1xx) && !defined(CH32F2xx) && !defined(CH32F4xx) && \
    !defined(CH32F7xx) && !defined(CH32F373xC) && !defined(CH32F378xx)
  AdcHandle.Init.LowPowerAutoWait      = DISABLE;                       /* Auto-delayed conversion feature disabled */
#endif
#if !defined(CH32F1xx) && !defined(CH32F2xx) && !defined(CH32F3xx) && \
    !defined(CH32F4xx) && !defined(CH32F7xx) && !defined(CH32H7xx) && \
    !defined(CH32L4xx) && !defined(CH32WBxx)
  AdcHandle.Init.LowPowerAutoPowerOff  = DISABLE;                       /* ADC automatically powers-off after a conversion and automatically wakes-up when a new conversion is triggered */
#endif
#ifdef ADC_CHANNELS_BANK_A
  AdcHandle.Init.ChannelsBank          = ADC_CHANNELS_BANK_A;
#endif
  AdcHandle.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
#if !defined(CH32F0xx) && !defined(CH32L0xx)
  AdcHandle.Init.NbrOfConversion       = 1;                             /* Specifies the number of ranks that will be converted within the regular group sequencer. */
#endif
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
#if !defined(CH32F0xx) && !defined(CH32G0xx) && !defined(CH32L0xx)
  AdcHandle.Init.NbrOfDiscConversion   = 0;                             /* Parameter discarded because sequencer is disabled */
#endif
  AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
#if !defined(CH32F1xx)
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
#endif
#if !defined(CH32F1xx) && !defined(CH32H7xx) && \
    !defined(CH32F373xC) && !defined(CH32F378xx)
  AdcHandle.Init.DMAContinuousRequests = DISABLE;                       /* DMA one-shot mode selected (not applied to this example) */
#endif
#ifdef ADC_CONVERSIONDATA_DR
  AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;      /* Regular Conversion data stored in DR register only */
#endif
#ifdef ADC_OVR_DATA_OVERWRITTEN
  AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;      /* DR register is overwritten with the last conversion result in case of overrun */
#endif
#ifdef ADC_LEFTBITSHIFT_NONE
  AdcHandle.Init.LeftBitShift          = ADC_LEFTBITSHIFT_NONE;         /* No bit shift left applied on the final ADC convesion data */
#endif

#if defined(CH32F0xx)
  AdcHandle.Init.SamplingTimeCommon    = samplingTime;
#endif
#if defined(CH32G0xx)
  AdcHandle.Init.SamplingTimeCommon1   = samplingTime;              /* Set sampling time common to a group of channels. */
  AdcHandle.Init.SamplingTimeCommon2   = samplingTime;              /* Set sampling time common to a group of channels, second common setting possible.*/
  AdcHandle.Init.TriggerFrequencyMode  = ADC_TRIGGER_FREQ_HIGH;
#endif
#if defined(CH32L0xx)
  AdcHandle.Init.LowPowerFrequencyMode = DISABLE;                       /* To be enabled only if ADC clock < 2.8 MHz */
  AdcHandle.Init.SamplingTime          = samplingTime;
#endif
#if !defined(CH32F0xx) && !defined(CH32F1xx) && !defined(CH32F2xx) && \
    !defined(CH32F3xx) && !defined(CH32F4xx) && !defined(CH32F7xx) && \
    !defined(CH32L1xx)
  AdcHandle.Init.OversamplingMode      = DISABLE;
  /* AdcHandle.Init.Oversample ignore for CH32L0xx as oversampling disabled */
  /* AdcHandle.Init.Oversampling ignored for other as oversampling disabled */
#endif
#if defined(ADC_CFGR_DFSDMCFG) && defined(DFSDM1_Channel0)
  AdcHandle.Init.DFSDMConfig           = ADC_DFSDM_MODE_DISABLE;        /* ADC conversions are not transferred by DFSDM. */
#endif
#ifdef ADC_TRIGGER_FREQ_HIGH
  AdcHandle.Init.TriggerFrequencyMode  = ADC_TRIGGER_FREQ_HIGH;
#endif

  AdcHandle.State = HAL_ADC_STATE_RESET;
  AdcHandle.DMA_Handle = NULL;
  AdcHandle.Lock = HAL_UNLOCKED;
  /* Some other ADC_HandleTypeDef fields exists but not required */

  g_current_pin = pin; /* Needed for HAL_ADC_MspInit*/

  if (HAL_ADC_Init(&AdcHandle) != HAL_OK) {
    return 0;
  }

  AdcChannelConf.Channel      = channel;                          /* Specifies the channel to configure into ADC */

#if defined(CH32L4xx) || defined(CH32WBxx)
  if (!IS_ADC_CHANNEL(&AdcHandle, AdcChannelConf.Channel)) {
    return 0;
  }
#else
  if (!IS_ADC_CHANNEL(AdcChannelConf.Channel)) {
    return 0;
  }
#endif /* CH32L4xx || CH32WBxx */
  AdcChannelConf.Rank         = ADC_REGULAR_RANK_1;               /* Specifies the rank in the regular group sequencer */
#if !defined(CH32L0xx)
#if !defined(CH32G0xx)
  AdcChannelConf.SamplingTime = samplingTime;                     /* Sampling time value to be set for the selected channel */
#else
  AdcChannelConf.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;        /* Sampling time value to be set for the selected channel */
#endif
#endif
#if !defined(CH32F0xx) && !defined(CH32F1xx) && !defined(CH32F2xx) && \
    !defined(CH32F4xx) && !defined(CH32F7xx) && !defined(CH32G0xx) && \
    !defined(CH32L0xx) && !defined(CH32L1xx) && \
    !defined(CH32F373xC) && !defined(CH32F378xx)
  AdcChannelConf.SingleDiff   = ADC_SINGLE_ENDED;                 /* Single-ended input channel */
  AdcChannelConf.OffsetNumber = ADC_OFFSET_NONE;                  /* No offset subtraction */
#endif
#if !defined(CH32F0xx) && !defined(CH32F1xx) && !defined(CH32F2xx) && \
    !defined(CH32G0xx) && !defined(CH32L0xx) && !defined(CH32L1xx) && \
    !defined(CH32WBxx) && !defined(CH32F373xC) && !defined(CH32F378xx)
  AdcChannelConf.Offset = 0;                                      /* Parameter discarded because offset correction is disabled */
#endif
#if defined (CH32H7xx)
  AdcChannelConf.OffsetRightShift = DISABLE;                      /* No Right Offset Shift */
  AdcChannelConf.OffsetSignedSaturation = DISABLE;                /* Signed saturation feature is not used */
#endif

  /*##-2- Configure ADC regular channel ######################################*/
  if (HAL_ADC_ConfigChannel(&AdcHandle, &AdcChannelConf) != HAL_OK) {
    /* Channel Configuration Error */
    return 0;
  }

#if defined(CH32F0xx) || defined(CH32F1xx) || defined(CH32F3xx) || \
    defined(CH32G0xx) || defined(CH32H7xx) || defined(CH32L0xx) || \
    defined(CH32L4xx) || defined(CH32WBxx)
  /*##-2.1- Calibrate ADC then Start the conversion process ####################*/
#if defined(CH32F0xx) || defined(CH32G0xx) || defined(CH32F1xx) || \
    defined(CH32F373xC) || defined(CH32F378xx)
  if (HAL_ADCEx_Calibration_Start(&AdcHandle) !=  HAL_OK)
#elif defined (CH32H7xx)
  if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
#else
  if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) !=  HAL_OK)
#endif
  {
    /* ADC Calibration Error */
    return 0;
  }
#endif

  /*##-3- Start the conversion process ####################*/
  if (HAL_ADC_Start(&AdcHandle) != HAL_OK) {
    /* Start Conversation Error */
    return 0;
  }

  /*##-4- Wait for the end of conversion #####################################*/
  /*  For simplicity reasons, this example is just waiting till the end of the
      conversion, but application may perform other tasks while conversion
      operation is ongoing. */
  if (HAL_ADC_PollForConversion(&AdcHandle, 10) != HAL_OK) {
    /* End Of Conversion flag not set on time */
    return 0;
  }

  /* Check if the continous conversion of regular channel is finished */
  if ((HAL_ADC_GetState(&AdcHandle) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC) {
    /*##-5- Get the converted value of regular channel  ########################*/
    uhADCxConvertedValue = HAL_ADC_GetValue(&AdcHandle);
  }

  if (HAL_ADC_Stop(&AdcHandle) != HAL_OK) {
    /* Stop Conversation Error */
    return 0;
  }

  if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK) {
    return 0;
  }

  LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(AdcHandle.Instance), LL_ADC_PATH_INTERNAL_NONE);

  return uhADCxConvertedValue;
}
#endif /* HAL_ADC_MODULE_ENABLED */

#ifdef HAL_TIM_MODULE_ENABLED
////////////////////////// PWM INTERFACE FUNCTIONS /////////////////////////////


/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIMx Peripheral clock enable */
  timer_enable_clock(htim);

  /* Configure PWM GPIO pins */
  pinmap_pinout(g_current_pin, PinMap_PWM);
}

/**
  * @brief  DeInitializes TIM PWM MSP.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *htim)
{
  timer_disable_clock(htim);
}

/**
  * @brief  This function will set the PWM to the required value
  * @param  port : the gpio port to use
  * @param  pin : the gpio pin to use
  * @param  clock_freq : frequency of the tim clock
  * @param  period : period of the tim counter
  * @param  value : the value to push on the PWM output
  * @param  do_init : if set to 1 the initialization of the PWM is done
  * @retval None
  */
void pwm_start(PinName pin, uint32_t clock_freq,
               uint32_t period, uint32_t value, uint8_t do_init)
{
  TIM_HandleTypeDef timHandle = {};
  TIM_OC_InitTypeDef timConfig = {};
  uint32_t timChannel;

  /* Compute the prescaler value to have TIM counter clock equal to clock_freq Hz */
  timHandle.Instance               = pinmap_peripheral(pin, PinMap_PWM);
  if (timHandle.Instance == NP) {
    return;
  }
  timHandle.Init.Prescaler         = (uint32_t)(getTimerClkFreq(timHandle.Instance) / clock_freq) - 1;
  timHandle.Init.Period            = period - 1;
  timHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  timHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
#if !defined(CH32L0xx) && !defined(CH32L1xx)
  timHandle.Init.RepetitionCounter = 0;
#endif
  timHandle.State = HAL_TIM_STATE_RESET;
  // TBC: is timHandle.State field should be saved ?

  if (do_init == 1) {
    g_current_pin = pin;
    if (HAL_TIM_PWM_Init(&timHandle) != HAL_OK) {
      return;
    }
  }
  timChannel = get_pwm_channel(pin);
  if (!IS_TIM_CHANNELS(timChannel)) {
    return;
  }
  //HAL_TIM_PWM_Stop(&timHandle, timChannel);

  /*##-2- Configure the PWM channels #########################################*/
  /* Common configuration for all channels */
  timConfig.OCMode       = TIM_OCMODE_PWM1;
  timConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  timConfig.OCFastMode   = TIM_OCFAST_DISABLE;
#if !defined(CH32L0xx) && !defined(CH32L1xx)
  timConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  timConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  timConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
#endif
  timConfig.Pulse = value;

  if (HAL_TIM_PWM_ConfigChannel(&timHandle, &timConfig, timChannel) != HAL_OK) {
    /*##-2- Configure the PWM channels #########################################*/
    return;
  }

#if !defined(CH32L0xx) && !defined(CH32L1xx)
  if (CH_PIN_INVERTED(pinmap_function(pin, PinMap_PWM))) {
    HAL_TIMEx_PWMN_Start(&timHandle, timChannel);
  } else
#endif
  {
    HAL_TIM_PWM_Start(&timHandle, timChannel);
  }
}

/**
  * @brief  This function will disable the PWM
  * @param  port : the gpio port to use
  * @param  pin : the gpio pin to use
  * @retval None
  */
void pwm_stop(PinName pin)
{
  TIM_HandleTypeDef timHandle;
  uint32_t timChannel;

  timHandle.Instance = pinmap_peripheral(pin, PinMap_PWM);
  if (timHandle.Instance == NP) {
    return;
  }
  timChannel = get_pwm_channel(pin);
  if (!IS_TIM_CHANNELS(timChannel)) {
    return;
  }

#if !defined(CH32L0xx) && !defined(CH32L1xx)
  if (CH_PIN_INVERTED(pinmap_function(pin, PinMap_PWM))) {
    HAL_TIMEx_PWMN_Stop(&timHandle, timChannel);
  } else
#endif
  {
    HAL_TIM_PWM_Stop(&timHandle, timChannel);
  }

  HAL_TIM_PWM_DeInit(&timHandle);
}
#endif /* HAL_TIM_MODULE_ENABLED */

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT CHicroelectronics *****END OF FILE****/