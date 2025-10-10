/*****************************************************************************
* File Name: main.c
*
* Version: 1.00
*
* Description: This example demonstrates custom CapSense data logging over UART.
* It includes a startup message for easy verification of the serial link.
*
*****************************************************************************/

/*******************************************************************************
* Included Headers
*******************************************************************************/
#include "project.h"
#include <stdio.h>
#include <string.h>

/*******************************************************************************
* MACRO Definitions
*******************************************************************************/

/* Boolean constants for LED control */
#define LED_ON           (0u)
#define LED_OFF          (1u)

/* Define the size for the serial message buffer */
#define TX_MESSAGE_SIZE  64

/*****************************************************************************
* Function Prototypes
*****************************************************************************/
void DetectTouchAndDriveLed(void);
void UART_PutString(const char *s);
void Post_Process(void);

// Global variables
uint16 raw_count; 
uint8_t mode_flag = 0; // positive = shear, zero = normal
int16_t processed_data_array[4];

// Functions

/*******************************************************************************
* Function Name: Post_Process()
********************************************************************************
* Summary:
* Changes the processed_data_array depending on the mode
*
* Parameters:
*
* Return:
* None
*******************************************************************************/

/**
 * @brief Processes CapSense raw counts using the recommended CapSense API.
 *
 * This function assumes the following are available:
 * - A CapSense widget named "Proximity0" (adjust macro if name is different).
 * - volatile uint8_t mode_flag;
 * - int16_t processed_array[4];
 */
void Post_Process(void)
{
    uint8_t i; // Local loop counter
    
    CapSense_FLASH_WD_STRUCT const *proximityWidgetId;

    proximityWidgetId = CapSense_dsFlash.wdgtArray;

    // --- Mode 0: Read individual, evenly-spaced sensors ---
    if (mode_flag == 0)
    {
        // An array of the sensor *element* indexes we want to read
        const uint8_t sensor_element_indexes[4] = {0, 2, 4, 6};
        
        for (i = 0; i < 4; i++)
        {
            // Use the API to get the raw count for a specific sensor element
            // within the main proximity widget.
            processed_data_array[i] = CapSense_DpProcessCsdWidgetRawCounts(proximityWidgetId + sensor_element_indexes[i]);
        }
    }
    // --- Mode 1: Read sensor pairs and find the difference ---
    else 
    {
        for (i = 0; i < 4; i++)
        {
            // Calculate the base sensor element index for each pair (0, 2, 4, 6)
            uint8_t base_element_index = i * 2;
            
            // Get the raw counts for the sensor pair using the API
            uint16_t sensor_a = CapSense_DpProcessCsdWidgetRawCounts(proximityWidgetId + base_element_index);
            uint16_t sensor_b = CapSense_DpProcessCsdWidgetRawCounts(proximityWidgetId + base_element_index + 1);
            
            // Subtract the second sensor's value from the first
            processed_data_array[i] = sensor_a - sensor_b;
        }
    }
}


/*******************************************************************************
* Function Name: UART_PutString_Robust()
********************************************************************************
* Summary:
* Sends a null-terminated string character-by-character using the lowest-level
* SCB UART transmit API (UART_SpiUartWriteTxData).
*
* Parameters:
* s: Pointer to the null-terminated string to transmit.
*
* Return:
* None
*******************************************************************************/
void UART_PutString(const char *s)
{
    while (*s != '\0')
    {
        UART_SpiUartWriteTxData((uint32)*s);
        s++;
    }
}

/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
* Initializes components, enables interrupts, and runs the main CapSense
* scanning loop.
*
* Return:
* int (Never returns in a typical embedded loop)
*******************************************************************************/
int main(void)
{
    /* Enable global interrupts (required for CapSense/UART operation) */
    CyGlobalIntEnable;

    /* Start the UART component */
    UART_Start();

    /* Send a welcome message to confirm the link */
    UART_PutString("--- PSoC CapSense Logger Initialized ---\r\n");

    /* Start the CapSense block */
    CapSense_Start();

    /* Initiate the first scan of all enabled widgets */
    CapSense_ScanAllWidgets();

    for (;;)
    {
        /* Check if the CapSense hardware block is done with the current scan */
        if (CapSense_NOT_BUSY == CapSense_IsBusy())
        {
            /* Process the raw sensor data (filtering, baseline, detection) */
            CapSense_ProcessAllWidgets();
            
            // Post process the sensor data
            Post_Process();
            
            /* Handle LED control and send the debug message over UART */
            DetectTouchAndDriveLed();

            // toggles the mode we are in after succesfully writing
            mode_flag = ~(mode_flag);
            /* Start the next scan of all enabled widgets */
            CapSense_ScanAllWidgets();
        }
    }
}

/*******************************************************************************
* Function Name: DetectTouchAndDriveLed
********************************************************************************
* Summary:
* Reads the CapSense Proximity sensor data and transmits ONLY the raw count 
* as a number followed by a newline via UART.
*
* Parameters:
* None
*
* Return:
* None
*******************************************************************************/
void DetectTouchAndDriveLed(void)
{
    // Reduce buffer size to just the raw number and necessary characters
    char txMessage[TX_MESSAGE_SIZE]; 

    // LED Control: (No Change)
    if (CapSense_IsWidgetActive(CapSense_PROXIMITY0_WDGT_ID))
    {
        LED_0_Write(LED_ON);
    }
    else
    {
        LED_0_Write(LED_OFF);
    }
    
    // *** CRITICAL CHANGE: Only format the number and newline characters ***
    // This is the most streamlined way to log data.
    
    // prints each value in the processed array with the corresponding electrode
    for (uint8_t i = 0; i < 4; i++)
    {
        // Format the string with the mode, electrode index, and processed count
        sprintf(txMessage, "\n Mode: %u, Electrode: %u, count: %d\r", 
                (unsigned int)mode_flag, 
                (unsigned int)i, 
                processed_data_array[i]);
        
        // Send the fully formatted string over the UART
        UART_PutString(txMessage);
    }
    CyDelay(50);
}

/* [] END OF FILE */
