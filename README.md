# PSOC&trade; Control C3M/P8: ADC-HWFilter-DAC dataflow

This code example applies for PSOC&trade; Control C3M/P8 MCUs. It demonstrates the working of ADC, HW filter, and DAC together when the data movement is managed by the PPCA CPU. 

In this code example, a sine wave is generated using the DAC by updating the DAC output from a TCPWM ISR. Along with that, the frequency of the sine wave will be modulated using a potentiometer on the evaluation kit (EVK).

This sine wave is fed to the ADC-HW 3P3Z filter-DAC datapath. This HW 3P3Z filter is configured as a low pass filter. In this, the LPF cutoff frequency is set at 1 kHz.

A TCPWM channel configured as PWM is used for triggering the conversion of the ADC channel on its terminal count. The hardware filter is configured as a low pass filter. The PWM triggers the ADC in frequent intervals and the conversion will happen. The data movement from ADC to HW filter happens through the dedicated data path. The HW filter processes the data and makes it available for the CPU to read it. The PPCA CPU reads the filter output and writes it to the DAC. 

You monitor the source sine wave and filtered sine wave by using an oscilloscope. By rotating the potentiometer R262, the output frequency can be varied and its amplitude will come down when the filter cutoff frequency 1 kHz is crossed.

The main CPU will print the instantaneous frequency on the UART console. So that you can monitor the change in amplitude along with the frequency.

[View this README on GitHub.](https://github.com/Infineon/mtb-example-ce241356-adc-hw3p3z-dac-data-flow)

[Provide feedback on this code example.](https://yourvoice.infineon.com/jfe/form/SV_1NTns53sK2yiljn?Q_EED=eyJVbmlxdWUgRG9jIElkIjoiQ0UyNDEzNTYiLCJTcGVjIE51bWJlciI6IjAwMi00MTM1NiIsIkRvYyBUaXRsZSI6IlBTT0MmdHJhZGU7IENvbnRyb2wgQzNNL1A4OiBBREMtSFdGaWx0ZXItREFDIGRhdGFmbG93IiwicmlkIjoic29vcmFqLmtvb3poYW1wYXJhbWJpbHN1YnJhbWFuaWFuQGluZmluZW9uLmNvbSIsIkRvYyB2ZXJzaW9uIjoiMS4wLjAiLCJEb2MgTGFuZ3VhZ2UiOiJFbmdsaXNoIiwiRG9jIERpdmlzaW9uIjoiTUNEIiwiRG9jIEJVIjoiSUNXIiwiRG9jIEZhbWlseSI6IlBTT0MifQ==)

## Requirements

- [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) v3.9.0 or later (tested with v3.9.0)
- Board support package (BSP) minimum required version for:
   - KIT_PSC3M8_EVK: v2.2.0
- Programming language: C
- Associated parts: All [PSOC&trade; Control C3M/P8 MCU](https://www.infineon.com/products/microcontroller/32-bit-psoc-arm-cortex/32-bit-psoc-control-arm-cortex-m33-mcu/psoc-control-c3-performance-line) parts


## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm&reg; Embedded Compiler v14.2.1 (`GCC_ARM`) – Default value of `TOOLCHAIN`
- Arm&reg; Compiler v6.22 (`ARM`)
- IAR C/C++ Compiler v9.70.4 (`IAR`)


## Supported kits (make variable 'TARGET')

- [PSOC&trade; Control C3M8 Evaluation Kit](https://www.infineon.com/KIT_PSC3M8_EVK) (`KIT_PSC3M8_EVK`) – Default value of `TARGET`


## Hardware setup

See the kit user guide to ensure that the board is configured correctly.

Connect a wire provided with the kit between pin AIN1 and AIN1P to provide the generated sine wave to the filter path input. Then connect an oscilloscope to pin AIN1 (to monitor the source signal) and AIN0 (to monitor the filtered signal).


## Software setup

See the [ModusToolbox&trade; tools package installation guide](https://www.infineon.com/ModusToolboxInstallguide) for information about installing and configuring the tools package.

Install a terminal emulator if you do not have one. Instructions in this document use [Tera Term](https://teratermproject.github.io/index-en.html).

This example requires no additional software or tools.


## Using the code example


### Create the project

The ModusToolbox&trade; tools package provides the Project Creator as both a GUI tool and a command line tool.

<details><summary><b>Use Project Creator GUI</b></summary>

1. Open the Project Creator GUI tool

   There are several ways to do this, including launching it from the dashboard or from inside the Eclipse IDE. For more details, see the [Project Creator user guide](https://www.infineon.com/ModusToolboxProjectCreator) (locally available at *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/docs/project-creator.pdf*)

2. On the **Choose Board Support Package (BSP)** page, select a kit supported by this code example. See [Supported kits](#supported-kits-make-variable-target)

   > **Note:** To use this code example for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work

3. On the **Select Application** page:

   a. Select the **Applications(s) Root Path** and the **Target IDE**

      > **Note:** Depending on how you open the Project Creator tool, these fields may be pre-selected for you

   b. Select this code example from the list by enabling its check box

      > **Note:** You can narrow the list of displayed examples by typing in the filter box

   c. (Optional) Change the suggested **New Application Name** and **New BSP Name**

   d. Click **Create** to complete the application creation process

</details>


<details><summary><b>Use Project Creator CLI</b></summary>

The 'project-creator-cli' tool can be used to create applications from a CLI terminal or from within batch files or shell scripts. This tool is available in the *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/* directory.

Use a CLI terminal to invoke the 'project-creator-cli' tool. On Windows, use the command-line 'modus-shell' program provided in the ModusToolbox&trade; installation instead of a standard Windows command-line application. This shell provides access to all ModusToolbox&trade; tools. You can access it by typing "modus-shell" in the search box in the Windows menu. In Linux and macOS, you can use any terminal application.

The following example clones the "[mtb-example-ce241356-adc-hw3p3z-dac-data-flow](https://github.com/Infineon/mtb-example-ce241356-adc-hw3p3z-dac-data-flow)" application with the desired name "AdcHwFilterDacDataflow" configured for the *KIT_PSC3M8_EVK* BSP into the specified working directory, *C:/mtb_projects*:

   ```
   project-creator-cli --board-id KIT_PSC3M8_EVK --app-id mtb-example-ce241356-adc-hw3p3z-dac-data-flow --user-app-name AdcHwFilterDacDataflow --target-dir "C:/mtb_projects"
   ```

The 'project-creator-cli' tool has the following arguments:

Argument | Description | Required/optional
---------|-------------|-----------
`--board-id` | Defined in the <id> field of the [BSP](https://github.com/Infineon?q=bsp-manifest&type=&language=&sort=) manifest | Required
`--app-id`   | Defined in the <id> field of the [CE](https://github.com/Infineon?q=ce-manifest&type=&language=&sort=) manifest | Required
`--target-dir`| Specify the directory in which the application is to be created if you prefer not to use the default current working directory | Optional
`--user-app-name`| Specify the name of the application if you prefer to have a name other than the example's default name | Optional

<br>

> **Note:** The project-creator-cli tool uses the `git clone` and `make getlibs` commands to fetch the repository and import the required libraries. For details, see the "Project creator tools" section of the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at {ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf).

</details>


### Open the project

After the project has been created, you can open it in your preferred development environment.


<details><summary><b>Eclipse IDE</b></summary>

If you opened the Project Creator tool from the included Eclipse IDE, the project will open in Eclipse automatically.

For more details, see the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_ide_user_guide.pdf*).

</details>


<details><summary><b>Visual Studio (VS) Code</b></summary>

Launch VS Code manually, and then open the generated *{project-name}.code-workspace* file located in the project directory.

For more details, see the [Visual Studio Code for ModusToolbox&trade; user guide](https://www.infineon.com/MTBVSCodeUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_vscode_user_guide.pdf*).

</details>


<details><summary><b>Command line</b></summary>

If you prefer to use the CLI, open the appropriate terminal, and navigate to the project directory. On Windows, use the command-line 'modus-shell' program; on Linux and macOS, you can use any terminal application. From there, you can run various `make` commands.

For more details, see the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>


## Operation

1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector

2. Open a terminal program and select the KitProg3 COM port. Set the serial port parameters to 8N1 and 115200 baud

3. Program the board using one of the following:

   <details><summary><b>Using Eclipse IDE</b></summary>

      1. Select the application project in the Project Explorer

      2. In the **Quick Panel**, scroll down, and click **\<Application Name> Program (KitProg3_MiniProg4)**
   </details>


   <details><summary><b>In other IDEs</b></summary>

   Follow the instructions in your preferred IDE.

   </details>


   <details><summary><b>Using CLI</b></summary>

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target. The default toolchain is specified in the application's Makefile but you can override this value manually:
      ```
      make program TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make program TOOLCHAIN=GCC_ARM
      ```
   </details>

4. After programming, the application starts automatically. Confirm that "PSOC Control C3M/P8: ADC-HWFilter-DAC dataflow" is displayed on the UART terminal

   **Figure 1. Terminal output on program startup**

   ![](images/terminal-print.png)


## Debugging

You can debug the example to step through the code.


<details><summary><b>In Eclipse IDE</b></summary>

Use the **\<Application Name> Debug (KitProg3_MiniProg4)** configuration in the **Quick Panel**. For details, see the "Program and debug" section in the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/MTBEclipseIDEUserGuide).


</details>


<details><summary><b>In other IDEs</b></summary>

Follow the instructions in your preferred IDE.

</details>


## Design and implementation

This code example demonstrates the capabilities of the PSOC&trade; Control C3M/P8 PPCA 3P3Z hardware filter to process the ADC output without the intervention of the CPU. This MCU has four different ADC groups. In this code example, the first channel from the first ADC group and the hardware filter from the second group are used.

**Figure 2. Peripheral configuration**

![](images/peripheral-configuration.png)

**Figure 2** shows the peripheral configuration of the code example. 

Consider that the code example has two components, one for generating the sine wave with variable frequency and another one for filtering the sine wave.

The DCSG Group 1 R2R DAC is used for generating the sine wave. The sine wave is generated using a look up table. The TCPWM[1] Group[0] Counter[0] is used for generating interrupts for the sine wave. It generates interrupts to PPCA CPU 0 in frequent intervals and inside the interrupt the look up table data is written to the R2R DAC. To change the frequency of the sine wave, Potentiometer R262 on the EVK is used. ADC group 3 channel 4 is used for that. Inside the PWM ISR, the ADC is triggered and in the next interrupt the value is read. Based on the ADC value, the PWM period is varied and variable frequency is achieved.

The ADC Group 0 Channel 2 is used in the filter path. This ADC is triggered in regular intervals using the timer TCPWM[0] Group[0] Channel[0] through EPU. EPU processing unit T1 0 and Combiner 62 are used for that. On the completion of the ADC processing the data is moved to an ADC filter for passing the data through it to the HW 3P3Z filter. The ADC Filter 0 is used for that. This is required as the dedicated hardware path from ADC to the HW 3P3Z filter is through the ADC filter. Data movement from ADC filter to HW Filter triggers the data processing. Completion of HW 3P3Z filter processing triggers an interrupt to PPCA CPU 0. EPU Processing unit T1 1 and Combiner 104 are used for that. Inside the EPU ISR the HW 3P3Z filter output is read, and written the data to a DAC to produce the analog output. DCSG Group 0 R2R DAC is used for that. 

**Figure 3** shows the timing of the peripheral activities in the code example.

**Figure 3. Timing diagram**

![](images/timing-diagram.png)

Use an external wire on the EVK to connect the generated sine wave (AIN1) to the input of the filter path (AIN1P). If the potentiometer is rotated, the sine wave frequency varies from 100 Hz to 20 kHz; when the frequency crosses 1000 Hz, observe that the amplitude of the filter output comes down. This is due to the cutoff frequency set to the filter path. Use an oscilloscope to monitor that by connecting it to AIN1 to see the source sine wave and AIN0 to see the filtered sine wave. 

**Figure 4** shows the sine wave generated and the filtered sine wave. 

**Figure 4. Output waveform**

![](images/output_sinewave.png)


### Resources and settings

**Table 1. Application resources**

Resource  |  Alias/object     |    Purpose
:-------- | :-------------    | :------------
UART (PDL)| DEBUG_UART | UART PDL object used for the debug UART port
PPCA ADC Group 0 ADC (PDL)    | ADC     | Reading analog input 0
PPCA ADC Group 0 AREF (PDL)    | AREF     | Generating analog reference 
PPCA ADC Group 0 Filter 0 (PDL)    | ADC_FILTER     | Digital filter for post processing ADC data 
PPCA Hardware Filter 0 (PDL) | HWFILTER | 3P3Z hardware filter for filtering the sine wave
PPCA TCPWM[0].Group[0].Counter[0] (PDL)    | PWM     | PWM for triggering ADC conversion and measurement
PPCA TCPWM[1].Group[0].Counter[0] (PDL)    | PWM     | PWM for running ISR to generate sine wave 
PPCA EPU Combiner 60 (PDL)    | ADC_TRIG     | Starting ADC conversion
PPCA EPU Combiner 104 (PDL)    | HWFILTER_INTR     | Interrupting PPCA CPU0 on HW filter EOP
PPCA EPU Processing Unit T1 0 (PDL)    | ADC_TRIG_PWM_TC     | Receiving terminal count trigger from PWM
PPCA EPU Processing Unit T1 1 (PDL)    | HWFILTER_EOC     | Receiving end of processing signal from HW filter
PPCA DCSG Group 0 R2R DAC (PDL)  | DAC_FILTERED  | DAC to output filtered signal
PPCA DCSG Group 1 R2R DAC (PDL)  | DAC_SINE  | DAC to generate sine wave

<br>


## Related resources

Resources  | Links
-----------|----------------------------------
Code examples  | [Using ModusToolbox&trade;](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software) on GitHub
Device documentation | [PSOC&trade; Control C3M/P8 MCU documents](https://www.infineon.com/products/microcontroller/32-bit-psoc-arm-cortex/32-bit-psoc-control-arm-cortex-m33-mcu/psoc-control-c3-performance-line?ftab=01#Documents)
Development kits | Select your kits from the [Evaluation board finder](https://www.infineon.com/cms/en/design-support/finder-selection-tools/product-finder/evaluation-board)
Libraries on GitHub  | [mtb-dsl-psc3m8](https://github.com/Infineon/mtb-dsl-psc3m8) – Device Support Library (DSL) <br> [retarget-io](https://github.com/Infineon/retarget-io) – Utility library to retarget STDIO messages to a UART port
Tools  | [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) – ModusToolbox&trade; software is a collection of easy-to-use libraries and tools enabling rapid development with Infineon MCUs for applications ranging from wireless and cloud-connected systems, edge AI/ML, embedded sense and control, to wired USB connectivity using PSOC&trade; Industrial/IoT MCUs, AIROC&trade; Wi-Fi and Bluetooth&reg; connectivity devices, XMC&trade; Industrial MCUs, and EZ-USB&trade;/EZ-PD&trade; wired connectivity controllers. ModusToolbox&trade; incorporates a comprehensive set of BSPs, HAL, libraries, configuration tools, and provides support for industry-standard IDEs to fast-track your embedded application development

<br>


## Other resources

Infineon provides a wealth of data at [www.infineon.com](https://www.infineon.com) to help you select the right device, and quickly and effectively integrate it into your design.


## Document history

Document title: *CE241356* – *PSOC&trade; Control C3M/P8: ADC-HWFilter-DAC dataflow*

 Version | Description of change
 ------- | ---------------------
 1.0.0   | New code example
<br>


All referenced product or service names and trademarks are the property of their respective owners.

The Bluetooth&reg; word mark and logos are registered trademarks owned by Bluetooth SIG, Inc., and any use of such marks by Infineon is under license.

PSOC&trade;, formerly known as PSoC&trade;, is a trademark of Infineon Technologies. Any references to PSoC&trade; in this document or others shall be deemed to refer to PSOC&trade;.

---------------------------------------------------------

(c) 2024-2026, Infineon Technologies AG, or an affiliate of Infineon Technologies AG. All rights reserved.
This software, associated documentation and materials ("Software") is owned by Infineon Technologies AG or one of its affiliates ("Infineon") and is protected by and subject to worldwide patent protection, worldwide copyright laws, and international treaty provisions. Therefore, you may use this Software only as provided in the license agreement accompanying the software package from which you obtained this Software. If no license agreement applies, then any use, reproduction, modification, translation, or compilation of this Software is prohibited without the express written permission of Infineon.
<br>
Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A SPECIFIC USE/PURPOSE OR MERCHANTABILITY. Infineon reserves the right to make changes to the Software without notice. You are responsible for properly designing, programming, and testing the functionality and safety of your intended application of the Software, as well as complying with any legal requirements related to its use. Infineon does not guarantee that the Software will be free from intrusion, data theft or loss, or other breaches (“Security Breaches”), and Infineon shall have no liability arising out of any Security Breaches. Unless otherwise explicitly approved by Infineon, the Software may not be used in any application where a failure of the Product or any consequences of the use thereof can reasonably be expected to result in personal injury.

