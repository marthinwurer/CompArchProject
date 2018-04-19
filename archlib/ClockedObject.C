// sccs id:  @(#)ClockedObject.C	1.3        12/04/03

// ClockedObject.C
//
// Jim Heliotis
//	December 1993
//
// updated by:		Warren Carithers
//			November 2002


#include <Clock.h>
#include <ClockedObject.h>

using namespace std;

ClockedObject::ClockedObject ( const char *id, int numBits):
    CPUObject(id,numBits) {
	Clock::announce(this);
}

ClockedObject::~ClockedObject() {
}
