#include "Bus.h"
#include "../CPU/CPU.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>



static uint8_t read(Bus *bus, uint16_t address, bool read_only){
    // 0x07FF for mirroring
    if (address <= RAM_END){
        return bus -> RAM[address & RAM_MIRROR];
    }

    printf("SHOULD NOT REACH HERE");
    return 0;
}

static void write(Bus *bus, uint16_t address, uint8_t data){
    // 0x07FF for mirroring
    if (address <= RAM_END){
        (bus -> RAM)[address & RAM_MIRROR] = data;
    }

    // Temporary Stub: Allow the CPU to read fake Reset Vector (0xFFFC/D) to boot safely
    if (address == 0xFFFC) return 0x00; // Low byte of boot address
    if (address == 0xFFFD) return 0x00; // High byte of boot address (starts execution at 0x0000)
    return 0;
    
}

Bus *Bus_Create(){

    Bus *result = malloc(sizeof(Bus));
    if (result == NULL){
        fprintf(stderr, "Failed to allocate memory for Bus\n");
        return NULL;
    }
    memset(result, 0, sizeof(Bus));
    

    // Create componenets
    result -> CPU = CPU_Create(result);

    if (result -> CPU == NULL){
        fprintf(stderr, "Failed to allocate memory for CPU\n");
        free(result);
        return NULL;
    }
   


    // Set function pointers
    result -> read = &read;
    result -> write = &write;
    return result;
}

// Bus will first disconnect components
void Bus_Destroy(Bus *bus){

    if (bus == NULL){
        return;
    }
    if (bus -> CPU != NULL){
        CPU_6502_Destroy(bus -> CPU);
        // Ensure CPU will not be freed after. 
        bus -> CPU = NULL;
    }
   
    free(bus);
    bus = NULL;
}
