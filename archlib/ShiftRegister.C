// sccs id:  @(#)ShiftRegister.C	1.3        12/04/03

// shiftreg.C
//
// Jim Heliotis
//
// Version 2.1 of 12/13/93
//
// updated by:		Warren Carithers
//			November 2002

#include <cstdlib>
#include <iostream>

#include <StorageObject.h>
#include <ShiftRegister.h>

using namespace std;

ShiftRegister::ShiftRegister ( const char *id, int numBits, long initVal ) :
    StorageObject( id, numBits, initVal ),
    CPUObject( id, numBits ),
    change( none ),
    lefterly( 0 ),
    righterly( 0 ) {
}

ShiftRegister::~ShiftRegister() {
}

void ShiftRegister::perform( Operation op ) {
	change = op;
}

void ShiftRegister::rightShiftInputIs( StorageObject& obj ) {
	righterly = &obj;
}

void ShiftRegister::leftShiftInputIs( StorageObject& obj ) {
	lefterly = &obj;
}

void ShiftRegister::phase1() {

	if( lefterly && righterly ) {
		cout << "Shift register " << name()
		     << ":  you cannot specify both left and right inputs"
		      << endl;
		throw ArchLibError( "ShiftRegister both left and right inputs specified" );
	}

	if( incoming() && change ) {
		cout << name() << "Clearable op & bus read in same cycle; "
		     << "Clearable op will be performed." << endl;
		clear_incoming();
	}

	long oldValue = value();
	long newValue;

	switch( change ) {

		case right:
			newValue = oldValue >> 1;
				// in case operator>>(int,int) is arithmetic,
				// not logical:
			newValue &= get_mask() - (1L<<(get_bits()-1));
			if( righterly ) {
				newValue |= ((*righterly)(0,0)
						<< (get_bits()-1));
			}
			if( CPUObject::debug&CPUObject::trace ) {
				cout << name() << ": " << oldValue
				     << "---SHIFT-RIGHT-->" << newValue
				     << endl << flush;
			}
			value( newValue );
			break;

		case left:
			newValue = oldValue << 1;
			if( lefterly ) {
				newValue |= (*lefterly)();
			}
			if( CPUObject::debug&CPUObject::trace ) {
				cout << "  " << name() << ": " << oldValue
				     << "---SHIFT-LEFT-->"
				     << (newValue & get_mask())
				     << endl << flush;
			}
			value( newValue );
			break;

		case rightArith:
			if( righterly ) {
				cout << "Shift register " << name() << ":  "
				     << "arithmetic shift AND righterly input "
				     << "cannot both be specified" << endl;
				throw ArchLibError( "ShiftRegister arithmetic shift has righterly input specified" );
			}
			newValue = (oldValue >> 1) |
					(oldValue & (1L << (get_bits()-1)));
			if( CPUObject::debug&CPUObject::trace ) {
				cout << "  " << name() << ": " << oldValue
				     << "---SHIFT-A-RIGHT-->" << newValue
				     << endl << flush;
			}
			value(newValue);
			break;

		case none:
			break;

		default:
			cout << "Shift register " << name() << ":  "
			     << "bad operation " << change << endl;
	}

	lefterly = 0;
	righterly = 0;
	change = none;
	StorageObject::phase1();
}
