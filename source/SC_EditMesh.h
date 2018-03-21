#ifndef SC_EDIT_MESH_H
#define SC_EDIT_MESH_H

#include "c4d.h"
#include <vector>

//A wrapper class for polygon objects, 
//allowing vertices and polygons to be inserted dynamically

class SC_EditMesh{
private:
	PolygonObject* mesh;
	std::vector<Vector> new_verts;
	std::vector<CPolygon> new_polys;

public:
	SC_EditMesh(PolygonObject* obj);
	int AddVertex(Vector v);
	int AddFace(CPolygon p);
	void Commit();

	Vector& Vertex(LONG index);
	CPolygon& Face(LONG index);
};

#endif