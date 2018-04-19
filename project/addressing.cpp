//
// Created by benjamin on 2/24/18.
//

#include <Clock.h>
#include <sstream>
#include "addressing.h"


void check_addressing(){
    if (m.MAR()(0) == 1){
        // unaligned memory access
        throw ArchLibError("unaligned memory access");

    }
}

/**
The PDP-11 supports eight addressing modes. Each operand is represented
in the instruction word with a three-bit mode specifier, followed by a
three-bit register designator. Some modes require an additional word of
 information, called the extension, in the instruction sequence.

Here are descriptions of the addressing modes when R0-R6 are used:

Mode	Name	            Assembler	Extension?	Effective Address (EA) Computed As
0	000    register	        Rn	        none	    Rn (not a memory address)
1   001	reg. deferred	    (Rn)	    none	    contents of Rn
2   010	autoincrement	    (Rn)+	    none	    contents of Rn, before increment of Rn
3   011	autoinc. deferred	@(Rn)+	    none	    contents of indirect word in memory pointed at by Rn, before increment of Rn
4   100	autodecrement	    -(Rn)	    none	    contents of Rn, after decrement of Rn
5   101	autodec. deferred	@-(Rn)	    none	    contents of indirect word in memory pointed at by Rn, after decrement of Rn
6   110	displacement	    D(Rn)	    16-bit D	D + contents of Rn
7   111	disp. deferred	    @D(Rn)	    16-bit D	contents of indirect word in memory pointed at by (D + contents of Rn)

 * @param am
 */
bool calc_addressing(ulong am, ulong reg, struct am_data & data){

    // 0 (register), 1 (register deferred), 2 (autoincrement), 4 (autodecrement), 6 (displacement)
    data.valid = true;
    data.am = am;
    data.memory = true;
    data.inc = false;
    data.dec = false;
    data.D = false;
    data.reg = reg;
    data.writeback = true;

    stringstream ss;

    switch (am){
        case 0:
            data.memory = false;
            ss << "R" << reg;
            data.mnemomic = ss.str();
            break;
        case 01:
            ss << "(R" << reg << ")";
            data.mnemomic = ss.str();
            break;
        case 02:
            data.inc = true;
            ss << "(R" << reg << ")+";
            data.mnemomic = ss.str();
            break;
        case 04:
            data.dec = true;
            ss << "-(R" << reg << ")";
            data.mnemomic = ss.str();
            break;
        case 06:
            data.D = true;
            ss << "(R" << reg << ")";
            data.mnemomic = ss.str();
            break;
        default:
//            cout << "AHHHHHHHHHHHHHHHHHH" << endl;
            addressing_failed = true;
            data.mnemomic = "????";
    }
}


void load(struct am_data &am, StorageObject &dest) {
    StorageObject & reg = *regs[am.reg];
    if (am.memory){
        if(am.dec){
            alu.perform(BusALU::op_sub);
            alu.OP1().pullFrom(reg);
            alu.OP2().pullFrom(const_2);
            reg.latchFrom(alu.OUT());
            Clock::tick();
        }

        // if D is set, then inc pc, load from there, then add
        if (am.D){
            // inc pc and load previous value into mar
            alu.perform(BusALU::op_add);
            alu.OP1().pullFrom(*regs[7]);
            alu.OP2().pullFrom(const_2);
            regs[7]->latchFrom(alu.OUT());
            abus.IN().pullFrom(*regs[7]);
            m.MAR().latchFrom(abus.OUT());
            Clock::tick();

            // load memory into mdr, then add with reg and store in mar
            m.read();
            mdr.latchFrom(m.READ());
            check_addressing();
            Clock::tick();

            alu.perform(BusALU::op_add);
            alu.OP1().pullFrom(reg);
            alu.OP2().pullFrom(mdr);
            m.MAR().latchFrom(alu.OUT());
            Clock::tick();

            am.D_addr = mdr.uvalue();

        }else{
            // basic version
            // load the mar
            abus.IN().pullFrom(reg);
            m.MAR().latchFrom(abus.OUT());
            Clock::tick();

        }

        // load the data into the op
        dest.latchFrom(m.READ());
        m.read();
        check_addressing();
        Clock::tick();
    }
    else{
        sbus.IN().pullFrom(reg);
        dest.latchFrom(sbus.OUT());
        Clock::tick();
    }
}

/**
 * Put out back into the right register
 * @param am
 */

void writeback(struct am_data &am) {
    StorageObject & reg = *regs[am.reg];
    if (am.memory) {
        // writeback
        if (am.writeback){
            m.write();
            m.WRITE().pullFrom(out);
            check_addressing();
            Clock::tick();
        }

        if (am.inc) {
            alu.perform(BusALU::op_add);
            alu.OP1().pullFrom(reg);
            alu.OP2().pullFrom(const_2);
            reg.latchFrom(alu.OUT());
            Clock::tick();
        }
    }
    else{
        // todo
        if (am.writeback){
            sbus.IN().pullFrom(out);
            reg.latchFrom(sbus.OUT());
            Clock::tick();
        }
    }
}
