// sccs id:  @(#)ClockedObject.h	1.3        12/04/03

// ClockedObject
// Jim Heliotis
// February 1994
//
// updated by:		Warren Carithers
//			November 2002

//
// The ClockedObject is a base class for all components of the simulated
// CPU that are to be connected to the system clock.
//
// Subclasses should conform to the following clocking operation:
//
// phase1:
// object pulls in any data from the outside that it may need.
//
// phase2:
// object updates its own state
//
// The major subclass of ClockedObject is StorageObject, a register.
// There is also Memory.
//

#ifndef _CLOCKEDOBJECT_H_
#define _CLOCKEDOBJECT_H_

#include <CPUObject.h>

using namespace std;

class Clock;

class ClockedObject: public virtual CPUObject {

public:
	ClockedObject ( const char *id, int numBits);
		// create a numBits-wide register named id
	~ClockedObject();

protected:
	friend class Clock;
	virtual void phase1() = 0;
	virtual void phase2() = 0;

};

#endif
