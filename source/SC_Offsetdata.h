#ifndef SC_OFFSETDATA_H
#define	SC_OFFSETDATA_H

#include <map>
#include "c4d.h"
#include "lib_modeling.h"

class SC_Offset{
public:
	Vector p; //original point
	Vector dir; //offset direction
public:
	SC_Offset(){
		p=Vector(0,0,0);
		dir=Vector(0,0,0);
	}

	SC_Offset(Vector origin, Vector direction){
		p=origin;
		dir=direction;
	}
};

class SC_Offsetdata{
private:
	std::map<int,SC_Offset> data;
public:
	void SetOffset(int i, SC_Offset);
	SC_Offset GetOffset(int i);
	void CalculateOffset(PolygonObject* obj, Modeling* mod, Real r);
};

#endif