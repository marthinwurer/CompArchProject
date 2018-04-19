// sccs id:  @(#)ShiftRegister.h	1.3        12/04/03

// ShiftRegister
// Jim Heliotis
// February 1994
//
// updated by:		Warren Carithers
//			November 2002

// Adds shifting capability to the basic StorageObject
// (ee StorageObject(3C++) for the basics)
//
//	To perform a normal left shift: leftShift()
//
//	To perform a normal right shift: rightShift()
//
//	To perform a right shift that propagates the sign bit for
//	two's complement division by two: rightArithShift()
//
//	To specify the right-hand input for a left shift: leftShiftInputIs(obj)
//
//	.. If this call is not made when a left shift is requested, a 0 bit
//	will be shifted in.
//
//	To specify the left-hand input for a right shift: rightShiftInputIs(obj)
//
//	.. If this call is not made when a right shift is requested, a 0 bit
//	will be shifted in.  This call is illegal in combination with
//	rightArithShift().
//

#ifndef _SHIFTREGISTER_H_
#define _SHIFTREGISTER_H_

#include <ArchLibError.h>
#include <StorageObject.h>

using namespace std;

class ShiftRegister: public virtual StorageObject {

public:
	ShiftRegister ( const char *id, int numBits, long initVal = 0 );
	~ShiftRegister();

	enum Operation { none = 0, right, rightArith, left };

	void perform( ShiftRegister::Operation );
	void rightShift();	// shorthand
	void rightArithShift();	// shorthand
	void leftShift();	// shorthand

	void rightShiftInputIs ( StorageObject& obj );
	void leftShiftInputIs ( StorageObject& obj );

protected:
	void phase1();

private:
	Operation change;
	StorageObject *lefterly;
	StorageObject *righterly;
};

inline void ShiftRegister::rightShift() { perform( right ); }

inline void ShiftRegister::rightArithShift() { perform( rightArith ); }

inline void ShiftRegister::leftShift() { perform( left ); }

#endif
