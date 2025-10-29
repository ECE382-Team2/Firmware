// globals.h

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h> // For standard types like uint8_t

// Declare the variable as 'extern'.
// This tells other files: "This variable exists, but it's defined elsewhere."
extern volatile uint8_t mode_flag;
extern volatile int32_t processed_data_array[8][4];
extern volatile uint32_t current_count;

#endif // GLOBALS_H