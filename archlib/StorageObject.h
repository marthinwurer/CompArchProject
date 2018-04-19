// sccs id:  @(#)StorageObject.h	1.3        12/04/03

// StorageObject
// Jim Heliotis
// January 1994
//
// updated by:		Warren Carithers
//			November 2002

//
// The StorageObject is a base class for all components of the simulated
// CPU capable of receiving and holding binary data.  It contains only
// some elementary operations so that it can interact with other
// components like data paths and the system clock.
//
// Classes deriving off of StorageObject should probably make this base
// class virtual to allow combining of multiple subclasses by the user to
// create new object types.
//
// Subclasses should also conform to the following clocking operation:
//
// phase1:
// object computes its new value and calls value(int) to store it.  The
// value that would be returned by value() is not yet changed.
//
// phase2:
// base class StorageObject updates real value
//
// One example of the utility of this 2-phase approach is daisy-chained
// shift registers.
//

#ifndef _STORAGEOBJECT_H_
#define _STORAGEOBJECT_H_

#include <iostream>

#include <ArchLibError.h>
#include <ClockedObject.h>
#include <FlowSet.h>

using namespace std;

class Clock;
class InFlow;
class OutFlow;

class StorageObject: public ClockedObject {

public:
	StorageObject ( const char *id, int numBits, long initVal = 0 );
	~StorageObject();

	virtual void enable( const InFlow *i );
		// This function gets called by an InFlow to announce the
		// enabling of the data path to it.  A valid connection
		// check is done.  PseudoInput uses this to do the input.
	virtual long value() const;
		// read current value
	unsigned long uvalue() const;
		// read current value as unsigned number
	int zero() const { return uvalue() == 0; }
		// short-cut predicate; tests value, does not change it.
	void detailed_dump() const;
		// print out entire state of this object

	void connectsTo( Flow &f );
		// Establish bus-register connection
		// (before start of simulation).
	void latchFrom( OutFlow &o );
		// Establish that the source of data _this_clock_cycle_
		// is the given "bus".

	long operator()( int startBit, int stopBit ) const;
		// Extract "substring".  msb is numBits-1; lsb is 0.
	long operator()( int bit ) const;
		// returns specified bit
	long operator()() const;
		// returns most significant bit

	void backDoor( long x );
		// "undocumented" function!
		// Sets reg. value without following normal data flow protocol.
		// Use only when you give up!

protected:
	void set_contents( unsigned long c );
	unsigned long get_contents() const;
	unsigned long get_newContents() const;

	void hello();	// increments item count
	int goodbye();	// decrements item count

	virtual void value( long x ); // set new value on next clock pulse
	void uvalue( unsigned long x ); // set new value on next clock pulse

	void printOn( ostream& o ) const; // used by operator<<

	friend class Clock;
	virtual void phase1();
		// compute the next value (within self or pulled from outside)
	virtual void phase2();
		// If value(int) was called, latch new value.
		// This should NOT be redefined in subclasses,
		// only augmented.
	
	int incoming();
		// am I hooked up to an OutFlow for a value this cycle?

	void clear_incoming();
		// disconnect me from the OutFlow early

	int updating();
		// is anyone updating my value?

private:
	OutFlow *newValueSource;	// ...if from the outside
	unsigned long contents;		// current visible value
	int update;
	unsigned long newContents;	// new value, not yet visible
					// (pre - phase 2)

	static int storObjCount;
	static int backDoorUsed;

	FlowSet flows; // to what am I permanently connected?
};

inline void StorageObject::hello() {
	storObjCount += 1;
}

inline int StorageObject::goodbye() {
	storObjCount -= 1;
	return storObjCount == 0;
}

inline int StorageObject::incoming() {
	return newValueSource != 0;
}

inline void StorageObject::clear_incoming() {
	newValueSource = 0;
}

inline int StorageObject::updating() {
	return update;
}

inline void StorageObject::set_contents( unsigned long c ) {
	contents = c;
}

inline unsigned long StorageObject::get_contents() const {
	return contents;
}

inline unsigned long StorageObject::get_newContents() const {
	return newContents;
}

inline void StorageObject::detailed_dump() const {
	cout << endl << name() << ':' << endl;
	cout << "   bits=" << get_bits() << endl;
	cout << "   mask=" << get_mask() << endl;
	cout << "   newValueSource=" << newValueSource << endl;
	cout << "   contents=" << contents << endl;
	cout << "   update=" << update << endl;
	cout << "   newContents=" << newContents << endl << endl;
}

#endif
