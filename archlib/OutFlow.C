// sccs id:  @(#)OutFlow.C	1.3        12/04/03

// OutFlow.C
//
// Jim Heliotis
//      December 1993
//
// updated by:		Warren Carithers
//			November 2002

#include <OutFlow.h>

using namespace std;

OutFlow::OutFlow( const char *id, int numbits, Connector &c ):
    Flow(id,numbits),
    connector(c) {
}

OutFlow::~OutFlow() {
}

long OutFlow::fetchValue() {
	return connector.computeValue();
}
