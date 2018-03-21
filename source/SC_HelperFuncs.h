#ifndef SC_HELPERFUNCS
#define SC_HELPERFUNCS

//Assumes that 0<=v<nelem
int GetNext(int v, int nelem){
	int next=v+1;
	if(next==nelem){next=0;}
	return next;
}

//Assumes that 0<=v<nelem
int GetPrevious(int v, int nelem){
	int prev=v-1;
	if(prev<0){prev=nelem-1;}
	return prev;
}

//Get the local index (0-3) of the vertex adjacent to a, on the opposite side of b
//Assumes that a and b are adjacent vertices in p.
LONG GetNextVert(LONG a, LONG b, CPolygon& p){
	LONG verts[4];
	verts[0]=p.a;
	verts[1]=p.b;
	verts[2]=p.c;
	verts[3]=p.d;
	
	int nverts=4; //quad
	if(p.c==p.d){nverts=3;} //triangle

	int i_a=0;
	int i_b=0;
	LONG result=0;

	for(int i=0; i<nverts; i++){
		if(verts[i]==a){i_a=i;}
		if(verts[i]==b){i_b=i;}
	}

	if(i_b==GetPrevious(i_a, nverts)){
		result=verts[GetNext(i_a, nverts)];
	}

	if(i_b==GetNext(i_a, nverts)){
		result=verts[GetPrevious(i_a, nverts)];
	}

	return result;
}

//Get the local index (0-3) of vertex v
int GetVertexId(LONG v, CPolygon& p){
	LONG verts[4];
	int result;
	verts[0]=p.a;
	verts[1]=p.b;
	verts[2]=p.c;
	verts[3]=p.d;
	
	int nverts=4; //quad
	if(p.c==p.d){nverts=3;} //triangle

	for(int i=0; i<nverts; i++){
		if(verts[i]==v){result=i;}
	}

	return result;
}

#endif