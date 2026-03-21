#ifndef BUS_H
#define BUS_H

#include "CPU.h"
#include <stdint.h>
#include <stdbool.h>
// 2KB of Memory
#define RAM_SIZE 0x800


struct Bus{

    CPU_6502* CPU;
    uint8_t RAM[RAM_SIZE];

    uint8_t (*read)(Bus* bus, uint16_t address, bool read_only);
    void (*write)(Bus* bus, uint16_t address, uint8_t data);

};

void Bus_Init(Bus* bus);

void Bus_Destroy(Bus* bus);

#endif