/*******************************************************************************
* File Name:   main.c
*
* Description: This is the main file for the main CPU non safe application of
* the code example. It initializes the peripherals, PPCA CPU cores and starts
* it. Then it reads the data shared by the PPCA CPUs and prints.
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
#include "cybsp.h"
#include <stdio.h>
#include "cy_retarget_io.h"

/*******************************************************************************
* Global Variables
*******************************************************************************/
/* Debug UART variables */
static cy_stc_scb_uart_context_t    DEBUG_UART_context; /* UART context */
static mtb_hal_uart_t               DEBUG_UART_hal_obj; /* Debug UART HAL object */

/******************************************************************************
* Macros
*******************************************************************************/
/* These are the addresses where the core0 and core1 images are located and its size*/
#define CORE0_IMAGE_ADDRESS    CYMEM_CM33_0_S_m33s_ppca0_nvm_C_S_START
#define CORE1_IMAGE_ADDRESS    CYMEM_CM33_0_S_m33s_ppca1_nvm_C_S_START
#define PPCA0_IMAGE_SIZE       CYMEM_CM33_0_S_ppca0_code_SIZE
#define PPCA1_IMAGE_SIZE       CYMEM_CM33_0_S_ppca1_code_SIZE

/* Shared memory addresses for inter-core communication */
/* PPCA cores write to these locations, main core reads from them */
/* Variables located in M4 shared memory space (16KB at 0x20040000-0x20043FFF from PPCA view) */
/* Main core accesses PPCA memory through PPCA peripheral base with memory windows: */
/* M1 (CPU0 data): 0x53020000, M3 (CPU1 data): 0x53040000, M4 (shared): 0x53050000 */
#define PPCA_CPU0_M4_VAR_ADDRESS   0x53050400  /* Written by PPCA Core 0 */
#define PPCA_CPU1_M4_VAR_ADDRESS   0x53050800  /* Written by PPCA Core 1 */

/* HW filter output bias: 12-bit DAC mid-scale (2048) in the filter's 24-bit output domain.
 * Kept here because Filter Coefficient Tool 1.3 has no input for the data-out offset and
 * clears it in design.modus whenever the .mtbcoef file is re-saved. */
#define HWFILTER_DATA_OUT_OFFSET   (0x00080000)

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*******************************************************************************
* Function Name: main
*********************************************************************************
* Summary:
* This is the main function for the non safe project for the main core. It
* performs the initialization of the peripherals, initialization and starting
* of the PPCA CPU cores, and send the data received from PPCA CPU Core 0 through
* UART.
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
    cy_rslt_t result;
    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize UART */
    Cy_SCB_UART_Init(DEBUG_UART_HW, &DEBUG_UART_config, &DEBUG_UART_context);
    Cy_SCB_UART_Enable(DEBUG_UART_HW);

    /* Setup the HAL UART */
    result = mtb_hal_uart_setup(&DEBUG_UART_hal_obj, &DEBUG_UART_hal_config,
                                &DEBUG_UART_context, NULL);

    /* HAL UART init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize redirecting of low level IO */
    result = cy_retarget_io_init(&DEBUG_UART_hal_obj);

    /* retarget IO init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Transmit header to the terminal */
    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("************************************************************\r\n");
    printf("PSOC Control C3M/P8: ADC-HWFilter-DAC dataflow\r\n");
    printf("************************************************************\r\n\n");
    printf("Connect a jumper wire provided with the kit between pins AIN1 \r\n");
    printf("and AIN1P. Connect an oscilloscope to the pins AIN1 and AIN0.\r\n");
    printf("It should show source sine wave from AIN1 and the filtered   \r\n");
    printf("sine wave from AIN0. Rotate the pot R262. If will vary the   \r\n");
    printf("source frequency. Filter frequency response can be monitored \r\n");
    printf("on the oscilloscope.\r\n\n");
    /* enable interrupts */
    __enable_irq();

    /* allocating pointers to the shared memory for where the data
     * from the PPCA Core 0 is located. */
    int32_t *frequency = (int32_t *)PPCA_CPU0_M4_VAR_ADDRESS;
    int32_t *new_data  = (int32_t *)PPCA_CPU0_M4_VAR_ADDRESS + 1;

    *frequency = 0 ;
    *new_data  = 0;

    /* Initializing and enabling the PPCA Configuration. */
    Cy_PPCA_CNFG_Init(PPCA_CNFG_HW, &PPCA_CNFG_config);
    Cy_PPCA_Enable(PPCA_CNFG_HW);

    /* Initializing ATOP ADC */
    Cy_PPCA_ADC_Init(ADC_HW, &ADC_config);

    /* Initializing ATOP Analog reference */
    Cy_PPCA_AREF_Init(AREF_HW, &AREF_config);
    Cy_PPCA_AREF_Enable(AREF_HW);

    /* Initializing ATOP ADC Filter */
    Cy_PPCA_ADC_Filter_Init(ADC_FLTR_HW, &ADC_FLTR_config);

    /* Enabling ADC */
    Cy_PPCA_ADC_Enable(ADC_HW);

    /* Initializing ATOP ADC */
    Cy_PPCA_ADC_Init(ADC_POT_HW, &ADC_POT_config);

    /* Enabling ADC */
    Cy_PPCA_ADC_Enable(ADC_POT_HW);

    /* Initialize and enable R2R DAC for filtered signal */
    Cy_PPCA_DAC_Init(DAC_FILTERED_HW, &DAC_FILTERED_config);
    Cy_PPCA_DACBUF_Enable(DAC_FILTERED_HW);

    /* Enable DAC */
    Cy_PPCA_DAC_Enable(DAC_FILTERED_HW);

    Cy_TCPWM_PWM_Init(ADC_TRIG_PWM_HW, ADC_TRIG_PWM_NUM, &ADC_TRIG_PWM_config);
    Cy_TCPWM_PWM_Enable(ADC_TRIG_PWM_HW, ADC_TRIG_PWM_NUM);

    /* Enabling Hardware filter block */
    Cy_PPCA_HWFILT3P3Z_SS_PeripheralEnable(HWFILTER_SS_HW);

    /* Enabling Hardware filter */
    Cy_PPCA_HWFILT3P3Z_FilterEnable(HWFILTER_HW, true);

    /* Initializing Hardware filter */
    Cy_PPCA_HWFILT3P3Z_InitFilterConfig(HWFILTER_HW, &HWFILTER_config);

    Cy_PPCA_HWFILT3P3Z_ConfigOffset(HWFILTER_HW, HWFILTER_DATA_OUT_OFFSET);

    /* Setting Data input 0 for the filter */
    Cy_PPCA_HWFILT3P3Z_Write_DATA_IN0(HWFILTER_HW, 0);

    /* Initialize the EPU processing units for trigger routing */
    Cy_PPCA_EPU_PU_T1_Configure(ADC_TRIG_PWM_TC_HW, ADC_TRIG_PWM_TC_INDEX, &ADC_TRIG_PWM_TC_put1_config);
    Cy_PPCA_EPU_PU_T1_Enable(ADC_TRIG_PWM_TC_HW, ADC_TRIG_PWM_TC_INDEX, ADC_TRIG_PWM_TC_ENABLE_MODE);

    Cy_PPCA_EPU_PU_T1_Configure(HWFILTER_EOC_HW, HWFILTER_EOC_INDEX, &HWFILTER_EOC_put1_config);
    Cy_PPCA_EPU_PU_T1_Enable(HWFILTER_EOC_HW, HWFILTER_EOC_INDEX, HWFILTER_EOC_ENABLE_MODE);

    /* Initialize the EPU combiner units for trigger routing */
    Cy_PPCA_EPU_Combo_Configure(ADC_TRIG_HW, ADC_TRIG_INDEX, &ADC_TRIG_combo_config);
    Cy_PPCA_EPU_Combo_Configure(HWFILTER_INTR_HW, HWFILTER_INTR_INDEX, &HWFILTER_INTR_combo_config);

    /* Initialise and enable PWM for sine wave generation. */
    Cy_TCPWM_PWM_Init(SINE_INTR_PWM_HW, SINE_INTR_PWM_NUM, &SINE_INTR_PWM_config);
    Cy_TCPWM_PWM_Enable(SINE_INTR_PWM_HW, SINE_INTR_PWM_NUM);

    /* Initialize and enable R2R DAC for source signal */
    Cy_PPCA_DAC_Init(DAC_SINE_HW, &DAC_SINE_config);
    Cy_PPCA_DACBUF_Enable(DAC_SINE_HW);

    /* Enable DAC */
    Cy_PPCA_DAC_Enable(DAC_SINE_HW);

    /* Initializing and starting PPCA CPU Core 0. */
    Cy_System_Init_CPU0((void*)CORE0_IMAGE_ADDRESS, PPCA0_IMAGE_SIZE);

    /* Initializing and starting PPCA CPU Core 1. Use the below line to start PPCA Core 1 */
    /*Cy_System_Init_CPU1((void*)CORE1_IMAGE_ADDRESS, PPCA1_IMAGE_SIZE);*/

    /* Enable exclusive access to the EPU resources based
     * on the provided resources allocation configuration. */
    Cy_PPCA_EPU_EnableExclusiveAccess(EPU_HW, true);

    /* Enable EPU */
    Cy_PPCA_EPU_Enable(EPU_HW);

    for (;;)
    {
        /* Synchronizing the print with the PPCA CPU interrupt. */
        if(1 == *new_data)
        {
            /* Printing frequency of the output wave */
            printf("Sine wave frequency: %d Hz\r\n",(int)*frequency);
            *new_data = 0;
        }
        Cy_SysLib_Delay(500);
    }
}

