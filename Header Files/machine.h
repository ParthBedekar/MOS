#ifndef MACHINE_H
#include <stdio.h>
#include <stdint.h>
#include "page_entry.h"
#define MACHINE_H

#define FRAME_SIZE 10
#define NUM_FRAMES 30
#define PAGE_TABLE_SIZE 10
#define PAGE_SIZE 10


typedef struct{
    char RAM[300][4];
    char IR[4];
    int IC;
    char R[4];
    char C;
    uint8_t SI;
    uint8_t TI;
    uint8_t PI;
    int ptr;
    PTE page_table[PAGE_TABLE_SIZE];
    int fault_ptr;
}Machine;

extern Machine machine;

void initialize();
void print_memory();
void clear_memory();
#endif
