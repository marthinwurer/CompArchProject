// sccs id:  @(#)Clearable.h	1.3        12/04/03

// Clearable
// Jim Heliotis
// February 1994
//
// updated by:		Warren Carithers
//			November 2002

// Adds clr/set capability to the basic StorageObject
// (read StorageObject(3C++) first)

#ifndef _CLEARABLE_H_
#define _CLEARABLE_H_

#include <ArchLibError.h>
#include <StorageObject.h>

using namespace std;

class Clearable: public virtual StorageObject {

public:
	enum Operation { none=0, clearOp, setOp };
		// This list of operations is all that I can perform

	Clearable ( const char *id, int numBits, long value = 0 );
	~Clearable();

	void perform( Operation op );
		// choose an operation to do on the next clock
	void set();
		// perform(setOp) [shorthand]
	void clear();
		// perform(clearOp) [shorthand]

protected:
	void phase1();
	// void phase2();

private:
	Operation change;

};

inline void Clearable::set() { perform( setOp ); }

inline void Clearable::clear() { perform( clearOp ); }

#endif
