// sccs id:  @(#)Counter.C	1.4        12/04/03

// counter.C
//
// Jim Heliotis
//
// Version 2.0 of 12/12/93
//
// updated by:		Warren Carithers
//			November 2002

#include <iostream>

#include <StorageObject.h>
#include <Counter.h>

using namespace std;

Counter::Counter ( const char *id, int numBits, long value ):
    StorageObject(id,numBits,value),
    CPUObject(id,numBits),
    change(none),
    oflow(0),
    newOflow(0) {
}

Counter::~Counter() {
}

void Counter::perform( Operation op ) {
	change = op;
}

void Counter::phase1() {

	long adj = long(change);
	unsigned long newVal;

	// handle clear operations differently
	if( change == Counter::clear0 ) {
		newVal = 0;	// do the operation
		newOflow = 0;	// can't overflow
	} else {
		newVal = ( uvalue() + adj ) & get_mask();
		if( adj < 0 ) {
			newOflow = (uvalue() < -adj);
		} else if( adj > 0 ) {
			newOflow = (newVal < adj);
		} else {
			newOflow = 0;
		}
	}

	if( adj ) {
		if( incoming() ) {
			cout << name()
			     << "Counter op & bus read in same cycle; "
			     << "Counter op will be favored." << endl;
			clear_incoming();
		}

		uvalue( newVal );

		if( CPUObject::debug&CPUObject::trace ) {
			cout << name()
			     << ( (adj>0) ? " incremented to ":
					( (adj==clear0) ? " cleared to " :
					  " decremented to " ) )
			     << (newVal & get_mask() )
			     << ( newOflow ? " (overflow)" : "" ) << endl;
		}
	}

	StorageObject::phase1();
	change = none;
}

void Counter::phase2() {
	if( updating() ) {
		oflow = newOflow;
	}
	StorageObject::phase2();
}

int Counter::overflow() {
	return oflow;
}
