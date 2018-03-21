#include "SC_Edge.h"
#include <algorithm>
using namespace std;

SC_Edge::SC_Edge(LONG v1, LONG v2){
	a=min(v1,v2);
	b=max(v1,v2);
}

SC_Edge::SC_Edge(){
	a=b=0;
}

LONG SC_Edge::GetFirst(){
	return a;
}

LONG SC_Edge::GetSecond(){
	return b;
}

bool SC_Edge::operator()(const SC_Edge& e1, const SC_Edge& e2) const{
	bool smaller;
	if(e1.a== e2.a){
		smaller=e1.b<e2.b;
	}
	else{
		smaller=e1.a<e2.a;
	}
	return smaller;
}