// sccs id:  @(#)OutFlow.h	1.3        12/04/03

// OutFlow
// Jim Heliotis
// February 1994
// The object through which a StorageObject receives a new value
//
// updated by:		Warren Carithers
//			November 2002

//
// An OutFlow is an object to which a StorageObject connects to
// get a new value on the next clock.  The StorageObjects will
// keep track of to which OutFlows they may be connected.  The
// other side of an OutFlow is a connector -- where it gets its
// input.  In the simplest case, the connector is a Bus, which
// contains both an InFlow and OutFlow.
//

#ifndef _OUTFLOW_H_
#define _OUTFLOW_H_

#include <Flow.h>
#include <Connector.h>

using namespace std;

class OutFlow: public Flow {

public:
	OutFlow( const char *id, int numbits, Connector &c );
	~OutFlow();

	virtual long fetchValue();
		// called internally from a StorageObject during phase1;
		// OutFlows get their values from their Connectors

private:
	Connector &connector;
};

#endif
