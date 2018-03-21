#ifndef SC_FACEDATA_H
#define	SC_FACEDATA_H

#include <set>
#include "c4d.h"

class SC_Facedata{
private:
	std::set<int> SelectedEdges;

public:
	void SelectEdge(int i);
	int GetNSelectedEdges();
	bool IsSelected(int i);

};

#endif
