#include "CPU.h"
#include "Bus.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

static uint8_t read(CPU_6502* CPU, uint16_t address, bool read_only){

    if (0 <= address && address < 0x1FFF){
        // 0x07FF for mirroring
        return CPU -> bus -> read(CPU->bus, address & 0x07FF, read_only);
    }

    return 0;
}

static void write(CPU_6502* CPU, uint16_t address, uint8_t data){
    // 0x07FF for mirroring
    if (0 <= address && address < 0x07FF){
        CPU->bus->write(CPU->bus, address, data);
    }
    
}


void Clock(CPU_6502* CPU){
    if (CPU -> cycles_left == 0){
        CPU -> opcode = (*(CPU -> cpu_read))(CPU, CPU -> PC, true);
    }
}

void CPU_6502_Init(CPU_6502* CPU, Bus *bus){
    memset(CPU, 0, sizeof(CPU_6502));
    CPU->bus = bus;
    CPU->cpu_read = &read;
    CPU->cpu_write = &write;
}

void CPU_6502_Destroy(CPU_6502* CPU){
    if (CPU == NULL){
        return;
    }
    if (CPU -> bus != NULL){
        Bus_Destroy(CPU -> bus);
    }
    free(CPU);
}