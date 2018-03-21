#ifndef SC_VERTEXDATA_H
#define	SC_VERTEXDATA_H

#include "c4d.h"
#include <set>

enum Vertextype{
	TYPE_ENDPOINT,
	TYPE_NONE,
	TYPE_CORNER
};

inline String SC_VertexTypeToString(Vertextype t){
	String s;
	switch (t){
	case TYPE_ENDPOINT:
		s="Endpoint";
		break;
	case TYPE_NONE:
		s="Standard";
		break;
	case TYPE_CORNER:
		s="Corner";
		break;
	}
	return s;
}

class SC_Vertexdata{
private:
	std::set<int> AdjacentVertices;
public:
	void AddAdjacent(int i);
	int GetValence();
	Vertextype GetType();

};

#endif