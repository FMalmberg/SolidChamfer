#include "SC_Facedata.h"

void SC_Facedata::SelectEdge(int i){
	SelectedEdges.insert(i);
}

int SC_Facedata::GetNSelectedEdges(){
	return SelectedEdges.size();
}

bool SC_Facedata::IsSelected(int i){
	return SelectedEdges.find(i)!=SelectedEdges.end();
}