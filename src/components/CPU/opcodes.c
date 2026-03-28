#include "CPU.h"
#include "opcodes.h"
#include <stdio.h>

// Loads a specific value directly (e.g. LDA #$05 -> Loads $05 into Accumulator)
// $05 is the next byte in memory 
uint8_t IMM_ADDR(CPU_6502* CPU){
    CPU -> addr_abs = CPU -> PC;
    CPU -> PC++;
    return 1;
}


// Next two bytes provide memory address (Byte 1 is the lowest 8 bytes, Byte 2 is the highest 8 bytes)
uint8_t ABS_ADDR(CPU_6502* CPU){
    uint16_t low = (*CPU -> cpu_read)(CPU, CPU -> PC, false);
    CPU -> PC++;
    uint16_t high = (*CPU -> cpu_read)(CPU, CPU -> PC, false);
    CPU -> PC++;

    CPU -> addr_abs = (high << 8) + low;
    return 1;
}


// Loads Contents of a memoroy address from 0x00 to 0xFF
// Same as ABS_ADDR except only one byte is needed
uint8_t ZERO_ADDR(CPU_6502* CPU){
    uint8_t byte = (*CPU -> cpu_read)(CPU, CPU -> PC, false);
    CPU-> PC++;
    CPU -> addr_abs = byte;
    return 1;
}

// E.g. LDA $10 X, Adds X to content of memory address which is wrapped around (0x00 to 0xFF) if nessecary
// Loads this memory address
uint8_t ZEROX_ADDR(CPU_6502* CPU){
    uint8_t address = (*CPU -> cpu_read)(CPU, CPU -> PC, false);
    CPU -> PC++;
    // Will handle wrapping around because its 8 bit int.
    address += CPU -> Reg_X;
    CPU -> addr_abs = address;
    return 1;
}



// Opcodes - Instructions
uint8_t LDA(CPU_6502* CPU){
    CPU -> Reg_Acc = CPU -> fetched;

    if (CPU -> fetched == 0){
        CPU -> Reg_Status |= (1 << 1);
    } else {
        CPU -> Reg_Status &= ~(1 << 1);
    }

    if ((CPU -> fetched & (1 << 7)) != 0){
        CPU -> Reg_Status |= (1 << 7);
    } else {
        CPU -> Reg_Status &= ~(1 << 7);
    }

    return 1;
}