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


class Stage{
public:
    BusALU alu;
    StorageObject v; // valid
    Counter pc; // program counter
    Counter npc; // next program counter
    StorageObject ir; // instruction
    StorageObject A; // rs register
    StorageObject B; // rt register
    StorageObject imm; // immediate value
    StorageObject out; // output of previous layer (ir, ?, alu out, read value)

    // busses come out of those registers in this layer
    Bus pcbus; // out of pc
    Bus imbus; // out of immediate
    Bus outbus; // out of out
    Bus abus; // out of a
    Bus bbus; // out of b




    explicit Stage(string s);

    void connect_next(Stage s); // connects the next stage to this one. (ie ifid(idex))

};

// hardware

extern Stage ifid, idex, exmem, memwb;


extern const unsigned int ADR_BITS;
extern const unsigned int DATA_BITS;
extern const unsigned int NUM_REGS;

extern vector<Clearable*> regs;
extern Counter pc;
extern StorageObject mdr;
extern Memory im; // instruction memory
extern Memory dm; // data memory

extern BusALU alu;
extern BusALU addr_alu; // ALU

extern Bus writeback;
extern Bus pcbus;





// other globals

extern bool halt, do_writeback, addressing_failed;

extern StorageObject se_mask_12, const_2, const_1, const_01;
extern ulong ps, instruction, addr, A, B, XR;
extern short immediate;
extern bool print_addr, bad_addr, bkpt, imm;
extern string mnemonic;

extern void (*operation)();

void print_trace();





#endif //P1_GLOBALS_H
