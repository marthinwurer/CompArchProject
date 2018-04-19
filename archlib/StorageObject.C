// sccs id:  @(#)StorageObject.C	1.7        3/10/12

// StorageObject.C
//
// Jim Heliotis
//
// Version 2.1a of 98/12/05
//	modified by wrc:  always print with full width, zero-filled
//
// based on
// Version 2.1 of 1/6/94
//
// updated by:		Warren Carithers
//			November 2002

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstring>

#include <StorageObject.h>
#include <InFlow.h>
#include <OutFlow.h>
#include <Clock.h>

using namespace std;

StorageObject::StorageObject ( const char *id, int numBits, long initVal ):
    ClockedObject( id, numBits ),
    CPUObject( id, numBits ),
    contents( initVal ),
    newContents( initVal ),
    update( 0 ),
    flows(),
    newValueSource( 0 ) {

	hello();
	set_contents( initVal & get_mask() );

	if( get_contents() != initVal ) {
		// switch to hex, but remember old settings
		// long k = cout.setf( ios::hex, ios::basefield );
		ios_base::fmtflags k = cout.setf( ios::hex, ios::basefield );
		cout << "StorageObject " << id
		     << ":  initial value of " << initVal
		     << " does not fit in " << numBits
		     << " bits;" << endl
		     << "        value truncated to "
		     << contents << endl;
		cout.setf( k, ios::basefield );		// reset cout flags
	}
}

StorageObject::~StorageObject() {
	if( goodbye() ) {
		if( backDoorUsed ) {
			cout << "The back door function was used!" << endl;
		}
	}
}

void StorageObject::enable( const InFlow *i ) {

	if( !flows.contains(i) ) {
		cout << "InFlow " << i->name()
		     << " is trying to pull from " << name()
		     << ".  They are not connected." << endl;
		throw ArchLibError( "StorageObject pull from unconnected component" );
	}
}

long StorageObject::value() const {
	return get_contents() & get_mask();
}

unsigned long StorageObject::uvalue() const {
	return (unsigned long)value();
}

void StorageObject::value( long x ) {
	newContents = x & get_mask();
	update = 1;
}

void StorageObject::uvalue( unsigned long x ) {
	value( long(x) );
}

void StorageObject::backDoor( long x ) {
	value( x );
	backDoorUsed = 1;
}

int StorageObject::storObjCount = 0;
int StorageObject::backDoorUsed = 0;

static void CheckClock( StorageObject &s, Flow &inOrOut ) {

	if( Clock::getTime() ) {
		cout << "Attempt to connect " << s.name() << " and "
		     << inOrOut.name()
		     << " after start of simulation!!!" << endl;
		cout << "All connections must be established"
		     << " before the first clock tick." << endl;
		throw ArchLibError( "StorageObject connection attempted after start of simulation" );
	}
}

void StorageObject::connectsTo( Flow& f ) {
	CheckClock( *this, f );
	flows.add( &f );
}

void StorageObject::latchFrom ( OutFlow &o ) {

	if( flows.contains( &o ) ) {
		newValueSource = &o;
	} else {
		cout << "StorageObject " << name() << " is "
		     << "trying to latch from something that is not connected."
		     << endl;
		throw ArchLibError( "StorageObject latch from non-connected component" );
	}
}

void StorageObject::phase1() {

	if( newValueSource ) {
		value( newValueSource->fetchValue() );
		if( CPUObject::debug&CPUObject::trace ) {
			cout << "-->" << name() << endl;
		}
	}
	newValueSource = 0;
}

void StorageObject::phase2() {

	if( update ) {
		set_contents( newContents );
		update = 0;
		newValueSource = 0;
	}
}

long StorageObject::operator()() const {
	return operator()( get_bits() - 1, get_bits() - 1 );
}

long StorageObject::operator()( int the_bit ) const {
	return operator()( the_bit, the_bit );
}

// does not seem to work if startBit==bits-1 and stopBit==0
long StorageObject::operator()( int startBit, int stopBit ) const {

	if( (startBit == get_bits()-1) && stopBit == 0 ) {
		return uvalue();
	} else if( (startBit < get_bits()) &&
		   (startBit >= stopBit) &&
		   (stopBit >= 0 ) ) {
		return  (uvalue() >> stopBit) &
			( (1 << (1+startBit-stopBit)) - 1 );
	} else {
		cout << name() << ".operator(): the bit range "
		     << '<' << startBit << ',' << stopBit << '>'
		     << " is not within " << (get_bits()-1)
		     << " to 0." << endl;
		throw ArchLibError( "StorageObject bit range error" );
	}
}

void StorageObject::printOn( ostream &o ) const {

	//
	// based on 2002/01/23 version by wrc
	//
	// prints full width, zero-filled
	//
	// figure out the correct "full width" for hex and oct
	// based on the output conversion setting
	//
	long k = o.flags();

	o << name() << '[';
	
	if( k & ios::hex ) {	// hex
		o << setw((get_bits()+3)/4) << setfill('0') << value() << ']';
	} else if( k & ios::oct ) {	// oct
		o << setw((get_bits()+2)/3) << setfill('0') << value() << ']';
	} else {			// default
		o << value() << ']';
	}
}
