// Solid chamfer tool

#include "c4d.h"
#include "c4d_symbols.h"
#include "SolidChamfer.h"
#include "SC_ToolDialog.h"


class SC_Tool : public ToolData
{

public:
	virtual Bool			MouseInput(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, EditorWindow *win, const BaseContainer &msg);
	virtual LONG			GetState(BaseDocument *doc);
	virtual Bool			InitTool(BaseDocument* doc, BaseContainer& data, BaseThread* bt);

	virtual SubDialog*	AllocSubDialog(BaseContainer* bc);
	Vector GetScale(PolygonObject* obj, BaseSelect* EdgeSelection, BaseDraw* bd);
};

LONG SC_Tool::GetState(BaseDocument *doc)
{
	if (doc->GetMode()==Medges){
		return CMD_ENABLED;}
	return 0;
}

Bool SC_Tool::InitTool(BaseDocument* doc, BaseContainer& data, BaseThread* bt){
	return TRUE;
}

Bool SC_Tool::MouseInput(BaseDocument *doc, BaseContainer &data, BaseDraw *bd,EditorWindow *win, const BaseContainer &msg)
{
	Real mx = msg.GetReal(BFM_INPUT_X);
	Real my = msg.GetReal(BFM_INPUT_Y);

	if(msg.GetLong(BFM_INPUT_CHANNEL)!=BFM_INPUT_MOUSELEFT){ return TRUE;}
	LONG button=KEY_MLEFT;

	BaseObject* obj=doc->GetActiveObject();
	if(obj==NULL){return TRUE;}
	if (!obj->IsInstanceOf(Opolygon)) return TRUE;

	doc->StartUndo();
	doc->AddUndo(UNDOTYPE_CHANGE,obj);

	LONG subdivision= SC_ToolDialog::GetSubdivision();

	PolygonObject* mesh = (PolygonObject*)obj;
	BaseSelect* edgeSelection =mesh->GetEdgeS();

	SC_Offsetdata o;
	SC_SubdivisionData sd(mesh);
	CORNER_STYLE cs=SC_ToolDialog::GetCornerstyle();
	SolidChamfer(mesh, edgeSelection, o, sd, cs);


	for(int i=0; i<subdivision; i++){
		sd.Subdivide(o);
	}

	BaseContainer device;
	Real dx,dy;

	AtomArray* a=AtomArray::Alloc();
	a->Append(obj);
	Real scale=GetToolScale(bd,a,FALSE,Medges);
	AtomArray::Free(a);
	Real r=0;

	AutoAlloc<Modeling> mod;
	mod->InitObject(mesh); //Do checks here!

	win->MouseDragStart(button,mx,my,MOUSEDRAGFLAGS_0);
	while (win->MouseDrag(&dx,&dy,&device)==MOUSEDRAGRESULT_CONTINUE){
		r+=dx;
		if(r<0){r=0;}
		o.CalculateOffset(mesh, mod, scale*r);
		mod->Commit(mesh,MODELING_COMMIT_UPDATE);
		obj->SetDirty(DIRTYFLAGS_DATA);
	
		DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW|DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_ANIMATION);
	}
	if (win->MouseDragEnd()==MOUSEDRAGRESULT_ESCAPE)
	{
		doc->EndUndo();
		doc->DoUndo();
	}

	doc->EndUndo();
	EventAdd();
	return TRUE;
}

SubDialog* SC_Tool::AllocSubDialog(BaseContainer* bc)
{
	return gNew SC_ToolDialog(bc); 
}



//ID obtained from plugincafe:
#define ID_SC_TOOL 1026425 

Bool RegisterSCTool(void)
{
	return RegisterToolPlugin(ID_SC_TOOL,"SolidChamfer",0,AutoBitmap("SolidChamfer.tif"),"Apply Solidchamfer to selected edges.",gNew SC_Tool);
}
