#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>
#include <stdbool.h>

typedef struct CPU_6502 CPU_6502;

// Addressing Modes

// Implied
uint8_t IMP_ADDR(CPU_6502 *CPU);

// Immediate
uint8_t IMM_ADDR(CPU_6502 *CPU);

// Accumulator
uint8_t ACC_ADDR(CPU_6502 *CPU);

// Absolute Addressing
uint8_t ABS_ADDR(CPU_6502 *CPU);

// Absolute X Addr
uint8_t ABSX_ADDR(CPU_6502 *CPU);

// Absolute Y Addr
uint8_t ABSY_ADDR(CPU_6502 *CPU);

// Zero Page Addr
uint8_t ZERO_ADDR(CPU_6502 *CPU);

// Zero Page X Addr
uint8_t ZEROX_ADDR(CPU_6502 *CPU);

// Zero Page Y Addr
uint8_t ZEROY_ADDR(CPU_6502 *CPU);

// Indirect
uint8_t IND_ADDR(CPU_6502 *CPU);

// Indirect X
uint8_t INDX_ADDR(CPU_6502 *CPU);

// Indirect Y
uint8_t INDY_ADDR(CPU_6502 *CPU);




// Relative (branch offsets)
uint8_t REL_ADDR(CPU_6502 *CPU);




// Opcodes - Instructions

// Debugging Opcode

uint8_t XXX(CPU_6502 *CPU);


///////////////////////////
///////ACCESS CODES////////
///////////////////////////
uint8_t LDA(CPU_6502 *CPU);
uint8_t LDX(CPU_6502 *CPU);
uint8_t LDY(CPU_6502 *CPU);
uint8_t STA(CPU_6502 *CPU);
uint8_t STX(CPU_6502 *CPU);
uint8_t STY(CPU_6502 *CPU);

///////////////////////////
/////TRANSFER CODES////////
///////////////////////////
uint8_t TAX(CPU_6502 *CPU);
uint8_t TAY(CPU_6502 *CPU);
uint8_t TSX(CPU_6502 *CPU);
uint8_t TXA(CPU_6502 *CPU);
uint8_t TXS(CPU_6502 *CPU);
uint8_t TYA(CPU_6502 *CPU);

///////////////////////////
///////STACK CODES/////////
///////////////////////////
uint8_t PHA(CPU_6502 *CPU);
uint8_t PLA(CPU_6502 *CPU);
uint8_t PHP(CPU_6502 *CPU);
uint8_t PLP(CPU_6502 *CPU);

///////////////////////////
///////FLAG CODES/////////
///////////////////////////
uint8_t CLC(CPU_6502 *CPU);
uint8_t SEC(CPU_6502 *CPU);
uint8_t CLD(CPU_6502 *CPU);
uint8_t SED(CPU_6502 *CPU);
uint8_t CLI(CPU_6502 *CPU);
uint8_t SEI(CPU_6502 *CPU);
uint8_t CLV(CPU_6502 *CPU);
uint8_t NOP(CPU_6502 *CPU);

///////////////////////////
///////BITWISE CODES/////////
///////////////////////////
uint8_t AND(CPU_6502 *CPU);
uint8_t ORA(CPU_6502 *CPU);
uint8_t EOR(CPU_6502 *CPU);
uint8_t BIT(CPU_6502 *CPU);

///////////////////////////
///////ARITHMETIC CODES/////////
///////////////////////////
uint8_t INX(CPU_6502 *CPU);
uint8_t INY(CPU_6502 *CPU);
uint8_t DEX(CPU_6502 *CPU);
uint8_t DEY(CPU_6502 *CPU);
uint8_t ADC(CPU_6502 *CPU);
uint8_t SBC(CPU_6502 *CPU);
uint8_t INC(CPU_6502 *CPU);
uint8_t DEC(CPU_6502 *CPU);

////////////////////
////////JUMPS CODES///////
////////////////////
uint8_t JMP(CPU_6502 *CPU);
uint8_t JSR(CPU_6502 *CPU);
uint8_t RTS(CPU_6502 *CPU);
uint8_t BRK(CPU_6502 *CPU);

////////////////////////////
//////Shift Codes///////////
////////////////////////////
uint8_t ASL(CPU_6502 *CPU);
uint8_t LSR(CPU_6502 *CPU);
uint8_t ROL(CPU_6502 *CPU);
uint8_t ROR(CPU_6502 *CPU);

//////////////////////////
//////COMPARE CODES///////
//////////////////////////
uint8_t CMP(CPU_6502 *CPU);
uint8_t CPX(CPU_6502 *CPU);
uint8_t CPY(CPU_6502 *CPU);

//////////////////////////
//////BRANCH CODES///////
//////////////////////////
uint8_t BCC(CPU_6502 *CPU);
uint8_t BCS(CPU_6502 *CPU);
uint8_t BEQ(CPU_6502 *CPU);
uint8_t BNE(CPU_6502 *CPU);
uint8_t BPL(CPU_6502 *CPU);
uint8_t BMI(CPU_6502 *CPU);
uint8_t BVC(CPU_6502 *CPU);
uint8_t BVS(CPU_6502 *CPU);

#endif