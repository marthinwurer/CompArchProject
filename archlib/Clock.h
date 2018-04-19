// sccs id:  @(#)Clock.h	1.3        12/04/03

// Clock
// Jim Heliotis
// February 1994
//
// updated by:		Warren Carithers
//			November 2002

// The clock that regulates actions of all StorageObjects
//
//    Do not declare any instances of Clock. All functions are static.
//    It controls the entire simulation, and holds all startup and
//    shutdown code (for example, the version number initial greeting,
//    and the number-of-cycles message at the end).
//
//    The clock is "wired" to all ClockedObject's (currently
//    StorageObjects and Memory.  It sends them all the phase1() and
//    phase2() messages when Clock::tick() is invoked.
//

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <CPUObject.h>
#include <ClockedObject.h>
#include <COSet.h>

using namespace std;

class Clock {

	friend class CPUObject;
	friend class ClockedObject;

public:
	static void tick();
		// The application should invoke this function when
		// everything has been set up for the next clock cycle.
		// Calling tick sends the phase1 and phase2 signals.

	static long getTime();

private:
	static void birth(); // sets up everything
	static void death(); // tears down everything; prints post-mortem
	static void announce( ClockedObject *obj );

	static int howMany; // should go from 0 to 1, then stay there
	static long time;
	static ClockedObjectSet *objList;

};

#endif
