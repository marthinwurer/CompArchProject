// sccs id:  @(#)COSet.C	1.3        12/04/03

// COSet.C
//
// Jim Heliotis
//	January 6, 1994
//
// updated by:		Warren Carithers
//			November 2002

#include <iostream>
#include <cassert>

#include <COSet.h>

using namespace std;

ClockedObjectNode::ClockedObjectNode( ClockedObject* obj,
					ClockedObjectNode* link ):
    thing(obj), next(link) {

	assert( thing );

}

ClockedObjectNode::~ClockedObjectNode() {
}

ClockedObjectSet::ClockedObjectSet(): head(0), rear(0), current(0) {
}

ClockedObjectSet::~ClockedObjectSet() {

	while( head ) {
		remove();
	}

}

void ClockedObjectSet::add( ClockedObject* obj ) {

	assert( (!rear) || (!rear->next) );
	assert( obj );

	if( contains( obj ) ) {
		cout << "\nDUPLICATE in ClockedObjectSet!\n";
	}

	ClockedObjectNode* n = new ClockedObjectNode(obj);

	if( head ) {
		rear->next = n;
		rear = n;
	} else {
		head = rear = n;
	}
}

ClockedObject* ClockedObjectSet::remove() {

	assert( (!rear) || (!rear->next) );

	ClockedObjectNode* n = head;

	if( !n ) {
		return 0;
	}

	if( rear == n ) {
		head = rear = 0;
	} else {
		head = head->next;
	}

	ClockedObject* obj = (ClockedObject *)n->thing;

	n->next = 0; // prevents cascade effect upon deletion
	delete n;

	return obj;
}

ClockedObject* ClockedObjectSet::first() {

	assert( (!rear) || (!rear->next) );

	current = head;

	if( current ) {
		return (ClockedObject *)current->thing;
	} else {
		return 0;
	}

}

ClockedObject* ClockedObjectSet::next() {

	assert( (!rear) || (!rear->next) );

	if( current ) {
		current = current->next;
		if( current ) {
			return (ClockedObject *)current->thing;
		} else {
	    		return 0;
		}
	} else {
		return 0;
	}
}

void *ClockedObjectSet::contains( ClockedObject *obj ) {

	assert( (!rear) || (!rear->next) );

	for ( ClockedObjectNode *n=head; n; n=n->next ) {
		if( obj == n->thing ) {
			return this;
		}
	}

	return 0;

}
