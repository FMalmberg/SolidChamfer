#include "SC_SubdivisionData.h"
#include "AddEdgePoint.h"


class VectorAccumulator{
private:

	map<int, double> data;

public:


	void Add(int vertex_index, double weight){
		data[vertex_index]=weight;
	}

	Vector GetAverage(SC_Offsetdata& off){
		if(data.size()>0){
			Vector sum=Vector(0,0,0);
			double weightsum=0;
			map<int,double>::iterator it;
			for(it=data.begin(); it!=data.end(); it++){
				sum+=off.GetOffset(it->first).dir;
				weightsum+=it->second;
			}
			
			return sum/weightsum;
		}
		else{ 
			return Vector(0,0,0);
		}
	
	}
};

LONG  SC_SubdivisionData::GetNewEdgeVertex(LONG a, LONG b, map<SC_Edge,LONG, SC_Edge> & edgemap, SC_Offsetdata& offset, SC_EditMesh & m){
	LONG newVertexIndex;
	Vector origin, dir;
	map<SC_Edge,LONG, SC_Edge>::iterator it=edgemap.find(SC_Edge(a,b));
	if(it!=edgemap.end()){
		newVertexIndex=it->second;
	}
	else{
		newVertexIndex=m.AddVertex(Vector(0,0,0)); //Initialize with origin, the final position will be determined by the Offsetdata
		origin=offset.GetOffset(a).p;
		dir=0.5*(offset.GetOffset(a).dir+offset.GetOffset(b).dir);
		offset.SetOffset(newVertexIndex, SC_Offset(origin,dir));
		edgemap[SC_Edge(a,b)]=newVertexIndex;
	}
	return newVertexIndex;
}

void SC_SubdivisionData::AddOpenEdge(SC_Edge e){
	OpenEdges.insert(e);
}

void SC_SubdivisionData::Subdivide(SC_Offsetdata& offset){

	map<LONG,VectorAccumulator> newOffsets;
	SC_EditMesh m(mesh);
	int nstrips=strips.size();
	int nfaces=faces.size();
	LONG newVertexIndex1, newVertexIndex2,newVertexIndex3, newVertexIndex4,newCenterVertexIndex, newFaceIndex;
	LONG a,b,c,d;
	CPolygon* polys=mesh->GetPolygonW();
	Vector origin, dir;
	map<SC_Edge, LONG, SC_Edge> edgemap;
	map<LONG,LONG> centermap;

	for(int i=0; i<nstrips; i++){
		a=polys[strips[i]].a;
		b=polys[strips[i]].b;
		c=polys[strips[i]].c;
		d=polys[strips[i]].d;

		newVertexIndex1=GetNewEdgeVertex(a, b, edgemap, offset,  m);
		newVertexIndex2=GetNewEdgeVertex(c, d, edgemap, offset,  m);

		if(!IsStripBoundary(a)){
			newOffsets[a].Add(newVertexIndex1, 1);
			newOffsets[a].Add(a, 1);
		}

		if(!IsStripBoundary(b)){
			newOffsets[b].Add(newVertexIndex1, 1);
			newOffsets[b].Add(b, 1);
		}

		if(!IsStripBoundary(c)){
			newOffsets[c].Add(newVertexIndex2, 1);
			newOffsets[c].Add(c, 1);
		}

		if(!IsStripBoundary(d)){
			newOffsets[d].Add(newVertexIndex2, 1);
			newOffsets[d].Add(d, 1);
		}
		newFaceIndex=m.AddFace(CPolygon(polys[strips[i]].a,newVertexIndex1, newVertexIndex2, polys[strips[i]].d ));

		AddStrip(newFaceIndex);

		polys[strips[i]].a=newVertexIndex1;
		polys[strips[i]].d=newVertexIndex2;
	}


	LONG newFaceIndex1, newFaceIndex2,newFaceIndex3;

	for(int i=0; i<nfaces; i++){
		a=polys[faces[i]].a;
		b=polys[faces[i]].b;
		c=polys[faces[i]].c;
		d=polys[faces[i]].d;

		if(c==d){ //Triangle
			newVertexIndex1=GetNewEdgeVertex(a, b, edgemap, offset,  m);
			newVertexIndex2=GetNewEdgeVertex(b, c, edgemap, offset,  m);
			newVertexIndex3=GetNewEdgeVertex(d, a, edgemap, offset,  m);

			if(IsFaceBoundary(a) && IsFaceBoundary(b)){ AddFaceBoundary(newVertexIndex1);}
			if(IsFaceBoundary(b) && IsFaceBoundary(c)){ AddFaceBoundary(newVertexIndex2);}
			if(IsFaceBoundary(c) && IsFaceBoundary(a)){ AddFaceBoundary(newVertexIndex3);}

			newCenterVertexIndex=m.AddVertex(Vector(0,0,0)); //Initialize with origin, the final position will be determined by the Offsetdata
			centermap[faces[i]]=newCenterVertexIndex;
			origin=offset.GetOffset(a).p;
			dir=(offset.GetOffset(a).dir+offset.GetOffset(b).dir+offset.GetOffset(c).dir)/3.0;
			offset.SetOffset(newCenterVertexIndex, SC_Offset(origin,dir));

			if(!IsFaceBoundary(newVertexIndex1)){
				newOffsets[newVertexIndex1].Add(newCenterVertexIndex,1);
				newOffsets[newVertexIndex1].Add(newVertexIndex1,1);
			}

			if(!IsFaceBoundary(newVertexIndex2)){
				newOffsets[newVertexIndex2].Add(newCenterVertexIndex,1);
				newOffsets[newVertexIndex2].Add(newVertexIndex2,1);
			}

			if(!IsFaceBoundary(newVertexIndex3)){
				newOffsets[newVertexIndex3].Add(newCenterVertexIndex,1);
				newOffsets[newVertexIndex3].Add(newVertexIndex3,1);
			}


		}
		else{ //Quad
			newVertexIndex1=GetNewEdgeVertex(a, b, edgemap, offset,  m);
			newVertexIndex2=GetNewEdgeVertex(b, c, edgemap, offset,  m);
			newVertexIndex3=GetNewEdgeVertex(c, d, edgemap, offset,  m);
			newVertexIndex4=GetNewEdgeVertex(d, a, edgemap, offset,  m);

			if(IsFaceBoundary(a) && IsFaceBoundary(b)){ AddFaceBoundary(newVertexIndex1);}
			if(IsFaceBoundary(b) && IsFaceBoundary(c)){ AddFaceBoundary(newVertexIndex2);}
			if(IsFaceBoundary(c) && IsFaceBoundary(d)){ AddFaceBoundary(newVertexIndex3);}
			if(IsFaceBoundary(d) && IsFaceBoundary(a)){ AddFaceBoundary(newVertexIndex4);}

			newCenterVertexIndex=m.AddVertex(Vector(0,0,0)); //Initialize with origin, the final position will be determined by the Offsetdata
			centermap[faces[i]]=newCenterVertexIndex;
			origin=offset.GetOffset(a).p;
			dir=0.25*(offset.GetOffset(a).dir+offset.GetOffset(b).dir+offset.GetOffset(c).dir+offset.GetOffset(d).dir);
			offset.SetOffset(newCenterVertexIndex, SC_Offset(origin,dir));

			if(!IsFaceBoundary(newVertexIndex1)){
				newOffsets[newVertexIndex1].Add(newCenterVertexIndex,1);
				newOffsets[newVertexIndex1].Add(newVertexIndex1,1);
			}

			if(!IsFaceBoundary(newVertexIndex2)){
				newOffsets[newVertexIndex2].Add(newCenterVertexIndex,1);
				newOffsets[newVertexIndex2].Add(newVertexIndex2,1);
			}

			if(!IsFaceBoundary(newVertexIndex3)){
				newOffsets[newVertexIndex3].Add(newCenterVertexIndex,1);
				newOffsets[newVertexIndex3].Add(newVertexIndex3,1);
			}

			if(!IsFaceBoundary(newVertexIndex4)){
				newOffsets[newVertexIndex4].Add(newCenterVertexIndex,1);
				newOffsets[newVertexIndex4].Add(newVertexIndex4,1);
			}

		


		}
	}

	SC_Offsetdata offset_tmp=offset;
	map<LONG,VectorAccumulator>::iterator it;
	for ( it=newOffsets.begin() ; it != newOffsets.end(); it++ ){
		origin=offset.GetOffset((*it).first).p;
		dir=(*it).second.GetAverage(offset_tmp);
		offset.SetOffset((*it).first,SC_Offset(origin,dir));
	}


	newOffsets.clear();



	//Nytt:
	for(int i=0; i<nfaces; i++){
		a=polys[faces[i]].a;
		b=polys[faces[i]].b;
		c=polys[faces[i]].c;
		d=polys[faces[i]].d;

		if(c==d){ //Triangle
			newVertexIndex1=GetNewEdgeVertex(a, b, edgemap, offset,  m);
			newVertexIndex2=GetNewEdgeVertex(b, c, edgemap, offset,  m);
			newVertexIndex3=GetNewEdgeVertex(d, a, edgemap, offset,  m);
			newCenterVertexIndex=centermap[faces[i]];

			newFaceIndex1=m.AddFace(CPolygon(newVertexIndex1,b,newVertexIndex2, newCenterVertexIndex));
			newFaceIndex2=m.AddFace(CPolygon(newCenterVertexIndex,newVertexIndex2,c,newVertexIndex3));

			polys[faces[i]].a=a;
			polys[faces[i]].b=newVertexIndex1;
			polys[faces[i]].c=newCenterVertexIndex;
			polys[faces[i]].d=newVertexIndex3;

			AddFace(newFaceIndex1);
			AddFace(newFaceIndex2);

			if(!IsFaceBoundary(a)){
				newOffsets[a].Add(newVertexIndex1,1);
				newOffsets[a].Add(newVertexIndex4,1);
				newOffsets[a].Add(a,1);
			}

			if(!IsFaceBoundary(b)){
				newOffsets[b].Add(newVertexIndex1,1);
				newOffsets[b].Add(newVertexIndex2,1);
				newOffsets[b].Add(b,1);
			}

			if(!IsFaceBoundary(c)){
				newOffsets[c].Add(newVertexIndex2,1);
				newOffsets[c].Add(newVertexIndex3,1);
				newOffsets[c].Add(c,1);
			}
		}
		else{ //Quad
			newVertexIndex1=GetNewEdgeVertex(a, b, edgemap, offset,  m);
			newVertexIndex2=GetNewEdgeVertex(b, c, edgemap, offset,  m);
			newVertexIndex3=GetNewEdgeVertex(c, d, edgemap, offset,  m);
			newVertexIndex4=GetNewEdgeVertex(d, a, edgemap, offset,  m);
			newCenterVertexIndex=centermap[faces[i]];

			newFaceIndex1=m.AddFace(CPolygon(newVertexIndex1,b,newVertexIndex2, newCenterVertexIndex));
			newFaceIndex2=m.AddFace(CPolygon(newCenterVertexIndex,newVertexIndex2,c,newVertexIndex3));
			newFaceIndex3=m.AddFace(CPolygon(newVertexIndex4,newCenterVertexIndex,newVertexIndex3,d)); 

			polys[faces[i]].a=a;
			polys[faces[i]].b=newVertexIndex1;
			polys[faces[i]].c=newCenterVertexIndex;
			polys[faces[i]].d=newVertexIndex4;

			AddFace(newFaceIndex1);
			AddFace(newFaceIndex2);
			AddFace(newFaceIndex3);

			if(!IsFaceBoundary(a)){
				newOffsets[a].Add(newVertexIndex1,1);
				newOffsets[a].Add(newVertexIndex4,1);
				newOffsets[a].Add(a,1);
				newOffsets[a].Add(newCenterVertexIndex,1);
				
			}

			if(!IsFaceBoundary(b)){
				newOffsets[b].Add(newVertexIndex1,1);
				newOffsets[b].Add(newVertexIndex2,1);
				newOffsets[b].Add(b,1);
				newOffsets[b].Add(newCenterVertexIndex,1);
			}

			if(!IsFaceBoundary(c)){
				newOffsets[c].Add(newVertexIndex2,1);
				newOffsets[c].Add(newVertexIndex3,1);
				newOffsets[c].Add(c,1);
				newOffsets[c].Add(newCenterVertexIndex,1);
			}

			if(!IsFaceBoundary(d)){
				newOffsets[d].Add(newVertexIndex3,1);
				newOffsets[d].Add(newVertexIndex4,1);
				newOffsets[d].Add(d,1);
				newOffsets[d].Add(newCenterVertexIndex,1);
			}

		}
	}

	m.Commit();
	
	offset_tmp=offset;
	for ( it=newOffsets.begin() ; it != newOffsets.end(); it++ ){
		origin=offset.GetOffset((*it).first).p;
		dir=(*it).second.GetAverage(offset_tmp);
		offset.SetOffset((*it).first,SC_Offset(origin,dir));
	}


	AutoAlloc<Modeling> mod;
	mod->InitObject(mesh); //Do checks here!

	set<SC_Edge,SC_Edge> new_OpenEdges;
	set<SC_Edge,SC_Edge>::iterator it2;
	for(it2=OpenEdges.begin(); it2!=OpenEdges.end(); it2++){
		SC_Edge edge=*it2;
		LONG midpoint= GetNewEdgeVertex(edge.GetFirst(), edge.GetSecond(), edgemap, offset,  m);
		AddEdgePoint(mesh, edge.GetFirst(), edge.GetSecond(), midpoint, mod);
		new_OpenEdges.insert(SC_Edge(edge.GetFirst(),midpoint ));
		new_OpenEdges.insert(SC_Edge(edge.GetSecond(),midpoint ));

	}
	OpenEdges=new_OpenEdges;
	mod->Commit();


}	

void SC_SubdivisionData::AddStrip(int poly_id){
	strips.push_back(poly_id);
}

void SC_SubdivisionData::AddFace(int poly_id){
	faces.push_back(poly_id);
}

void SC_SubdivisionData::AddStripBoundary(LONG vertex_id){
	StripBoundary.insert(vertex_id);
}

void SC_SubdivisionData::AddFaceBoundary(LONG vertex_id){
	FaceBoundary.insert(vertex_id);
}

bool SC_SubdivisionData::IsStripBoundary(LONG vertex_id){
	return (StripBoundary.find(vertex_id)!=StripBoundary.end());
}

bool SC_SubdivisionData::IsFaceBoundary(LONG vertex_id){
	return (FaceBoundary.find(vertex_id)!=FaceBoundary.end());
}