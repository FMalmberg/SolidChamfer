#include "AddEdgePoint.h"
#include <vector>
using namespace std;

void AddEdgePoint(C4DAtom* op, LONG a, LONG b, LONG c, Modeling* mod){
	LONG pcnt;

	LONG* polys=mod->GetEdgeNgons(op, a, b, pcnt);
	
	Ngon ngon, new_ngon;

	
	for(int i=0; i<pcnt; i++){

		mod->GetNgon(op,polys[i], &ngon);
		vector<LONG> new_verts;
		LONG seg=0;
		LONG segsum=ngon.segments[0];

		for(int j=0; j<ngon.count-1; j++){
			if(j==segsum){
				seg++;
				segsum+=ngon.segments[seg];
			}
			new_verts.push_back(ngon.points[j]);
			if( (ngon.points[j]==a && ngon.points[j+1]==b) || (ngon.points[j]==b && ngon.points[j+1]==a) ){
					ngon.segments[seg]++;
					new_verts.push_back(c);

			}
		}
		new_verts.push_back(ngon.points[ngon.count-1]);
		if( (ngon.points[ngon.count-1]==a && ngon.points[0]==b) || (ngon.points[ngon.count-1]==b && ngon.points[0]==a) ){
				ngon.segments[seg]++;	
				new_verts.push_back(c);
		}

		new_ngon.CopyFrom(new_verts.size(), ngon.segcount,&new_verts[0], ngon.segments);

		mod->DeleteNgon(op,polys[i],false);
		mod->AddNgon(op, new_ngon);

	


	}
	mod->FreeTable(op,polys);

}