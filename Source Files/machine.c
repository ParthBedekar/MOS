#include "../Header Files/machine.h"

Machine machine;

void initialize() {
    for (int i = 0; i < 300; i++)
        for (int j = 0; j < 4; j++)
            machine.RAM[i][j] = ' ';

    machine.IC = 0;
    machine.SI = 0;
    machine.TI = 0;
    machine.PI = 0;
    machine.C  = 'F';
    machine.ptr = -1;
    machine.fault_ptr = -1;
    for (int i = 0; i < 4; i++) { machine.IR[i] = ' '; machine.R[i] = ' '; }
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        machine.page_table[i].frame = -1;
        machine.page_table[i].valid = 0;
    }
}

void clear_memory() {
    for (int i = 0; i < 290; i++) {
        for (int j = 0; j < 4; j++) {
            machine.RAM[i][j] = ' ';
        }
    }
}

void print_memory() {
    printf("\n========= RAM DUMP =========\n");
    printf("       [0]  [1]  [2]  [3]\n");
    for (int i = 0; i < 300; i++) {
        if (i % FRAME_SIZE == 0)
            printf("       ----------- Frame %02d (RA %03d-%03d) -----------\n",
                   i / FRAME_SIZE, i, i + FRAME_SIZE - 1);
        printf("%03d  | ", i);
        for (int j = 0; j < 4; j++)
            printf("  %c  ", machine.RAM[i][j] == ' ' ? '.' : machine.RAM[i][j]);
        printf("\n");
    }
    printf("============================\n");
}
