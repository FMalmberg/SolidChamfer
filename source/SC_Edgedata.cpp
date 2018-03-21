#include "SC_Edgedata.h"
#include <algorithm>
using namespace std;

SC_Edgedata::SC_Edgedata(){
	a=0; b=0; new_a=0; new_b=0; a_open=false; b_open=false;
}

SC_Edgedata::SC_Edgedata(LONG v1, LONG v2){
	a=min(v1,v2);
	b=max(v1,v2);
	new_a=a;
	new_b=b;
	a_open=false; 
	b_open=false;
}

LONG SC_Edgedata::GetNew(LONG v){
	if(v==a){
		return new_a;
	}
	else{
		return new_b;
	}	
}
void SC_Edgedata::SetNew(LONG v, LONG new_v){
	if(v==a){
		new_a=new_v;
	}
	else{
		new_b=new_v;
	}	
}

bool SC_Edgedata::IsOpen(LONG v){
	if(v==a){
		return a_open;
	}
	else{
		return b_open;
	}	
}
void SC_Edgedata::SetIsOpen(LONG v, bool IsOpen){
	if(v==a){
		a_open=IsOpen;
	}
	else{
		b_open=IsOpen;
	}	
}
