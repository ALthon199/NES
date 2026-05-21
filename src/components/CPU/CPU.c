#include "CPU.h"
#include "../Bus/Bus.h"
#include <stdbool.h>
#include <stdlib.h>
#include <opcodes.h>

static uint8_t read(CPU_6502 *CPU, uint16_t address, bool read_only){

    if (0 <= address && address <= 0x1FFF){
        // 0x07FF for mirroring
        return CPU -> bus -> read(CPU->bus, address & 0x07FF, read_only);
    }

    return 0;
}

static void write(CPU_6502 *CPU, uint16_t address, uint8_t data){
    // 0x07FF for mirroring
    if (0 <= address && address <= 0x1FFF){
        CPU->bus->write(CPU->bus, address & 0x07FF, data);
    }
    
}

void Instructions_Init(CPU_6502 *CPU){
    Instruction *lookup_table = CPU -> Instructions;

    // Default every opcode to the invalid-opcode handler `XXX` so
    // the lookup table is fully populated and no entries are left
    // uninitialized.
    for (int i = 0; i < 256; ++i){
        lookup_table[i].name = "XXX";
        lookup_table[i].operate = &XXX;
        lookup_table[i].address_mode = &IMP_ADDR;
        lookup_table[i].cycles = 2;
    }


    // Row 0x00
    lookup_table[0x00] = (Instruction){"BRK", &BRK, &IMP_ADDR, 7};

    // Row 0x10
    

    // Row 0x20
    

    // Row 0x30
    

    // Row 0x40
    

    // Row 0x50
    

    // Row 0x60
    lookup_table[0x60] = (Instruction){"RTS", &RTS, &IMP_ADDR, 6};

    // Row 0x70
    

    // Row 0x80
    

    // Row 0x90
    

    // Row 0xA0
    lookup_table[0xA0] = (Instruction){"LDY", &LDY, &IMM_ADDR, 2};
    lookup_table[0xA2] = (Instruction){"LDX", &LDX, &IMM_ADDR, 2};
    lookup_table[0xA9] = (Instruction){"LDA", &LDA, &IMM_ADDR, 2};

    // Row 0xB0
    

    // Row 0xC0
    

    // Row 0xD0
    

    // Row 0xE0
    

    // Row 0xF0
    // NOP is often 0xEA; place it in its own spot
    lookup_table[0xEA] = (Instruction){"NOP", &NOP, &IMP_ADDR, 2};

}



bool get_flag(CPU_6502 *CPU, FLAGS_6502 F){
    return (CPU -> Reg_Status && F) != 0;
}

void set_flag(CPU_6502 *CPU, FLAGS_6502 F, bool on){
    if (on) CPU -> Reg_Status |= F;
    else CPU -> Reg_Status &= (~F);
}

void fetch(CPU_6502 *CPU){
    CPU -> fetched = (*CPU -> cpu_read)(CPU, CPU -> addr_abs, false);
}

void Reset(CPU_6502 *CPU){
    CPU -> Reg_Acc = 0;
    CPU -> Reg_X = 0;
    CPU -> Reg_Y = 0;
    CPU -> SP = 0xFD;
    set_flag(CPU, INTERRUPT_D_F, true);
    CPU -> addr_abs = 0xFFFC;
    CPU -> PC = ((*CPU -> cpu_read)(CPU, CPU -> addr_abs, false) << 8) | (*CPU -> cpu_read)(CPU, CPU -> addr_abs + 1, false);

    CPU -> addr_abs = 0x00;
    CPU -> addr_rel = 0x00;
    CPU -> fetched = 0x00;  

}

void IRQ(CPU_6502 *CPU){

    
    return;
}


void Clock(CPU_6502 *CPU){
    if (CPU -> cycles_left == 0){
        CPU -> opcode = (*(CPU -> cpu_read))(CPU, CPU -> PC, true);
        CPU -> PC++;
    }
}

void CPU_6502_Init(CPU_6502 *CPU, Bus *bus){
    memset(CPU, 0, sizeof(CPU_6502));
    CPU->bus = bus;
    CPU->cpu_read = &read;
    CPU->cpu_write = &write;
}

// CPU will "disconnect"
void CPU_6502_Destroy(CPU_6502 *CPU){
    if (CPU == NULL) return;
    free(CPU);
}