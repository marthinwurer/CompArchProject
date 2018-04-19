//
// Created by benjamin on 3/27/18.
//

#include <Clock.h>
#include <bitset>
#include "opcodes.h"
#include "globals.h"

void setbit(Clearable & s, bool val){
    if (val){
        s.latchFrom(bitbus.OUT());
    }
    else{
        s.clear();
    }
}

void clear() {
    out.clear();
    N.clear();
    bitbus.IN().pullFrom(const_1);
    Z.latchFrom(bitbus.OUT());
    V.clear();
    C.clear();
    Clock::tick();
}

void mov() {
    sbus.IN().pullFrom(sss);
    out.latchFrom(sbus.OUT());
    bitbus.IN().pullFrom(const_1);
    setbit(N, sss(DATA_BITS - 1) == 1);
    setbit(Z, sss.uvalue() == 0);
    setbit(V, false);
    Clock::tick();
}

void add() {
    alu.perform(BusALU::op_add);
    alu.OP1().pullFrom(ddd);
    alu.OP2().pullFrom(sss);
    out.latchFrom(alu.OUT());
    V.latchFrom(alu.OFLOW());
    C.latchFrom(alu.CARRY());
    Clock::tick();

    bitbus.IN().pullFrom(const_1);
    setbit(N, out(DATA_BITS - 1) == 1);
    setbit(Z, out.uvalue() == 0);
    Clock::tick();

}

void sub() {
    alu.perform(BusALU::op_sub);
    alu.OP1().pullFrom(ddd);
    alu.OP2().pullFrom(sss);
    out.latchFrom(alu.OUT());
    Clock::tick();
    /*
     * 	sub*	N <- result < 0
		Z <- result == 0
		V <- opnds had different signs & result has sign of (src)
		C <- (dst) + ~(src) + 1 < 2^16
     */

    bitbus.IN().pullFrom(const_1);
    setbit(N, out() == 1);
    setbit(Z, out.uvalue() == 0);
    setbit(V, ((ddd() ^ sss()) == 1) && (sss() == out()));
//    cout << (ddd.uvalue()) << " " << (~(sss.uvalue()) & 0xFFFF) << " " << (1 << 16) << endl;
    setbit(C, ((ddd.uvalue()) + (~(sss.uvalue()) & 0xFFFF) + 1) < 1<<16);
    Clock::tick();

}

void bgt() {
    // BGT	0030vv	0 000 011 0vv vvv vvv	Branch on >
    // bgt	0030vv	if( !Z & (N == V) ) PC <- PC + 2*vv
    imm = true;
    immediate = (char) ir(7, 0);
//    cout <<dec << immediate<<endl;
//    cout <<bitset<8>(immediate)<<endl;
    // sign extend
//    if (immediate & 1<<7){
//        immediate |= 0b1111111110000000;
//    }
//    cout <<dec << immediate<<endl;
    immediate *= 2;
    immediate -= 2;
//    cout <<dec << immediate<<endl;
    if ( Z.uvalue() == 0 && (N.uvalue() == V.uvalue())){
        regs[7]->backDoor((short)regs[7]->uvalue() + immediate);
    }
    Clock::tick();



}

void pass() {

}

void cmp() {
    alu.perform(BusALU::op_sub);
    alu.OP1().pullFrom(sss);
    alu.OP2().pullFrom(ddd);
    out.latchFrom(alu.OUT());
//    cout << out<< endl;
    Clock::tick();
//    cout << out<< endl;
    dest.writeback = false;
    /* "result" is (src) - (dst)
     * 	N <- result < 0
		Z <- result == 0
		V <- opnds had different signs & result has sign of (dst)
		C <- (src) + ~(dst) + 1 < 2^16
     sub*	N <- result < 0
		Z <- result == 0
		V <- opnds had different signs & result has sign of (src)
		C <- (dst) + ~(src) + 1 < 2^16
     */

    bitbus.IN().pullFrom(const_1);
    setbit(N, out() == 1);
//    cout << bitset<16>(sss.uvalue())<< " " << bitset<16>(sss.uvalue()) << endl;
//    cout << (short) sss.uvalue() - (short) ddd.uvalue() << endl;
//    cout << sss<< endl;
//    cout << ddd<< endl;
//    cout << out<< endl;
    setbit(Z, out.uvalue() == 0);
    setbit(V, ((ddd() ^ sss()) == 1) && (ddd() == out()));
//    cout << (ddd.uvalue()) << " " << (~(sss.uvalue()) & 0xFFFF) << " " << (1 << 16) << endl;
    setbit(C, ((sss.uvalue()) + (~(ddd.uvalue()) & 0xFFFF) + 1) < 1<<16);
    Clock::tick();

}

void sob() {
    //SOB	077rnn	0 111 111 rrr nnn nnn	Subtract one and branch

    // sob	077rnn	r <- r - 1; if( new r != 0 ) PC <- PC - 2*nn

}

void br() {
    // br	0004vv	PC <- PC + 2*vv
    imm = true;
    immediate = (char) ir(7, 0);
    immediate *= 2;
//    immediate -= 2;
    regs[7]->backDoor((short)regs[7]->uvalue() + immediate);
    Clock::tick();

}

/*
 * INC	0052dd	0 000 101 010 ddd ddd	Increment
 * inc*	N <- result < 0
		Z <- result == 0
		V <- (dst) was MPI
		C is unchanged

 inc	0052dd	dd <- dd + 1
 */
void inc() {
    alu.perform(BusALU::op_add);
    alu.OP1().pullFrom(ddd);
    alu.OP2().pullFrom(const_01);
    out.latchFrom(alu.OUT());
    Clock::tick();

    bitbus.IN().pullFrom(const_1);
    setbit(N, out(DATA_BITS - 1) == 1);
    setbit(Z, out.uvalue() == 0);
    setbit(V, ddd.uvalue() == 077777);
    Clock::tick();

}

void dec() {
    alu.perform(BusALU::op_sub);
    alu.OP1().pullFrom(ddd);
    alu.OP2().pullFrom(const_01);
    out.latchFrom(alu.OUT());
    Clock::tick();

    bitbus.IN().pullFrom(const_1);
    setbit(N, out(DATA_BITS - 1) == 1);
    setbit(Z, out.uvalue() == 0);
    setbit(V, ddd.uvalue() == 0100000);
    Clock::tick();

}

void ccodes() {
    /*
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
     */

    ulong code = ir(3,0);
//    cout << code << endl;
    ulong op = ir(4);

    if (op == 0 && code == 0){
        mnemonic = "NOP";
        return;
    }

    // set
    if (op == 1){
        bitbus.IN().pullFrom(const_1);
        switch (code){
            case 0b0001:
                mnemonic = "SEC";
                setbit(C, true);
                break;
            case 0b0010:
                mnemonic = "SEV";
                setbit(V, true);
                break;
            case 0b0100:
                mnemonic = "SEZ";
                setbit(Z, true);
                break;
            case 0b1000:
                mnemonic = "SEN";
                setbit(N, true);
                break;
            case 0b1111:
                mnemonic = "SCC";
                setbit(N, true);
                setbit(Z, true);
                setbit(V, true);
                setbit(C, true);
                break;
            default:
                mnemonic = "????";
                bad_addr = true;
                throw ArchLibError("opcode error");

        }

    }

    else{
        bitbus.IN().pullFrom(const_1);
        switch (code){
            case 0b0001:
                mnemonic = "CLC";
                setbit(C, false);
                break;
            case 0b0010:
                mnemonic = "CLV";
                setbit(V, false);
                break;
            case 0b0100:
                mnemonic = "CLZ";
                setbit(Z, false);
                break;
            case 0b1000:
                mnemonic = "CLN";
                setbit(N, false);
                break;
            case 0b1111:
                mnemonic = "CCC";
                setbit(N, false);
                setbit(Z, false);
                setbit(V, false);
                setbit(C, false);
                break;
            default:
                mnemonic = "????";
                bad_addr = true;
                throw ArchLibError("opcode error");

        }

    }

    Clock::tick();




}































