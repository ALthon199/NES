#ifndef OPCODES_H
#define OPCODES_H

#include <stdio.h>
#include "CPU.h"


// Addressing Modes

// Immediate 
uint8_t IMM_ADDR(CPU_6502* CPU);

// Absolute Addressing
uint8_t ABS_ADDR(CPU_6502* CPU);

// Zero Page Addr
uint8_t ZERO_ADDR(CPU_6502* CPU);

// Zero Page X Addr
uint8_t ZEROX_ADDR(CPU_6502* CPU);

// Opcodes - Instructions
uint8_t LDA(CPU_6502* CPU);

#endif