#ifndef MOS_H
#define MOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine.h"
#include "job.h"
#include "cpu.h"
#include "mmu.h"

void init_mos(char* ip,char* op);
int readCard();
void run();
void boot();
int is_valid_instruction(char* str, int i, int len);
void mos_read();
void mos_write();
int mos_eof();

void mos_terminate();
void mos_close();
extern int lines;
#endif
