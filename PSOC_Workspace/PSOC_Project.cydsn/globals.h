// globals.h

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h> // For standard types like uint8_t

    
//#define CALIBRATION_MODE
#define VISUALIZATION_MODE

    
// Declare the variable as 'extern'.
// This tells other files: "This variable exists, but it's defined elsewhere."
extern volatile uint8_t mode_flag;
    
#ifdef CALIBRATION_MODE
extern volatile int32_t processed_data_array[8][4];
extern volatile uint32_t current_count;
#endif

#ifdef VISUALIZATION_MODE
extern volatile int32_t processed_data_array[16];
#endif


#endif // GLOBALS_H