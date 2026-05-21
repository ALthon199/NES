#include "components/CPU/CPU.h"
#include "components/Bus/Bus.h"
#include "components/CPU/opcodes.h"
#include "tests.h"
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>




bool LDA_Test(){
    Bus* bus = malloc(sizeof(Bus));
    Bus_Init(bus);

    bus -> CPU -> cpu_write(bus -> CPU, bus -> CPU -> addr_abs, 0xFF);


    
    assert(LDA(bus -> CPU) == 1);
    assert(bus -> CPU -> Reg_Acc == 255);
    assert((((bus -> CPU -> Reg_Status) >> 7) & 0x1) == 1);
    assert((((bus -> CPU -> Reg_Status) >> 1) & 0x1) == 0);
    
    
    bus -> CPU -> cpu_write(bus -> CPU, bus -> CPU -> addr_abs, 0x00);

    assert(LDA(bus -> CPU) == 1);
    assert(bus -> CPU -> Reg_Acc == 0x00);
    assert((((bus -> CPU -> Reg_Status) >> 7) & 0x1) == 0);
    assert((((bus -> CPU -> Reg_Status) >> 1) & 0x1) == 1);

    Bus_Destroy(bus);
    printf("LDA Test Passed\n");
    return true;


}

