// sccs id:  @(#)Constant.C	1.3        12/04/03

// Constant.C
//
// Jim Heliotis
//      December 1993
//
// updated by:		Warren Carithers
//			November 2002

#include <Constant.h>

using namespace std;

Constant::Constant( const char *id, int numbits, long value ):
    Connector(id,numbits),
    CPUObject(id,numbits),
    output(id,numbits,*this),
    v(value) {
}

Constant::~Constant() {
}

long Constant::computeValue() {
	if( CPUObject::debug&CPUObject::trace ) {
		cout << *this;
	}
	return v;
}

void Constant::printOn( ostream &o ) const {
	o << name() << "[<" << v << ">]";
}
