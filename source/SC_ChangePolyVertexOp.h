#ifndef SC_CHANGE_POLY_VERTEX_OP
#define SC_CHANGE_POLY_VERTEX_OP

#include "c4d.h"

class SC_ChangePolyVertexOp{
public:
	int pIndex; 
	int pVertex; // vertex identifier, a value between 0 and 3
	int newIndex;

public:
	SC_ChangePolyVertexOp(int polygonIndex, int polygonVertex, int newVertexIndex){
		pIndex=polygonIndex;
		pVertex=polygonVertex;
		newIndex=newVertexIndex;
	}

	SC_ChangePolyVertexOp(){
		SC_ChangePolyVertexOp(0,0,0);
	}

	void Execute(PolygonObject* obj){
		CPolygon* polys=obj->GetPolygonW();
		switch ( pVertex ) {
		 case 0 : 
			 polys[pIndex].a=newIndex;
			 break;
		 case 1 : 
			 polys[pIndex].b=newIndex;
			 break;
		 case 2 : 
			 if(polys[pIndex].c==polys[pIndex].d){
				 polys[pIndex].d=newIndex;
			 }
			 polys[pIndex].c=newIndex;
			 break;
		 case 3 : 
			 if(polys[pIndex].c==polys[pIndex].d){
				 polys[pIndex].c=newIndex;
			 }
			 polys[pIndex].d=newIndex;
			 break;
		}	

	}


};

#endif