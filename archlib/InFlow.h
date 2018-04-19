// sccs id:  @(#)InFlow.h	1.3        12/04/03

// InFlow
// Jim Heliotis
// February 1994
// Pulls values from StorageObjects
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

#ifndef _INFLOW_H_
#define _INFLOW_H_

#include <ArchLibError.h>
#include <Flow.h>
#include <StorageObject.h>

using namespace std;

class InFlow: public Flow {

public:
	InFlow( const char *id, int numbits );
	~InFlow();

	void pullFrom( StorageObject &so );
		// Define from where I get my value during the next
		// clock cycle.

	virtual long fetchValue() const;
		// Called by the Connector that contains me during
		// execution of a clock cycle (phase1, computeValue)

	virtual void printSourceInfo() const;
		// For tracing only; do not call.

private:
	const StorageObject *source;

};

#endif
