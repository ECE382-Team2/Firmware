/*******************************************************************************
* File Name: cyapicallbacks.c
*
* Version: 1.0
* 
* Description: This file contains the function that is used to change the 
*              inactive sensor connections based on the sensor that is
*              being scanned currently.
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

#include "cyapicallbacks.h"

/*******************************************************************************
* Function Name: CapSense_StartSampleCallback
********************************************************************************
* Summary:
* This function is called before the CapSense scan begins.
* The drive mode of the sensors which are adjacent to the sensor being scanned 
* is set to shield and all other inactive sensors are connected to ground.
*
* Parameters:
*  \param currentWidgetIndex
*  Specifies the ID number of the widget that is going to be
*  scanned.
*  A macro for the widget ID can be found in the CapSense Configuration header
*  file defined as CapSense_<WidgetName>_WDGT_ID.
*
* \param currentSensorIndex
*  Specifies the ID number of the sensor within the widget that is
*  going to be scanned.
*  A macro for the sensor ID within a specified widget can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_SNS<SensorNumber>_ID
*
*******************************************************************************/
/*
void CapSense_StartSampleCallback (uint32 currentWidgetIndex, uint32 currentSensorIndex)
{
   
    uint8 sensorIndex;
    
    if(currentWidgetIndex == CapSense_LINEARSLIDER_WDGT_ID)
    {
        for(sensorIndex = 0; sensorIndex < CapSense_LINEARSLIDER_NUM_SENSORS; sensorIndex++)
        {
            if(sensorIndex != currentSensorIndex)
            {
                if((sensorIndex == (currentSensorIndex - 1)) || (sensorIndex == (currentSensorIndex + 1)))
                {
                    // If the sensor is adjacent to the sensor being scanned, 
                     // configure it as shield. 
                     
                    CapSense_SetPinState(CapSense_LINEARSLIDER_WDGT_ID, sensorIndex, CapSense_SHIELD);
                }
                else
                {
                    // If the sensor is not adjacent to the sensor being 
                     / scanned, connect it to ground. 
                     //
                    CapSense_SetPinState(CapSense_LINEARSLIDER_WDGT_ID, sensorIndex, CapSense_GROUND);
                }
            }
        }
    }
}
*/

/* [] END OF FILE */
