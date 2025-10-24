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
#define TX_MESSAGE_SIZE  64

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


uint16 raw_count; 
uint8_t mode_flag = 0; // positive = shear, zero = normal
int16_t processed_data_array[8];

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

    // --- Mode 0: Read individual, evenly-spaced sensors ---
    if (mode_flag == 0)
    {

        for (i = 0; i < 4; i++)
        {
             uint8_t base_element_index = i * 2;
            
            // Read the raw count for each specified proximity sensor
            uint16_t sensor_a = CapSense_dsRam.snsList.top_plate[base_element_index].raw[0];
            uint16_t sensor_b = CapSense_dsRam.snsList.top_plate[base_element_index+1].raw[0];
            
            // Store the sum
            processed_data_array[i] = sensor_a + sensor_b;
        }
    }
    // --- Mode 1: Read sensor pairs and find the difference ---
    else
    {
        for (i = 0; i < 4; i++)
        {
            uint8_t base_element_index = i * 2;

            // Read both raw counts from the pair
            uint16_t sensor_a = CapSense_dsRam.snsList.top_plate[base_element_index].raw[0];
            uint16_t sensor_b = CapSense_dsRam.snsList.top_plate[base_element_index+1].raw[0];

            // Store the difference
            processed_data_array[i+4] = sensor_a - sensor_b;
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
    while (*s != '\0') {
        UART_SpiUartWriteTxData((uint32)*s);
        s++;
    }
}

// /*******************************************************************************
// * Function Name: IsShieldingActive
// ********************************************************************************
// * Summary:
// * Checks the CapSense configuration data structure for the Active Shield flag.
// * According to the CapSense register map, 
// *   CDS0_CONFIG is a uint16
// *   SHLD_EN, the shield electrode signal enable/disable, is bit 8
// * Return:
// * 1 if shielding is configured as Active, 0 otherwise.
// *******************************************************************************/
// uint8 IsShieldingActive(void)
// {    
//     if ((CapSense_dsRam.csd0Config >> 8) & 1) {
//         return 1;  
//     } else {
//         return 0;  
//     } 
// }

/*******************************************************************************
* Function Name: CalibrateCapSense
********************************************************************************
* Summary:
*
* Parameters:
* widgetId: The ID of the widget to calibrate (e.g., CapSense_top_plate_WDGT_ID).
*
* Return:
* None
*******************************************************************************/

// Macros

#define MOD_IDAC        4u    // Lower = more sensitive (4–8 typical)
#define COMP_IDAC       4u
#define SNS_CLK_DIV     1u    // Lower divider = higher resolution
#define RESOLUTION_BITS 16u   // Max precision


void CalibrateCapSense(uint32 widgetID) {
    
    
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

    
    
    // *** CRITICAL CHANGE: Only format the number and newline characters ***
    // This is the most streamlined way to log data.
    
    // prints each value in the processed array with the corresponding electrode
        // Format the string with the mode, electrode index, and processed count
    
        uint mode_bit = (mode_flag == 0) ? 0 : 1;
        sprintf(txMessage, "\n%u,%d,%d,%d,%d,%d,%d,%d,%d\r", 
                mode_bit,  
                processed_data_array[0],
                processed_data_array[1],
                processed_data_array[2],
                processed_data_array[3],
                processed_data_array[4],
                processed_data_array[5],
                processed_data_array[6],
                processed_data_array[7]
        );
        
        // only prints after both modes have been done
        if(mode_flag){
            UART_PutString(txMessage);
            CyDelay(100);
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


/* [] END OF FILE */