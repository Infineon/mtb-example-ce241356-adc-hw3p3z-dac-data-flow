/*******************************************************************************
* File Name:   main.c
*
* Description: This is the main file for the PPCA CPU core 0. This file contains
* the main function for the core, interrupt service routine, and global variables
* used by these functions.
*
* Related Document: See README.md
*
*
********************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cy_pdl.h"
#include "cycfg.h"
#include <stdio.h>

/******************************************************************************
* Macros
*******************************************************************************/
/* Shared memory addresses in M4 shared memory space (0x20040000-0x20043FFF) */
/* All cores can access M4 shared memory for inter-core communication */
#define PPCA_CPU0_M4_VAR_ADDRESS 0x20040400  /* Used by this core (CPU0) */


/* Macros for frequency update */
#define SINE_INTR_PERIOD_MIN 100
#define POT_INTR_PERIOD_SCALE 5
#define MIN_PERIOD_POT_COUNT  10

#define GET_SINE_FREQ(period) (200000000/(100*period))
#define SINE_COUNTER_MAX 99

/*******************************************************************************
* Global Variables
*******************************************************************************/
cy_stc_sysint_t hwfilter_intr_config =
{
    .intrSrc = EPU_IRQ_EPU_0,
    .intrPriority = 1U,
};

/* variables used in the project */
int32_t *frequency   = (int32_t *)PPCA_CPU0_M4_VAR_ADDRESS;
int32_t *new_data    = (int32_t *)PPCA_CPU0_M4_VAR_ADDRESS + 1;
uint32_t filter_data = 0;
uint32_t pot_data    = 0;
uint32_t period      = SINE_INTR_PERIOD_MIN;
uint32_t counter     = 0;

/* Look up table for sine wave generation */
uint32_t sinewave_pattern[] = {2062,2198,2300,2434,2565,2662,2788,2880,2998,3112,
           3193,3296,3369,3461,3545,3603,3674,3721,3777,3824,
           3853,3883,3900,3915,3920,3917,3905,3890,3861,3824,
           3789,3736,3690,3622,3545,3483,3393,3321,3219,3112,
           3027,2910,2819,2694,2565,2467,2333,2232,2096,1960,
           1857,1721,1619,1485,1354,1257,1131,1039,921,807,
           726,623,550,458,374,316,245,198,142,95,
           66,36,19,4,0,2,14,29,58,95,
           130,183,229,297,374,436,526,598,700,807,
           892,1009,1100,1225,1354,1452,1586,1687,1823,1959};

/* Configure interrupt for generating sine wave */
cy_stc_sysint_t sine_pwm_intr_config =
{
    .intrSrc = SINE_INTR_PWM_IRQ,
    .intrPriority = 1U,
};

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void hwfilter_isr();
void sine_pwm_isr();

/*******************************************************************************
* Function Name: main
*********************************************************************************
* Summary:
* This is the main function for PPCA CPU 0. It performs the initialization of the
* variables used in the code, and initializes and enables the interrupt from the
* PPCA timer required to interrupt this CPU.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    /* Configure EPU interrupt for interrupting the PPCA CPU on HW filter end of processing */
    Cy_PPCA_EPU_InterruptSourceSelect(EPU_EPU_IRQ0_HW, false, epuIrqSrc0);
    Cy_PPCA_EPU_SetInterruptMask(EPU_EPU_IRQ0_HW);

    /* Register ISR for the HW filter end of processing */
    Cy_SysInt_Init(&hwfilter_intr_config, &hwfilter_isr);
    NVIC_EnableIRQ(hwfilter_intr_config.intrSrc);

    /* Initialise and enable PWM for sine wave generation. */
    Cy_TCPWM_PWM_Init(SINE_INTR_PWM_HW, SINE_INTR_PWM_NUM, &SINE_INTR_PWM_config);
    Cy_TCPWM_PWM_Enable(SINE_INTR_PWM_HW, SINE_INTR_PWM_NUM);

    /* Register ISR and enable interupt for sine wave generation. */
    Cy_SysInt_Init(&sine_pwm_intr_config, &sine_pwm_isr);
    NVIC_EnableIRQ(sine_pwm_intr_config.intrSrc);

    /* Enable global interrupts */
    __enable_irq();

    /* Start TCPWM PWMs */
    Cy_TCPWM_TriggerStart_Single(SINE_INTR_PWM_HW, SINE_INTR_PWM_NUM);
    Cy_TCPWM_TriggerStart_Single(ADC_TRIG_PWM_HW, ADC_TRIG_PWM_NUM);

    for(;;)
    {
        *frequency = GET_SINE_FREQ(period);
        Cy_SysLib_Delay(10);
    }
}

/*******************************************************************************
* Function Name: hwfilter_isr
*********************************************************************************
* Summary:
* This is the interrupt service routine for the hardware filter end of conversion
* signal. Here the signal is copied from the HW filter output and writtent to the
* DAC input.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void hwfilter_isr()
{
    /* Clear interrupt. */
    Cy_PPCA_EPU_ClearInterrupt(EPU_EPU_IRQ0_HW);

    /* Read HW filter output */
    filter_data = Cy_PPCA_HWFILT3P3Z_ReadFilterDataOutput(HWFILTER_HW) >> 8;

    /* Writing the HW filter output to the DAC input */
    Cy_PPCA_DAC_Set_DACOut(DAC_FILTERED_HW, filter_data);
}

/*******************************************************************************
* Function Name: sine_pwm_isr
*********************************************************************************
* Summary:
* This is the interrupt service routine for the generation of the sine wave.
* Modulation of the sine wave frequency using the pot is also done inside this ISR.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void sine_pwm_isr()
{
    /* Clear interrupt. */
    Cy_TCPWM_ClearInterrupt(SINE_INTR_PWM_HW, SINE_INTR_PWM_NUM, CY_TCPWM_INT_ON_CC0_OR_TC);

    /* Writing the lookup table entries to the DAC input to generate sine wave. */
    Cy_PPCA_DAC_Set_DACOut(DAC_SINE_HW, sinewave_pattern[counter]);

    /* counter for indexing the sine wave look up table. */
    if(counter < SINE_COUNTER_MAX)
    {
        counter+=1;
    }
    else
    {
        counter = 0;
    }

    /* read the potentiometer data */
    pot_data = Cy_PPCA_ADC_Read_ADC_Data(ADC_POT_HW, 4);

    /* Calculate the period for changing the frequency based
     * on the potentiometer state*/
    if(pot_data > MIN_PERIOD_POT_COUNT)
    {
        period = pot_data * POT_INTR_PERIOD_SCALE;
    }
    else
    {
        period = SINE_INTR_PERIOD_MIN;
    }

    /* Update the frequency of the sinewave based on the period. */
    Cy_TCPWM_PWM_SetPeriod0(SINE_INTR_PWM_HW, SINE_INTR_PWM_NUM, period);

    /* Trigger the ADC used for reading the potentiometer */
    Cy_PPCA_ADC_Manual_Trigger(ADC_POT_HW, 5);

    *new_data = 1;
}
