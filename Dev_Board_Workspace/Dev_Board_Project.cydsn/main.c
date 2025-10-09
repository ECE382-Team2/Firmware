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

// Global variables
uint16 raw_count; 

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

            /* Handle LED control and send the debug message over UART */
            DetectTouchAndDriveLed();

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
    
    // This line retrieves the raw capacitance count.
    raw_count = CapSense_dsRam.snsList.proximity0[0].raw[0]; 

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
    sprintf(txMessage, "%u\r\n", raw_count);
    
    UART_PutString(txMessage);
    CyDelay(50);
}

/* [] END OF FILE */
