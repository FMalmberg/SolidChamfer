#ifndef SC_EDGE_H
#define	SC_EDGE_H

#include "c4d.h"

class SC_Edge{
private:
	LONG a;
	LONG b;


public:
	SC_Edge(LONG v1, LONG v2);
	SC_Edge();

	LONG GetFirst();
	LONG GetSecond();

	bool operator()(const SC_Edge& e1, const SC_Edge& e2) const; //Compare two edges
};


#endif