#include "CPU.h"
#include "stdio.h"


uint8_t LDA(CPU_6502* CPU){
    CPU -> Reg_Acc = CPU -> fetched;
    printf("Fetched: %d\n", CPU -> fetched);
    if (CPU -> fetched == 0){
        CPU -> Reg_Status |= (1 << 1);
        printf("Zero Flag Set\n");
    } else {
        CPU -> Reg_Status &= ~(1 << 1);
        printf("Zero Flag Unset\n");
    }

    if ((CPU -> fetched & (1 << 7)) != 0){
        CPU -> Reg_Status |= (1 << 7);
        printf("Negative Flag Set\n");
    } else {
        CPU -> Reg_Status &= ~(1 << 7);
        printf("Negative Flag Unset\n");
    }

    return 1;
}