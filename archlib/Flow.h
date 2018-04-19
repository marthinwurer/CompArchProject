// sccs id:  @(#)Flow.h	1.3        12/04/03

// Flow.h
//
// Jim Heliotis
//      February 1994
//
// updated by:		Warren Carithers
//			November 2002

//
// A common superclass for InFlow and OutFlow.  This is only useful
// because it avoids some otherwise redundant code, e.g. in
// StorageObject.  Not a class to be used by client simulations.
//

#ifndef _FLOW_H_
#define _FLOW_H_

#include <CPUObject.h>
#include <Connector.h>

using namespace std;

class Flow: public CPUObject {

public:
	Flow( const char *id, int numbits );
	~Flow();

};

#endif
