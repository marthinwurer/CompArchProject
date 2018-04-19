// sccs id:  @(#)FlowSet.h	1.3        12/04/03

// FlowSet.h
//
// Jim Heliotis
//	February 1994
//
// updated by:		Warren Carithers
//			November 2002

//
// Internal implementation class -- not for client simulators
//

#ifndef _FLOWSET_H_
#define _FLOWSET_H_

using namespace std;

class Flow;

class FlowNode {

public:
	const Flow* thing;
	FlowNode* next;
	FlowNode ( const Flow* flow, FlowNode* link=0 );
	~FlowNode();
};

class FlowSet {

private:
	FlowNode* head;
	FlowNode* rear;
	FlowNode* current;

public:
	FlowSet();
	~FlowSet();
	void add( const Flow *flow );
	const Flow* remove();
	const Flow* first();
	const Flow* next();
	int contains ( const Flow *flow );
};

#endif
