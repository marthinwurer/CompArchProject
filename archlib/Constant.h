// sccs id:  @(#)Constant.h	1.3        12/04/03

// Constant
// Jim Heliotis
// February 1994
// A "hardwired" bus that always provides the same value
//
// updated by:		Warren Carithers
//			November 2002

//
// A Constant is a Connector that simply provides the same value
// to its OutFlow every time.  It is not connected to any InFlow.
//

#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#include <iostream>

#include <Connector.h>
#include <OutFlow.h>

using namespace std;

class Constant: public Connector {

public:
	Constant( const char *id, int numbits, long value );
	~Constant();

	long computeValue();
	void printOn( ostream &o ) const;
	OutFlow &OUT() { return output; }

private:
	const long v;
	OutFlow output;
};

#endif
