#include "../Header Files/cpu.h"
#include <stdio.h>



void fetch() {
    int ra = compute_address(machine.IC);
    if (machine.PI != 0) return;
    memcpy(machine.IR, machine.RAM[ra], 4);
    machine.IC++;
}


void exe_LR(int addr){ memcpy(machine.R, machine.RAM[addr], 4); }
void exe_SR(int addr){ memcpy(machine.RAM[addr], machine.R, 4); }
void exe_CR(int addr){ machine.C = memcmp(machine.R, machine.RAM[addr], 4) == 0 ? 'T' : 'F'; }
void exe_BT(int addr){ if(machine.C == 'T') machine.IC = addr; }

void decode() {
    if (machine.IR[0]=='G' && machine.IR[1]=='D') { machine.SI = 1; return; }
    if (machine.IR[0]=='P' && machine.IR[1]=='D') { machine.SI = 2; return; }
    if (machine.IR[0]=='H')                        { machine.SI = 3; return; }
    if (machine.IR[0]==' ')                        { return; }

    int va = extract_address(machine.IR[2], machine.IR[3]);

    if (machine.IR[0]=='B' && machine.IR[1]=='T') { exe_BT(va); return; }

    int ra = compute_address(va);
    if (machine.PI != 0) return;

    if      (machine.IR[0]=='L' && machine.IR[1]=='R') exe_LR(ra);
    else if (machine.IR[0]=='S' && machine.IR[1]=='R') exe_SR(ra);
    else if (machine.IR[0]=='C' && machine.IR[1]=='R') exe_CR(ra);
    else machine.PI = 1;
}
void run() {
    int instr_cnt = 0;
    while (machine.SI == 0 && machine.PI == 0 && machine.TI == 0) {
        if (instr_cnt > job.time_limit) { machine.TI = 2; break; }
        instr_cnt++;
        fetch();
        if (machine.PI != 0) break;
        decode();
    }
}
