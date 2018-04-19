// sccs id:  @(#)Bus.C	1.3        12/04/03

// Bus.C
//
// original author:	Jim Heliotis
//			December 1993
//
// updated by:		Warren Carithers
//			November 2002

#include <Bus.h>

using namespace std;

Bus::Bus( const char *id, int numbits ):
    Connector(id,numbits),
    CPUObject(id,numbits), 
    input(id,numbits),
    output(id,numbits,*this) {
}

Bus::~Bus() {
}

long Bus::computeValue() {

	long val = input.fetchValue() & get_mask();

	if( CPUObject::debug&CPUObject::trace ) {

		input.printSourceInfo();
		cout << "-->" << val;

	}

	return val;
}
