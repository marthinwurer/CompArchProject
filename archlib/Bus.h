// sccs id:  @(#)Bus.h	1.3        12/04/03

// Bus.h
//
// original author:	Jim Heliotis
//			February 1994
//
// updated by:		Warren R. Carithers
//			November 2002
//
// Simple StorageObject interconnector
//
// A Bus is the standard Connector, containing one InFlow and one
// OutFlow.  It just passes data straight through.
//
  
#ifndef _BUS_H_
#define _BUS_H_

#include <Connector.h>
#include <InFlow.h>
#include <OutFlow.h>

using namespace std;

class Bus: public Connector {

public:
	Bus( const char *id, int numbits );
	~Bus();

	long computeValue();

	InFlow &IN() { return input; }
	OutFlow &OUT() { return output; }

private:
	InFlow input;
	OutFlow output;

};

#endif
