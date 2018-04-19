// sccs id:  @(#)PseudoOutput.h	1.3        12/04/03

// PseudoOutput
// Jim Heliotis
// February 1994
//
// updated by:		Warren Carithers
//			November 2002

// Simple, unrealistic input device
//
// A StorageObject that prints, on stdout, every new value it
// gets from an OutFlow.
//


#ifndef _PSEUDOOUTPUT_H_
#define _PSEUDOOUTPUT_H_

#include <StorageObject.h>

using namespace std;

class PseudoOutput: public StorageObject {

public:
	PseudoOutput( const char *id, int numBits );
	~PseudoOutput();

protected:
	void phase2();

private:
	void printOn( ostream &o ) const;
};

#endif
