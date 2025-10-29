/*****************************************************************************
* File Name: main.c
*****************************************************************************/
/*******************************************************************************
* Included Headers
*******************************************************************************/
#include "project.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>

/*******************************************************************************
* MACRO Definitions
*******************************************************************************/
/* Boolean constants for LED control */
#define LED_ON           (0u)
#define LED_OFF          (1u)

/* Define the size for the serial message buffer */
#define TX_MESSAGE_SIZE  128

/* Calibration Constants */
#define CALIB_NUM_SAMPLES 50
#define CALIB_DELAY_MS

/*****************************************************************************
* Function Prototypes & Global Variables
*****************************************************************************/
void UART_PutString(const char *s);
void Post_Process(void);
void CalibrateCapSense(uint32 widgetID);
void DetectTouchAndDriveLed(void);

// global definitions
uint16 raw_count; 
volatile uint8_t mode_flag = 0; // positive = shear, zero = normal
volatile int32_t processed_data_array[8][4] = {0};
volatile uint32_t current_count = 0;

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
    uint8_t i;
    uint32_t widgetId = 0u; // Assuming proximity widget is index 0 — change if needed

    // Write to processed_data_array[0..7][3]
    for (i = 0; i < 8; i++)
    {
        uint16_t sensor_value = CapSense_dsRam.snsList.top_plate[i].raw[0];
        processed_data_array[i][3] = sensor_value;
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
    while (*s != '\0') {
        UART_SpiUartWriteTxData((uint32)*s);
        s++;
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
    
    // calculates the time that passed between publishing

    //if(delta < 0){delta = delta + My_Time_TC_PERIOD_VALUE;}
    
    // *** CRITICAL CHANGE: Only format the number and newline characters ***
    // This is the most streamlined way to log data.
    
    // prints each value in the processed array with the corresponding electrode
        // Format the string with the mode, electrode index, and processed count
    
        for( int i = 0; i<8; i++)
        {
            // creates message
            sprintf(txMessage, "\n%d,%d,%d,%d\r",
            processed_data_array[i][0],
            processed_data_array[i][1],
            processed_data_array[i][2],
            processed_data_array[i][3]);
            UART_PutString(txMessage);
     
        }
        
        // small delay to slow datarate
        if(mode_flag == 1)
        {
            //CyDelay(1000);
        }
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

    // Timer Stuff for time stamps
    My_Time_Start();
    
    /* Start the UART component */
    UART_Start();

    /* Send a start message to confirm the link */
    UART_PutString("--- PSoC CapSense Logger Initialized ---\r\n");
    
    /* Start the CapSense block */
    CapSense_Start();
    
    /* Calibrate CapSense block */
    //CalibrateCapSense(CapSense_PROXIMITY0_WDGT_ID);
    
    //CapSense_CalibrateAllWidgets();
    /* Initiate the first scan of all enabled widgets */
    CapSense_ScanAllWidgets();

    for (;;)
    {
        /* Check if the CapSense hardware block is done with the current scan */
        if (CapSense_NOT_BUSY == CapSense_IsBusy())
        {
            /* Process the raw sensor data (filtering, baseline, detection) */
            CapSense_ProcessAllWidgets();
            
            // Post process the sensor data. currently commented out so that each sensor reading is handled seperately
            Post_Process(); 
            
            /* Handle LED control and send the debug message over UART */
            DetectTouchAndDriveLed();

            // toggles the mode we are in after succesfully writing
            if( mode_flag == 1){mode_flag = 0;}
            else               {mode_flag = 1;}
            
            /* Start the next scan of all enabled widgets */
            CapSense_ScanAllWidgets();
        }
    }
}


/* [] END OF FILE */