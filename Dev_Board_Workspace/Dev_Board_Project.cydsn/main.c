/*****************************************************************************
* File Name: main.c
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
#define TX_MESSAGE_SIZE  16

/* Calibration Constants */
#define CALIB_NUM_SAMPLES 50
#define CALIB_DELAY_MS

/*****************************************************************************
* Function Prototypes & Global Variables
*****************************************************************************/
void UART_PutString(const char *s);
uint8 IsShieldingActive(void);
void CalibrateCapSense(uint32 widgetId);
void DetectTouchAndDriveLed(void);

uint16 raw_count; 

/*******************************************************************************
* Function Name: UART_PutString()
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
    while (*s != '\0') {
        UART_SpiUartWriteTxData((uint32)*s);
        s++;
    }
}

/*******************************************************************************
* Function Name: IsShieldingActive
********************************************************************************
* Summary:
* Checks the CapSense configuration data structure for the Active Shield flag.
* According to the CapSense register map, 
*   CDS0_CONFIG is a uint16
*   SHLD_EN, the shield electrode signal enable/disable, is bit 8
* Return:
* 1 if shielding is configured as Active, 0 otherwise.
*******************************************************************************/
uint8 IsShieldingActive(void)
{    
    if ((CapSense_dsRam.csd0Config >> 8) & 1) {
        return 1;  
    } else {
        return 0;  
    } 
}

/*******************************************************************************
* Function Name: CalibrateCapSense
********************************************************************************
* Summary:
*
* Parameters:
* widgetId: The ID of the widget to calibrate (e.g., CapSense_PROXIMITY0_WDGT_ID).
*
* Return:
* None
*******************************************************************************/
void CalibrateCapSense(uint32 widgetID) {
    
    char txMessage[TX_MESSAGE_SIZE];
    uint32 status;
        
    // Start the tuning process. This is required before calling auto-calibration.
    CapSense_RunTuner();
    
    UART_PutString("--- Starting CapSense Widget Calibration ---\r\n");
    
    /* CapSense block as a CalibrateWidget(). It calibrates all the IDACs for a given 
    widget. IDAC is Current Digital-to-Analog Converter. I think it adjusts the current range?
    
    Returns 0 if calibration was successful. 
    */ 
    status = CapSense_CalibrateWidget(widgetID); 
    sprintf(txMessage, "%lu\r\n", status); 
    UART_PutString(txMessage);
    
    UART_PutString("--- Calibration Complete ---\r\n");
    
}

/*******************************************************************************
* Function Name: DetectTouchAndDriveLed
********************************************************************************
* Summary:
* Reads CapSense data, gets shield status, and transmits both in the format: [Status],[Raw Count]\r\n
*******************************************************************************/
void DetectTouchAndDriveLed(void)
{
    char txMessage[TX_MESSAGE_SIZE]; 
    uint8 shield_status; // Local variable to hold the status
    
    // Raw capacitance count
    raw_count = CapSense_dsRam.snsList.proximity0[0].raw[0]; 
    
    // Shield status
    shield_status = IsShieldingActive(); 

    // LED Control: 
    if (CapSense_IsWidgetActive(CapSense_PROXIMITY0_WDGT_ID)) {
        LED_0_Write(LED_ON);
    } else {
        LED_0_Write(LED_OFF);
    }
    
    // Message: [ShieldStatus],[RawCount]\r\n
    sprintf(txMessage, "%u,%u\r\n", shield_status, raw_count); 
    UART_PutString(txMessage);
    CyDelay(50);
}

/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
* Initializes components, enables interrupts, and runs the main CapSense
* scanning loop.
*******************************************************************************/
int main(void)
{
    /* Enable global interrupts (required for CapSense/UART operation) */
    CyGlobalIntEnable;

    /* Start the UART component */
    UART_Start();

    /* Send a start message to confirm the link */
    UART_PutString("--- PSoC CapSense Logger Initialized ---\r\n");

    /* Start the CapSense block */
    CapSense_Start();
    
    /* Calibrate CapSense block */
    CalibrateCapSense(CapSense_PROXIMITY0_WDGT_ID);

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


/* [] END OF FILE */