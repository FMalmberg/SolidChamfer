#ifndef SC_EDGEDATA_H
#define	SC_EDGEDATA_H

#include "c4d.h"

class SC_Edgedata{
private:
	LONG a;
	LONG b;
	LONG new_a;
	LONG new_b;
	bool a_open;
	bool b_open;

public:
	SC_Edgedata();
	SC_Edgedata(LONG a, LONG b);

	LONG GetNew(LONG v);
	void SetNew(LONG v, LONG new_v);
	bool IsOpen(LONG v);
	void SetIsOpen(LONG v, bool IsOpen);
};

#endif