#include "CPU.h"
#include "Bus.h"
#include "opcodes.h"
#include <stdio.h>

// Loads a specific value directly (e.g. LDA #$05 -> Loads $05 into Accumulator)
// $05 is the next byte in memory 
uint8_t IMM_ADDR(CPU_6502 *CPU){
    CPU -> addr_abs = CPU -> PC;
    CPU -> PC++;
    return 0;
}


// Next two bytes provide memory address (Byte 1 is the lowest 8 bytes, Byte 2 is the highest 8 bytes)
uint8_t ABS_ADDR(CPU_6502 *CPU){
    uint16_t low = (*CPU -> cpu_read)(CPU, CPU -> PC, false);
    CPU -> PC++;
    uint16_t high = (*CPU -> cpu_read)(CPU, CPU -> PC, false);
    CPU -> PC++;

    CPU -> addr_abs = (high << 8) + low;
    return 0;
}

// Loads Contents of a memory address from 0x00 to 0xFF
// Same as ABS_ADDR except only one byte is needed
uint8_t ZERO_ADDR(CPU_6502 *CPU){
    uint8_t byte = (*CPU -> cpu_read)(CPU, CPU -> PC, false);
    CPU-> PC++;
    CPU -> addr_abs = byte;
    return 0;
}

// E.g. LDA $10 X, Adds X to content of memory address which is wrapped around (0x00 to 0xFF) if nessecary
// Loads this memory address
uint8_t ZEROX_ADDR(CPU_6502 *CPU){
    uint8_t address = (*CPU -> cpu_read)(CPU, CPU -> PC, false);
    CPU -> PC++;
    // Will handle wrapping around because its 8 bit int.
    address += CPU -> Reg_X;
    CPU -> addr_abs = address;
    return 0;
}


// Opcodes - Instructions

// LDA: Loads into accumulator the fetched value
// Requires fetch of address
// If fetched value is 0, set zero flag
// If fetched value is <0, set negative flag
uint8_t LDA(CPU_6502 *CPU){
    fetch(CPU);
    CPU -> Reg_Acc = CPU -> fetched;

    set_flag(CPU, ZERO_F, CPU -> fetched == 0);
    set_flag(CPU, NEGATIVE_F, (CPU -> fetched & NEGATIVE_F) != 0);

    return 1;
}

// LDX: Loads into Reg x the fetched value
// Requires fetch of address
// If fetched value is 0, set zero flag
// If fetched value is <0, set negative flag
uint8_t LDX(CPU_6502 *CPU){
    fetch(CPU);
    CPU -> Reg_X = CPU -> fetched;

    set_flag(CPU, ZERO_F, CPU -> fetched == 0);
    set_flag(CPU, NEGATIVE_F, (CPU -> fetched & NEGATIVE_F) != 0);

    return 1;
}

// LDY: Loads into Reg Y the fetched value
// Requires fetch of address
// If fetched value is 0, set zero flag
// If fetched value is < 0, set negative flag
uint8_t LDY(CPU_6502 *CPU){
    fetch(CPU);
    CPU -> Reg_Y = CPU -> fetched;

    set_flag(CPU, ZERO_F, CPU -> fetched == 0);
    set_flag(CPU, NEGATIVE_F, (CPU -> fetched & NEGATIVE_F) != 0);

    return 1;
}

// Loads Accumulator into memory (fetched data)
uint8_t STA(CPU_6502 *CPU){
    uint16_t address = CPU -> addr_abs;
    (*(CPU -> cpu_write))(CPU, address, CPU -> Reg_Acc);
    return 0;
}
// Loads Reg X into memory (fetched data)
uint8_t STX(CPU_6502 *CPU){
    uint16_t address = CPU -> addr_abs;
    (*(CPU -> cpu_write))(CPU, address, CPU -> Reg_X);
    return 0;
}
// Loads Reg Y into memory (fetched data)
uint8_t STY(CPU_6502 *CPU){
    uint16_t address = CPU -> addr_abs;
    (*(CPU -> cpu_write))(CPU, address, CPU -> Reg_Y);
    return 0;
}

// Transfer Accumulator to X
// Set zero and negative flags accordingly if accumulator value is negative or zero
uint8_t TAX(CPU_6502 *CPU){
    uint8_t value = CPU -> Reg_Acc;
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU -> Reg_X = value;
    return 0;
}

// Transfer Accumulator to Register Y
// Set zero and negative flags accordingly if accumulator value is negative or zero
uint8_t TAY(CPU_6502 *CPU){
    uint8_t value = CPU -> Reg_Acc;
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU -> Reg_Y = value;
    return 0;
}

// Tranfer Stack Pointer to Register X 
// Set zero and negative flags accordingly if accumulator value is negative or zero
uint8_t TSX(CPU_6502 *CPU){
    uint8_t value = CPU -> SP;
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU -> Reg_X = value;
    return 0;
}

// Transfer Register X to Accumulator 
// Set zero and negative flags accordingly if accumulator value is negative or zero
uint8_t TXA(CPU_6502 *CPU){
    uint8_t value = CPU -> Reg_X;
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU -> Reg_Acc = value;
    return 0;
}

// Transfer Register X to Stack Pointer
// NO FLAGS CAREFUL!!
uint8_t TXS(CPU_6502 *CPU){
    uint8_t value = CPU -> Reg_X;
    CPU -> SP = value;
    return 0;
}


// Transfer Register Y to Accumulator
// Set zero and negative flags accordingly if accumulator value is negative or zero
uint8_t TYA(CPU_6502 *CPU){
    uint8_t value = CPU -> Reg_Y;
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU -> Reg_Acc = value;
    return 0;
}

// NOTE: Stacks start from below and go up
// So if you push, the SP should go up.

// Push Accumulator to Stack
// Decrement Stack Pointer
uint8_t PHA(CPU_6502 *CPU){
    CPU -> cpu_write(CPU, CPU -> SP + STACK_START, CPU -> Reg_Acc);
    CPU -> SP--;
    return 0;
}

// Increments Stack Pointer
// Pull Accumulator from stack
uint8_t PLA(CPU_6502 *CPU){
    CPU -> SP++;
    uint8_t value = CPU -> cpu_read(CPU, CPU -> SP + STACK_START, false);
    CPU -> Reg_Acc = value;
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    return 0;
}

// Clear and Set Flag Codes

// Clear Carry Flag
uint8_t CLC(CPU_6502 *CPU){
    set_flag(CPU, CARRY_F, false);
    return 0;
}

// Set Carry Flag
uint8_t SEC(CPU_6502 *CPU){
    set_flag(CPU, CARRY_F, true);
    return 0;
}

// Clear Decimal Flag
uint8_t CLD(CPU_6502 *CPU){
    set_flag(CPU, DECIMAL_F, false);
    return 0;
}

// Set Decimal Flag
uint8_t SED(CPU_6502 *CPU){
    set_flag(CPU, DECIMAL_F, true);
    return 0;
}

// Clear Interrupt Disable Flag
uint8_t CLI(CPU_6502 *CPU){
    set_flag(CPU, INTERRUPT_D_F, false);
    return 0;
}

// Set Interrupt Disable Flag
uint8_t SEI(CPU_6502 *CPU){
    set_flag(CPU, INTERRUPT_D_F, true);
    return 0;
}

// Clear Overflow Flag
uint8_t CLV(CPU_6502 *CPU){
    set_flag(CPU, OVERFLOW_F, false);
    return 0;
}

// End of Set/Clear Flags 

// No Operation, Does Nothing, Takes up Time
// EX: Helps line up code for branching, or can be used for timing purposes
uint8_t NOP(CPU_6502 *CPU){
    return 0;
}

// Logic Operations
// These operations deal with logic operations on accumulator

// And operations with A & fetched value
uint8_t AND(CPU_6502 *CPU){
    Fetch(CPU);
    // A & Fetched
    uint8_t value = (CPU -> Reg_Acc) & (CPU -> fetched);
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU -> Reg_Acc = value;
    return 1;
}

// OR operations with A & fetched value
uint8_t ORA(CPU_6502 *CPU){
    Fetch(CPU);
    // A & Fetched
    uint8_t value = (CPU -> Reg_Acc) | (CPU -> fetched);
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU -> Reg_Acc = value;
    return 1;
}


// XOR operations with A & fetched value
uint8_t EOR(CPU_6502 *CPU){
    Fetch(CPU);
    // A & Fetched
    uint8_t value = (CPU -> Reg_Acc) ^ (CPU -> fetched);
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU -> Reg_Acc = value;
    return 1;
}

// This opcode ANDs A and Fetched
// Then sets Z_flag to the result
// N and V flag are set based on the Fetched 7th and 6th bit
// EX: fetched = 11000000 => N = 1 and V = 1;
// DOES NOT CHANGE REG_ACC!
uint8_t BIT(CPU_6502 *CPU){
    Fetch(CPU);
    // A & Fetched
    uint8_t value = (CPU -> Reg_Acc) & (CPU -> fetched);
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (CPU -> fetched & NEGATIVE_F) != 0);
    set_flag(CPU, OVERFLOW_F, (CPU -> fetched & OVERFLOW_F) != 0);
   
    return 0;
}




