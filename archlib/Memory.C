// sccs id:  @(#)Memory.C	1.6        12/04/03

// memory.C
//
// Jim Heliotis
//
//	January 1994
//
// updated by:		Warren Carithers
//			November 2002

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>

#include <Memory.h>

using namespace std;

LongArray::LongArray():
    data(0),
    size(0) {
}

LongArray::~LongArray() {
	if( data ) {
		delete[] data;
	}
}

void LongArray::allocate( long sz ) {

	data = new long[ size = sz ];
	if( !data ) {
		cout << "Could not allocate memory data array" << endl;
		throw ArchLibError( "Memory::LongArray cannot allocate data array" );
	}
}

long LongArray::get( long index ) {

	if( index >= size ) {
		cout << "Illegal address " << index
		     << " for memory data array" << endl;
		throw ArchLibError( "Memory::LongArray illegal address used for get" );
	}
	return data[index];
}

void LongArray::put( long index, long value ) {

	if( index >= size ) {
		cout << "Illegal address " << index
		     << " for memory data array" << endl;
		throw ArchLibError( "Memory::LongArray illegal address used for put" );
	}
	data[index] = value;
}

const unsigned long MaxMemSize = 0x100000;

Memory::Memory ( const char *id,
		 int bitsInAddr,
		 int bitsPerUnit,
		 unsigned long maxAddr,
			// maxAddr==-1 => all addresses valid
		 int unitsInDataPath,
			// set if size(MBR) != bitsPerUnit
		 bool littleEndian
			// set if LSB is first (e.g., PDP-11)
               ):
    Connector( id, unitsInDataPath*bitsPerUnit ),
    ClockedObject( id, unitsInDataPath*bitsPerUnit ),
    CPUObject( id, unitsInDataPath*bitsPerUnit ),
    mar( "MAR", bitsInAddr ),
    writeFlow( "MemoryWrite", unitsInDataPath*bitsPerUnit ),
    readFlow( "MemoryRead", unitsInDataPath*bitsPerUnit, *this ),
    unitSize( bitsPerUnit ),
    dataPathWidth( unitsInDataPath ),
    op( none ),
    mem(), // size set below
    addressFieldWidth( (bitsInAddr-1)/4 + 1 ),
    dataFieldWidth( (bitsPerUnit-1)/4 + 1 ),
    byteSwap( littleEndian ) {

	char *buf;
	int b = bitsPerUnit;

	// first, fix names of our important components

	buf = new char[ strlen(id) + 13 ];       // id + ".MemoryWrite" + 1

	strcpy( buf, id ); strcat( buf, ".MAR" );
	mar.set_name( buf );

	strcpy( buf, id ); strcat( buf, ".MemoryWrite" );
	writeFlow.set_name( buf );

	strcpy( buf, id ); strcat( buf, ".MemoryRead" );
	readFlow.set_name( buf );

	delete [] buf;

	// next, set up internal masks, etc.

	for( unit_mask = 1; b > 1; b-- ) {
		unit_mask = (unit_mask<<1) | unit_mask;
	}

	b = bitsInAddr;
	for( address_mask = 1; b > 1; b-- ) {
		address_mask = (address_mask<<1) | address_mask;
	}

	if( maxAddr == (unsigned long)(-1) ) {
		highPoint = address_mask;
	} else {
		highPoint = maxAddr;
	}

	if( highPoint > (MaxMemSize-1) ) {
		cout << id << ":  " << (highPoint+1)
		     << "-unit memory is too large" << endl;
		throw ArchLibError( "Memory size too large" );
	}

	if ((highPoint & address_mask) < highPoint) {
		cout << hex << "Memory " << id << ":  " << maxAddr
		     << " maximum address size is too large" << endl;
		throw ArchLibError( "Memory maximum address size too large" );
	}

	// allocate the actual memory space

	mem.allocate(highPoint+1);

	for( int i = 0; i < mem.size; i++ ) {
		mem.put( i, 0 );
	}

	if( CPUObject::debug&CPUObject::create ) {
		cout << "  " << name() << " is " << mem.size
		     << " units by " << unitSize << " bits" << endl;
	}
}

Memory::~Memory() {
}

void Memory::perform( Operation o ) {
	op = o;
}

void Memory::phase1() {

	switch( op ) {

		case readOp:
			break;

		case writeOp:
			currentAddr = mar.uvalue();
			newValue = writeFlow.fetchValue();
			if( CPUObject::debug&CPUObject::trace ) {
				cout << newValue << "-->";
				cout << name() << '@' << currentAddr << endl;
			}
			break;

		default:
			rangeError = 0;

	}
}

long Memory::computeValue() {

	long tempStore = 0;
	const unsigned long actualAddr = mar.uvalue();
	unsigned long lastAddr = actualAddr+dataPathWidth-1;
	unsigned long a = 0;
	int n;

	switch( op ) {

		case loadOp:
			tempStore = newValue;
			if( CPUObject::debug&CPUObject::trace ) {
				cout << tempStore << '(' << name() << ')';
			}
			break;

		case readOp:
			if( rangeError = (highPoint < lastAddr) ) {
				return 0;
			}
			tempStore = 0;
			for( n = 0, a = actualAddr; a <= lastAddr; a++ ) {
				if( byteSwap ) {
					tempStore |=
					    mem.get(a) << (unitSize * n++);
				} else {
					tempStore =
					    (tempStore << unitSize) |
					    mem.get( a );
				}
			}
			if( CPUObject::debug&CPUObject::trace ) {
				cout << name() << '@' << actualAddr
				     << "-->" << tempStore;
			}
			break;

		default:
			cout << "Someone is trying to pull a value from "
			     << name()
			     << ", and no read or load is being performed."
			     << endl;
	}

	return tempStore;

}

void Memory::phase2() {

	 unsigned long lastAddr = 0;
	 unsigned long a = 0;
	 long tempStore = 0;

	 switch( op ) {

		case readOp:
			break;

		case writeOp:	// note that LSB is in highest address
				// by default; if byteSwap, LSB is in
				// lowest address
			tempStore = newValue;
			lastAddr = currentAddr+dataPathWidth-1;
			if( rangeError = (highPoint < lastAddr) ) {
				return;
			}

			if( byteSwap ) {
				for( a = currentAddr; a <= lastAddr; a++ ) {
					mem.put( a, tempStore & unit_mask );
					if( a == 0 ) {
						break;
					}
					tempStore >>= unitSize;
				}
			} else {
				for( a = lastAddr; a >= currentAddr; a-- ) {
					mem.put( a, tempStore & unit_mask );
					if( a == 0 ) {
						break;
					}
					tempStore >>= unitSize;
				}
			}
			break;

		default:
			rangeError = 0;

	}

	op = none;

}

void Memory::load( const char *fileName, long defaultValue ) {

	ifstream objFile(fileName);

	if( !objFile ) {
		cout << "Could not open " << fileName << endl;
		throw ArchLibError( "Memory can't open object file" );
	}

	if( (defaultValue & get_mask()) != defaultValue ) {
		cout << "Memory load of " << name() << ":  ";
		cout << hex << "default value of " << defaultValue;
		cout << " is too large" << endl;
		throw ArchLibError( "Memory default value too large" );
	}

	for( unsigned long m = 0; m <= highPoint; m++ ) {
		mem.put( m, defaultValue );
	}

	unsigned long addr;
	unsigned int numUnits;

	objFile >> hex;

	while( 1 ) {

		objFile >> addr;
		objFile >> numUnits;

		if( objFile.eof() ) {
			break;
		}

		for( unsigned int i = 0; i < numUnits; i++ ) {
			long unitVal;

			objFile >> unitVal;

			if( addr > highPoint ) {
				cout << name() << ": addr out of range"
				     << endl;
				throw ArchLibError( "Memory address too large during load" );
			}
			mem.put( addr, (unitVal &= unit_mask) );
			if( CPUObject::debug&CPUObject::memload ) {
				cout << "  m[" << addr << "] = "
				     << unitVal << endl;
			}
			addr++;
		}
	}

	if( (unitSize*dataPathWidth) < mar.size() ) {
		cout << "WARNING: starting address will be truncated to "
		     << (unitSize*dataPathWidth) << " bits!" << endl;
	}

	newValue = addr & address_mask;
	cout << name() << " sets starting address to " << newValue << endl;
	op = loadOp;
}

int Memory::badAddress() { return rangeError; }

void Memory::dump( unsigned long startAddr,
		   unsigned long endAddr, ostream& o ){

	o << endl << "MEMORY DUMP" << endl << "-----------" << endl;

	for( unsigned long i = startAddr; i<=endAddr; ) {
		o << setw(addressFieldWidth) << i;
		for( int j = 1; j <= 8; j++ ) {
			if( i > highPoint ) {
				o << " XXXXXXX out of range";
				i = endAddr+1;
				break;
			}
			o << ' ' << setw(dataFieldWidth) << mem.get( i );
			if( ++i > endAddr ) {
				break;
			}
		}
		o << endl;
	}
	o << endl;
}

void Memory::dumpLite( unsigned long startAddr,
		      unsigned long endAddr, ostream& o ){

	for( unsigned long i = startAddr; i<=endAddr; ++i ) {
		if( i > highPoint ) {
			o << " XXXXXX";
		} else {
			o << ' ' << setw(dataFieldWidth) << setfill('0')
			  << mem.get( i ) << '/';
		}
	}
}
