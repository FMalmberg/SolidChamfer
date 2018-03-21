#ifndef SOLIDCHAMFER
#define SOLIDCHAMFER

#include "c4d.h"
#include "SC_Offsetdata.h" 
#include "SC_SubdivisionData.h"

enum CORNER_STYLE {CORNER_CLOSED, CORNER_OPEN};

void SolidChamfer(PolygonObject* mesh, BaseSelect* edgeSelection, SC_Offsetdata& offset, SC_SubdivisionData& sd, CORNER_STYLE cs);

#endif