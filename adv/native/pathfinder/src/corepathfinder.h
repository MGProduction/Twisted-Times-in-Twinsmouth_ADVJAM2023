#ifndef _COREPATHFINDER_H_
#define _COREPATHFINDER_H_

typedef struct{
 float X,Y;
}Vector2;

int  LineSegmentsCross(Vector2 a, Vector2 b, Vector2 c, Vector2 d);
int  IsVertexConcave(Vector2*vertices,int verticesCount, int vertex);
int  IsPntInPoly(Vector2*vert,int nvert,Vector2 test);
int  FindNearestInsidePoint(Vector2*vert,int nvert,Vector2 test,Vector2*nearest);
int  InLineOfSight(Vector2*polygon,int polygoncnt, Vector2 start, Vector2 end,int mask);
int  FindPath(int*cache,Vector2*vert,char*used,int nvert,Vector2 start,int idx,Vector2 end,Vector2*path,int depth,int*bdepth,int dist,int*bdist,Vector2*selpath);

int doFindPath(Vector2*vert,int nvert,Vector2 start,Vector2 end,Vector2*selpath);

#endif