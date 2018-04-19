// sccs id:  @(#)FlowSet.C	1.3        12/04/03

// FlowSet.C
//
// Jim Heliotis
//
// Version 2.1 of 1/6/94
//
// updated by:		Warren Carithers
//			November 2002

#include <iostream>
#include <cassert>

#include <Flow.h>
#include <FlowSet.h>

using namespace std;

FlowNode::FlowNode( const Flow* flow, FlowNode* link ):
    thing(flow),
    next(link) {

	assert( thing );

}

FlowNode::~FlowNode() {
}

FlowSet::FlowSet():
    head(0),
    rear(0),
    current(0) {
}

FlowSet::~FlowSet() {
	while( head ) {
		remove();
	}
}

void FlowSet::add( const Flow* flow ) {

	FlowNode* n = new FlowNode(flow);

	assert( (!rear) || (!rear->next) );
	assert( flow );

	if( head ) {
		rear->next = n;
		rear = n;
	} else {
		head = rear = n;
	}

}

const Flow* FlowSet::remove() {

	FlowNode* n = head;

	assert( (!rear) || (!rear->next) );

	if( !n ) {
		return 0;
	}

	if( rear == n ) {
		head = rear = 0;
	} else {
		head = head->next;
	}

	const Flow* flow = n->thing;

	n->next = 0; // prevents cascade effect upon deletion
	delete n;

	return flow;
}

const Flow* FlowSet::first() {

	assert( (!rear) || (!rear->next) );

	current = head;

	if( current ) {
		return current->thing;
	} else {
		return 0;
	}
}

const Flow* FlowSet::next() {

	assert( (!rear) || (!rear->next) );

	if( current ) {
		current = current->next;
		if( current ) {
			return current->thing;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

int FlowSet::contains( const Flow *flow ) {

	assert( (!rear) || (!rear->next) );

	for( FlowNode *n = head; n; n = n->next ) {
		if( flow == n->thing ) {
	    		return 1;
		}
	}

	return 0;
}
