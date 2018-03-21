#include "SC_EditMesh.h"

SC_EditMesh::SC_EditMesh(PolygonObject* obj){
	mesh=obj;
}

int SC_EditMesh::AddVertex(Vector v){
	new_verts.push_back(v);
	return new_verts.size()+mesh->GetPointCount()-1;
}

int SC_EditMesh::AddFace(CPolygon p){
	new_polys.push_back(p);
	return new_polys.size()+mesh->GetPolygonCount()-1;
}

void SC_EditMesh::Commit(){
	LONG oldpcnt=mesh->GetPointCount();
	LONG oldvcnt=mesh->GetPolygonCount();

	mesh->ResizeObject(oldpcnt+new_verts.size(), oldvcnt+new_polys.size());
	
	Vector* verts=mesh->GetPointW();
	CPolygon* polys=mesh->GetPolygonW();

	unsigned int i;

	for(i=0; i<new_verts.size(); i++){
		verts[oldpcnt+i]=new_verts[i];
	}

	for(i=0; i<new_polys.size(); i++){
		polys[oldvcnt+i]=new_polys[i];
	}

	new_verts.clear();
	new_polys.clear();

}

Vector& SC_EditMesh::Vertex(LONG index){
	if(index<mesh->GetPointCount()){
		return (mesh->GetPointW())[index];
	}
	else{
		return new_verts[index-mesh->GetPointCount()];
	}

}

CPolygon& SC_EditMesh::Face(LONG index){
	if(index<mesh->GetPolygonCount()){
		return (mesh->GetPolygonW())[index];
	}
	else{
		return new_polys[index-mesh->GetPolygonCount()];
	}
}