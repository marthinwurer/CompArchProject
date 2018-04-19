//
// Created by benjamin on 2/19/18.
//

#ifndef P1_GLOBALS_H
#define P1_GLOBALS_H


#include <vector>
#include <Bus.h>
#include <Counter.h>
#include <Memory.h>
#include <BusALU.h>
#include <Clearable.h>

typedef unsigned long ulong;


struct am_data{
    bool valid;
    ulong reg;
    ulong am;
    bool memory;
    bool inc;
    bool dec;
    bool D;
    ulong D_addr;
    string mnemomic;
    bool writeback;
};

extern struct am_data dest, src;


extern const unsigned int ADR_BITS;
extern const unsigned int DATA_BITS;
extern const unsigned int NUM_REGS;

extern Bus abus;
extern Bus dbus, bitbus, immbus;
extern Bus sbus; // Setup Bus. Used to set up the operands

extern vector<Clearable*> regs;

extern Clearable sss, ddd, out;
extern Clearable N, Z, V, C;
extern StorageObject mdr;
extern StorageObject xr;
extern StorageObject immr;
extern StorageObject ir;
extern Memory m;
extern BusALU alu;
extern BusALU addr_alu; // ALU

extern bool halt, do_writeback, addressing_failed;

extern StorageObject se_mask_12, const_2, const_1, const_01;
extern ulong ps, instruction, addr, A, B, XR;
extern short immediate;
extern bool print_addr, bad_addr, bkpt, imm;
extern string mnemonic;

extern void (*operation)();

void print_trace();





#endif //P1_GLOBALS_H
