// sccs id:  @(#)Memory.h	1.6        12/04/03

// Memory
// Jim Heliotis
// February 1994
// Main memory simulation
//
// updated by:		Warren Carithers
//			November 2002

//
// Memory is a collection of Memory cells, arranged as an array,
// and individually addressable.  It provides access to them through
// a memory address register, MAR, a WRITE InFlow, and a READ
// OutFlow.  Multi-unit transfer and detection of improper addresses
// are supported.
// 
// Do not use LongArray.  It is part of Memory's implementation.
//

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <iostream>

#include <ArchLibError.h>
#include <Connector.h>
#include <ClockedObject.h>
#include <StorageObject.h>
#include <InFlow.h>
#include <OutFlow.h>

using namespace std;

class Memory;

class LongArray {

	friend class Memory;

private:
	LongArray();
	~LongArray();
	void allocate( long sz );
	long get( long index );
	void put( long index, long value );
	long size;
	long *data;
};

class Memory : public Connector, public ClockedObject {

public:
	Memory (
		const char *id,		// name of module
		int bitsInAddr,		// implies potential size of memory
		int bitsPerUnit,	// bits in each ADDRESSABLE unit
					// (e.g. 8 for a byte)
		unsigned long maxAddr = (unsigned long)(-1),
					// -1 => all addr's valid
		int unitsInDataPath = 1, // or #units in MBR
		bool littleEndian = false    // MSB vs. LSB first
	);
	~Memory();

	StorageObject& MAR() { return mar; }
	// a reference to the memory unit's address register
	InFlow & WRITE() { return writeFlow; }
	// a reference to the memory unit's ingoing data path for writing
	OutFlow & READ() { return readFlow; }
	// a reference to the memory unit's outgoing data path for reading

	enum Operation { none, loadOp, readOp, writeOp };
	void perform( Operation o );	// to be performed on the next clock
	void read() { perform( readOp ); } // shorthand for perform(readOp)
	void write() { perform( writeOp ); } // shorthand for perform(writeOp)

	void load( const char *fileName, long defaultValue = 0 );
		// Read in the object file named fileName.
		// Each line in the file is <#units> <unit1> ... <unitN>,
		// all in hex.  Last line is starting address for program,
		// ready to be latched through the READ OutFlow.
		// Rest of memory is initialized to defaultValue.
	void dump( unsigned long startAddr, unsigned long endAddr,
		   ostream &o = cout );
		// diagnostic memory dump for debugging.
	void dumpLite( unsigned long startAddr, unsigned long endAddr,
		       ostream &o = cout );
		// terse diagnostic memory dump - no addresses,
		// just data; no line feeds
	int badAddress();
		// Reflects just completed read or write; this ought to be
		// called after every such operation, e.g. to cause a trap.

protected:
	void phase1();
	void phase2();

private:
	long computeValue();

	Operation op;
	StorageObject mar;
	InFlow writeFlow;
	OutFlow readFlow;

	unsigned long currentAddr;
	long newValue;

	LongArray mem;

	unsigned long highPoint;
	int unitSize;
	int rangeError;
	unsigned long address_mask;
	unsigned long unit_mask;
	int dataPathWidth;

	int addressFieldWidth;
	int dataFieldWidth;

	bool byteSwap;
};

#endif
