#if !defined(WIN32)
#include <dmsdk/sdk.h>
#endif

#include <stdlib.h>
#include <math.h>

#include "corepathfinder.h"
/*
int LineSegmentsCross(Vector2 a, Vector2 b, Vector2 c, Vector2 d)
{
    float denominator = ((b.X - a.X) * (d.Y - c.Y)) - ((b.Y - a.Y) * (d.X - c.X)),numerator1,numerator2,r,s;

    if (denominator == 0)
    {
        return 0;
    }

    numerator1 = ((a.Y - c.Y) * (d.X - c.X)) - ((a.X - c.X) * (d.Y - c.Y));

    numerator2 = ((a.Y - c.Y) * (b.X - a.X)) - ((a.X - c.X) * (b.Y - a.Y));

    if (numerator1 == 0 || numerator2 == 0)
    {
        return 0;
    }

    r = numerator1 / denominator;
    s = numerator2 / denominator;

    return (r > 0 && r < 1) && (s > 0 && s < 1);
}

int IsVertexConcave(Vector2*vertices,int verticesCount, int vertex)
{
    Vector2 current = vertices[vertex];
    Vector2 next = vertices[(vertex + 1) % verticesCount];
    Vector2 previous = vertices[vertex == 0 ? verticesCount - 1 : vertex - 1];

    Vector2 left;
    Vector2 right;
    float   cross;

    left.X=current.X - previous.X;left.Y=current.Y - previous.Y;
    right.X=next.X - current.X;right.Y=next.Y - current.Y;

    cross = (left.X * right.Y) - (left.Y * right.X);

    return cross < 0;
}

float DistanceSquared(Vector2 a,Vector2 b)
{
 return sqrtf((a.X-b.X)*(a.X-b.X)+(a.Y-b.Y)*(a.Y-b.Y));
}

int Inside(Vector2*polygon,int polygoncnt, Vector2 position, int toleranceOnOutside)
{
    Vector2 point = position;

    const float epsilon = 0.5f;

    int inside = 0,i;
    float oldSqDist;

    // Must have 3 or more edges
    if (polygoncnt < 3) 
     return 0;
    else
    {
    Vector2 oldPoint = polygon[polygoncnt - 1];
    oldSqDist = DistanceSquared(oldPoint, point);

    for (i = 0; i < polygoncnt; i++)
    {
        Vector2 newPoint = polygon[i];
        float newSqDist = DistanceSquared(newPoint, point);

        if (oldSqDist + newSqDist + 2.0f * sqrt(oldSqDist * newSqDist) - DistanceSquared(newPoint, oldPoint) < epsilon)
            return toleranceOnOutside;
        else
         {
         Vector2 left;
         Vector2 right;
         if (newPoint.X > oldPoint.X)
         {
             left = oldPoint;
             right = newPoint;
         }
         else
         {
             left = newPoint;
             right = oldPoint;
         }

         if (left.X < point.X && point.X <= right.X && (point.Y - left.Y) * (right.X - left.X) < (right.Y - left.Y) * (point.X - left.X))
             inside = !inside;

         oldPoint = newPoint;
         oldSqDist = newSqDist;
         }
    }
    }

    return inside;
}



int IsPntInPoly(Vector2*vert,int nvert,Vector2 test)
{
  int i, j, c = 0;
  for (i = 0, j = nvert-1; i < nvert; j = i++) {
    if ( ((vert[i].Y>test.Y) != (vert[j].Y>test.Y)) &&
     (test.X < (vert[j].X-vert[i].X) * (test.Y-vert[i].Y) / (vert[j].Y-vert[i].Y) + vert[i].X) )
       c = !c;
  }
  return c;
}

int InLineOfSight(Vector2*polygon,int polygoncnt, Vector2 start, Vector2 end,int mask)
{
  const float epsilon = 0.5f;
  int i;
  Vector2 avg;
  // Not in LOS if any of the ends is outside the polygon
  if((mask&1)&&(!IsPntInPoly(polygon,polygoncnt,start)))
  //if(!Inside(polygon,polygoncnt,start))
   return 0;

  //if(!Inside(polygon,polygoncnt,end))
  if(!IsPntInPoly(polygon,polygoncnt,end))
   return 0;

  // In LOS if it's the same start and end location
  if (DistanceSquared(start, end) < epsilon) 
   return 1;

  // Not in LOS if any edge is intersected by the start-end line segment  
  for (i = 0; i < polygoncnt; i++)
   if (LineSegmentsCross(start, end, polygon[i], polygon[(i+1)%polygoncnt]))
     return 0;  

  // Finally the middle point in the segment determines if in LOS or not
  
  avg.X=(start.X + end.X) / 2;
  avg.Y=(start.Y + end.Y) / 2;
  return IsPntInPoly(polygon,polygoncnt,avg);
}*/
float distanceFromPoint(Vector2 p,Vector2 l1,Vector2 l2)
{
    float A = p.X - l1.X;
    float B = p.Y - l1.Y;
    float C = l2.X - l1.X;
    float D = l2.Y - l1.Y;

    float dot = A * C + B * D;
    float len_sq = C * C + D * D;
    float param = dot / len_sq;

    float xx, yy,dx,dy;

    if( ( param < 0) || ((l1.X == l2.X) && (l1.Y == l2.Y)) )
    {
        xx = l1.X;
        yy = l1.Y;
    } 
    else if (param > 1) 
    {
        xx = l2.X;
        yy = l2.Y;
    }
    else 
    {
        xx = l1.X + param * C;
        yy = l1.Y + param * D;
    }

    dx = p.X - xx;
    dy = p.Y - yy;

    return (float)sqrt(dx * dx + dy * dy);
}
float PointSegmentDistanceSquared( float px, float py,
                                    float p1x, float p1y,
                                    float p2x, float p2y,
                                    float* t,
                                    float* qx, float* qy)
{
    static const float kMinSegmentLenSquared = 0.000001f;  // adjust to suit.  If you use float, you'll probably want something like 0.000001f
    static const float kEpsilon = 1E-7f;  // adjust to suit.  If you use floats, you'll probably want something like 1E-7f
    float dx = p2x - p1x;
    float dy = p2y - p1y;
    float dp1x = px - p1x;
    float dp1y = py - p1y;
    float dpqx,dpqy;
    const float segLenSquared = (dx * dx) + (dy * dy);
    if (segLenSquared >= -kMinSegmentLenSquared && segLenSquared <= kMinSegmentLenSquared)
    {
        // segment is a point.
        *qx = p1x;
        *qy = p1y;
        *t = 0.0;
        return ((dp1x * dp1x) + (dp1y * dp1y));
    }
    else
    {
        // Project a line from p to the segment [p1,p2].  By considering the line
        // extending the segment, parameterized as p1 + (t * (p2 - p1)),
        // we find projection of point p onto the line. 
        // It falls where t = [(p - p1) . (p2 - p1)] / |p2 - p1|^2
        *t = ((dp1x * dx) + (dp1y * dy)) / segLenSquared;
        if (*t < kEpsilon)
        {
            // intersects at or to the "left" of first segment vertex (p1x, p1y).  If t is approximately 0.0, then
            // intersection is at p1.  If t is less than that, then there is no intersection (i.e. p is not within
            // the 'bounds' of the segment)
            if (*t > -kEpsilon)
            {
                // intersects at 1st segment vertex
                *t = 0.0;
            }
            // set our 'intersection' point to p1.
            *qx = p1x;
            *qy = p1y;
            // Note: If you wanted the ACTUAL intersection point of where the projected lines would intersect if
            // we were doing PointLineDistanceSquared, then qx would be (p1x + (t * dx)) and qy would be (p1y + (t * dy)).
        }
        else if (*t > (1.0 - kEpsilon))
        {
            // intersects at or to the "right" of second segment vertex (p2x, p2y).  If t is approximately 1.0, then
            // intersection is at p2.  If t is greater than that, then there is no intersection (i.e. p is not within
            // the 'bounds' of the segment)
            if (*t < (1.0 + kEpsilon))
            {
                // intersects at 2nd segment vertex
                *t = 1.0;
            }
            // set our 'intersection' point to p2.
            *qx = p2x;
            *qy = p2y;
            // Note: If you wanted the ACTUAL intersection point of where the projected lines would intersect if
            // we were doing PointLineDistanceSquared, then qx would be (p1x + (t * dx)) and qy would be (p1y + (t * dy)).
        }
        else
        {
            // The projection of the point to the point on the segment that is perpendicular succeeded and the point
            // is 'within' the bounds of the segment.  Set the intersection point as that projected point.
            *qx = p1x + (*t * dx);
            *qy = p1y + (*t * dy);
        }
        // return the squared distance from p to the intersection point.  Note that we return the squared distance
        // as an optimization because many times you just need to compare relative distances and the squared values
        // works fine for that.  If you want the ACTUAL distance, just take the square root of this value.
         dpqx = px - *qx;
         dpqy = py - *qy;
        return ((dpqx * dpqx) + (dpqy * dpqy));
    }
}

int FindNearestInsidePoint(Vector2*vert,int nvert,Vector2 test,Vector2*nearest)
{
 int   i,bi=-1;
 float tdist=100000;
 for(i=0;i<nvert;i++)
  {
   float cdist=distanceFromPoint(test,vert[i],vert[(i+1)%nvert]);
   if(cdist<tdist)
    {
     tdist=cdist;
     bi=i;
    }
  }
 if(nearest)
  {
   float px,py,t;
   PointSegmentDistanceSquared(test.X,test.Y,vert[bi].X,vert[bi].Y,vert[(bi+1)%nvert].X,vert[(bi+1)%nvert].Y,&t,&px,&py);
   nearest->X=px;
   nearest->Y=py;
  }
 return 1;
}
/*
int ptdist(Vector2 a,Vector2 b)
{
 return (int)floor((a.X-b.X)*(a.X-b.X)+(a.Y-b.Y)*(a.Y-b.Y));
}

int seq_compare(const void*a,const void*b)
{
 int*A=(int*)a;
 int*B=(int*)b;
 return A[2]-B[2];
}

#define MAXPATH 10

int FindPath(int*cache,Vector2*vert,char*used,int nvert,Vector2 start,int idx,Vector2 end,Vector2*path,int depth,int*bdepth,int dist,int*bdist,Vector2*selpath)
{ 
 if(depth<MAXPATH)
  {
  int cdist=0;
  if(cache&&(idx!=-1))
   {
    if(cache[idx*nvert+nvert]==0)
     if(InLineOfSight(vert,nvert,start,end,255-(depth==0)))
      cdist=cache[idx*nvert+nvert]=ptdist(start,end);
     else
      cdist=cache[idx*nvert+nvert]=-1;
    else
     cdist=cache[idx*nvert+nvert];
   }
  else
   if(InLineOfSight(vert,nvert,start,end,255-(depth==0)))
    cdist=ptdist(start,end);
   else
    cdist=0;
  if(cdist>0)//InLineOfSight(vert,nvert,start,end))
   {   
    int n;   
    int ddist=dist+cdist;
    if(ddist<*bdist)
     {
      path[depth]=end;    
      *bdist=ddist;    
      *bdepth=depth+1;
      // --- dmLogInfo("found=%d / %d\n",*bdist,*bdepth);
      for(n=0;n<depth+1;n++)
       selpath[n]=path[n];
    }
   }
  else
   {
    int i,iseq=0;
    int*seq=(int*)calloc(nvert,sizeof(int)*3);
    for(i=0;i<nvert;i++)
     if(used[i]==0)
      {
       int cdist=0;     
       if(cache&&(idx!=-1))
        {
         if(cache[idx*nvert+i]==0)
          if(InLineOfSight(vert,nvert,start,vert[i],255-(depth==0)))
           cdist=cache[idx*nvert+i]=ptdist(start,vert[i]);
          else
           cdist=cache[idx*nvert+i]=-1;
         else
          cdist=cache[idx*nvert+i];
        }
       else
        if(InLineOfSight(vert,nvert,start,vert[i],255-(depth==0)))
         cdist=ptdist(start,vert[i]);
        else
         cdist=0;     
       if((cdist>0)&&(dist+cdist<*bdist))
        {
         seq[iseq*3]=i;
         seq[iseq*3+1]=dist+cdist;
         seq[iseq*3+2]=ptdist(vert[i],end);
         iseq++;
        }
      }
    if(iseq)
    {
     int j;
     qsort(seq,iseq,sizeof(int)*3,seq_compare);
     for(j=0;j<iseq;j++)
      {
       int i,ddist;
       i=seq[j*3];ddist=seq[j*3+1];
       if(ddist<*bdist)
        {
         used[i]=1;path[depth]=vert[i];
         // --- dmLogInfo("*i=%d\n",i);
         FindPath(cache,vert,used,nvert,vert[i],i,end,path,depth+1,bdepth,ddist,bdist,selpath);      
         used[i]=0;
        }
      }
    }
    free(seq);
   }
  }
 return 0;
}
*/

//  Public-domain code by Darel Rex Finley, 2006.



//  (This function automatically knows that enclosed polygons are "no-go"
//  areas.)

#define YES true
#define NO  false

typedef struct{
 int     corners,m;
 Vector2*pt;
}polygon;

typedef struct{
 double x,y;
 double totalDist;
 int    prev;
}point;

typedef struct{
 int     count,m;
 polygon*poly;
}polySet;

int pointInPolygonSet(double testX, double testY, polySet allPolys) {

  int   oddNodes=NO ;
  int   polyI, i, j ;

  for (polyI=0; polyI<allPolys.count; polyI++) {
    for (i=0;    i< allPolys.poly[polyI].corners; i++) {
      j=i+1; if (j==allPolys.poly[polyI].corners) j=0;
      if   ( allPolys.poly[polyI].pt[i].Y< testY
      &&     allPolys.poly[polyI].pt[j].Y>=testY
      ||     allPolys.poly[polyI].pt[j].Y< testY
      &&     allPolys.poly[polyI].pt[i].Y>=testY) {
        if ( allPolys.poly[polyI].pt[i].X+(testY-allPolys.poly[polyI].pt[i].Y)
        /   (allPolys.poly[polyI].pt[j].Y       -allPolys.poly[polyI].pt[i].Y)
        *   (allPolys.poly[polyI].pt[j].X      -allPolys.poly[polyI].pt[i].X)<testX) {
          oddNodes=!oddNodes; }}}}

  return oddNodes; }



//  This function should be called with the full set of *all* relevant polygons.
//  (The algorithm automatically knows that enclosed polygons are �no-go�
//  areas.)
//
//  Note:  As much as possible, this algorithm tries to return YES when the
//         test line-segment is exactly on the border of the polygon, particularly
//         if the test line-segment *is* a side of a polygon.

int lineInPolygonSet(double testSX, double testSY, double testEX, double testEY, polySet allPolys) {

  double  theCos, theSin, dist, sX, sY, eX, eY, rotSX, rotSY, rotEX, rotEY, crossX ;
  int     i, j, polyI ;

  testEX-=testSX;
  testEY-=testSY; dist=sqrt(testEX*testEX+testEY*testEY);
  theCos =testEX/ dist;
  theSin =testEY/ dist;

  for (polyI=0; polyI<allPolys.count; polyI++) {
    for (i=0;    i< allPolys.poly[polyI].corners; i++) {
      j=i+1; if (j==allPolys.poly[polyI].corners) j=0;

      sX=allPolys.poly[polyI].pt[i].X-testSX;
      sY=allPolys.poly[polyI].pt[i].Y-testSY;
      eX=allPolys.poly[polyI].pt[j].X-testSX;
      eY=allPolys.poly[polyI].pt[j].Y-testSY;
      if (sX==0. && sY==0. && eX==testEX && eY==testEY
      ||  eX==0. && eY==0. && sX==testEX && sY==testEY) {
        return YES; }

      rotSX=sX*theCos+sY*theSin;
      rotSY=sY*theCos-sX*theSin;
      rotEX=eX*theCos+eY*theSin;
      rotEY=eY*theCos-eX*theSin;
      if (rotSY<0. && rotEY>0.
      ||  rotEY<0. && rotSY>0.) {
        crossX=rotSX+(rotEX-rotSX)*(0.-rotSY)/(rotEY-rotSY);
        if (crossX>=0. && crossX<=dist) return NO; }

      if ( rotSY==0.   && rotEY==0.
      &&  (rotSX>=0.   || rotEX>=0.  )
      &&  (rotSX<=dist || rotEX<=dist)
      &&  (rotSX< 0.   || rotEX< 0.
      ||   rotSX> dist || rotEX> dist)) {
        return NO; }}}

  return pointInPolygonSet(testSX+testEX/2.,testSY+testEY/2.,allPolys); }



double calcDist(double sX, double sY, double eX, double eY) {
  eX-=sX; eY-=sY; return sqrt(eX*eX+eY*eY); }



void swapPoints(point *a, point *b) {
  point swap=*a; *a=*b; *b=swap; }



//  Finds the shortest path from sX,sY to eX,eY that stays within the polygon set.
//
//  Note:  To be safe, the solutionX and solutionY arrays should be large enough
//         to accommodate all the corners of your polygon set (although it is
//         unlikely that anywhere near that many elements will ever be needed).
//
//  Returns YES if the optimal solution was found, or NO if there is no solution.
//  If a solution was found, solutionX and solutionY will contain the coordinates
//  of the intermediate nodes of the path, in order.  (The startpoint and endpoint
//  are assumed, and will not be included in the solution.)

int adjustPoint(polySet allPolys,Vector2 start,Vector2*pstart)
{
 if(FindNearestInsidePoint(allPolys.poly->pt,allPolys.poly->corners,start,pstart))
  {
   double sX=pstart->X,sY=pstart->Y;
   if (!pointInPolygonSet(sX,sY,allPolys))
    {
     int x,y,bx=0,by=0;
     for(y=-1;y<=1;y++)
      for(x=-1;x<=1;x++)
       if(pointInPolygonSet(sX+x,sY+y,allPolys))
        {
         bx=x;by=y;
         break;
        }
     if(bx||by)
      {
       sX+=bx;sY+=by;
       pstart->X=sX;pstart->Y=sY;
       return YES;
      }
     else
      return NO;
    }
   else
    return YES;
  }
 else
  return NO;
}

int shortestPath(Vector2 start, Vector2 end, polySet allPolys,Vector2*path, int *solutionNodes) 
{

  #define  INF  9999999.     //  (larger than total solution dist could ever be)

  point  pointList[1000] ;   //  (enough for all polycorners plus two)
  int    pointCount      ;

  int     treeCount, polyI, i, j, bestI, bestJ ;
  double  bestDist, newDist ;
  double sX=start.X, sY=start.Y;
  double eX=end.X, eY=end.Y;

  //  Fail if either the startpoint or endpoint is outside the polygon set.
  if (!pointInPolygonSet(sX,sY,allPolys))
  {
   if(adjustPoint(allPolys,start,&start))
    {sX=start.X;sY=start.Y;}
   else
    return NO;
  }
  if(!pointInPolygonSet(eX,eY,allPolys))
  {
   if(adjustPoint(allPolys,end,&end))
    {eX=end.X;eY=end.Y;}
   else
    return NO; 
  }

  //  If there is a straight-line solution, return with it immediately.
  if (lineInPolygonSet(sX,sY,eX,eY,allPolys))
  {
    path[0].X=eX;
    path[0].Y=eY; 
    (*solutionNodes)=1; 
    return YES; 
  }

  //  Build a point list that refers to the corners of the
  //  polygons, as well as to the startpoint and endpoint.
  pointList[0].totalDist=0;pointList[0].prev=-1;
  pointList[0].x=sX;pointList[0].y=sY;pointCount=1;

  for (polyI=0; polyI<allPolys.count; polyI++) {
    for (i=0; i<allPolys.poly[polyI].corners; i++) {
      pointList[pointCount].totalDist=0;pointList[pointCount].prev=-1;
      pointList[pointCount].x=allPolys.poly[polyI].pt[i].X;
      pointList[pointCount].y=allPolys.poly[polyI].pt[i].Y; pointCount++; }}
  
  pointList[pointCount].totalDist=0;pointList[pointCount].prev=-1;
  pointList[pointCount].x=eX;pointList[pointCount].y=eY;pointCount++;

  //  Initialize the shortest-path tree to include just the startpoint.
  treeCount=1; pointList[0].totalDist=0.;

  //  Iteratively grow the shortest-path tree until it reaches the endpoint
  //  -- or until it becomes unable to grow, in which case exit with failure.
  bestJ=0;
  while (bestJ<pointCount-1)
  {
    bestDist=INF;
    for (i=0; i<treeCount; i++)
    {
      for (j=treeCount; j<pointCount; j++)
      {
        if (lineInPolygonSet(pointList[i].x,pointList[i].y, pointList[j].x,pointList[j].y,allPolys))
         {
           newDist=pointList[i].totalDist+calcDist(pointList[i].x,pointList[i].y,pointList[j].x,pointList[j].y);
           if (newDist<bestDist)
            {
              bestDist=newDist; 
              bestI=i; 
              bestJ=j; 
            }
         }
      }
    }
    if (bestDist==INF) 
     return NO;   //  (no solution)
    pointList[bestJ].prev     =bestI   ;
    pointList[bestJ].totalDist=bestDist;
    swapPoints(&pointList[bestJ],&pointList[treeCount]); treeCount++;
  }

  //  Load the solution arrays.
  (*solutionNodes)= -1; 
  i=treeCount-1;
  while (i> 0)
   {
     i=pointList[i].prev; (*solutionNodes)++;
   }
  j=(*solutionNodes); // --- qui c'era un -1 - mentre io voglio anche il punto finale
  i=treeCount-1;
  path[j].X=pointList[i].x; // ---
  path[j].Y=pointList[i].y; // ---
  j--; // ---
  while (j>=0)
  {
    i=pointList[i].prev;
    path[j].X=pointList[i].x;
    path[j].Y=pointList[i].y; 
    j--;
  }

  (*solutionNodes)++; // ---

  //  Success.
  return YES; 
}

int doFindPath(Vector2*vert,int nvert,Vector2 start,Vector2 end,Vector2*selpath)
{
 int     mdepth;
 polySet allpoly;
 polygon one;
 allpoly.count=allpoly.m=1;
 one.corners=one.m=nvert;
 one.pt=vert;
 allpoly.poly=&one;
 if(shortestPath(start,end,allpoly,selpath,&mdepth))
  return mdepth;
 else
  return 0;
}
/*
int _doFindPath(Vector2*vert,int nvert,Vector2 start,Vector2 end,Vector2*selpath)
{
 int     mdepth=0,mdist=0x7FFFFFFF;
 Vector2 path[MAXPATH];
 int*cache=(int*)calloc((nvert+1)*(nvert+1),sizeof(int));
 char*used=(char*)calloc(nvert,1);
 FindPath(&cache[0],
          &vert[0],&used[0],nvert,
          start,-1,end,
          &path[0],0,&mdepth,0,&mdist,
          &selpath[0]);
 free(used);
 free(cache);
 return mdepth;
}
*/