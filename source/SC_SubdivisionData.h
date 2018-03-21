#ifndef SC_SUBDIVISIONDATA_H
#define	SC_SUBDIVISIONDATA_H

#include <vector>
#include "SC_Offsetdata.h"
#include "SC_Edge.h"
#include "SC_EditMesh.h"
#include <map>
#include <set>
using namespace std;

class SC_SubdivisionData{
private:
	vector<int> strips;	 
	vector<int> faces;
	set<LONG> StripBoundary;
	set<LONG> FaceBoundary;
	set<SC_Edge,SC_Edge> OpenEdges;


	PolygonObject* mesh;
	LONG GetNewEdgeVertex(LONG a, LONG b,map<SC_Edge,LONG, SC_Edge> & edgemap, SC_Offsetdata& offset, SC_EditMesh & m);

	bool IsStripBoundary(LONG vertex_id);
	bool IsFaceBoundary(LONG vertex_id);

public:
	SC_SubdivisionData(PolygonObject* obj){mesh=obj;}
	void AddStrip(int poly_id);
	void AddFace(int poly_id);
	void Subdivide(SC_Offsetdata& offset);
	void AddStripBoundary(LONG vertex_id);
	void AddFaceBoundary(LONG vertex_id);
	void AddOpenEdge(SC_Edge e);
};

#endif