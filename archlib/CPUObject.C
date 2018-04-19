// sccs id:  @(#)CPUObject.C	1.6        12/04/03

// CPUObject.C
//
// Jim Heliotis
//
//	12/8/93
//
// updated by:		Warren Carithers
//			November 2002

#include <cstdlib>
#include <iostream>
#include <cstring>

#include <CPUObject.h>
#include <Clock.h>
#include <Version.h>

using namespace std;

#ifndef __DATE__
#define __DATE__ "2003/12/04"
#endif

static void CheckLateCreate( CPUObject &o ) {

	if( Clock::getTime() ) {
		cout << "Attempt to create CPUObject " << o.name()
                     << " after start of simulation!!!" << endl;
                cout << "All CPUObjects must be created"
                     << " before the first clock tick." << endl;
		throw ArchLibError( "CPUObject late creation" );
	}
}

unsigned int CPUObject::objCount = 0;

void CPUObject::birth() {

	cout << "CPU \"ARCH\" Simulator, " << VERSION << "("
		<< __DATE__ << ")" << endl;
	cout << "-----------------------------------------" << endl;
	cout << endl;

}

void CPUObject::death() {
	cout << endl;
	cout << "LAST CPUObject DESTROYED; END OF SIMULATION" << endl;
}

CPUObject::CPUObject ( const char *id, int numBits ):
    bits(numBits),
    myName(new char[strlen(id)+1]) {

	if( (objCount++) == 0 ) {
		birth();
		Clock::birth();
	}

	strcpy(myName,id);
	if( CPUObject::debug&CPUObject::create ) {
		cout << "Object " << myName << " created\n";
	}

	if( (numBits<1) || (numBits > (8*sizeof(long))) ) {
		cout << id << ":  " << numBits << " bits is out of range\n";
		throw ArchLibError( "CPUObject size out of range" );
	}

	int b = numBits;
	for( mask = 1; b > 1; b-- ) {
		mask = (mask<<1) | mask;
	}
	CheckLateCreate( *this );
}

CPUObject::~CPUObject() {

	if( CPUObject::debug&CPUObject::create ) {
		cout << "Object " << myName << " destroyed\n";
	}

	delete[] myName;

	if( (--objCount) == 0 ) {
		Clock::death();
		death();
	}
}

CPUObject::CPUObject( const CPUObject &foo ) :
    bits( foo.bits ),
    myName( new char[strlen(foo.myName)+5] ) {

	cout << "Attempting to copy " << foo << " !!!" << endl;
	throw ArchLibError( "CPUObject copy constructor used" );

}

// CPUObject &operator=( CPUObject &foo )
// {
// 	cout << "Attempting to assign " << foo.myName << " to " << myName
// 	     << " !!!" << endl;
// 	throw ArchLibError( "CPUObject assignment attempted" );
// 	return 0;
// }

const char *CPUObject::name() const {
	return myName;
}

void CPUObject::set_name( const char *id ) {
	if( myName != 0 ) {
		delete [] myName;
	}
	myName = new char[ strlen(id) + 1 ];
	strcpy( myName, id );
}

unsigned int CPUObject::size() {
	return bits;
}

ostream& operator<< ( ostream& o, const CPUObject& c ) {
	c.printOn(o);
	return o;
}

void CPUObject::printOn ( ostream& o ) const {
	o << myName;
}

int CPUObject::debug = CPUObject::stats;
