// sccs id:  @(#)PseudoInput.h	1.3        12/04/03

// PseudoInput
// Jim Heliotis
// February 1994
//
// updated by:		Warren Carithers
//			November 2002

// Simple, unrealistic input device
//
// A StorageObject that gets its value from stdin every time it is
// an InFlow for its value.
//

#ifndef _PSEUDOINPUT_H_
#define _PSEUDOINPUT_H_

#include <cstdlib>
#include <iostream>

#include <ArchLibError.h>
#include <StorageObject.h>

using namespace std;

class PseudoInput: public StorageObject {

public:
	PseudoInput( const char *id, int numBits );
	~PseudoInput();

protected:
	void value( long x );	// not legal
	long value() const;	// still legal
	void enable( const InFlow *i );	// this is where input is done

private:
	void printOn( ostream &o ) const;
	long latestTime;
};

#endif
