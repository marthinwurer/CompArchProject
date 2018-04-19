//
// Created by benjamin on 2/19/18.
//

#include <iomanip>
#include "globals.h"
const unsigned int ADR_BITS = 16;
const unsigned int DATA_BITS = 16;
const unsigned int NUM_REGS = 8;

Bus abus("ABUS", ADR_BITS); // address bus. Used when addresses are to be moved.
Bus dbus("DBUS", DATA_BITS); // Data Bus. Used when data and instructions are to be moved.
Bus sbus("SBUS", DATA_BITS); // Setup Bus. Used to set up the operands
Bus bitbus("BITBUS", DATA_BITS); // bus for setting status registers

vector<Clearable*> regs;

Clearable N("N", 1);
Clearable V("V", 1);
Clearable C("C", 1);
Clearable Z("Z", 1);
Clearable sss("SSS", DATA_BITS);
Clearable ddd("DDD", DATA_BITS);
Clearable out("OUT", DATA_BITS);
StorageObject mdr("MDR", DATA_BITS); // Data to be written into, or data most recently read from, memory.
StorageObject ir("IR", DATA_BITS); // Instruction Register. Instruction being decoded and executed.
StorageObject xr("XR", ADR_BITS); // Index register. Contains a value to be used in calculating a memory address.
StorageObject immr("IMMR", DATA_BITS);
Memory m("Memory", 16, 8, -1, 2, true); // Memory
BusALU alu("ALU", DATA_BITS); // ALU
BusALU addr_alu("A_ALU", ADR_BITS); // ALU

StorageObject se_mask_12("SE12", ADR_BITS, 0x800);
StorageObject const_2("CONST2", DATA_BITS, 2);
StorageObject const_01("CONST01", DATA_BITS, 1);
StorageObject const_1("CONST1", 1, 1);

bool halt(false);
bool do_writeback(false);
bool addressing_failed(false);

struct am_data dest;
struct am_data src;

void (*operation)();


// globals for trace printing
ulong ps, instruction, addr, A, B, XR, PS;
short immediate;
bool print_addr, bad_addr, bkpt, imm;
string mnemonic;


void print_am(struct am_data & am){
    if( am.D){
        cout << setfill('0') << setw(6) << oct << am.D_addr;
    }
    cout << am.mnemomic;
}


void print_trace() {

    cout << setfill('0') << setw(6) << oct << addr << ":  PS[";
    cout << setfill('0') << setw(2) << oct << ps << "]  ";
    if (print_addr) {
        cout << setfill('0') << setw(6) << oct << instruction << " ";
    } else if (bad_addr) {
        cout << "???? ";
    } else {
        cout << "     ";
    }
    if( src.D){
        cout << setfill('0') << setw(6) << oct << src.D_addr << " ";
    }
    if( dest.D){
        cout << setfill('0') << setw(6) << oct << dest.D_addr << " ";
    }




    if (! bad_addr){
        cout << " " << setfill(' ') << left << mnemonic << right;
    } else {
        cout << " " << setfill(' ') << left << setw(3) << "????" << right;
        bad_addr = false;
    }

    if (src.valid || dest.valid){
        cout << " ";
    }

    if (imm){
        cout << " " << dec << immediate;
    }

    if (src.valid){
        print_am(src);
        cout << ",";
    }
    if (dest.valid){
        print_am(dest);
    }

//    if (print_addr) {
//        cout << setfill('0') << setw(3) << hex << addr << "  ";
//    } else if (bad_addr) {
//        cout << "???  ";
//    } else {
//        cout << "     ";
//    }

    cout <<endl;

    if (bkpt){
        for ( int ii = 0; ii < 8; ii++){
            cout << "  " << *regs[ii];
            if ( ii == 3){
                cout << endl;
            }
        }

        cout << endl;
        bkpt = false;

    }
}


