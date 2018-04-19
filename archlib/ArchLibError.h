// sccs id:  @(#)ArchLibError.h	1.1        12/04/03

// ArchLibError.h
//
// Warren Carithers
//      December 2003
//

//
// An exception class for the library.  Thrown whenever a fatal
// runtime error is encountered.
//

#ifndef _ARCHLIBERROR_H_
#define _ARCHLIBERROR_H_

#include <stdexcept>

using namespace std;

class ArchLibError: public runtime_error {

public:
	ArchLibError( const char * what_arg );

};

#endif
