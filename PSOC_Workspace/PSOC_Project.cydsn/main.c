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
#include <stdint.h>     // for fixed width types
#include <stdbool.h>    // for bool

/*******************************************************************************
* MACRO Definitions
*******************************************************************************/
/* Boolean constants for LED control */
#define LED_ON          (0u)
#define LED_OFF         (1u)

/* Define the size for the serial message buffer */
#define TX_MESSAGE_SIZE  128

/* Calibration Constants */
#define CALIB_NUM_SAMPLES 50
#define CALIB_DELAY_MS

/* Moving Average filter configuration */
#define AVG_NUM_SENSORS        8u     /* number of proximity sensors */
#define AVG_FILTER_NUM_SAMPLES 10u    /* Number of samples to average. Adjust as needed. */


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

#ifdef CALIBRATION_MODE
volatile int32_t processed_data_array[8][4] = {0};
volatile uint32_t current_count = 0;
#endif

#ifdef VISUALIZATION_MODE
volatile int32_t processed_data_array[16] = {0};
#endif
/* Moving Average filter internal state */
static uint16_t avg_history_buffer[AVG_NUM_SENSORS][AVG_FILTER_NUM_SAMPLES];
static uint32_t avg_current_sum[AVG_NUM_SENSORS] = { 0 };
static uint8_t  avg_current_index[AVG_NUM_SENSORS] = { 0 };
static bool     avg_initialized[AVG_NUM_SENSORS] = { false };


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
 * @brief Processes CapSense raw counts and applies an N-sample moving
 * average filter per sensor.
 *
 * Writes filtered output to processed_data_array[i][0] and raw count to
 * processed_data_array[i][3].
 */
void Post_Process(void)
{
    uint8_t i;
    
    #ifdef CALIBRATION_MODE
    for (i = 0; i < AVG_NUM_SENSORS; i++)
    {
        /* Read raw sensor value — adjust path if your CapSense RAM layout differs */
        uint16_t sensor_raw = CapSense_dsRam.snsList.top_plate[i].raw[0];
        
        /* Store raw in column 3 (as original code used) */
        processed_data_array[i][3] = (int32_t)sensor_raw;

        /* Initialize filter state on first sample for this sensor */
        if (!avg_initialized[i])
        {
            // Pre-fill the history buffer with the first sample
            for(uint8_t j = 0; j < AVG_FILTER_NUM_SAMPLES; j++)
            {
                avg_history_buffer[i][j] = sensor_raw;
            }
            // Set the initial sum
            avg_current_sum[i] = (uint32_t)sensor_raw * AVG_FILTER_NUM_SAMPLES;
            avg_current_index[i] = 0;
            avg_initialized[i] = true;
        }
        else
        {
            /* Implement the efficient moving average (running sum) */
            
            // 1. Subtract the oldest sample (which is at the current index)
            avg_current_sum[i] -= avg_history_buffer[i][avg_current_index[i]];
            
            // 2. Add the new sample
            avg_current_sum[i] += sensor_raw;
            
            // 3. Replace the oldest sample with the new one
            avg_history_buffer[i][avg_current_index[i]] = sensor_raw;
            
            // 4. Increment and wrap the circular buffer index
            avg_current_index[i]++;
            if (avg_current_index[i] >= AVG_FILTER_NUM_SAMPLES)
            {
                avg_current_index[i] = 0;
            }
        }

        /* Store filtered value into processed_data_array[i][0] */
        /* Calculate average, adding (N/2) for proper integer rounding */
        uint32_t rounded_avg = (avg_current_sum[i] + (AVG_FILTER_NUM_SAMPLES / 2)) / AVG_FILTER_NUM_SAMPLES;
        processed_data_array[i][3] = (int32_t)rounded_avg;
        
        /* processed_data_array[i][1] and processed_data_array[i][2] are left unchanged */
    }
    #endif
    
    #ifdef VISUALIZATION_MODE
    //insert averaging shit here
    uint8_t mode_offset = 8*mode_flag;
    for(i = 0; i<8; i++)
    {
        processed_data_array[i+mode_offset] = CapSense_dsRam.snsList.top_plate[i].raw[0];
    }
    #endif
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
    
    // This is the most streamlined way to log data.
    
    // prints each value in the processed array with the corresponding electrode
        // Format the string with the mode, electrode index, and processed count
    
        #ifdef CALIBRATION_MODE
        for( int i = 0; i<8; i++)
        {
            // creates message: filtered, raw, unused, raw (we keep original 4 columns)
            sprintf(txMessage, "\n%d,%d,%d,%d\r",
            processed_data_array[i][0], /* filtered */
            processed_data_array[i][1],
            processed_data_array[i][2],
            processed_data_array[i][3]); /* raw */
            UART_PutString(txMessage);
            
        }
        // small delay to slow datarate
        if(mode_flag == 1)
        {
            //CyDelay(1000);
            
        }
        #endif
        
        #ifdef VISUALIZATION_MODE
            
        // small delay to slow datarate
        if(mode_flag == 1)
        {
            sprintf(txMessage, "\n%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r", 
                processed_data_array[0],
                processed_data_array[1],
                processed_data_array[2],
                processed_data_array[3],
                processed_data_array[4],
                processed_data_array[5],
                processed_data_array[6],
                processed_data_array[7],
                processed_data_array[8],
                processed_data_array[9],
                processed_data_array[10],
                processed_data_array[11],
                processed_data_array[12],
                processed_data_array[13],
                processed_data_array[14],
                processed_data_array[15]
            );
            UART_PutString(txMessage);
            //CyDelay(1000);
        }
        // Send the fully formatted string over the UART       
        #endif
              
     
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