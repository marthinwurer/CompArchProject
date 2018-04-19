// sccs id:  @(#)Flow.C	1.3        12/04/03

// Flow.C
//
// Jim Heliotis
//      January 1994
//
// updated by:		Warren Carithers
//			November 2002

#include <Flow.h>

using namespace std;

Flow::Flow( const char *id, int numbits ):
    CPUObject(id,numbits) {
}

Flow::~Flow() {
}
