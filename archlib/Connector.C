// sccs id:  @(#)Connector.C	1.3        12/04/03

// Connector.C
//
// Jim Heliotis
//	December 1993
//
// updated by:		Warren Carithers
//			November 2002

#include <Connector.h>

using namespace std;

Connector::Connector( const char *id, int numbits ):
    CPUObject(id,numbits) {
}

Connector::~Connector() {
}
