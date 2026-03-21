#include "CPU.h"
#include "Bus.h"
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

uint8_t LDA(CPU_6502* CPU);


bool LDA_Test(){
    Bus* bus = malloc(sizeof(Bus));
    Bus_Init(bus);

    bus -> CPU -> fetched = 0xFF;

    assert(LDA(bus -> CPU) == 1);
    assert(bus -> CPU -> Reg_Acc == 255);
    assert((((bus -> CPU -> Reg_Status) >> 7) & 0x1) == 1);
    assert((((bus -> CPU -> Reg_Status) >> 1) & 0x1) == 0);
    
    
    bus -> CPU -> fetched = 0x00;

    assert(LDA(bus -> CPU) == 1);
    assert(bus -> CPU -> Reg_Acc == 0x00);
    assert((((bus -> CPU -> Reg_Status) >> 7) & 0x1) == 0);
    assert((((bus -> CPU -> Reg_Status) >> 1) & 0x1) == 1);

    free(bus);
    return true;


}