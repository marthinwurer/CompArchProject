// sccs id:  @(#)Counter.h	1.5        12/04/03

// Counter
// Jim Heliotis
// February 1994
//
// updated by:		Warren Carithers
//			November 2002

// Adds counting capability to the basic StorageObject
// (read storobj(3C++) first)

#ifndef _COUNTER_H_
#define _COUNTER_H_

#include <StorageObject.h>

using namespace std;

class Counter: public virtual StorageObject {

public:
	Counter(const char *id, int numBits, long value = 0 );
	~Counter();

	// This list of operations is all that the counter
	// can perform: increment or decrement by 1, 2, 4, or 8.
	enum Operation {
		clear0 = -999,
		decr8 = -8, decr4 = -4, decr2 = -2, decr1 = -1,
		none = 0,
		incr1 = 1, incr2 = 2, incr4 = 4, incr8 = 8
	};

	void perform( Operation op );	// operation to do on the next clock

	void incr();			// perform(incr1) [shorthand]
	void decr();			// perform(decr1) [shorthand]
	void clear();			// perform(clear0) [shorthand]

	int overflow(); // predicate to see if incr or decr caused "rollover"
		   // (FFF..FF <--> 000..00); use after executing clock pulse

protected:
	void phase1();
	void phase2();

private:
	Operation change;
	int oflow;
	int newOflow;
};

inline void Counter::incr() { perform( incr1 ); }

inline void Counter::decr() { perform( decr1 ); }

inline void Counter::clear() { perform( clear0 ); }

#endif
