// sccs id:  @(#)Clock.C	1.5        3/10/12

// Clock.C
//
// Jim Heliotis
//
//	1/2/95
//
// updated by:		Warren Carithers
//			November 2002

#include <cstdlib>
#include <iostream>
#include <iomanip>

#include <Clock.h>
#include <CPUObject.h>
#include <COSet.h>
#include <ClockedObject.h>

using namespace std;

int Clock::howMany = 0;

ClockedObjectSet *Clock::objList = 0;

long Clock::time = 0;

void Clock::birth() {
	 objList = new ClockedObjectSet;
}

void Clock::death() {

	 cout << endl;

//	 if( CPUObject::debug & CPUObject::stats ) {
	 	cout << "Simulated time " << dec << time << " cycle";
	 	cout << ((time==1)?"\n":"s\n");
//	 }

	 delete objList;
}

void Clock::announce( ClockedObject *obj ) {
	 objList->add(obj);
}

void Clock::tick() {
	ClockedObject *obj;
	bool trace1, trace2;

	trace1 = CPUObject::debug & CPUObject::trace;
	trace2 = CPUObject::debug & CPUObject::trace_ticks;

	if( trace1 ) {
		 // long old = cout.setf( ios::dec, ios::basefield );
		 ios_base::fmtflags old = cout.setf( ios::dec, ios::basefield );

		 cout << "      ________\n_____/" << setw(7) << time
			<< " \\_____\n" << flush;
		 (void)cout.flags( old );
	}

	for( obj = objList->first(); obj; obj = objList->next() ) {
		if( trace2 ) {
			cout << "tick phase 1 for " << obj->name() << flush;
		}
		obj->phase1();
		if( trace2 ) {
			cout << " done" << endl;
		}
	}

	for( obj = objList->first(); obj; obj = objList->next() ) {
		if( trace2 ) {
			cout << "tick phase 2 for " << obj->name() << flush;
		}
		obj->phase2();
		if( trace2 ) {
			cout << " done" << endl;
		}
	}

	time++;
}

long Clock::getTime() {
	return time;
}
