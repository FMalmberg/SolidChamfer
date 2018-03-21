#include "SolidChamfer.h"

#include "SC_Facedata.h"
#include "AddEdgePoint.h"
#include "SC_Edgemap.h"
#include "SC_Vertexdata.h"
#include "SC_EditMesh.h"
#include "SC_HelperFuncs.h"
#include "SC_ChangePolyVertexOp.h"

#include "lib_modeling.h"
#include "lib_ngon.h"

#include <map>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;


//Ensure that selectedEdges is consistent
//Assumes that n is initialized with all polygons in obj
void FixSelection(BaseSelect* selectedEdges, PolygonObject* obj, Neighbor* n){
	set<LONG> newElements;

	CPolygon* polys=obj->GetPolygonW();

	LONG verts[4];
	LONG pind, eind, v1, v2, nsides, pind2, eind2, elem;
	LONG seg=0,a,b,i;
	while (selectedEdges->GetRange(seg++,&a,&b)){
		for (i=a; i<=b; ++i){
			if(pind<obj->GetPolygonCount()){

				// ... do something - i is the selected element
				pind=i/4; 
				eind=i%4; 


				verts[0]=polys[pind].a; 
				verts[1]=polys[pind].b; 
				verts[2]=polys[pind].c; 
				verts[3]=polys[pind].d; 

				nsides=4; 
				if(verts[2]==verts[3]){nsides=3;} //triangle
				if(nsides==3 && eind==3){eind=2;}

				v1=verts[eind];
				v2=verts[(eind+1)%nsides];

				pind2=n->GetNeighbor(v1, v2, pind);

				if(pind2!=NOTOK){
					eind2=0;
					if(polys[pind2].a == v1 && polys[pind2].b==v2){eind2=0;}
					else if(polys[pind2].a == v2 && polys[pind2].b==v1){eind2=0;}
					else if(polys[pind2].b == v1 && polys[pind2].c==v2){eind2=1;}
					else if(polys[pind2].b == v2 && polys[pind2].c==v1){eind2=1;}
					else if(polys[pind2].c == v1 && polys[pind2].d==v2){eind2=2;}
					else if(polys[pind2].c == v2 && polys[pind2].d==v1){eind2=2;}
					else if(polys[pind2].d == v1 && polys[pind2].a==v2){eind2=3;}
					else if(polys[pind2].d == v2 && polys[pind2].a==v1){eind2=3;}

					elem=4*pind2+eind2;

					if(!(selectedEdges->IsSelected(elem))){
						newElements.insert(elem);
					}

				}
			}
		}
		set<LONG>::iterator it;
		for(it=newElements.begin(); it!=newElements.end(); it++){
			selectedEdges->Select(*it);
		}
	}


}

Vector Merge(Vector OldV, Vector NewV){
	return (OldV+NewV)/(1+(!OldV)*(!NewV));
}


void TraverseAdjacent(LONG active_poly, 
	LONG center_vertex,
	LONG active_vertex, 
	SC_Edgemap& edges, 
	Neighbor& neighbors, 
	set<SC_Edge,SC_Edge>& selectedEdges,
	vector<SC_ChangePolyVertexOp>& vertexOps,
	CPolygon* polys){

		LONG last_active_vertex=active_vertex;
		LONG new_edge_vertex=edges(center_vertex,active_vertex).GetNew(center_vertex);

		bool gap=false;
		bool done=false;

		while(!done){
			active_poly=neighbors.GetNeighbor(center_vertex, active_vertex, active_poly);
			if(active_poly==NOTOK){
				gap=false;
				done=true;
			}
			else{
				last_active_vertex=active_vertex;
				active_vertex=GetNextVert(center_vertex,active_vertex, polys[active_poly]);
				if(selectedEdges.find(SC_Edge(center_vertex,active_vertex))!=selectedEdges.end()){
					done=true;
				}
				else{
					gap=true;
					vertexOps.push_back(SC_ChangePolyVertexOp(active_poly,GetVertexId(center_vertex,polys[active_poly]),new_edge_vertex ));
				}
			}
		}

		if(gap){
			edges(center_vertex,last_active_vertex).SetNew(center_vertex,new_edge_vertex);
		}
}

Vector GetOffset(Vector edgedir, Vector crossdir, bool orthogonal){
	Vector off=!crossdir;
	if(orthogonal){
		edgedir.Normalize();
		off=off-(off*edgedir)*edgedir;
		off.Normalize();
	}
	return off;
}



void SolidChamfer(PolygonObject* mesh, BaseSelect* edgeSelection, SC_Offsetdata& offset, SC_SubdivisionData& sd,CORNER_STYLE cs){

	map<LONG,SC_Facedata> faces;
	map<LONG,SC_Vertexdata> vertices;
	SC_Edgemap edges;
	set<SC_Edge,SC_Edge> selectedEdges;
	set<LONG> new_edges;
	set<SC_Edge,SC_Edge> openEdges;
	map<LONG,LONG> openFaceEdges;



	CPolygon* polys=mesh->GetPolygonW();
	Vector* points=mesh->GetPointW();
	BaseSelect* point_selection = mesh->GetPointS();
	vector<int> new_vertex_sel;

	NgonBase* ngonbase=mesh->GetNgonBase();
	if(!ngonbase){
		return;
	}

	Neighbor neighbors;
	neighbors.Init(mesh->GetPointCount(), polys, mesh->GetPolygonCount(), NULL);

	FixSelection(edgeSelection, mesh, &neighbors);

	LONG verts[4];
	LONG seg=0,a,b,i;
	LONG pind,eind,v1,v2,nsides;
	while (edgeSelection->GetRange(seg++,&a,&b)){
		for (i=a; i<=b; ++i){
			//i is the selected element
			pind=i/4; 
			eind=i%4; 
			if(pind < mesh->GetPolygonCount()){

				verts[0]=polys[pind].a; 
				verts[1]=polys[pind].b; 
				verts[2]=polys[pind].c; 
				verts[3]=polys[pind].d;   

				nsides=4; 
				if(verts[2]==verts[3]){nsides=3;} //triangle

				if(nsides==3 && eind==3){
					eind=2;
				}

				//Update faces
				faces[pind].SelectEdge(eind);

				//Update vertices
				v1=verts[eind];
				v2=verts[(eind+1)%nsides];

				vertices[v1].AddAdjacent(v2);
				vertices[v2].AddAdjacent(v1);

				selectedEdges.insert(SC_Edge(v1,v2));
			}
		}
	}

	SC_EditMesh m(mesh);
	vector<SC_ChangePolyVertexOp> vertexOps;

	//Loop over affected polygons
	map<LONG,SC_Facedata>::iterator it2;

	LONG new_face_verts[4];
	bool prev_edge_sel, next_edge_sel;
	Vector p_cur, p_next, p_prev;
	LONG i_cur, i_next, i_prev;
	LONG next_edge_index;
	LONG prev_edge_index;

	for ( it2=faces.begin() ; it2 != faces.end(); it2++ ){
		pind=it2->first;
		SC_Facedata f=it2->second;

		bool CurrentPolygonIsNgon= (ngonbase->FindPolygon(pind)!=NOTOK);

		verts[0]=polys[pind].a; 
		verts[1]=polys[pind].b; 
		verts[2]=polys[pind].c; 
		verts[3]=polys[pind].d;

		nsides=4; 
		if(verts[2]==verts[3]){nsides=3;} //triangle

		new_face_verts[0]=verts[0];
		new_face_verts[1]=verts[1];
		new_face_verts[2]=verts[2];
		new_face_verts[3]=verts[3];


		for(int i=0; i<nsides; i++){
			prev_edge_sel=f.IsSelected(GetPrevious(i,nsides));
			next_edge_sel=f.IsSelected(i);

			prev_edge_index=GetPrevious(i,nsides);
			next_edge_index=i;
			i_cur=verts[i];
			i_next=verts[GetNext(i,nsides)];
			i_prev=verts[GetPrevious(i,nsides)];
			p_cur=points[i_cur];
			p_next=points[i_next];
			p_prev=points[i_prev];

			if(prev_edge_sel||next_edge_sel){
				offset.SetOffset(i_cur, SC_Offset(p_cur, Vector(0,0,0)));		
			}

			if(prev_edge_sel && next_edge_sel){
				new_face_verts[i]=m.AddVertex(p_cur);
				if(CurrentPolygonIsNgon){ //ngon
					new_vertex_sel.push_back(new_face_verts[i]);
				}


				Vector off1=GetOffset(p_next-p_cur, p_prev-p_cur, true);
				Vector off2=GetOffset(p_prev-p_cur, p_next-p_cur, true);
				offset.SetOffset(new_face_verts[i], SC_Offset(p_cur, Merge(off1,off2)));

				vertexOps.push_back(SC_ChangePolyVertexOp(pind, i, new_face_verts[i]));

				if(vertices[i_cur].GetType()==TYPE_CORNER){
					//Standard corner;
					if(edges(i_cur, i_prev).GetNew(i_cur)==i_cur){
						int vtmp=m.AddVertex(p_cur);
						edges(i_cur,i_prev).SetNew(i_cur,vtmp);
						offset.SetOffset(vtmp, SC_Offset(p_cur, !(p_prev-p_cur)) );	

					}
					if(edges(i_cur, i_next).GetNew(i_cur)==i_cur){
						int vtmp=m.AddVertex(p_cur);
						edges(i_cur,i_next).SetNew(i_cur,vtmp);
						offset.SetOffset(vtmp, SC_Offset(p_cur, !(p_next-p_cur)) );	
					}
					sd.AddFace(m.AddFace(CPolygon(edges(i_cur, i_next).GetNew(i_cur), 
						new_face_verts[i], 
						edges(i_cur, i_prev).GetNew(i_cur),
						i_cur)));
					sd.AddFaceBoundary(edges(i_cur, i_next).GetNew(i_cur));
					sd.AddFaceBoundary(new_face_verts[i]);
					sd.AddFaceBoundary(edges(i_cur, i_prev).GetNew(i_cur));
				}
			}
			else if(prev_edge_sel && !next_edge_sel){
				AutoAlloc<BaseSelect> tmp_sel;
				tmp_sel->Select(4*pind+next_edge_index);
				mesh->ValidateEdgeSelection(tmp_sel);
				Bool IsNgonEdge=!(tmp_sel->IsSelected(4*pind+next_edge_index));

				if(edges(i_cur, i_next).GetNew(i_cur)==i_cur){
					new_face_verts[i]=m.AddVertex(p_cur);
					if(CurrentPolygonIsNgon){ //ngon
						new_vertex_sel.push_back(new_face_verts[i]);
					}


					Vector off =GetOffset(p_prev-p_cur, p_next-p_cur, IsNgonEdge);

					offset.SetOffset(new_face_verts[i], SC_Offset(p_cur, off) );
					edges(i_cur,i_next).SetNew(i_cur,new_face_verts[i]);

					if(vertices[i_cur].GetType()==TYPE_ENDPOINT){
						openEdges.insert(SC_Edge(i_cur,i_next));
						edges(i_cur, i_next).SetIsOpen(i_cur,true);
					}
					else{
						TraverseAdjacent(pind,i_cur,i_next, edges, neighbors, selectedEdges, vertexOps, polys);
					}
				}
				else{
					new_face_verts[i]=edges(i_cur,i_next).GetNew(i_cur);
					if(CurrentPolygonIsNgon){ //ngon
						new_vertex_sel.push_back(new_face_verts[i]);
					}
					Vector off=offset.GetOffset(new_face_verts[i]).dir;
					Vector new_off=GetOffset(p_prev-p_cur, p_next-p_cur, IsNgonEdge);

					off =Merge(off,new_off);
					offset.SetOffset(new_face_verts[i], SC_Offset(p_cur,off) );

				}
				vertexOps.push_back(SC_ChangePolyVertexOp(pind, i, new_face_verts[i]));

				if(vertices[i_cur].GetType()==TYPE_CORNER){
					//Termination corner
					if(edges(i_cur, i_prev).GetNew(i_cur)==i_cur){
						int vtmp=m.AddVertex(p_cur);	
						edges(i_cur,i_prev).SetNew(i_cur,vtmp);
						offset.SetOffset(vtmp, SC_Offset(p_cur, !(p_prev-p_cur)) );
					}

					sd.AddFace(m.AddFace(CPolygon(edges(i_cur, i_next).GetNew(i_cur), 
						edges(i_cur, i_prev).GetNew(i_cur), 
						i_cur)));
					sd.AddFaceBoundary(edges(i_cur, i_next).GetNew(i_cur));
					sd.AddFaceBoundary(edges(i_cur, i_prev).GetNew(i_cur));


				}
			}
			else if(!prev_edge_sel && next_edge_sel){	
				AutoAlloc<BaseSelect> tmp_sel;
				tmp_sel->Select(4*pind+prev_edge_index);
				mesh->ValidateEdgeSelection(tmp_sel);
				Bool IsNgonEdge=!(tmp_sel->IsSelected(4*pind+prev_edge_index));
				if(edges(i_cur, i_prev).GetNew(i_cur)==i_cur){
					new_face_verts[i]=m.AddVertex(p_cur);
					if(ngonbase->FindPolygon(pind)!=NOTOK){ //Is ngon
						new_vertex_sel.push_back(new_face_verts[i]);
					}

					Vector off= GetOffset(p_next-p_cur, p_prev-p_cur, IsNgonEdge);

					offset.SetOffset(new_face_verts[i], SC_Offset(p_cur, off) );
					edges(i_cur,i_prev).SetNew(i_cur,new_face_verts[i]);


					if(vertices[i_cur].GetType()==TYPE_ENDPOINT){
						openEdges.insert(SC_Edge(i_cur,i_prev));
						edges(i_cur, i_prev).SetIsOpen(i_cur,true);
					}
					else{
						TraverseAdjacent(pind,i_cur,i_prev, edges, neighbors, selectedEdges, vertexOps, polys);	
					}
				}
				else{
					new_face_verts[i]=edges(i_cur,i_prev).GetNew(i_cur);
					Vector off=offset.GetOffset(new_face_verts[i]).dir;
					Vector new_off=GetOffset(p_next-p_cur, p_prev-p_cur, IsNgonEdge);

					off=Merge(off, new_off);
					offset.SetOffset(new_face_verts[i], SC_Offset(p_cur,off) );
				}
				vertexOps.push_back(SC_ChangePolyVertexOp(pind, i, new_face_verts[i]));

				if(vertices[i_cur].GetType()==TYPE_CORNER){
					//Termination corner
					if(edges(i_cur, i_next).GetNew(i_cur)==i_cur){
						int vtmp=m.AddVertex(p_cur);	
						edges(i_cur,i_next).SetNew(i_cur,vtmp);
						offset.SetOffset(vtmp, SC_Offset(p_cur, !(p_next-p_cur)) );
					}
					sd.AddFace(m.AddFace(CPolygon(i_cur,
						edges(i_cur, i_next).GetNew(i_cur), 
						edges(i_cur, i_prev).GetNew(i_cur))));
					sd.AddFaceBoundary(edges(i_cur, i_next).GetNew(i_cur));
					sd.AddFaceBoundary(edges(i_cur, i_prev).GetNew(i_cur));

				}
			}
		}

		LONG new_face_id;
		for(int i=0; i<nsides; i++){
			i_cur=verts[i];
			i_next=verts[GetNext(i,nsides)];
			p_cur=points[i_cur];
			p_next=points[i_next];
			if(f.IsSelected(i)){
				new_face_id=m.AddFace(CPolygon(edges(i_cur, i_next).GetNew(i_next),
					new_face_verts[GetNext(i,nsides)],
					new_face_verts[i],
					edges(i_cur, i_next).GetNew(i_cur)));
				new_edges.insert(4*new_face_id+1);
				sd.AddStrip(new_face_id);
				sd.AddStripBoundary(new_face_verts[GetNext(i,nsides)]);
				sd.AddStripBoundary(new_face_verts[i]);
			}
		}
	}

	m.Commit();	

	point_selection->DeselectAll();
	for(unsigned int i=0; i<new_vertex_sel.size(); i++){
		point_selection->Select(new_vertex_sel[i]);
	}

	for(unsigned int i=0; i<vertexOps.size(); i++ ){
		vertexOps[i].Execute(mesh);
	}

	set<LONG>::iterator it;
	for(it=new_edges.begin(); it!=new_edges.end(); it++){
		edgeSelection->Select(*it);
	}

	polys=mesh->GetPolygonW();
	map<LONG,LONG>::iterator iter2;
	for(iter2=openFaceEdges.begin(); iter2!=openFaceEdges.end(); iter2++){

		LONG pind=iter2->first/8;
		LONG eind=(iter2->first%8)/2;

		nsides=4;

		verts[0]=polys[pind].a; 
		verts[1]=polys[pind].b; 
		verts[2]=polys[pind].c; 
		verts[3]=polys[pind].d;


		if(verts[2]==verts[3]){nsides=3;}
		LONG p1=verts[eind];
		LONG p2=verts[GetNext(eind,nsides)];

		LONG adjacent=p1;
		if((iter2->first%8)%2==0){adjacent=p2;}

		LONG new_p=iter2->second;

		edges(p1,p2).SetNew(adjacent,new_p);
		edges(p1, p2).SetIsOpen(adjacent,true);
		openEdges.insert(SC_Edge(p1,p2));

	}


	AutoAlloc<Modeling> mod;

	set<SC_Edge,SC_Edge>::iterator iter;

	mod->InitObject(mesh); //Do checks here!
	SC_Edge e;
	SC_Edgedata edata;
	LONG p1, p2,new1,new2;

	for(iter=openEdges.begin(); iter!=openEdges.end(); iter++){
		e=*iter;
		p1=e.GetFirst();
		p2=e.GetSecond();
		edata=edges(p1, p2);
		new1=edata.GetNew(p1);
		new2=edata.GetNew(p2);

		if(edata.IsOpen(p1) && !(edata.IsOpen(p2))){
			AddEdgePoint(mesh, p1, new2, new1, mod);
			sd.AddOpenEdge(SC_Edge(p1,new1));
		}
		if(edata.IsOpen(p2) && !(edata.IsOpen(p1))){
			AddEdgePoint(mesh, p2, new1, new2, mod);
			sd.AddOpenEdge(SC_Edge(p2,new2));
		}
		if(edata.IsOpen(p2) && edata.IsOpen(p1)){
			AddEdgePoint(mesh, p1, p2, new1, mod);
			AddEdgePoint(mesh, new1, p2, new2, mod);
			sd.AddOpenEdge(SC_Edge(p1,new1));
			sd.AddOpenEdge(SC_Edge(p2,new2));
		}

	}


	mod->Commit();

}