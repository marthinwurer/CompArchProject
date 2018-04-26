//
// Created by benjamin on 4/18/18.
//
#include <cstdlib>
#include <iostream>
#include <Clock.h>
#include <bitset>


#include "globals.h"
#include "addressing.h"
#include "setup.h"
#include "decode.h"

int main( int argc, char * argv[]) {

//    CPUObject::debug |= CPUObject::trace | CPUObject::memload;
    cout << hex;

    // get command line input
    // taken from dumbest
    if (argc != 2) {
        cerr << "Usage:  " << argv[0] << " object-file-name\n\n";
        exit(1);
    }

    im.load(argv[1]);
    dm.load(argv[1]);


    setup();
    // entry point hack
    pc.latchFrom(im.READ());
    Clock::tick();

    try {


        // main loop
        while (!halt) {

            // Test for stall first

            // test for

            // nops
            // break
            /*
             * load
             * alu
             *
             */

        }
    }catch( ArchLibError & e){
        print_trace();
        cout << endl << "Machine Halted - " << e.what() << endl;
    }

    // teardown

    // free all the registers
    for( int ii = 0; ii < NUM_REGS; ii++){
        delete regs[ii];
    }

    return 0;

}


