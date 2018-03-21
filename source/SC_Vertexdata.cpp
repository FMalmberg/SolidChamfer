#include "SC_Vertexdata.h"

void SC_Vertexdata::AddAdjacent(int i){
	AdjacentVertices.insert(i);
}

int SC_Vertexdata::GetValence(){
	return AdjacentVertices.size();
}

Vertextype SC_Vertexdata::GetType(){
	Vertextype t=TYPE_NONE;
	int v=GetValence();
	if(v==1){t=TYPE_ENDPOINT;}
	if(v>2){t=TYPE_CORNER;}
	return t;
}