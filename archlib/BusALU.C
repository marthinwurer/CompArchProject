// sccs id:  @(#)BusALU.C	1.4        12/04/03

// BusALU.C
//
// Jim Heliotis
//	December 1993
//
// updated by:		Warren Carithers
//			November 2002

#include <cstdlib>
#include <iostream>
#include <cstring>

#include <BusALU.h>

using namespace std;

CarryConnector::CarryConnector( const char *id, BusALU &b ):
    Connector(id,1),
    CPUObject(id,1),
    busALU(b),
    carry("Carry",1,*this) {
}


CarryConnector::~CarryConnector() {
}

long CarryConnector::computeValue() {
	return busALU.computeCarry();
}

OverflowConnector::OverflowConnector( const char *id, BusALU &b ):
    Connector(id,1),
    CPUObject(id,1),
    busALU(b),
    overflow("Overflow",1,*this) {
}


OverflowConnector::~OverflowConnector() {
}

long OverflowConnector::computeValue() {
	return busALU.computeOverflow();
}

const char *BusALU::opNames[int(BusALU::op_one)+1] = {
	"op_none", "op_add", "op_sub", "op_and", "op_or", "op_xor", "op_not",
	"op_extendSign", "op_lshift", "op_rshift", "op_rashift",
	"op_rop1", "op_rop2", "op_zero", "op_one"
};

BusALU::BusALU( const char *id, int numbits ):
    Connector(id,numbits), CPUObject(id,numbits),
    op1("OP1",numbits), op2("OP2",numbits),
    result("Result",numbits,*this), operation(op_none),
    carryConnector("Carry",*this),
    overflowConnector("Overflow",*this) {

	char *buf;

	// Fix the names of the ALU components

	buf = new char[ strlen(id) + 10 ];	// id + ".Overflow" + 1

	strcpy( buf, id ); strcat( buf, ".Op1" );
	op1.set_name( buf );

	strcpy( buf, id ); strcat( buf, ".Op2" );
	op2.set_name( buf );

	strcpy( buf, id ); strcat( buf, ".Result" );
	result.set_name( buf );

	strcpy( buf, id ); strcat( buf, ".Carry" );
	carryConnector.set_name( buf );

	strcpy( buf, id ); strcat( buf, ".Overflow" );
	overflowConnector.set_name( buf );

	delete [] buf;

}

BusALU::~BusALU() {
}

void BusALU::perform( Operation op ) {
	operation = op;
}

long BusALU::computeValue() {

	compute();

	if( (operation) && (debug&trace) ) {

		cout << name() << '.';
		cout << opNames[(int)operation] << '(';
		op1.printSourceInfo();
		cout << ',';
		op2.printSourceInfo();
		cout << ")-->" << value;
	}

	return value;
}

int BusALU::computeCarry() {

	compute();

	if( (operation) && (debug&trace) ) {

		cout << name() << ".CARRY-->" << carryFlag;

	}

	return carryFlag;
}

int BusALU::computeOverflow() {

	compute();

	if( (operation) && (debug&trace) ) {
		cout << name() << ".OVERFLOW-->" << overflowFlag;
	}

	return overflowFlag;

}

static void BadOp( const char *name, BusALU::Operation op ) {

	cout << name;
	cout << ": illegal operation code " << op << endl;
	throw ArchLibError( "Illegal BusALU operation code" );

}

static long op1Copy = 0, op2Copy = 0;

void BusALU::compute() {

	switch (operation) {	// first operand

		case op_add:
		case op_sub:
		case op_and:
		case op_or:
		case op_xor:
		case op_not:
		case op_extendSign:
		case op_lshift:
		case op_rshift:
		case op_rashift:
		case op_rop1:
	        	op1Copy = op1.fetchValue();

		case op_rop2:	// these ops don't use OP1
		case op_zero:
		case op_one:
		case op_none:
	        	break;
		default:
	        	BadOp( name(), operation );
	}

	switch (operation) {	// second operand

		case op_add:
		case op_sub:
		case op_and:
		case op_or:
		case op_xor:
		case op_extendSign:
		case op_lshift:
		case op_rshift:
		case op_rashift:
		case op_rop2:
	        	op2Copy = op2.fetchValue();

		case op_not:	// these don't use OP2
		case op_rop1:
		case op_zero:
		case op_one:
		case op_none:
	        	break;
		default:
			BadOp( name(), operation );
	}

	carryFlag = 0;
	overflowFlag = 0;

	switch (operation) {
		case op_add:
			addFunction();
			break;
		case op_sub:
			subtractFunction();
			break;
		case op_and:
			value = (op1Copy&op2Copy) & get_mask();
			break;
		case op_or:
			value = (op1Copy|op2Copy) & get_mask();
			break;
		case op_xor:
			value = (op1Copy^op2Copy) & get_mask();
			break;
		case op_not:
			value = (~op1Copy) & get_mask();
			break;
		case op_extendSign:
			extendSignFunction();
			break;
		case op_lshift:
			value = (op1Copy << op2Copy) & get_mask();
			break;
		case op_rshift:
			rshiftFunction();
			break;
		case op_rashift:
			rashiftFunction();
			break;
		case op_rop1:
			value = op1Copy;
			break;
		case op_rop2:
			value = op2Copy;
			break;
		case op_zero:
			value = 0;
			break;
		case op_one:
			value = 1;
			break;
		case op_none:
			cout << name();
			cout << ":  someone wants my value but";
			cout << " no operation was enabled in this cycle";
			cout << endl;
			throw ArchLibError( "BusALU used without an operation" );
	}

	value &= get_mask();

}

void BusALU::addFunction() {

	long x = op1Copy;
	long y = op2Copy;
	long z = (x + y) & get_mask();

	long x1 = (x >> (get_bits()-1)) & 1;
	long y1 = (y >> (get_bits()-1)) & 1;
	long z1 = (z >> (get_bits()-1)) & 1;

	overflowFlag = ((x1==y1) && (z1!=x1))?1:0;

	carryFlag = (((x1+y1)==2) || (((x1+y1)==1) && (z1==0)))?1:0;

	value = z;

}

void BusALU::subtractFunction() {

	long x = op1Copy;
	long y = op2Copy;

	long noty = ~y;
	long negy = (1L + noty) & get_mask();

	long y1 = (noty >> (get_bits()-1)) & 1;
	long z1 = (negy >> (get_bits()-1)) & 1;

	overflowFlag = ((y1==0) && (z1!=0))?1:0;

	carryFlag = ((y1==1) && (z1==0))?1:0;

	long z = (x + negy) & get_mask();
	long x1 = (x >> (get_bits()-1)) & 1;

	y1 = (negy >> (get_bits()-1)) & 1;
	z1 = (z >> (get_bits()-1)) & 1;

	if ((x1==y1) && (z1!=x1)) overflowFlag = 1; // sets, but doesn't clear

	if (((x1+y1)==2) || (((x1+y1)==1) && (z1==0))) carryFlag = 1;

	value = z;

}

void BusALU::extendSignFunction() {

	long num = op1Copy;
	long signMask = op2Copy;

	if( num & signMask ) {

		while( !(signMask&0x80000000) )
			signMask |= (signMask<<1);

		value = (num | signMask) & get_mask();

	} else {

		while( !(signMask&0x00000001) )
			signMask |= (signMask>>1);

		value = (num & signMask) & get_mask();

	}
}

void BusALU::rshiftFunction() {

	unsigned long sh = (unsigned long) op2Copy;

	if( !sh ) {
		value = op1Copy;
		return;
	}

	unsigned long clearMask = get_mask() ^ (1<<(get_bits()-1));

	clearMask >>= sh - 1;

	value = ((unsigned long)op1Copy >> op2Copy) & clearMask;
}

void BusALU::rashiftFunction() {

	unsigned long x = (unsigned long)op1Copy;
	unsigned long signBit = x & (1 << (get_bits()-1));
	unsigned long sh = (unsigned long)op2Copy;

	for( long i=0; i<sh; i++ )
		x = (x >> 1) | signBit;

	value = x;
}
