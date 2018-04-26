//
// Created by benjamin on 3/24/18.
//

#include <sstream>
#include "setup.h"
#include "globals.h"


void setup(){
    for( int ii = 0; ii < NUM_REGS; ii++){
        stringstream ss;
        ss << "R" << ii;
        regs.push_back(new Clearable(ss.str().c_str(), DATA_BITS));
        regs[ii]->connectsTo(ifid.abus.IN());
        regs[ii]->connectsTo(ifid.bbus.IN());
        regs[ii]->connectsTo(writeback.OUT());
    }



    // setup
    // memory
    im.MAR().connectsTo(pcbus.OUT());
    ifid.ir.connectsTo(im.READ());
    pc.connectsTo(im.READ());

    dm.MAR().connectsTo(exmem.outbus.OUT());
    exmem.B.connectsTo(dm.WRITE());
    memwb.out.connectsTo(dm.READ());
    memwb.out.connectsTo(exmem.outbus.OUT());

//    mdr.connectsTo(m.READ());
//    mdr.connectsTo(m.WRITE());
//    mdr.connectsTo(dbus.IN());
//    mdr.connectsTo(dbus.OUT());
//    mdr.connectsTo(alu.OP2());


    // accumulators

    // connections required for alu logic
    idex.A.connectsTo(alu.OP1());
    idex.B.connectsTo(alu.OP2());
    se_mask_12.connectsTo(alu.OP2());




}
