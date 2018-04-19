// sccs id:  @(#)BusALU.h	1.4        12/04/03

// BusALU
// Jim Heliotis
// February 1994
//
// updated by:		Warren Carithers
//			November 2002
//
// A special kind of bus that can do arithmetic and logical operations
//
// Interface is
//
//     void perform(op)  where op(eration) is from the following set:
//
//		op_add:		add OP1 & OP2
//		op_sub:		subtract OP2 from OP1
//		op_and:		and OP1 & OP2
//		op_or:		or OP1 & OP2
//		op_xor:		xor OP1 & OP2
//		op_not:		complement OP1
//		op_extendSign:	extend the sign of OP1; OP2 is a mask
//					telling us which bit is the sign
//		op_lshift:	left shift OP1 by OP2's amount.
//		op_rshift:	right shift OP1 by OP2's amount.
//		op_rashift:	right shift OP1 by OP2's amount; sign of
//					OP1 will stay constant.
//		op_rop1:	just pass OP1 through
//		op_rop2:	just pass OP2 through
//		op_zero:	value is the constant 0
//		op_one:		value is the constant 1
//
//	perform(op) MUST be called in every clock cycle that
//	the output will be needed, even if the operation
//	is the same as the previous cycle.  Also, it must be
//	done after the inputs for OP1 and OP2 have been set.
//  
//InFlow &OP1()
//InFlow &OP2()	references to the two incoming paths associated with the
//		ALU inputs
//
//OutFlow &OUT()	reference to the outgoing path associated with the
//			ALU output
//OutFlow &CARRY()
//OutFlow &OFLOW()	condition codes from operation of previous clock cycle
//

#ifndef _BUSALU_H_
#define _BUSALU_H_

#include <iostream>

#include <ArchLibError.h>
#include <Connector.h>
#include <InFlow.h>
#include <OutFlow.h>

using namespace std;

class BusALU;

class CarryConnector: public Connector {

	friend class BusALU;

private:
	CarryConnector( const char *id, BusALU &b );
	~CarryConnector();

	long computeValue();
	OutFlow carry;

	BusALU &busALU;
};

class OverflowConnector: public Connector {

	friend class BusALU;

private:
	OverflowConnector( const char *id, BusALU &b );
	~OverflowConnector();

	long computeValue();
	OutFlow overflow;

	BusALU &busALU;
};

class BusALU : public Connector {

public:
	enum Operation {
		op_none=0, op_add=1, op_sub=2, op_and=3, op_or=4, op_xor=5,
		op_not=6, op_extendSign=7, op_lshift=8, op_rshift=9,
		op_rashift=10, op_rop1=11, op_rop2=12, op_zero=13, op_one=14
	};

	static const char *opNames[int(op_one)+1];

	BusALU ( const char *id, int numBits );
		// constructor describes size of ALU and its name
	~BusALU();

	void perform( Operation op );
		// perform the specified operation, using OP1 and OP2
		// (WHICH SHOULD ALREADY HAVE BEEN CONNECTED TO SOMETHING
		// VIA THE PULLFROM OPERATION)
	InFlow &OP1() { return op1; }
	InFlow &IN1() { return OP1(); }
		// used to establish from where operand 1 is being pulled
	InFlow &OP2() { return op2; }
	InFlow &IN2() { return OP2(); }
		// used to establish from where operand 2 is being pulled
	OutFlow &OUT() { return result; }
		// used to establish destination of result
	OutFlow &CARRY() { return carryConnector.carry; }
		// Boolean path tells whether there was a carry
		// from the operation in the previous clock cycle
	OutFlow &OFLOW() { return overflowConnector.overflow; }
		// Boolean path tells whether there was a signed overflow
		// from the operation in the prefvious clock cycle

private:
	long value;
	long computeValue();	// std. phase1 operation called by my OutFlow
	int carryFlag;
	int computeCarry();	// called by my CarryConnector
	int overflowFlag;
	int computeOverflow();	// called by my OverflowConnector
	
	void compute();		// returns the value of the currently
				// chosen operation & operands

	void addFunction();
	void subtractFunction();
	void extendSignFunction();
	void rshiftFunction();
	void rashiftFunction();

	Operation operation;
	InFlow op1;
	InFlow op2;
	OutFlow result;
	friend class CarryConnector;
	CarryConnector carryConnector;
	friend class OverflowConnector;
	OverflowConnector overflowConnector;
};

#endif
