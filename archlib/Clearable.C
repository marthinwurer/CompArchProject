// sccs id:  @(#)Clearable.C	1.3        12/04/03

// Clearable.C
//
// Jim Heliotis
//
// Version 2.0 of 12/12/93
//
// updated by:		Warren Carithers
//			November 2002

#include <cstdlib>
#include <iostream>

#include <StorageObject.h>
#include <Clearable.h>

using namespace std;

Clearable::Clearable ( const char *id, int numBits, long value ):
    StorageObject(id,numBits,value),
    CPUObject(id,numBits),
    change(none) {
}

Clearable::~Clearable() {
}

void Clearable::perform( Operation op ) {
	change = op;
}

void Clearable::phase1() {

	if( change ) {

		if( incoming() ) {
			cout << name()
			     << "Clearable op & bus read in same cycle; "
			     << "Clearable op will be performed." << endl;
			clear_incoming();
		}

		switch (change) {
			case none:
				break;

			case clearOp:
				uvalue(0);
				if( CPUObject::debug&CPUObject::trace ) {
					cout << name() << " cleared\n"
					     << flush;
				}
				break;

			case setOp:
				uvalue(0xFFFFFFFF);
				if( CPUObject::debug&CPUObject::trace ) {
					cout << name() << " set\n" << flush;
				}
				break;

			default:
				cout << name()
				     << ": arch logic fault: illegal"
				     << " Clearable Op" << endl;
				throw ArchLibError( "Clearable - bad op" );
		}
	}

	StorageObject::phase1();

	change = none;

}
