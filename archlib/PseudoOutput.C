// sccs id:  @(#)PseudoOutput.C	1.3        12/04/03

// PseudoOutput.C
//
// Jim Heliotis
//	December 1993
//
// updated by:		Warren Carithers
//			November 2002

// A StorageObject that displays each new value it receives on stdout

#include <PseudoOutput.h>

using namespace std;

PseudoOutput::PseudoOutput( const char *id, int numBits ):
    StorageObject(id,numBits),
    CPUObject(id,numBits) {
}

PseudoOutput::~PseudoOutput() {
}

void PseudoOutput::phase2() {

	if( updating() ) {
		cout << endl << "Output from " << name() << ": "
		     << get_newContents() << endl;
	}
	StorageObject::phase2();
}

void PseudoOutput::printOn( ostream &o ) const {
	o << name() << "-output";
}
