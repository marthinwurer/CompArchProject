// sccs id:  @(#)Connector.h	1.3        12/04/03

// Connector
// Jim Heliotis
// February 1994
// Represents all classes containing In & Out Flows
//
// updated by:		Warren Carithers
//			November 2002

//
// A Connector is something that provides the data flow from various
// sources to an OutFlow.  InFlows and OutFlows are always components
// of Connectors, so only Connectors are declared directly by client
// programs.  Abstract superclass.
//

#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include <CPUObject.h>

using namespace std;

class Connector: public virtual CPUObject {

public:
	Connector( const char *id, int numbits );
	~Connector();

	friend class OutFlow;

private:
	long virtual computeValue() = 0;
		// When a StorageObject asks its OutFlow for a new value
		// (see StorageObject::latchFrom), the OutFlow in turn
		// asks its Connector to compute that value.  Implemented
		// only in subclasses.
};

#endif
