// sccs id:  @(#)PseudoInput.C	1.4        12/04/03

// PseudoInput.C
//
// Jim Heliotis
//	December 1993
//
// updated by:		Warren Carithers
//			November 2002

// A StorageObject that fets its value from stdin every time it is
// asked for its value.

#include <PseudoInput.h>
#include <Clock.h>

using namespace std;

PseudoInput::PseudoInput( const char *id, int numBits ):
    StorageObject(id,numBits),
    CPUObject(id,numBits),
    latestTime(-1) {
}

PseudoInput::~PseudoInput() {
}

void PseudoInput::value( long x ) {
	cout << "PseudoInput " << name() << ": attempt to assign value ("
	     << x << ')' << endl;
	throw ArchLibError( "PseudoInput assignment attempt" );
}

long PseudoInput::value() const {
	return StorageObject::value();
}

void PseudoInput::enable( const InFlow *i ) {

	StorageObject::enable( i );
	unsigned long read_value = 0;
	const int MaxTries = 5;
	int tries;

	for( tries = 0; tries < MaxTries; tries++ ) {
		cout << "  Provide value for PseudoInput " << name()
		     << ": " << flush;
		cin >> hex >> read_value;
		if( cin.eof() ) {
			cerr << "EOF on standard in?!?!" << endl;
			throw ArchLibError( "PseudoInput stdin EOF" );
		}
		if( cin.fail() ) {
			cin.clear();
			cout << "Entered data does not compform to "
			     << "hexadecimal number format. "
			     << " Please try again.";
			cin.ignore ( 80, '\n' );
			continue;
		}
		long negative = ~(get_mask() >> 1);
		long upper = read_value & negative;
		if( (upper != negative) && (read_value & ~get_mask() )) {
			cout << "Out-of-range value. " << " Please try again.";
			cin.ignore ( 80, '\n' );
			continue;
		}
		break;
	}
	if( tries == 5 ) {
		cout << "It appears there is a serious input problem."
		     << "  Good-bye." << endl;
		throw ArchLibError( "PseudoInput fatal input error" );
	}
	set_contents( read_value );
}


void PseudoInput::printOn( ostream &o ) const {
	o << name() << "-input";
}
