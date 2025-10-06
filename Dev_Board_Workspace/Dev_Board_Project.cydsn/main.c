/*****************************************************************************
* File Name: main.c
*
* Version: 1.00
*
* Description: This example demonstrates the custom scanning capability 
*              of CapSense using callbacks for linear slider. 
*
* Hardware Dependency: This example supports the following kits:
*                      CY8CKIT-149 PSoC® 4100S Plus Prototyping Kit,   
*                      CY8CKIT-145-40XX PSoC® 4000S CapSense Prototyping Kit.
******************************************************************************
* Copyright (2020), Cypress Semiconductor Corporation.
******************************************************************************
* This software, including source code, documentation and related materials
* ("Software") is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and 
* foreign), United States copyright laws and international treaty provisions. 
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the 
* Cypress source code and derivative works for the sole purpose of creating 
* custom software in support of licensee product, such licensee product to be
* used only in conjunction with Cypress's integrated circuit as specified in the
* applicable agreement. Any reproduction, modification, translation, compilation,
* or representation of this Software except as specified above is prohibited 
* without the express written permission of Cypress.
* 
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes to the Software without notice. 
* Cypress does not assume any liability arising out of the application or use
* of Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use as critical components in any products 
* where a malfunction or failure may reasonably be expected to result in 
* significant injury or death ("ACTIVE Risk Product"). By including Cypress's 
* product in a ACTIVE Risk Product, the manufacturer of such system or application
* assumes all risk of such use and in doing so indemnifies Cypress against all
* liability. Use of this Software may be limited by and subject to the applicable
* Cypress software license agreement.
*****************************************************************************/

/*******************************************************************************
*   Included Headers
*******************************************************************************/ 
#include "project.h"

/*******************************************************************************
* MACRO Definitions
*******************************************************************************/     
/* Boolean constants */
#define LED_ON         (0u)
#define LED_OFF        (1u)

/* Define the number of slider LEDs in the board. The number of LEDs is equal 
 * to no. of slider segments.
 */
#define NUM_LEDS CapSense_LINEARSLIDER_NUM_SENSORS

/* Defining step size for LED control based on slider resolution */
#define POSITION_STEP_SIZE (CapSense_LINEARSLIDER_X_RESOLUTION/NUM_LEDS)

/* Slider position threshold to make the corresponding LED glow */
#define LED0_SLIDER_POS_THRESHOLD  (0 * POSITION_STEP_SIZE)
#define LED1_SLIDER_POS_THRESHOLD  (1 * POSITION_STEP_SIZE)
#define LED2_SLIDER_POS_THRESHOLD  (2 * POSITION_STEP_SIZE)
#define LED3_SLIDER_POS_THRESHOLD  (3 * POSITION_STEP_SIZE)
#define LED4_SLIDER_POS_THRESHOLD  (4 * POSITION_STEP_SIZE)

/*****************************************************************************
*   Function Prototypes
*****************************************************************************/ 
void DetectTouchAndDriveLed(void);
 
/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
* This function performs the following actions:
*
*     1. Starts the EZI2C component and exposes the CapSense_dsRam structure as 
*        the EZI2C buffer.
*     2. Starts the CapSense component.
*     3. Scans the CSD sensor in a continuous loop and processes the sensor 
*        status when scan is completed.
*     4. Calls CapSense_RunTuner() to establish synchronized communication with
*        Tuner application.
*
*******************************************************************************/
int main(void)
{
    /* Enable global interrupts. */
    CyGlobalIntEnable; 
    
    /* Start the EZI2C component to tune CapSense sensors */  
    EZI2C_Start(); 
    
    /* Set the CapSense data structure as the I2C buffer to be exposed to the 
     * master on primary slave address interface. Any I2C host tools such as the
     * Tuner or the Bridge Control Panel can read this buffer but you can 
     * connect only one tool at a time.
     */
    EZI2C_EzI2CSetBuffer1(sizeof(CapSense_dsRam), sizeof(CapSense_dsRam),
                            (uint8_t *)&(CapSense_dsRam));
    

    /* Start the CapSense block */
    CapSense_Start();
    
    /* Scan all the enabled widgets */
    CapSense_ScanAllWidgets();
    
    for(;;)
    {
        /* Initiate a new scan only if the CapSense block is idle */ 
        if(CapSense_NOT_BUSY == CapSense_IsBusy())
        {
            /* Performs full data processing of all enabled widgets */
            CapSense_ProcessAllWidgets();
            
            /* Sync with the Tuner application to fetch the latest CapSense  
             * parameters and update back the sensor data such as rawcount,
             * baseline etc.
             */
            CapSense_RunTuner();
            
            /* Controls LEDs Status based on the result of Widget processing. */
            DetectTouchAndDriveLed();
            
            /* Scan all enabled widgets */
            CapSense_ScanAllWidgets();
        }
    }
}


/*******************************************************************************
* Function Name: DetectTouchAndDriveLEDs
********************************************************************************
* Summary:
* This function controls LEDs based on the slider touch position. 
* When a touch is detected, LEDs up to the touch position are turned ON 
* to provide a bar graph representation of the slider position.
*
*******************************************************************************/
void DetectTouchAndDriveLed(void)
{   
    /* Variable to check the position of the slider */
    uint32 sliderPos;   
    
    /* If slider is touched, drive the LEDs based on the touch position */
    if(CapSense_IsWidgetActive(CapSense_LINEARSLIDER_WDGT_ID))
    {
        /* Get the touch position (centroid) of the slider */
        sliderPos = CapSense_GetCentroidPos(CapSense_LINEARSLIDER_WDGT_ID);
        
        /* Turn the LEDs ON based on the finger position (centroid) on the 
         * CapSense Linear slider 
         */
        LED_0_Write((sliderPos > LED0_SLIDER_POS_THRESHOLD) ? LED_ON : LED_OFF);
        LED_1_Write((sliderPos > LED1_SLIDER_POS_THRESHOLD) ? LED_ON : LED_OFF);
        LED_2_Write((sliderPos > LED2_SLIDER_POS_THRESHOLD) ? LED_ON : LED_OFF);
        LED_3_Write((sliderPos > LED3_SLIDER_POS_THRESHOLD) ? LED_ON : LED_OFF);
        LED_4_Write((sliderPos > LED4_SLIDER_POS_THRESHOLD) ? LED_ON : LED_OFF);
    }
    else 
    {
        /* If the slider is not touched, turn all the LEDs OFF */
        LED_0_Write(LED_OFF);
        LED_1_Write(LED_OFF);
        LED_2_Write(LED_OFF);
        LED_3_Write(LED_OFF);
        LED_4_Write(LED_OFF); 
    }
}


/* [] END OF FILE */
