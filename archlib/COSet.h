// sccs id:  @(#)COSet.h	1.3        12/04/03

// COSet.h
//
// Jim Heliotis
//	February 1994
//
// updated by:		Warren Carithers
//			November 2002

//
// Internal implementation class -- not for client simulators
//

#ifndef _CLOCKEDOBJECTSET_H_
#define _CLOCKEDOBJECTSET_H_

#include <ClockedObject.h>

using namespace std;

class ClockedObject;

class ClockedObjectNode {

private:
	friend class ClockedObjectSet;

	ClockedObject* thing;
	ClockedObjectNode* next;
	ClockedObjectNode ( ClockedObject* obj, ClockedObjectNode* link=0 );
	~ClockedObjectNode();

};

class ClockedObjectSet {

public:
	ClockedObjectSet();
	~ClockedObjectSet();
	void add( ClockedObject *obj );
	ClockedObject* remove();
	ClockedObject* first();
	ClockedObject* next();
	void *contains ( ClockedObject *obj );

private:
	ClockedObjectNode* head;
	ClockedObjectNode* rear;
	ClockedObjectNode* current;

};

#endif
