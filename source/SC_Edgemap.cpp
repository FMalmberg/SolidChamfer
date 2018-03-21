#include "SC_Edgemap.h"

void SC_Edgemap::insert(LONG v1, LONG v2){
	m[SC_Edge(v1,v2)]=SC_Edgedata(v1,v2);
}


SC_Edgedata& SC_Edgemap::operator()(LONG v1, LONG v2){
	if(m.find(SC_Edge(v1,v2))==m.end()){
		insert(v1,v2);
	}
	return m[SC_Edge(v1,v2)];
}

//Return true if the edge (v1,v2) is in the map
bool SC_Edgemap::find(LONG v1, LONG v2){
	return (m.find(SC_Edge(v1,v2))!=m.end());
}