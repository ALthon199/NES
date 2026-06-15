#include "CPU.h"
#include "../Bus/Bus.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <opcodes.h>
#include <stdio.h>

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
    lookup_table[0x01] = (Instruction){"ORA", &ORA, &INDX_ADDR, 6};
    lookup_table[0x05] = (Instruction){"ORA", &ORA, &ZERO_ADDR, 3};
    lookup_table[0x06] = (Instruction){"ASL", &ASL, &ZERO_ADDR, 5};
    lookup_table[0x08] = (Instruction){"PHP", &PHP, &IMP_ADDR, 3};
    lookup_table[0x09] = (Instruction){"ORA", &ORA, &IMM_ADDR, 2};
    lookup_table[0x0A] = (Instruction){"ASL", &ASL, &ACC_ADDR, 2};
    lookup_table[0x0D] = (Instruction){"ORA", &ORA, &ABS_ADDR, 4};
    lookup_table[0x0E] = (Instruction){"ASL", &ASL, &ABS_ADDR, 6};

    // Row 0x10
    lookup_table[0x10] = (Instruction){"BPL", &BPL, &REL_ADDR, 2};
    lookup_table[0x11] = (Instruction){"ORA", &ORA, &INDY_ADDR, 5};
    lookup_table[0x15] = (Instruction){"ORA", &ORA, &ZEROX_ADDR, 4};
    lookup_table[0x16] = (Instruction){"ASL", &ASL, &ZEROX_ADDR, 6};
    lookup_table[0x18] = (Instruction){"CLC", &CLC, &IMP_ADDR, 2};
    lookup_table[0x19] = (Instruction){"ORA", &ORA, &ABSY_ADDR, 4};
    lookup_table[0x1D] = (Instruction){"ORA", &ORA, &ABSX_ADDR, 4};
    lookup_table[0x1E] = (Instruction){"ASL", &ASL, &ABSX_ADDR, 7};

    // Row 0x20
    lookup_table[0x20] = (Instruction){"JSR", &JSR, &ABS_ADDR, 6};
    lookup_table[0x21] = (Instruction){"AND", &AND, &INDX_ADDR, 6};
    lookup_table[0x24] = (Instruction){"BIT", &BIT, &ZERO_ADDR, 3};
    lookup_table[0x25] = (Instruction){"AND", &AND, &ZERO_ADDR, 3};
    lookup_table[0x26] = (Instruction){"ROL", &ROL, &ZERO_ADDR, 5};
    lookup_table[0x28] = (Instruction){"PLP", &PLP, &IMP_ADDR, 4};
    lookup_table[0x29] = (Instruction){"AND", &AND, &IMM_ADDR, 2};
    lookup_table[0x2A] = (Instruction){"ROL", &ROL, &ACC_ADDR, 2};
    lookup_table[0x2C] = (Instruction){"BIT", &BIT, &ABS_ADDR, 4};
    lookup_table[0x2D] = (Instruction){"AND", &AND, &ABS_ADDR, 4};
    lookup_table[0x2E] = (Instruction){"ROL", &ROL, &ABS_ADDR, 6};

    // Row 0x30
    lookup_table[0x30] = (Instruction){"BMI", &BMI, &REL_ADDR, 2};
    lookup_table[0x31] = (Instruction){"AND", &AND, &INDY_ADDR, 5};
    lookup_table[0x35] = (Instruction){"AND", &AND, &ZEROX_ADDR, 4};
    lookup_table[0x36] = (Instruction){"ROL", &ROL, &ZEROX_ADDR, 6};
    lookup_table[0x38] = (Instruction){"SEC", &SEC, &IMP_ADDR, 2};
    lookup_table[0x39] = (Instruction){"AND", &AND, &ABSY_ADDR, 4};
    lookup_table[0x3D] = (Instruction){"AND", &AND, &ABSX_ADDR, 4};
    lookup_table[0x3E] = (Instruction){"ROL", &ROL, &ABSX_ADDR, 7};

    // Row 0x40
    lookup_table[0x41] = (Instruction){"EOR", &EOR, &INDX_ADDR, 6};
    lookup_table[0x45] = (Instruction){"EOR", &EOR, &ZERO_ADDR, 3};
    lookup_table[0x46] = (Instruction){"LSR", &LSR, &ZERO_ADDR, 5};
    lookup_table[0x48] = (Instruction){"PHA", &PHA, &IMP_ADDR, 3};
    lookup_table[0x49] = (Instruction){"EOR", &EOR, &IMM_ADDR, 2};
    lookup_table[0x4A] = (Instruction){"LSR", &LSR, &ACC_ADDR, 2};
    lookup_table[0x4C] = (Instruction){"JMP", &JMP, &ABS_ADDR, 3};
    lookup_table[0x4D] = (Instruction){"EOR", &EOR, &ABS_ADDR, 4};
    lookup_table[0x4E] = (Instruction){"LSR", &LSR, &ABS_ADDR, 6};

    // Row 0x50
    lookup_table[0x50] = (Instruction){"BVC", &BVC, &REL_ADDR, 2};
    lookup_table[0x51] = (Instruction){"EOR", &EOR, &INDY_ADDR, 5};
    lookup_table[0x55] = (Instruction){"EOR", &EOR, &ZEROX_ADDR, 4};
    lookup_table[0x56] = (Instruction){"LSR", &LSR, &ZEROX_ADDR, 6};
    lookup_table[0x58] = (Instruction){"CLI", &CLI, &IMP_ADDR, 2};
    lookup_table[0x59] = (Instruction){"EOR", &EOR, &ABSY_ADDR, 4};
    lookup_table[0x5D] = (Instruction){"EOR", &EOR, &ABSX_ADDR, 4};
    lookup_table[0x5E] = (Instruction){"LSR", &LSR, &ABSX_ADDR, 7};

    // Row 0x60
    lookup_table[0x60] = (Instruction){"RTS", &RTS, &IMP_ADDR, 6};
    lookup_table[0x61] = (Instruction){"ADC", &ADC, &INDX_ADDR, 6};
    lookup_table[0x65] = (Instruction){"ADC", &ADC, &ZERO_ADDR, 3};
    lookup_table[0x66] = (Instruction){"ROR", &ROR, &ZERO_ADDR, 5};
    lookup_table[0x68] = (Instruction){"PLA", &PLA, &IMP_ADDR, 4};
    lookup_table[0x69] = (Instruction){"ADC", &ADC, &IMM_ADDR, 2};
    lookup_table[0x6A] = (Instruction){"ROR", &ROR, &ACC_ADDR, 2};
    lookup_table[0x6C] = (Instruction){"JMP", &JMP, &IND_ADDR, 5};
    lookup_table[0x6D] = (Instruction){"ADC", &ADC, &ABS_ADDR, 4};
    lookup_table[0x6E] = (Instruction){"ROR", &ROR, &ABS_ADDR, 6};

    // Row 0x70
    lookup_table[0x70] = (Instruction){"BVS", &BVS, &REL_ADDR, 2};
    lookup_table[0x71] = (Instruction){"ADC", &ADC, &INDY_ADDR, 5};
    lookup_table[0x75] = (Instruction){"ADC", &ADC, &ZEROX_ADDR, 4};
    lookup_table[0x76] = (Instruction){"ROR", &ROR, &ZEROX_ADDR, 6};
    lookup_table[0x78] = (Instruction){"SEI", &SEI, &IMP_ADDR, 2};
    lookup_table[0x79] = (Instruction){"ADC", &ADC, &ABSY_ADDR, 4};
    lookup_table[0x7D] = (Instruction){"ADC", &ADC, &ABSX_ADDR, 4};
    lookup_table[0x7E] = (Instruction){"ROR", &ROR, &ABSX_ADDR, 7};

    // Row 0x80
    lookup_table[0x81] = (Instruction){"STA", &STA, &INDX_ADDR, 6};
    lookup_table[0x84] = (Instruction){"STY", &STY, &ZERO_ADDR, 3};
    lookup_table[0x85] = (Instruction){"STA", &STA, &ZERO_ADDR, 3};
    lookup_table[0x86] = (Instruction){"STX", &STX, &ZERO_ADDR, 3};
    lookup_table[0x88] = (Instruction){"DEY", &DEY, &IMP_ADDR, 2};
    lookup_table[0x8A] = (Instruction){"TXA", &TXA, &IMP_ADDR, 2};
    lookup_table[0x8C] = (Instruction){"STY", &STY, &ABS_ADDR, 4};
    lookup_table[0x8D] = (Instruction){"STA", &STA, &ABS_ADDR, 4};
    lookup_table[0x8E] = (Instruction){"STX", &STX, &ABS_ADDR, 4};

    // Row 0x90
    lookup_table[0x90] = (Instruction){"BCC", &BCC, &REL_ADDR, 2};
    lookup_table[0x91] = (Instruction){"STA", &STA, &INDY_ADDR, 6};
    lookup_table[0x94] = (Instruction){"STY", &STY, &ZEROX_ADDR, 4};
    lookup_table[0x95] = (Instruction){"STA", &STA, &ZEROX_ADDR, 4};
    lookup_table[0x96] = (Instruction){"STX", &STX, &ZEROY_ADDR, 4};
    lookup_table[0x98] = (Instruction){"TYA", &TYA, &IMP_ADDR, 2};
    lookup_table[0x99] = (Instruction){"STA", &STA, &ABSY_ADDR, 5};
    lookup_table[0x9A] = (Instruction){"TXS", &TXS, &IMP_ADDR, 2};
    lookup_table[0x9D] = (Instruction){"STA", &STA, &ABSX_ADDR, 5};

    // Row 0xA0
    lookup_table[0xA0] = (Instruction){"LDY", &LDY, &IMM_ADDR, 2};
    lookup_table[0xA1] = (Instruction){"LDA", &LDA, &INDX_ADDR, 6};
    lookup_table[0xA2] = (Instruction){"LDX", &LDX, &IMM_ADDR, 2};
    lookup_table[0xA4] = (Instruction){"LDY", &LDY, &ZERO_ADDR, 3};
    lookup_table[0xA5] = (Instruction){"LDA", &LDA, &ZERO_ADDR, 3};
    lookup_table[0xA6] = (Instruction){"LDX", &LDX, &ZERO_ADDR, 3};
    lookup_table[0xA8] = (Instruction){"TAY", &TAY, &IMP_ADDR, 2};
    lookup_table[0xA9] = (Instruction){"LDA", &LDA, &IMM_ADDR, 2};
    lookup_table[0xAA] = (Instruction){"TAX", &TAX, &IMP_ADDR, 2};
    lookup_table[0xAC] = (Instruction){"LDY", &LDY, &ABS_ADDR, 4};
    lookup_table[0xAD] = (Instruction){"LDA", &LDA, &ABS_ADDR, 4};
    lookup_table[0xAE] = (Instruction){"LDX", &LDX, &ABS_ADDR, 4};

    // Row 0xB0
    lookup_table[0xB0] = (Instruction){"BCS", &BCS, &REL_ADDR, 2};
    lookup_table[0xB1] = (Instruction){"LDA", &LDA, &INDY_ADDR, 5};
    lookup_table[0xB4] = (Instruction){"LDY", &LDY, &ZEROX_ADDR, 4};
    lookup_table[0xB5] = (Instruction){"LDA", &LDA, &ZEROX_ADDR, 4};
    lookup_table[0xB6] = (Instruction){"LDX", &LDX, &ZEROY_ADDR, 4};
    lookup_table[0xB8] = (Instruction){"CLV", &CLV, &IMP_ADDR, 2};
    lookup_table[0xB9] = (Instruction){"LDA", &LDA, &ABSY_ADDR, 4};
    lookup_table[0xBA] = (Instruction){"TSX", &TSX, &IMP_ADDR, 2};
    lookup_table[0xBC] = (Instruction){"LDY", &LDY, &ABSX_ADDR, 4};
    lookup_table[0xBD] = (Instruction){"LDA", &LDA, &ABSX_ADDR, 4};
    lookup_table[0xBE] = (Instruction){"LDX", &LDX, &ABSY_ADDR, 4};

    // Row 0xC0
    lookup_table[0xC0] = (Instruction){"CPY", &CPY, &IMM_ADDR, 2};
    lookup_table[0xC1] = (Instruction){"CMP", &CMP, &INDX_ADDR, 6};
    lookup_table[0xC4] = (Instruction){"CPY", &CPY, &ZERO_ADDR, 3};
    lookup_table[0xC5] = (Instruction){"CMP", &CMP, &ZERO_ADDR, 3};
    lookup_table[0xC6] = (Instruction){"DEC", &DEC, &ZERO_ADDR, 5};
    lookup_table[0xC8] = (Instruction){"INY", &INY, &IMP_ADDR, 2};
    lookup_table[0xC9] = (Instruction){"CMP", &CMP, &IMM_ADDR, 2};
    lookup_table[0xCA] = (Instruction){"DEX", &DEX, &IMP_ADDR, 2};
    lookup_table[0xCC] = (Instruction){"CPY", &CPY, &ABS_ADDR, 4};
    lookup_table[0xCD] = (Instruction){"CMP", &CMP, &ABS_ADDR, 4};
    lookup_table[0xCE] = (Instruction){"DEC", &DEC, &ABS_ADDR, 6};

    // Row 0xD0
    lookup_table[0xD0] = (Instruction){"BNE", &BNE, &REL_ADDR, 2};
    lookup_table[0xD1] = (Instruction){"CMP", &CMP, &INDY_ADDR, 5};
    lookup_table[0xD5] = (Instruction){"CMP", &CMP, &ZEROX_ADDR, 4};
    lookup_table[0xD6] = (Instruction){"DEC", &DEC, &ZEROX_ADDR, 6};
    lookup_table[0xD8] = (Instruction){"CLD", &CLD, &IMP_ADDR, 2};
    lookup_table[0xD9] = (Instruction){"CMP", &CMP, &ABSY_ADDR, 4};
    lookup_table[0xDD] = (Instruction){"CMP", &CMP, &ABSX_ADDR, 4};
    lookup_table[0xDE] = (Instruction){"DEC", &DEC, &ABSX_ADDR, 7};

    // Row 0xE0
    lookup_table[0xE0] = (Instruction){"CPX", &CPX, &IMM_ADDR, 2};
    lookup_table[0xE1] = (Instruction){"SBC", &SBC, &INDX_ADDR, 6};
    lookup_table[0xE4] = (Instruction){"CPX", &CPX, &ZERO_ADDR, 3};
    lookup_table[0xE5] = (Instruction){"SBC", &SBC, &ZERO_ADDR, 3};
    lookup_table[0xE6] = (Instruction){"INC", &INC, &ZERO_ADDR, 5};
    lookup_table[0xE8] = (Instruction){"INX", &INX, &IMP_ADDR, 2};
    lookup_table[0xE9] = (Instruction){"SBC", &SBC, &IMM_ADDR, 2};
    lookup_table[0xEA] = (Instruction){"NOP", &NOP, &IMP_ADDR, 2};
    lookup_table[0xEC] = (Instruction){"CPX", &CPX, &ABS_ADDR, 4};
    lookup_table[0xED] = (Instruction){"SBC", &SBC, &ABS_ADDR, 4};
    lookup_table[0xEE] = (Instruction){"INC", &INC, &ABS_ADDR, 6};

    // Row 0xF0
    lookup_table[0xF0] = (Instruction){"BEQ", &BEQ, &REL_ADDR, 2};
    lookup_table[0xF1] = (Instruction){"SBC", &SBC, &INDY_ADDR, 5};
    lookup_table[0xF5] = (Instruction){"SBC", &SBC, &ZEROX_ADDR, 4};
    lookup_table[0xF6] = (Instruction){"INC", &INC, &ZEROX_ADDR, 6};
    lookup_table[0xF8] = (Instruction){"SED", &SED, &IMP_ADDR, 2};
    lookup_table[0xF9] = (Instruction){"SBC", &SBC, &ABSY_ADDR, 4};
    lookup_table[0xFD] = (Instruction){"SBC", &SBC, &ABSX_ADDR, 4};
    lookup_table[0xFE] = (Instruction){"INC", &INC, &ABSX_ADDR, 7};

}



bool get_flag(CPU_6502 *CPU, FLAGS_6502 F){
    return (CPU -> Reg_Status & F) != 0;
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

CPU_6502 *CPU_Create(Bus *bus){
    CPU_6502 *result = malloc(sizeof(CPU_6502));
    if (result == NULL){
        fprintf(stderr, "Failed to allocate memory for CPU\n");
        return NULL;
    }
    memset(result, 0, sizeof(CPU_6502));
    result->bus = bus;
    result->cpu_read = &read;
    result->cpu_write = &write;
    Instructions_Init(result);
    return result;
}

// CPU will "disconnect"
void CPU_6502_Destroy(CPU_6502 *CPU){
    if (CPU == NULL) return;
    free(CPU);
}