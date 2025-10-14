// globals.h

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h> // For standard types like uint8_t

// Declare the variable as 'extern'.
// This tells other files: "This variable exists, but it's defined elsewhere."
extern volatile uint8_t mode_flag;

#endif // GLOBALS_H