// sccs id:  @(#)CPUObject.h	1.5        3/10/12

// CPUObject
// Jim Heliotis
// February 1994
//
// updated by:		Warren Carithers
//			November 2002

// The base class for all objects controlled by a clock
//
// CPUObject is an abstract base class that defines the behavior that
// all its subclasses share.  Its properties are few: number of bits,
// and a name.  Not quite all subclasses use the bit size.  The name
// is defined here so that diagnostic printing can be centrally defined.
//


#ifndef _CPUOBJECT_H_
#define _CPUOBJECT_H_

#include <iostream>

#include <ArchLibError.h>

using namespace std;

class Clock;

enum Operation {};	// gets around bug in cfront 2.1;
			// many subclasses define this.

class CPUObject {

	friend ostream& operator<< ( ostream& o, const CPUObject& c );
		// All CPUObjects can be printed.
		// Many subclasses redefine the way they are printed.
	friend class Memory;
		// Incredibly gross hack to allow Memory to change
		// the names of some of its components.
	friend class BusALU;
		// Suggested by Benjamin Mayes (bdm8233), needed
		// to allow BusALU to compile under gcc 4

private:	// to help prevent copying
	CPUObject( const CPUObject & );
	CPUObject & operator=( CPUObject & );

public:
	CPUObject ( const char *id, int numBits );
	// All CPUObjects have a name for debugging purposes.  It is expected
	// that the vast majority of CPUObjects will have a size measurable
	// in bits, so that is collected here as well.  Subclasses have a
	// more complicated state, indicated in their constructors.

	virtual ~CPUObject();

	const char *name() const;
	unsigned int size(); // how many bits (2nd ctor arg)

	enum DebugMode { create = 1, trace = 2, memload = 4, stats = 8,
			 trace_ticks = 16 };

	static int debug;

protected:
	int get_bits() const;
	unsigned long get_mask() const;
	void set_mask( unsigned long m );
	void set_name( const char *id );

private:
	char *myName;

	int bits;
	unsigned long mask; // mask representing above data (right-justified)

private:
	virtual void printOn( ostream &o ) const;
		// used by operator<< above; don't call directly,
		// except possibly from a subclass.
	static void birth();	// called when first CPUObject created
	static void death();	// called when last CPUObject destroyed
	static unsigned int objCount;

};

inline int CPUObject::get_bits() const {
	return bits;
}

inline unsigned long CPUObject::get_mask() const {
	return mask;
}

inline void CPUObject::set_mask( unsigned long m ) {
	mask = m;
}

#endif
