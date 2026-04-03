#include "Bus.h"
#include "../CPU/CPU.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
static uint8_t read(Bus* bus, uint16_t address, bool read_only){
    // 0x07FF for mirroring
    if (0 <= address && address < 0x800){
        return bus -> RAM[address & 0x07FF];
    }

    printf("SHOULD NOT REACH HERE");
    return 0;
}

static void write(Bus* bus, uint16_t address, uint8_t data){
    // 0x07FF for mirroring
    if (0 <= address && address < 0x07FF){
        (bus -> RAM)[address] = data;
    }
    
}

void Bus_Init(Bus* bus){
    memset(bus, 0, sizeof(Bus));
    
    // Allocate componenets
    bus -> CPU = malloc(sizeof(CPU_6502));


    // Initialize components
    CPU_6502_Init(bus -> CPU, bus);


    // Set function pointers
    bus -> read = &read;
    bus -> write = &write;
}

// Bus will first disconnect components
void Bus_Destroy(Bus* bus){
    if (bus == NULL){
        return;
    }
    if (bus -> CPU != NULL){
        CPU_6502_Destroy(bus -> CPU);
        // Ensure CPU will not be freed after. 
        bus -> CPU = NULL;
    }
    free(bus -> CPU);
}
