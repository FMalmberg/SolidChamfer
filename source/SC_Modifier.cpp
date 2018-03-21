

// SolidChamfer modifier

#include "c4d.h"
#include "c4d_symbols.h"
#include "osolidchamfer.h"
#include "SolidChamfer.h"

void AddDependence(BaseObject* op,HierarchyHelp* hh, BaseObject* object){
	while(object){
		op->AddDependence(hh,object);
		AddDependence(op,hh,object->GetDown());
		object=object->GetNext();
	}
}


class SC_Modifier : public ObjectData
{
public:
	virtual Bool Init(GeListNode *node);

	virtual BaseObject* GetVirtualObjects(BaseObject* op, HierarchyHelp* hh);

	static NodeData *Alloc(void) { return gNew SC_Modifier; }
};


BaseObject* SC_Modifier::GetVirtualObjects(BaseObject* op, HierarchyHelp* hh){

	op->NewDependenceList(); // start

	BaseObject* child=op->GetDown();
	if(!child ){ //|| !child->IsInstanceOf(Opolygon)){
		return PolygonObject::Alloc(0,0);
	}

	ModelingCommandData cd;
	cd.doc = op->GetDocument();
	cd.op = child;
	if (!SendModelingCommand(MCOMMAND_CURRENTSTATETOOBJECT, cd)) return FALSE;
	BaseObject* tmp=(BaseObject*)(cd.result->GetIndex(0));
	if(!tmp->IsInstanceOf(Opolygon)){
		return PolygonObject::Alloc(0,0);
	}

	PolygonObject* mesh = (PolygonObject*) tmp;

	//...go through all objects replaced by the generator...



	op->AddDependence(hh, child); // add this object to the list
	AddDependence(op,hh, child->GetDown());

	bool dirty = !op->CompareDependenceList(); // has any of those objects been changed?
	op->TouchDependenceList(); // mark those objects as 'used by generator'

	//if (!dirty || op->IsDirty(DIRTY_DATA) ){
	//	return op->GetCache(hh); // if nothing has changed return cache
	//}
	//else{
	//GePrint("Doing calculations");
	//BaseObject* modifier=(BaseObject*)(this->Get());
	BaseContainer* data=op->GetDataInstance();



	AutoAlloc<BaseSelect> edgeSelection;

	long mode=data->GetLong(SOLIDCHAMFER_EDGESELECTION);

	long npolys=mesh->GetPolygonCount();
	CPolygon* polys=mesh->GetPolygonW();

	BaseTag* tag;
	PolyInfo* pinfo;
	Vector norm1, norm2;

	switch ( mode ) {
	case  SC_LIVE: 
		mesh->GetEdgeS()->CopyTo(edgeSelection);
		break;
	case SC_ALL:
		for(int i=0; i<npolys; i++){
			edgeSelection->Select(i*4);
			edgeSelection->Select(i*4+1);
			edgeSelection->Select(i*4+2);
			if(polys[i].c!=polys[i].d){
				edgeSelection->Select(i*4+3);
			}
		}
		mesh->ValidateEdgeSelection(edgeSelection);
		break;
	case SC_PHONG:
		tag=mesh->GetTag(Tphong);
		if(tag){
			BaseContainer* tagdata=tag->GetDataInstance();
			bool useAngleLimit=tagdata->GetBool(PHONGTAG_PHONG_ANGLELIMIT);
			if(!useAngleLimit){break;}

			bool useBreaks=tagdata->GetBool(PHONGTAG_PHONG_USEEDGES);
			if(useBreaks){ mesh->GetPhongBreak()->CopyTo(edgeSelection);}

			float angle=tagdata->GetReal(PHONGTAG_PHONG_ANGLE);
			
			Neighbor n;
			n.Init(mesh->GetPointCount(), mesh->GetPolygonR(), mesh->GetPolygonCount(),NULL);
			for(int i=0; i<npolys; i++){
				norm1=CalcFaceNormal(mesh->GetPointR(), mesh->GetPolygonR()[i]);
				pinfo=n.GetPolyInfo(i);

				for(int j=0; j<4; j++){
					if(pinfo->face[j]!=NOTOK){
						norm2=CalcFaceNormal(mesh->GetPointR(), mesh->GetPolygonR()[pinfo->face[j]]);
						if(ACos(norm1*norm2)>=angle){	
							edgeSelection->Select(i*4+j);
						}
					}
				}

			
			}
			mesh->ValidateEdgeSelection(edgeSelection);
		}
		break;
	case SC_TAG:
		SelectionTag* tag=(SelectionTag*)(data->GetLink(SOLIDCHAMFER_TAGLINK,op->GetDocument()));
		if(tag){
			tag->GetBaseSelect()->CopyTo(edgeSelection);
		}
		break;
	}

	SC_Offsetdata o;
	SC_SubdivisionData sd(mesh);
	SolidChamfer(mesh, edgeSelection, o, sd, CORNER_CLOSED);


	long subdivision=data->GetLong(SOLIDCHAMFER_SUBDIVISION);
	double Radius=data->GetReal(SOLIDCHAMFER_RADIUS);

	for(int i=0; i<subdivision; i++){
		sd.Subdivide(o);
	}

	AutoAlloc<Modeling> mod;
	mod->InitObject(mesh); //Do checks here!

	o.CalculateOffset(mesh, mod, Radius);
	mesh->SetDirty(DIRTYFLAGS_DATA);

	mod->Commit();

	// send update message
	mesh->Message(MSG_UPDATE);

	return mesh;
	//}
}






Bool SC_Modifier::Init(GeListNode *node)
{	
	BaseObject		*op   = (BaseObject*)node;
	BaseContainer *data = op->GetDataInstance();

	data->SetReal(SOLIDCHAMFER_RADIUS,5.0);
	data->SetLong(SOLIDCHAMFER_SUBDIVISION,0);
	data->SetLong(SOLIDCHAMFER_EDGESELECTION,SC_LIVE);

	return TRUE;
}

//ID obtained from www.plugincafe.com
#define ID_SC_MODIFIER 1028443

Bool RegisterSCModifier(void)
{
	return RegisterObjectPlugin(ID_SC_MODIFIER,"SolidChamfer Modifier",OBJECT_GENERATOR|OBJECT_INPUT,SC_Modifier::Alloc,"osolidchamfer",AutoBitmap("SolidChamferModifier.tif"),0);
}
