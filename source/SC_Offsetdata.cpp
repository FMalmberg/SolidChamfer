#include "SC_Offsetdata.h"
using namespace std;

void SC_Offsetdata::SetOffset(int i, SC_Offset o){
	data[i]=o;
}

SC_Offset SC_Offsetdata::GetOffset(int i){
	return data[i];
}

void SC_Offsetdata::CalculateOffset(PolygonObject* obj,Modeling* mod, Real r){
	Vector* points=obj->GetPointW();

	map<int, SC_Offset>::iterator it;
	SC_Offset o;
	for(it=data.begin(); it!=data.end(); it++){
		mod->SetPoint(obj, it->first, it->second.p +r*it->second.dir);
	}
	obj->GetAndBuildNgon();
}