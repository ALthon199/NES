// Emulates MOS 6502 CPU
#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
// Forward Declaration of BUS to avoid circular dependency
typedef struct Bus Bus;

// Forward Declaration of CPU_6502 to avoid circular dependency
typedef struct CPU_6502 CPU_6502;

typedef struct{
    char name[4];
    uint8_t (*operate)(CPU_6502*);
    uint8_t (*address_mode)(CPU_6502*);
    
    uint8_t cycles;

} Instruction;

typedef struct CPU_6502{
    
    // Registers
    uint8_t Reg_Acc;
    uint8_t Reg_X;
    uint8_t Reg_Y; 

    // Pointer
    uint16_t PC;
    uint8_t  SP;

    // Status Register
    // Bit 0: Carry Flag
    // Bit 1: Zero Flag
    // Bit 2: Interrupt Disable
    // Bit 3: Decimal Mode
    // Bit 4: Break Command
    // Bit 5: Unused
    // Bit 6: Overflow Flag
    // Bit 7: Negative Flag
    uint8_t Reg_Status;


    // Bus
    Bus* bus;
    uint8_t (*cpu_read)(struct CPU_6502* CPU, uint16_t address, bool read_only);
    void (*cpu_write)(struct CPU_6502* CPU, uint16_t address, uint8_t data);

    // Internal
    uint64_t total_cycles;
    uint8_t cycles_left;
    Instruction Instructions[256];
    
    uint8_t fetched;
    uint16_t addr_abs;
    uint16_t addr_rel;
    uint8_t opcode;
    

} CPU_6502;


void Init_Instructions(CPU_6502* cpu);

void Clock(CPU_6502* cpu);

void Reset();

void IRQ();

void NMI();

void Fetch(CPU_6502* cpu);

void CPU_6502_Init(CPU_6502* cpu, Bus* bus);

void CPU_6502_Destroy(CPU_6502* cpu);
#endif