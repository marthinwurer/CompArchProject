//
// Created by benjamin on 3/26/18.
//

#include <bitset>
#include <Clock.h>
#include "decode.h"
#include "addressing.h"
#include "opcodes.h"

void opcode_error(){
    mnemonic = "????";
//    print_addr = false;
    bad_addr = true;
    throw ArchLibError("opcode error");
}

void decode(ulong category) {
//    cout << "cat " << bitset<3>(category) << endl; // todo

    ulong s_am = ir.uvalue() >> 9 & 0b111;
    ulong s_reg = ir.uvalue() >> 6 & 0b111;
    ulong am = ir.uvalue() >> 3 & 0b111;
    ulong reg = ir.uvalue() & 0b111;

    switch (category){
        case 000:
            // other instruction
            decode_other();
            break;
        case 001:
            // mov
            operation = mov;
            mnemonic = "MOV";
            calc_addressing(s_am, s_reg, src);
            calc_addressing(am, reg, dest);
            break;
        case 002:
            // cmp
            // CMP	02ssdd	0 010 sss sss ddd ddd	Compare
            operation = cmp;
            mnemonic = "CMP";
            calc_addressing(s_am, s_reg, src);
            calc_addressing(am, reg, dest);
            break;
        case 006:
            // add
            // ADD	06ssdd	0 110 sss sss ddd ddd	Add
            operation = add;
            mnemonic = "ADD";
            calc_addressing(s_am, s_reg, src);
            calc_addressing(am, reg, dest);
            break;
        case 007:
            // sob
            //SOB	077rnn	0 111 111 rrr nnn nnn	Subtract one and branch
            operation = sob;
            mnemonic = "SOB";
            calc_addressing(0, s_reg, src);
            break;
        case 010:
            // a non-relational conditional branch instruction
            opcode_error();
            break;
        case 016:
            // SUB	16ssdd	1 110 sss sss ddd ddd	Subtract
            operation = sub;
            mnemonic = "SUB";
            calc_addressing(s_am, s_reg, src);
            calc_addressing(am, reg, dest);
            break;
        default:
            // todo
//            cout << bitset<3>(category) << endl;
            opcode_error();
    }

}

void decode_other() {
    // get the next bits
    ulong next = ir.uvalue() >> 9 & 0b111;
//    cout << "next " << bitset<3>(next) << endl; // todo


    switch (next){
        case 00:
            // a zero-operand instruction, br, rts, or jmp
            decode_0_op();
            break;
        case 01:
            opcode_error();
            break;
        case 02:
            opcode_error();
            break;
        case 03:
            // BGT
            operation = bgt;
            mnemonic = "BGT";
            break;
        case 04:
            opcode_error();
            break;
        case 05:
//            cout << "clr, com, inc, dec, neg, or tst" << endl; // todo
            decode_1_op();
            break;
        default:
            // todo
//            cout << "000" << bitset<3>(next) << endl;
            opcode_error();
    }

}

/**
CLR	0050dd	0 000 101 000 ddd ddd	Clear
COM	0051dd	0 000 101 001 ddd ddd	Complement
INC	0052dd	0 000 101 010 ddd ddd	Increment
DEC	0053dd	0 000 101 011 ddd ddd	Decrement
NEG	0054dd	0 000 101 100 ddd ddd	Negate
TST	0057dd	0 000 101 111 ddd ddd	Test
 */
void decode_1_op() {
    ulong opcode = ir.uvalue() >> 6 & 0b111;
//    cout << "opcode " << bitset<3>(opcode) << endl; // todo
    ulong am = ir.uvalue() >> 3 & 0b111;
    ulong reg = ir.uvalue() & 0b111;

    // latch the data to a register
    calc_addressing(am, reg, dest);


    switch (opcode){
        case 0b000:
            operation = clear;
            mnemonic = "CLR";
            break;
        case 0b001:
            opcode_error();
            break;
        case 0b010:
            operation = inc;
            mnemonic = "INC";
            break;
        case 0b011:
            operation = dec;
            mnemonic = "DEC";
            break;
        case 0b100:
            opcode_error();
            break;
        case 0b111:
            opcode_error();
            break;
        default:
            opcode_error();
            break;
    }


}

/***
HALT000000	0 000 000 000 000 000	Halt
BPT	000003	0 000 000 000 000 011	Breakpoint
JMP	0001dd	0 000 000 001 ddd ddd	Jump
RTS	00020r	0 000 000 010 000 rrr	Return from subroutine
NOP	000240	0 000 000 010 100 000	No operation
CLC	000241	0 000 000 010 100 001	Clear C
CLV	000242	0 000 000 010 100 010	Clear V
CLZ	000244	0 000 000 010 100 100	Clear Z
CLN	000250	0 000 000 010 101 000	Clear N
CCC	000257	0 000 000 010 101 111	Clear condition code
SEC	000261	0 000 000 010 110 001	Set C
SEV	000262	0 000 000 010 110 010	Set V
SEZ	000264	0 000 000 010 110 100	Set Z
SEN	000270	0 000 000 010 111 000	Set N
SCC	000277	0 000 000 010 111 111	Set condition code
BR	0004vv	0 000 000 1vv vvv vvv	Branch
 */
void decode_0_op() {
    // a zero-operand instruction, br, rts, or jmp
    ulong next = ir.uvalue() >> 6 & 0b111;
    ulong am = ir.uvalue() >> 3 & 0b111;
    ulong reg = ir.uvalue() & 0b111;


    switch (next){
        case 00:
            // a zero-operand instruction, br, rts, or jmp
            if ( reg == 0){
                // HALT
                mnemonic = "HALT";
                operation = pass;
//                print_addr = false;


                throw ArchLibError("HALT instruction");
            } else if (reg == 0b011){
//                cout << "BREAKPOINT" << endl;
                mnemonic = "BPT";
                bkpt = true;
                operation = pass;
            }
            else{
                opcode_error();
            }
            break;
        case 01:
            // JMP
            opcode_error();
            break;
        case 02:
            // cc stuff and rts
            operation = ccodes;
            break;
        case 03:
            opcode_error();
            break;
        default:
            // todo
            if (next & 0b100){
                // br
                mnemonic = "BR";
                operation = br;

            }else{
//            cout << "nnext" << bitset<3>(next) << endl;
                opcode_error();

            }
    }




}
