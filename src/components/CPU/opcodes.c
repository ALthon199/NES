#include "CPU.h"
#include "../Bus/Bus.h"
#include "opcodes.h"
#include <stdio.h>


// Opcodes are from https://www.nesdev.org/wiki/Instruction_reference#TXS


// Doesn't do anything strictly. The opcode likely does not require 
// the addr_abs or addr_rel
uint8_t IMP_ADDR(CPU_6502 *CPU){
    return 0;
}

// Loads a specific value directly (e.g. LDA #$05->Loads $05 into Accumulator)
// $05 is the next byte in memory 
uint8_t IMM_ADDR(CPU_6502 *CPU){
    CPU->addr_abs = CPU->PC;
    CPU->PC++;
    return 0;
}

// Use Reg_Acc as the value to be used for the instruction
uint8_t ACC_ADDR(CPU_6502 *CPU){
    return 0;
}


// Next two bytes provide memory address (Byte 1 is the lowest 8 bytes, Byte 2 is the highest 8 bytes)
uint8_t ABS_ADDR(CPU_6502 *CPU){
    uint16_t low = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;
    uint16_t high = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;

    CPU->addr_abs = (high << 8) | low;
    return 0;
}

// Next two bytes provide memory address then add Reg_X. Add cycle if needed
uint8_t ABSX_ADDR(CPU_6502 *CPU){
    uint16_t low = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;
    uint16_t high = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;

    uint16_t new_addr = (high << 8) | low;

    CPU->addr_abs = new_addr + CPU->Reg_X;

    if ((new_addr & 0xFF00) == (CPU->addr_abs & 0xFF00)){
        return 0;
    }

    return 1;
}

// Next two bytes provide memory address then add Reg_Y. Add cycle if needed
uint8_t ABSY_ADDR(CPU_6502 *CPU){
    uint16_t low = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;
    uint16_t high = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;

    uint16_t new_addr = (high << 8) | low;

    CPU->addr_abs = new_addr + CPU->Reg_Y;

    if ((new_addr & 0xFF00) == (CPU->addr_abs & 0xFF00)){
        return 0;
    }

    return 1;
}

// Loads Contents of a memory address from 0x00 to 0xFF
// Same as ABS_ADDR except only one byte is needed
uint8_t ZERO_ADDR(CPU_6502 *CPU){
    uint8_t byte = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU-> PC++;
    CPU->addr_abs = byte;
    return 0;
}

// E.g. LDA $10 X, Adds X to content of memory address which is wrapped around (0x00 to 0xFF) if nessecary
// Loads this memory address
uint8_t ZEROX_ADDR(CPU_6502 *CPU){
    uint8_t address = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;
    // Will handle wrapping around because its 8 bit int.
    address += CPU->Reg_X;
    CPU->addr_abs = address;
    return 0;
}


// E.g. LDA $10 Y, Adds Y to content of memory address which is wrapped around (0x00 to 0xFF) if nessecary
// Loads this memory address
uint8_t ZEROY_ADDR(CPU_6502 *CPU){
    uint8_t address = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;
    // Will handle wrapping around because its 8 bit int.
    address += CPU->Reg_Y;
    CPU->addr_abs = address;

    return 0;
}


// Indirect
uint8_t IND_ADDR(CPU_6502 *CPU){
    uint16_t low = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;
    uint16_t high = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;

    uint16_t pointer = (high << 8) | low;
    uint16_t low_addr = (*CPU->cpu_read)(CPU, pointer, false);
    // 6502 Bug: If the low byte of the pointer is 0xFF, the high byte will be fetched from the beginning of the page instead of the next page
    if ((pointer & 0x00FF) == 0x00FF){
        pointer = pointer & 0xFF00;
    }
    else{
        pointer++;
    }

    uint16_t high_addr = (*CPU->cpu_read)(CPU, pointer, false);

    CPU->addr_abs = (high_addr << 8) | low_addr;
    return 0;

}

// Indirect X
uint8_t INDX_ADDR(CPU_6502 *CPU){
    uint8_t byte = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;
    uint8_t pointer = byte + CPU->Reg_X;
    
    uint16_t low_addr = (*CPU->cpu_read)(CPU, (uint16_t)pointer, false);

    if (pointer == 0xFF){
        // Wraps to 0 
        // Will wrap to zero regardless, but clarity
        pointer = 0;
    }   
    else{
        pointer++;
    }
    uint16_t high_addr = (*CPU->cpu_read)(CPU, (uint16_t)pointer, false);

    CPU->addr_abs = (high_addr << 8) | low_addr;
    return 0;
}

// Indirect Y
uint8_t INDY_ADDR(CPU_6502 *CPU){
    uint8_t byte = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;
    uint8_t pointer = byte;
    
    uint16_t low_addr = (*CPU->cpu_read)(CPU, (uint16_t)pointer, false);

    if (pointer == 0xFF){
        // Wraps to 0 
        // Will wrap to zero regardless, but clarity
        pointer = 0;
    }   
    else{
        pointer++;
    }
    uint16_t high_addr = (*CPU->cpu_read)(CPU, (uint16_t)pointer, false);

    uint16_t new_addr = (high_addr << 8) | low_addr;
    CPU->addr_abs = new_addr + CPU->Reg_Y;

    if ((new_addr & 0xFF00) == (CPU->addr_abs & 0xFF00)){
        return 0;
    }

    return 1;
}



// Specififes Offset, implies a break is coming
uint8_t REL_ADDR(CPU_6502 *CPU){
    uint8_t offset = (*CPU->cpu_read)(CPU, CPU->PC, false);
    CPU->PC++;
    CPU->addr_rel = (int8_t)(offset);
    return 0;
}





// Opcodes - Instructions



// This opcode does not exist
// Purely for Debugging purposes, indicates an invalid opcode was read

uint8_t XXX(CPU_6502 *CPU){
    printf("Invalid Opcode: %02X\n", CPU->opcode);
    return 0;
}


///////////////////////////
///////ACCESS CODES////////
///////////////////////////


// LDA: Loads into accumulator the fetched value
// Requires fetch of address
// If fetched value is 0, set zero flag
// If fetched value is <0, set negative flag
uint8_t LDA(CPU_6502 *CPU){
    fetch(CPU);
    CPU->Reg_Acc = CPU->fetched;

    set_flag(CPU, ZERO_F, CPU->fetched == 0);
    set_flag(CPU, NEGATIVE_F, (CPU->fetched & NEGATIVE_F) != 0);

    return 1;
}

// LDX: Loads into Reg x the fetched value
// Requires fetch of address
// If fetched value is 0, set zero flag
// If fetched value is <0, set negative flag
uint8_t LDX(CPU_6502 *CPU){
    fetch(CPU);
    CPU->Reg_X = CPU->fetched;

    set_flag(CPU, ZERO_F, CPU->fetched == 0);
    set_flag(CPU, NEGATIVE_F, (CPU->fetched & NEGATIVE_F) != 0);

    return 1;
}

// LDY: Loads into Reg Y the fetched value
// Requires fetch of address
// If fetched value is 0, set zero flag
// If fetched value is < 0, set negative flag
uint8_t LDY(CPU_6502 *CPU){
    fetch(CPU);
    CPU->Reg_Y = CPU->fetched;

    set_flag(CPU, ZERO_F, CPU->fetched == 0);
    set_flag(CPU, NEGATIVE_F, (CPU->fetched & NEGATIVE_F) != 0);

    return 1;
}

// Loads Accumulator into memory (fetched data)
uint8_t STA(CPU_6502 *CPU){
    uint16_t address = CPU->addr_abs;
    (*(CPU->cpu_write))(CPU, address, CPU->Reg_Acc);
    return 0;
}

// Loads Reg X into memory (fetched data)
uint8_t STX(CPU_6502 *CPU){
    uint16_t address = CPU->addr_abs;
    (*(CPU->cpu_write))(CPU, address, CPU->Reg_X);
    return 0;
}

// Loads Reg Y into memory (fetched data)
uint8_t STY(CPU_6502 *CPU){
    uint16_t address = CPU->addr_abs;
    (*(CPU->cpu_write))(CPU, address, CPU->Reg_Y);
    return 0;
}

///////////////////////////
/////TRANSFER CODES////////
///////////////////////////


// Transfer Accumulator to X
// Set zero and negative flags accordingly if accumulator value is negative or zero
uint8_t TAX(CPU_6502 *CPU){
    uint8_t value = CPU->Reg_Acc;
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU->Reg_X = value;
    return 0;
}

// Transfer Accumulator to Register Y
// Set zero and negative flags accordingly if accumulator value is negative or zero
uint8_t TAY(CPU_6502 *CPU){
    uint8_t value = CPU->Reg_Acc;
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU->Reg_Y = value;
    return 0;
}


// Transfer Register X to Accumulator 
// Set zero and negative flags accordingly if accumulator value is negative or zero
uint8_t TXA(CPU_6502 *CPU){
    uint8_t value = CPU->Reg_X;
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU->Reg_Acc = value;
    return 0;
}


// Transfer Register Y to Accumulator
// Set zero and negative flags accordingly if accumulator value is negative or zero
uint8_t TYA(CPU_6502 *CPU){
    uint8_t value = CPU->Reg_Y;
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU->Reg_Acc = value;
    return 0;
}




///////////////////////////
///////STACK CODES/////////
///////////////////////////

// NOTE: Stacks start grows down
// So if you push, the SP should go down.

// Push Accumulator to Stack
// Decrement Stack Pointer
uint8_t PHA(CPU_6502 *CPU){
    CPU->cpu_write(CPU, CPU->SP + STACK_START, CPU->Reg_Acc);
    CPU->SP--;
    return 0;
}

// Increments Stack Pointer
// Pulls Accumulator from stack
uint8_t PLA(CPU_6502 *CPU){
    CPU->SP++;
    uint8_t value = CPU->cpu_read(CPU, CPU->SP + STACK_START, false);
    CPU->Reg_Acc = value;
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    return 0;
}

// Pushes Flags to the stack
// Decrements stack Pointer
uint8_t PHP(CPU_6502* CPU){
    CPU->cpu_write(CPU, CPU->SP + STACK_START, CPU->Reg_Status);
    CPU->SP--;
    return 0;
}

// Increments Stack Pointer
// Pull Status Flags froms stack
uint8_t PLP(CPU_6502* CPU){
    CPU->SP++;
    CPU->Reg_Status = CPU->cpu_read(CPU, CPU->SP + STACK_START, false);
    return 0;
}

// Transfer Register X to Stack Pointer
// NO FLAGS CAREFUL!!
uint8_t TXS(CPU_6502 *CPU){
    uint8_t value = CPU->Reg_X;
    CPU->SP = value;
    return 0;
}

// Tranfer Stack Pointer to Register X 
// Set zero and negative flags accordingly if accumulator value is negative or zero
uint8_t TSX(CPU_6502 *CPU){
    uint8_t value = CPU->SP;
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU->Reg_X = value;
    return 0;
}




///////////////////////////
///////FLAG CODES/////////
///////////////////////////

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

///////////////////////////
///////BITWISE CODES/////////
///////////////////////////

// And operations with A & fetched value
uint8_t AND(CPU_6502 *CPU){
    fetch(CPU);
    // A & Fetched
    uint8_t value = (CPU->Reg_Acc) & (CPU->fetched);
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU->Reg_Acc = value;
    return 1;
}

// OR operations with A & fetched value
uint8_t ORA(CPU_6502 *CPU){
    fetch(CPU);
    // A & Fetched
    uint8_t value = (CPU->Reg_Acc) | (CPU->fetched);
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU->Reg_Acc = value;
    return 1;
}


// XOR operations with A & fetched value
uint8_t EOR(CPU_6502 *CPU){
    fetch(CPU);
    // A & Fetched
    uint8_t value = (CPU->Reg_Acc) ^ (CPU->fetched);
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    CPU->Reg_Acc = value;
    return 1;
}

// This opcode ANDs A and Fetched
// Then sets Z_flag to the result
// N and V flag are set based on the Fetched 7th and 6th bit
// EX: fetched = 11000000 => N = 1 and V = 1;
// DOES NOT CHANGE REG_ACC!
uint8_t BIT(CPU_6502 *CPU){
    fetch(CPU);
    // A & Fetched
    uint8_t value = (CPU->Reg_Acc) & (CPU->fetched);
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (CPU->fetched & NEGATIVE_F) != 0);
    set_flag(CPU, OVERFLOW_F, (CPU->fetched & OVERFLOW_F) != 0);
   
    return 0;
}

///////////////////////////
///////ARITHMETIC CODES/////////
///////////////////////////

// Increment value of Register X
// Set zero and negative flags accordingly if new value is negative or zero
uint8_t INX(CPU_6502 *CPU){
    CPU->Reg_X++;
    set_flag(CPU, ZERO_F, CPU->Reg_X == 0);
    set_flag(CPU, NEGATIVE_F, (CPU->Reg_X & NEGATIVE_F) != 0);
    return 0;
}

// Increment value of Register Y
// Set zero and negative flags accordingly if new value is negative or zero
uint8_t INY(CPU_6502 *CPU){
    CPU->Reg_Y++;
    set_flag(CPU, ZERO_F, CPU->Reg_Y == 0);
    set_flag(CPU, NEGATIVE_F, (CPU->Reg_Y & NEGATIVE_F) != 0);
    return 0;
}  

// Decrement value of Register X
// Set zero and negative flags accordingly if new value is negative or zero
uint8_t DEX(CPU_6502 *CPU){
    CPU->Reg_X--;
    set_flag(CPU, ZERO_F, CPU->Reg_X == 0);
    set_flag(CPU, NEGATIVE_F, (CPU->Reg_X & NEGATIVE_F) != 0);
    return 0;
}

// Decrement value of Register Y
// Set zero and negative flags accordingly if new value is negative or zero
uint8_t DEY(CPU_6502 *CPU){
    CPU->Reg_Y--;
    set_flag(CPU, ZERO_F, CPU->Reg_Y == 0);
    set_flag(CPU, NEGATIVE_F, (CPU->Reg_Y & NEGATIVE_F) != 0);
    return 0;
}

// ADD with Carry
// A = A + memory + C
// Set C, Z, V, N flags accordingly 
// Typically after a CLC for first byte
uint8_t ADC(CPU_6502 *CPU){
    fetch(CPU);
    uint8_t A = CPU->Reg_Acc;
    uint8_t memory = CPU->fetched;
    uint16_t real_value = (uint16_t)A + (uint16_t)(memory) + (uint16_t)get_flag(CPU, CARRY_F);
    CPU->Reg_Acc = (uint8_t)real_value;
    uint8_t result = CPU->Reg_Acc;
    
    // Checks for overflow (this trick checks the signs of the new value, old value, and fetched value)
    set_flag(CPU, OVERFLOW_F, ((result ^ A) & (result ^ memory) & 0x80) != 0);

    // Check for Zero
    set_flag(CPU, ZERO_F, result == 0);

    // Check for Negative
    set_flag(CPU, NEGATIVE_F, (result & NEGATIVE_F) != 0);

    // Check for overflow
    set_flag(CPU, CARRY_F, real_value > 0xFF);
    
    return 1;
}

// SUBTRACT with CARRY
// A = A - memory - ~C or A = A + ~memory + C
uint8_t SBC(CPU_6502 *CPU){
    fetch(CPU);
    uint8_t A = CPU->Reg_Acc; 
    uint8_t memory = CPU->fetched;
    uint16_t real_value = (uint16_t)A + (uint16_t)(~memory) + (uint16_t)get_flag(CPU, CARRY_F);
    CPU->Reg_Acc = (uint8_t)real_value;
    uint8_t result = CPU->Reg_Acc;

    // Checks for overflow (this trick checks the signs of the new value, old value, and fetched value)
    set_flag(CPU, OVERFLOW_F, ((result ^ A) & (result ^ ~memory) & 0x80) != 0);

    // Check for Zero
    set_flag(CPU, ZERO_F, result == 0);

    // Check for Negative
    set_flag(CPU, NEGATIVE_F, (result & NEGATIVE_F) != 0);

    // Check for underflow
    // E.g. (if we have 2 - 1 => 0000 0010 + 1111 1110 => 1 0000 0000, no underflow, 9th bit is triggered)
    
    set_flag(CPU, CARRY_F, (real_value & 0x100) != 0);
    
    return 1;
}

// Memory Increments and Decrement

// Fetch new value, increment, and rewrite that value into memory
// Adjust flags
uint8_t INC(CPU_6502 *CPU){
    fetch(CPU);
    CPU->fetched++;
    CPU->cpu_write(CPU, CPU->addr_abs, CPU->fetched);
    set_flag(CPU, ZERO_F, CPU->fetched == 0);
    set_flag(CPU, NEGATIVE_F, (CPU->fetched & NEGATIVE_F) != 0);
    return 0;
}



// Fetch new value, decrement, and rewrite that value into memory
// Adjust flags
uint8_t DEC(CPU_6502 *CPU){
    fetch(CPU);
    CPU->fetched--;
    CPU->cpu_write(CPU, CPU->addr_abs, CPU->fetched);
    set_flag(CPU, ZERO_F, CPU->fetched == 0);
    set_flag(CPU, NEGATIVE_F, (CPU->fetched & NEGATIVE_F) != 0);
    return 0;
}

///////////////////////
////////JUMPS CODES///////
//////////////////////////

uint8_t JMP(CPU_6502 *CPU){
    CPU->PC = CPU->addr_abs;
    return 0;
}


// First push the highbyte and lowbyte of PC (after addressing mode) onto stack
// Then adjust PC to fetched value;
uint8_t JSR(CPU_6502 *CPU){
    fetch(CPU);
    uint8_t lowbyte = (CPU->PC - 1) & (0xFF);
    uint8_t highbyte =  ((CPU->PC - 1) >> 8) & (0XFF);
    CPU->cpu_write(CPU, CPU->SP + STACK_START, highbyte);
    CPU->SP--;
    CPU->cpu_write(CPU, CPU->SP + STACK_START, lowbyte);
    CPU->SP--;
    CPU->PC = CPU->fetched;
    return 0;
}

// Return from Subrountine
// Pull low byte then high byte for PC 
// Then increments it, matches JSR
uint8_t RTS(CPU_6502 *CPU){
    fetch(CPU);
    CPU->SP++;
    uint8_t lowbyte = CPU->cpu_read(CPU, CPU->SP + STACK_START, false);
    CPU->SP++;
    uint8_t highbyte = CPU->cpu_read(CPU, CPU->SP + STACK_START, false);
    CPU->PC = (((uint16_t)highbyte) << 8) | lowbyte;
    CPU->PC++;
    return 0;
}

// Break (IRQ)
// First push the highbyte and lowbyte of PC (after addressing mode) onto stack
// Interrupt Disable Flag Set
// Push Flag with Break set
// Sets PC to 0xFFFE -0xFFFF for IRQ handler
uint8_t BRK(CPU_6502 *CPU){
    uint8_t lowbyte = (CPU->PC - 1) & (0xFF);
    uint8_t highbyte =  ((CPU->PC - 1) >> 8) & (0XFF);
    CPU->cpu_write(CPU, CPU->SP + STACK_START, highbyte);
    CPU->SP--;
    CPU->cpu_write(CPU, CPU->SP + STACK_START, lowbyte);
    CPU->SP--;
    

    set_flag(CPU, INTERRUPT_D_F, true);
    set_flag(CPU, BREAK_F, true);

    CPU->cpu_write(CPU, CPU->SP + STACK_START, CPU->Reg_Status);
    CPU->SP--;

    set_flag(CPU, BREAK_F, false);

    uint16_t lo = (*CPU->cpu_read)(CPU, 0xFFFE, false);
    uint16_t hi = ((*CPU->cpu_read)(CPU, 0xFFFF, false) << 8);


    CPU->PC = (hi | lo);
    
    return 0;
}


////////////////////////////
//////Shift Codes///////////
////////////////////////////

// Left shift Accumulator or Memory Value by 1
// Sets Flags accordingly.
uint8_t ASL(CPU_6502 *CPU){
    
    
    if (CPU -> Instructions[CPU -> opcode].address_mode == &ACC_ADDR){
        uint8_t original = CPU -> Reg_Acc;
        set_flag(CPU, ZERO_F, original == 0);
        set_flag(CPU, CARRY_F, (original & 0x80) != 0);
        original <<= 1;
        set_flag(CPU, NEGATIVE_F, (original & 0x80) != 0);
        CPU -> Reg_Acc = original;
        return 0;
    }


    fetch(CPU);
    uint8_t original = CPU -> fetched;
    set_flag(CPU, ZERO_F, original == 0);
    set_flag(CPU, CARRY_F, (original & 0x80) != 0);
    original <<= 1;
    set_flag(CPU, NEGATIVE_F, (original & 0x80) != 0);
    (*CPU -> cpu_write)(CPU, CPU -> addr_abs, original);
    return 0;
}

// Right shift Accumulator or Memory Value by 1
// Sets Flags accordingly.
uint8_t LSR(CPU_6502 *CPU){
    if (CPU -> Instructions[CPU -> opcode].address_mode == &ACC_ADDR){
        uint8_t original = CPU -> Reg_Acc;
        set_flag(CPU, CARRY_F, (original & 0x1) != 0);
        set_flag(CPU, NEGATIVE_F, false);
        original >>= 1;
        set_flag(CPU, ZERO_F, original == 0);
        CPU -> Reg_Acc = original;
        return 0;
    }


    fetch(CPU);
    uint8_t original = CPU -> fetched;
    set_flag(CPU, CARRY_F, (original & 0x1) != 0);
    set_flag(CPU, NEGATIVE_F, false);
    original >>= 1;
    set_flag(CPU, ZERO_F, original == 0);
    (*(CPU -> cpu_write))(CPU, CPU -> addr_abs, original);
    return 0;
}

// Rotate Left Accumulator or Memory Value by 1, uses Carry Flag as bit 0, and bit 7 as new Carry Flag
// Sets Flags accordingly.
uint8_t ROL(CPU_6502 *CPU){
    if (CPU -> Instructions[CPU -> opcode].address_mode == &ACC_ADDR){
        uint8_t original = CPU -> Reg_Acc;
        uint8_t carry = get_flag(CPU, CARRY_F);
        set_flag(CPU, CARRY_F, (original & 0x80) != 0);
        original <<= 1;
        original |= carry;
        set_flag(CPU, ZERO_F, original == 0);
        set_flag(CPU, NEGATIVE_F, (original & 0x80) != 0);
        CPU -> Reg_Acc = original;
        return 0;
    }
    uint8_t original = CPU -> fetched;
    uint8_t carry = get_flag(CPU, CARRY_F);
    set_flag(CPU, CARRY_F, (original & 0x80) != 0);
    original <<= 1;
    original |= carry;
    set_flag(CPU, ZERO_F, original == 0);
    set_flag(CPU, NEGATIVE_F, (original & 0x80) != 0);
    (*CPU -> cpu_write)(CPU, CPU -> addr_abs, original);
    return 0;
}

// Rotate Right Accumulator or Memory Value by 1, uses Carry Flag as bit 7, and bit 0 as new Carry Flag
// Sets Flags accordingly.
uint8_t ROR(CPU_6502 *CPU){
    if (CPU -> Instructions[CPU -> opcode].address_mode == &ACC_ADDR){
        uint8_t original = CPU -> Reg_Acc;
        uint8_t carry = get_flag(CPU, CARRY_F);
        set_flag(CPU, CARRY_F, (original & 0x1) != 0);
        original >>= 1;
        original |= (carry << 7);
        set_flag(CPU, ZERO_F, original == 0);
        set_flag(CPU, NEGATIVE_F, (original & 0x80) != 0);
        CPU -> Reg_Acc = original;
        return 0;
    }
    uint8_t original = CPU -> fetched;
    uint8_t carry = get_flag(CPU, CARRY_F);
    set_flag(CPU, CARRY_F, (original & 0x1) != 0);
    original >>= 1;
    original |= (carry << 7);
    set_flag(CPU, ZERO_F, original == 0);
    set_flag(CPU, NEGATIVE_F, (original & 0x80) != 0);
    (*CPU -> cpu_write)(CPU, CPU -> addr_abs, original);
    return 0;

    return 0;
}



//////////////////////////
//////COMPARE CODES///////
//////////////////////////



// Compare A
// Compares A to a Memory Value via subtraction
uint8_t CMP(CPU_6502 *CPU){
    fetch(CPU);
    uint8_t value = CPU->Reg_Acc - CPU->fetched;
    set_flag(CPU, CARRY_F, value >= 0);
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    return 1;
}


// Compare X
// Compares X to a Memory Value via subtraction
uint8_t CPX(CPU_6502 *CPU){
    fetch(CPU);
    uint8_t value = CPU->Reg_X - CPU->fetched;
    set_flag(CPU, CARRY_F, value >= 0);
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    return 1;
}

// Compare Y
// Compares Y to a Memory Value via subtraction
uint8_t CPY(CPU_6502 *CPU){ 
    fetch(CPU);
    uint8_t value = CPU->Reg_Y - CPU->fetched;
    set_flag(CPU, CARRY_F, value >= 0);
    set_flag(CPU, ZERO_F, value == 0);
    set_flag(CPU, NEGATIVE_F, (value & NEGATIVE_F) != 0);
    return 1;
}



//////////////////////////
//////BRANCH CODES///////
//////////////////////////

// Branch if Carry Clear
// If Carry Flag is clear, add the relative address to the program counter to cause a branch
uint8_t BCC(CPU_6502 *CPU){
    bool flag = get_flag(CPU, CARRY_F);
    if (flag == 0){
        CPU->cycles_left++;
        uint16_t old_pc = CPU->PC;
        uint16_t new_pc = CPU->PC + CPU ->addr_rel;
        if ((old_pc & 0xFF00) != (new_pc & 0xFF00)) CPU->cycles_left++;

        CPU->PC = new_pc;
        return 1;
    }
    
    return 0;
}


// Branch if Carry Set
// If Carry Flag is Set, add the relative address to the program counter to cause a branch
uint8_t BCS(CPU_6502 *CPU){
    bool flag = get_flag(CPU, CARRY_F);
    if (flag == 1){
        CPU->cycles_left++;
        uint16_t old_pc = CPU->PC;
        uint16_t new_pc = CPU->PC + CPU ->addr_rel;
        if ((old_pc & 0xFF00) != (new_pc & 0xFF00)) CPU->cycles_left++;

        CPU->PC = new_pc;
        return 1;
    }
    
    return 0;
}



// Branch if Equal
// If Zero Flag is Set, add the relative address to the program counter to cause a branch
uint8_t BEQ(CPU_6502 *CPU){
    bool flag = get_flag(CPU, ZERO_F);
    if (flag == 1){
        CPU->cycles_left++;
        uint16_t old_pc = CPU->PC;
        uint16_t new_pc = CPU->PC + CPU ->addr_rel;
        if ((old_pc & 0xFF00) != (new_pc & 0xFF00)) CPU->cycles_left++;

        CPU->PC = new_pc;
        return 1;
    }
    
    return 0;
}


// Branch if Not Equal
// If Zero Flag is Clear, add the relative address to the program counter to cause a branch
uint8_t BNE(CPU_6502 *CPU){
    bool flag = get_flag(CPU, ZERO_F);
    if (flag == 0){
        CPU->cycles_left++;
        uint16_t old_pc = CPU->PC;
        uint16_t new_pc = CPU->PC + CPU ->addr_rel;
        if ((old_pc & 0xFF00) != (new_pc & 0xFF00)) CPU->cycles_left++;

        CPU->PC = new_pc;
        return 1;
    }
    
    return 0;
}


// Branch if Plus
// If Negative Flag is Clear, add the relative address to the program counter to cause a branch
uint8_t BPL(CPU_6502 *CPU){
    bool flag = get_flag(CPU, NEGATIVE_F);
    if (flag == 0){
        CPU->cycles_left++;
        uint16_t old_pc = CPU->PC;
        uint16_t new_pc = CPU->PC + CPU ->addr_rel;
        if ((old_pc & 0xFF00) != (new_pc & 0xFF00)) CPU->cycles_left++;

        CPU->PC = new_pc;
        return 1;
    }
    
    return 0;
}

// Branch if Minus
// If Negative Flag is Set, add the relative address to the program counter to cause a branch
uint8_t BMI(CPU_6502 *CPU){
    bool flag = get_flag(CPU, NEGATIVE_F);
    if (flag == 1){
        CPU->cycles_left++;
        uint16_t old_pc = CPU->PC;
        uint16_t new_pc = CPU->PC + CPU ->addr_rel;
        if ((old_pc & 0xFF00) != (new_pc & 0xFF00)) CPU->cycles_left++;

        CPU->PC = new_pc;
        return 1;
    }
    
    return 0;
}



// Branch if Overflow Clear
// If Overflow Flag is Clear, add the relative address to the program counter to cause a branch
uint8_t BVC(CPU_6502 *CPU){
    bool flag = get_flag(CPU, OVERFLOW_F);
    if (flag == 0){
        CPU->cycles_left++;
        uint16_t old_pc = CPU->PC;
        uint16_t new_pc = CPU->PC + CPU ->addr_rel;
        if ((old_pc & 0xFF00) != (new_pc & 0xFF00)) CPU->cycles_left++;

        CPU->PC = new_pc;
        return 1;
    }
    
    return 0;
}


// Branch if Overflow Set
// If Overflow Flag is Set, add the relative address to the program counter to cause a branch
uint8_t BVS(CPU_6502 *CPU){
    bool flag = get_flag(CPU, OVERFLOW_F);
    if (flag == 1){
        CPU->cycles_left++;
        uint16_t old_pc = CPU->PC;
        uint16_t new_pc = CPU->PC + CPU ->addr_rel;
        if ((old_pc & 0xFF00) != (new_pc & 0xFF00)) CPU->cycles_left++;

        CPU->PC = new_pc;
        return 1;
    }
    
    return 0;
}


