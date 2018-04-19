// sccs id:  @(#)InFlow.C	1.3        12/04/03

// InFlow.C
//
// Jim Heliotis
//	December 1993
//
// updated by:		Warren Carithers
//			November 2002

//
// An InFlow is an object to which a StorageObject connects to
// send its value to other StorageObjects.  Only one such
// object may be connected during any clock cycle.  The
// StorageObjects will keep track of to which InFlows they
// are connected.
//

#include <cstdlib>
#include <iostream>

#include <InFlow.h>

using namespace std;

InFlow::InFlow( const char *id, int numbits ):
    Flow(id,numbits) {
}

InFlow::~InFlow() {
}

void InFlow::pullFrom( StorageObject &so ) {
	source = &so;
	so.enable( this );
}

void InFlow::printSourceInfo() const {

	if( source ) {
		cout << *source << "-->" << name();
	} else {
		cout << "???";
	}
}

long InFlow::fetchValue() const {

	if( source ) {
		return source->value();
	} else {
		cout << "InFlow " << name()
		     << " is asked to fetchValue without" << endl;
		cout << "   having been told to pull from anything." << endl;
		throw ArchLibError( "InFlow input not enabled" );
	}
}
