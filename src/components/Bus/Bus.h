#ifndef BUS_H
#define BUS_H

#include "../CPU/CPU.h"
#include <stdint.h>
#include <stdbool.h>
// 2KB of Memory
#define RAM_SIZE 0x0800
#define STACK_START 0x100
#define RAM_END 0x1FFF
#define RAM_MIRROR 0x07FF


struct Bus{

    CPU_6502 *CPU;
    uint8_t RAM[RAM_SIZE];

    uint8_t (*read)(Bus* bus, uint16_t address, bool read_only);
    void (*write)(Bus* bus, uint16_t address, uint8_t data);

};

Bus *Bus_Create();
void Bus_Destroy(Bus *bus);

#endif