/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFillingHole
 Authors: Fuli Wu, Josef Kohout
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "vtkALBAFillingHole.h"

#include "vtkDoubleArray.h"
#include "vtkLine.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkPolyData.h"
#include "vtkPriorityQueue.h"
#include "vtkTriangle.h"
#include "vtkCellArray.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkMatrix4x4.h"
#include <map>
#include <assert.h>
#include <float.h>


vtkCxxRevisionMacro(vtkALBAFillingHole, "$Revision: 1.1.2.6 $");
vtkStandardNewMacro(vtkALBAFillingHole);

#include "albaMemDbg.h"

#define MAXPATCHVERTEX 4096

#define VEC3_SQUAREDIST(a, b)          (((a)[0]-(b)[0])*((a)[0]-(b)[0]) +       \
  ((a)[1]-(b)[1])*((a)[1]-(b)[1]) +       \
  ((a)[2]-(b)[2])*((a)[2]-(b)[2]))

#pragma region Nested classes

//----------------------------------------------------------------------------
vtkALBAFillingHole::CVertex::CVertex(double *pCoord)
//----------------------------------------------------------------------------
{
  DCoord[0] = pCoord[0];
  DCoord[1] = pCoord[1];
  DCoord[2] = pCoord[2];
  DWeight = DOneRingEdgeLength = 0;
  BBoundary = BMarked = false;
}

//----------------------------------------------------------------------------
vtkALBAFillingHole::CVertex::~CVertex()
//----------------------------------------------------------------------------
{
  OneRingVertex.clear();
  OneRingTriangle.clear();
  OneRingEdge.clear();
  TwoRingVertex.clear();
}

//----------------------------------------------------------------------------
bool vtkALBAFillingHole::CVertex::IsTwoRingVertex(int id)
//----------------------------------------------------------------------------
{
  vtkstd::vector<int>::iterator end;

  end = TwoRingVertex.end();
  if( end == find(TwoRingVertex.begin(),end,id) ) return false;
  else		return true;
}

//----------------------------------------------------------------------------
vtkALBAFillingHole::CTriangle::CTriangle()
//----------------------------------------------------------------------------
{
  AEdge[0]=AEdge[1]=AEdge[2]=-1;
  BDeleted = BMarked = false;
}

//----------------------------------------------------------------------------
vtkALBAFillingHole::CTriangle::CTriangle(int v0,int v1,int v2)
//----------------------------------------------------------------------------
{
  AVertex[0]=v0;
  AVertex[1]=v1;
  AVertex[2]=v2;

  AEdge[0]=AEdge[1]=AEdge[2]=-1;
  BDeleted = BMarked = false;
}

//----------------------------------------------------------------------------
void vtkALBAFillingHole::CTriangle::SetEdge(int e0,int e1,int e2)
//----------------------------------------------------------------------------
{
  AEdge[0]=e0;
  AEdge[1]=e1;
  AEdge[2]=e2; 
}

//----------------------------------------------------------------------------
vtkALBAFillingHole::CEdge::CEdge()
//----------------------------------------------------------------------------
{
  AVertex[0]=AVertex[1]=AVertex[2]=AVertex[3]=-1;
  ATriangle[0]=ATriangle[1]=-1;
  BMarked = BBoundary = false;
}

//----------------------------------------------------------------------------
vtkALBAFillingHole::CEdge::CEdge(int v0,int v1)
//----------------------------------------------------------------------------
{
  AVertex[0] = v0;
  AVertex[1] = v1;
  AVertex[2] = AVertex[3] = -1;
  ATriangle[0]=ATriangle[1]=-1;
  BMarked = BBoundary = false;
}

//----------------------------------------------------------------------------
vtkALBAFillingHole::CEdge::CEdge(int v0,int v1,int v2,int v3)
//----------------------------------------------------------------------------
{
  AVertex[0] = v0;
  AVertex[1] = v1;
  AVertex[2] = v2;
  AVertex[3] = v3;
  ATriangle[0]=ATriangle[1]=-1;
  BMarked = BBoundary = false;
}

//----------------------------------------------------------------------------
void vtkALBAFillingHole::CEdge::SetTriangle(int t0,int t1)
//----------------------------------------------------------------------------
{
  ATriangle[0]=t0;
  ATriangle[1]=t1;
}

//----------------------------------------------------------------------------
vtkALBAFillingHole::CLaplacian::CLaplacian(int size)
//----------------------------------------------------------------------------
{
  DLaplacian = 0;
  DLaplacianCoord[0] = 0;
  DLaplacianCoord[1] = 0;
  DLaplacianCoord[2] = 0;
  ALaplacian = new double[size];
}

//----------------------------------------------------------------------------
vtkALBAFillingHole::CLaplacian::~CLaplacian()
//----------------------------------------------------------------------------
{
  delete ALaplacian;
}
#pragma endregion Nested classes

//------------------------------------------------------------------------
//Computes the normalized normal of the triangle. 
//The triangle is defined by the three given vertices 
void  vtkALBAFillingHole::ComputeNormal(const double *v1, const double *v2, 
                                    const double *v3, double *n)
//------------------------------------------------------------------------
{
  double u[3] = { v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2]};
  double v[3] = { v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]};
  
  vtkMath::Cross(u, v, n);
  vtkMath::Normalize(n);
}

//------------------------------------------------------------------------
//Computes the dihedral angle between two planes.
//Planes are defined by their normal vectors (n1, n2). The function does not 
//return the angle in degrees but a value ranging from -1 to 3
//that is proportional to this angle
double vtkALBAFillingHole::ComputeDihedralAngleF(const double* n1, const double* n2)
//------------------------------------------------------------------------
{
  double nn[3];
  vtkMath::Cross(n1, n2, nn);

  //u x v = |u|*|v|*sin(alfa)*n =>
  //|u x v| = ||u|*|v|*sin(alfa)*n| = |u|*|v|*sin(alfa)*|n| =>
  //sin(alfa) = |u x v| / (|u|*|v|)
  double dsin = vtkMath::Norm(nn);  //n1 and n2 are already normalized
  
  //u*v = |u|*|v|*cos(alfa) =>
  //cos(alfa) = u*v / (|u|*|v|)
  double dcos = n1[0]*n2[0] + n1[1]*n2[1] + n1[2]*n2[2]; //both vectors normalized already

  if (dsin >= 0)      //angle is 0..180
    return -dcos;     //return value -1..1
  else                //angle is 180..360
    return 2 + dcos;  //return value 1..3
}

//------------------------------------------------------------------------
//Returns true, if the point v4 lies outside the circum-circle subscribed
//to the triangle formed by points v1, v2 and v3.
//N.B. points v2, v1 and v4 must form the adjacent triangle 
bool vtkALBAFillingHole::CircumCircleTest(double *v1,double *v2,double *v3, double *v4)
//------------------------------------------------------------------------
{   
  double u_1[3], v_1[3], u_2[3], v_2[3];
  double n1[3], n2[3];

  for (int i = 0; i < 3; i++)
  {
    u_1[i] = v2[i] - v3[i];
    v_1[i] = v1[i] - v3[i];
    u_2[i] = v1[i] - v4[i];
    v_2[i] = v2[i] - v4[i];
  }
  
  //compute cotangens of angles between vectors u_1, v_1 and u_2, v_2
  //cotg(alfa) = cos(alfa)/sin(alfa)
  //cos(alfa) = u*v /(|u|*|v|), where u and v are vectors of the triangle
  //sin(alfa) = |u x v| / (|u|*|v|)
  //thus cotg(alfa) = u*v / |u x v|  
  vtkMath::Cross(u_1, v_1, n1);
  vtkMath::Cross(u_2, v_2, n2);  
  double cotg_aij = vtkMath::Dot(u_1, v_1) / vtkMath::Norm(n1);
  double cotg_aji = vtkMath::Dot(u_2, v_2) / vtkMath::Norm(n2);

  //wij = cotgaij + cotgaji
  //wij < 0 <==> the sum of angles is larger than PI, i.e.,
  //when the edge should be swapped
  //see http://www.math.tu-berlin.de/~bobenko/SCG.pdf
  return (cotg_aij + cotg_aji >= 0);
}

//----------------------------------------------------------------------------
// Compute a triangle area
double vtkALBAFillingHole::TriangleArea(double *pCoordv0,double *pCoordv1,double *pCoordv2)
//----------------------------------------------------------------------------
{
  double a[3],b[3],c[3];

  a[0] = pCoordv1[0] - pCoordv0[0];
  a[1] = pCoordv1[1] - pCoordv0[1];
  a[2] = pCoordv1[2] - pCoordv0[2];

  b[0] = pCoordv2[0] - pCoordv0[0];
  b[1] = pCoordv2[1] - pCoordv0[1];
  b[2] = pCoordv2[2] - pCoordv0[2];

  c[0] = a[1]*b[2] - a[2]*b[1];
  c[1] = a[2]*b[0] - a[0]*b[2];
  c[2] = a[0]*b[1] - a[1]*b[0];

  double length = c[0]*c[0]+c[1]*c[1]+c[2]*c[2];
  return sqrt(length)*0.5;
}

//----------------------------------------------------------------------------
// vtkALBAFillingHole Constructor
vtkALBAFillingHole::vtkALBAFillingHole()
//----------------------------------------------------------------------------
{
  FillingType = Flat;    //flat filling
  FillingHoles = -1;      //filling all holes or a hole. -1 means no filling.
  BorderPointID = 0;
  SmoothThinPlateSteps = 500; //large number of steps
  LastPatch = vtkPolyData::New();

	NumOfVertex = 0;
	NumOfTriangle = 0;
	NumOfEdge = 0;
}

//----------------------------------------------------------------------------
// vtkALBAFillingHole Destructor
vtkALBAFillingHole::~vtkALBAFillingHole()
//----------------------------------------------------------------------------
{
  HolePointIDs.clear();
  HoleEdgeIDs.clear();

  ClearPatch();
  ClearMesh();
 if (LastPatch!=NULL)
  {
    LastPatch->Delete();
    LastPatch = NULL;
  }

}

//----------------------------------------------------------------------------
void vtkALBAFillingHole::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkALBAFillingHole::ClearPatch()
//----------------------------------------------------------------------------
{
  int i, nCount;
  nCount = (int)PatchEdges.size();
  for(i=0;i<nCount;i++)
  {
    delete  PatchEdges[i];
  }

  nCount = (int)PatchTriangles.size();
  for(i=0;i<nCount;i++)
  {
    delete  PatchTriangles[i];
  }

  nCount = (int)PatchVertexes.size();
  for(i=0;i<nCount;i++)
  {
    delete  PatchVertexes[i];    
  }

  nCount = (int)PatchLaplacian.size();
  for (i = 0; i < nCount; i++) {
    delete PatchLaplacian[i];
  }

  PatchVertexes.clear();
  PatchTriangles.clear();
  PatchEdges.clear();
  PatchLaplacian.clear();

  NumOfPatchVertex = 0;
  NumOfPatchTriangle = 0;
  NumOfPatchEdge = 0;
}

//----------------------------------------------------------------------------
// initialize the internal relationship of vertices, triangles and edges of a patch.
void vtkALBAFillingHole::BuildPatch()
//----------------------------------------------------------------------------
{
  int i,j,t;
  int sv1,sv2;
  int dv1,dv2;
  bool bflag;
  double dLength;
	vtkIdType *pVertexIndex,*pVertexIndex2,*pEdgeIndex;

  CVertex		*pVertex1,*pVertex2;
  CTriangle	*pTriangle, *pNeighTriangle;
  CEdge		*pEdge;

  vtkstd::vector<int>::iterator	neight,end;
  vtkstd::vector<CTriangle*>::iterator	start,triangle;

  start = PatchTriangles.begin();
  for( t=0,triangle=start; t<NumOfPatchTriangle; t++,triangle++)
  {
    pVertexIndex = (*triangle)->AVertex;

    //each triangle has three vertex;
    for(i=0;i<3;i++)
    {
      //add the triangle to the vertex as an first degree ring triangle.      
      PatchVertexes[pVertexIndex[i]]->OneRingTriangle.push_back(t);
    }
  }

  NumOfPatchEdge = 0;
  PatchEdges.resize(3*NumOfPatchTriangle);
  for( t=0,triangle=start; t<NumOfPatchTriangle; t++,triangle++)
  {
    pTriangle = *triangle;		
    pVertexIndex = pTriangle->AVertex;
    pEdgeIndex = pTriangle->AEdge;
    //each triangle has three vertex;
    for(i=0;i<3;i++)
    {
      if( pEdgeIndex[i] >= 0 )				continue;

      pEdgeIndex[i] = NumOfPatchEdge;

      sv1 = pVertexIndex[i];
      sv2 = pVertexIndex[(i+1)%3];

      pVertex1 = PatchVertexes[sv1];
      pVertex2 = PatchVertexes[sv2];

      //BES: 23.6.2008 - manifold check
      if (pVertex1->OneRingVertex.end() != find(pVertex1->OneRingVertex.begin(), 
        pVertex1->OneRingVertex.end(),sv2))
      {
        //we might have produced non-manifold due to numeric instability of hole filling
        //this would cause severe troubles in smoothing, so hot fix it
        vtkErrorMacro(<< "vtkALBAFillingHole::BuildPatch detected non-manifold");
#ifdef _MSC_VER
        _RPTF2(_CRT_WARN, "Non-manifold edge %d, %d detected!\n", sv1, sv2);
#endif
        continue;
      }    

      pVertex1->OneRingVertex.push_back(sv2);
      pVertex2->OneRingVertex.push_back(sv1);			
      pVertex1->OneRingEdge.push_back(NumOfPatchEdge);
      pVertex2->OneRingEdge.push_back(NumOfPatchEdge);

      dLength = VEC3_SQUAREDIST(pVertex1->DCoord,pVertex2->DCoord);
      dLength = sqrt(dLength);
      pVertex1->DOneRingEdgeLength += dLength;
      pVertex2->DOneRingEdgeLength += dLength;

      pEdge = new CEdge(sv1,sv2);
      pEdge->Id = NumOfPatchEdge;
      pEdge->AVertex[2] = pVertexIndex[(i+2)%3];
      pEdge->ATriangle[0] = t;
      PatchEdges[NumOfPatchEdge] = pEdge;

      bflag = false;
      end = pVertex1->OneRingTriangle.end();
      neight = pVertex1->OneRingTriangle.begin();
      for( ;neight != end; neight++ )
      {
        if( *neight <= t) continue;
        pNeighTriangle = PatchTriangles[*neight];	
        pVertexIndex2 = pNeighTriangle->AVertex;

        for(j=0; j<3; j++)
        {
          // when find the edge of the neighbor triangle was computed, continue find 
          // next edge of the neighbor triangle.
          if( pNeighTriangle->AEdge[j] >=0 ) continue;	

          dv1 = pVertexIndex2[j];
          dv2 = pVertexIndex2[(j+1)%3];
          if( sv2 == dv1 && sv1==dv2 )
          {
            pNeighTriangle->AEdge[j] = NumOfPatchEdge;
            // if the mesh is manifold, each edge only have two adjacent triangles.
            pEdge->AVertex[3] = pVertexIndex2[(j+2)%3];
            pEdge->ATriangle[1] = *neight;
            bflag = true;
            break;						
          }
        }
      }
      if( bflag == false )
      {
        //find a boundary edge and vertex
        pEdge->BBoundary = true;
      }
      NumOfPatchEdge++;
    }
  }
  PatchEdges.resize(NumOfPatchEdge);
}

//----------------------------------------------------------------------------
// See: G. Barequet,  M. Sharir: Filling Gaps in the Boundary of a Polyhedron. 
// Computer-Aided Geometric Design, 12(2):207-229, March 1995
void vtkALBAFillingHole::Trace(int i,int k)
//----------------------------------------------------------------------------
{
  int j;
  CTriangle *pTriangle;

  //build triangles for the patching mesh
  if( i+2 == k)
  {
    //triangles[] = triangles[] + triangle(vi, vi+1, vk);
    //the triangle must be count-clock-wise.
    pTriangle = new CTriangle(i,k,i+1);
    pTriangle->Id = NumOfPatchTriangle++;
    PatchTriangles.push_back(pTriangle);
  }
  else
  {
    j = Lambda[i][k];
    if( j != i+1 )  Trace(i,j);

    //triangles[] = triangles[] + triangle(vi, vj, vk); 
    //the triangle must be count-clock-wise.
    pTriangle = new CTriangle(i,k,j);
    pTriangle->Id = NumOfPatchTriangle++;
    PatchTriangles.push_back(pTriangle);

    if( j != k-1 )  Trace(j,k);
  }
}

//----------------------------------------------------------------------------
// Build a patch for the hole on the surface.
// See: P. Liepa: Filling Holes in Meshes. 
// Eurographics Symposium on Geometry Processing(2003)
//
// See also: G. Barequet,  M. Sharir: Filling Gaps in the Boundary of a Polyhedron. 
// Computer-Aided Geometric Design, 12(2):207-229, March 1995
void vtkALBAFillingHole::CreatePatch()
//----------------------------------------------------------------------------
{
  int i,j,k,m;
  double	**weight, **weight_angle,temp, temp2;
  double *pCoordv0,*pCoordv1,*pCoordv2,*pCoordv3;
  
  //normals
  double n1[3], n2[3];

  Lambda = new int*[NumOfPatchVertex+1];
  weight = new double*[NumOfPatchVertex+1];
  weight_angle = new double*[NumOfPatchVertex+1];
  for( i=0; i<= NumOfPatchVertex; i++ )
  {
    Lambda[i] = new int[NumOfPatchVertex+1];
    weight[i] = new double[NumOfPatchVertex+1];
    weight_angle[i] = new double[NumOfPatchVertex+1];
  }

  //weight[i][j] denote the weight of the best triangulation of the polygonal
  //chain vi, vi+1, ...vj, vj+1 = vi, if we consider minimal triangle areas
  //thus it produces quite good triangles

  //weight_angle[i][j] denote the weight of the best triangulation of the polygonal
  //chain vi, vi+1, ...vj, vj+1 = vi, if we consider dihedral angles
  //it produces ugly triangles but not overlapping

  //lambda[i][j] contains indices where the best triangulations was reached

  //init lambda[i][i] and lambda[i][i+1]
  //init weight[i][i] and weight[i][i+1]
  for( i=0; i<NumOfPatchVertex; i++ )
  {
    //record the absolute position:  lambda[i][i] and lambda[i][i+1]
    //record the relative position:  lambda[i][j] ( i + 1 < j)
    //the absolute position of the vertex in surrounding mesh
    Lambda[i][i] = HolePointIDs[i];  

    //the absolute position of the vertex of edge(i,i+1) oppsite in surrounding mesh
    Lambda[i][i+1] = Edges[HoleEdgeIDs[i]]->AVertex[2];	

    weight[i][i] = weight_angle[i][i] = 0;
    weight[i][i+1]=	weight_angle[i][i+1] = 0;
  }

  //compute lambda[i][i+2] and weight[i][i+2]
  //this is actually an ear of the hole
  for( i=0; i< NumOfPatchVertex-2; i++ )
  {
    //record the relative position in the boundary polygon
    Lambda[i][i+2] = i+1;
    //computing the area of the triangle(i,i+1,i+2);    
    pCoordv0 = Vertexes[Lambda[i][i]]->DCoord;    
    pCoordv1 = Vertexes[Lambda[i+1][i+1]]->DCoord;
    pCoordv2 = Vertexes[Lambda[i+2][i+2]]->DCoord;
    weight[i][i+2] = TriangleArea(pCoordv0,pCoordv2,pCoordv1);

    //compute max dihedral angle for both adjacent triangles
    //having edge i,i+1 and i+1,i+2    
    ComputeNormal(pCoordv0,pCoordv2,pCoordv1, n1);
    
    pCoordv3 = Vertexes[Lambda[i][i+1]]->DCoord;
    ComputeNormal(pCoordv0,pCoordv1,pCoordv3, n2);

    //if the mesh is CCW oriented, the dihedral angle cannot be larger 
    //than 180 degrees and, therefore, cos(angle) is sufficient. 
    //we need to minimize maximal dihedral angle, which means
    //that we need to minimize maximal -cos(dihedral angle)  
    //N.B. beware of changing it to the minimization of minimal cos(angle)
    //it does not work (I'm not sure why)
    weight_angle[i][i+2] = ComputeDihedralAngleF(n1, n2);
      //-(n1[0]*n2[0] + n1[1]*n2[1] + n1[2]*n2[2]);

    pCoordv3 = Vertexes[Lambda[i+1][i+2]]->DCoord;
    ComputeNormal(pCoordv1,pCoordv2,pCoordv3, n2);
    temp = ComputeDihedralAngleF(n1, n2);
      //-(n1[0]*n2[0] + n1[1]*n2[1] + n1[2]*n2[2]);
    if (temp > weight_angle[i][i+2])
      weight_angle[i][i+2] = temp; //maximize angle
  }

  //compute weight[0][nBoundaryVertex-1]
  //j = length of chain, chains of 1, 2 or 3 vertices were computed above
  for(j=3; j<= NumOfPatchVertex-1; j++)
  {
    //for the given length of chain, compute the weight for every vertex
    for( i=0; i<NumOfPatchVertex-j; i++)
    {
      //the chain starts at vertex i and ends at vertex k
      //when j=nBoundaryVertex-1, i only is 0, conclude k=nBoundaryVertex-1
      k=i+j;
      weight[i][k] = 0x7fffffff;        //max double value;
      weight_angle[i][k] = 0x7fffffff;  //max double value;

      pCoordv0 = Vertexes[Lambda[i][i]]->DCoord;
      pCoordv2 = Vertexes[Lambda[k][k]]->DCoord;

      //for every vertex between i and k
      for(m=i+1; m<k; m++)
      {
        //compute wa[i][m] + wa[m][k] + weight_a(vi, vm, vk);
        //compute maximal dihedral angle
        pCoordv1 = Vertexes[Lambda[m][m]]->DCoord;        
        ComputeNormal(pCoordv0,pCoordv2,pCoordv1, n1);

        //get the coordinate for Lambda[i][m], if m==i+1, then it
        //is actually the vertex in the surrounding mesh, the far vertex of
        //in the triangle adjacent to the ear i,i+1,i+2 (sharing edge i, i+1);
        //otherwise we have in Lambda[i][m] only relative index RI and
        //the absolute index is stored at Lambda[RI][RI]
        if (m == i + 1)        
          pCoordv3 = Vertexes[Lambda[i][m]]->DCoord;
        else
          pCoordv3 = Vertexes[Lambda[Lambda[i][m]][Lambda[i][m]]]->DCoord;

        ComputeNormal(pCoordv0,pCoordv1,pCoordv3, n2);
        temp = ComputeDihedralAngleF(n1, n2);
        //-(n1[0]*n2[0] + n1[1]*n2[1] + n1[2]*n2[2]);        

        if (m + 1 == k)
          pCoordv3 = Vertexes[Lambda[m][k]]->DCoord;
        else
          pCoordv3 = Vertexes[Lambda[Lambda[m][k]][Lambda[m][k]]]->DCoord;
        ComputeNormal(pCoordv1,pCoordv2,pCoordv3, n2);        
        temp2 = ComputeDihedralAngleF(n1, n2);
        //-(n1[0]*n2[0] + n1[1]*n2[1] + n1[2]*n2[2]);
        if (temp2 > temp)
          temp = temp2;

        if (i == 0 && k == NumOfPatchVertex-1)
        {
          //we need to take into account also the triangle
          //adjacent to edge v0,vn-1
          pCoordv3 = Vertexes[Lambda[NumOfPatchVertex-1][NumOfPatchVertex]]->DCoord;
          ComputeNormal(pCoordv2,pCoordv0,pCoordv3, n2);        
          temp2 = ComputeDihedralAngleF(n1, n2);
          //-(n1[0]*n2[0] + n1[1]*n2[1] + n1[2]*n2[2]);
          if (temp2 > temp)
            temp = temp2;
        }

        if (weight_angle[i][m] > temp)
          temp = weight_angle[i][m];
        if (weight_angle[m][k] > temp)
          temp = weight_angle[m][k];                    

        //compute w[i][m] + w[m][k] + weight(vi, vm, vk);
        //computing area of the triangle(i,m,k);
        temp2 = weight[i][m] + weight[m][k] + TriangleArea(pCoordv0,pCoordv2,pCoordv1);

        //update weights, we minimize maximal dihedral angle        
        //reliable comparison test for floating point numbers. Extracted from article:
        //"Work Around Floating-Point Accuracy/Comparison Problems" 
        //Article ID: Q69333 of MSDN library - see http://support.microsoft.com/kb/69333
        double diff = fabs(temp - weight_angle[i][k]);
        double max_err = fabs(temp / pow((double)10,(double)15));

        bool bEquals = diff <= max_err;
        if ((!bEquals && temp <= weight_angle[i][k]) || (bEquals && temp2 <= weight[i][k]))
        {        
          weight_angle[i][k] = temp;  //record the maximum dihedral angle (min cos(angle))
          weight[i][k] = temp2;	//record the minimum weight
          Lambda[i][k] = m;		  //record the vertex index( the position in the polygon ).
        }
      }
    }
  }

  CVertex *pVertex;
  for(i=0;i<NumOfPatchVertex;i++)
  {
    j = Lambda[i][i];
    //compute the weight of vertex j
    pVertex = new CVertex(Vertexes[j]->DCoord);
    pVertex->Id = j;    //record vertex id of original mesh.
    pVertex->BMarked = true;

    m = Vertexes[j]->OneRingEdge.size();
    pVertex->DWeight = Vertexes[j]->DOneRingEdgeLength/m;

    PatchVertexes.push_back(pVertex);      
  }
  Trace(0,NumOfPatchVertex-1);
  
  //for (i = 0; i < NumOfPatchTriangle; i++)
  //{
  //  _RPT4(_CRT_WARN, "PATCH(%d): %d, %d, %d\n", i,
  //    PatchTriangles[i]->aVertex[0],
  //    PatchTriangles[i]->aVertex[1],
  //    PatchTriangles[i]->aVertex[2]);
  //}

  
  BuildPatch();

  for( i=0; i<= NumOfPatchVertex; i++ )
  {
    delete (Lambda[i]);
    delete (weight[i]);
    delete[] weight_angle[i];
  }
  delete (Lambda);
  delete (weight);
  delete[] weight_angle;
}

//----------------------------------------------------------------------------
// Insert a point into a triangle and divide the triangle to three small triangles
// Update the relationship of vertices, triangles and edges
vtkALBAFillingHole::CVertex* vtkALBAFillingHole::AddOnePointToTriangle(double *pCoord,CTriangle*pTriangle)
//----------------------------------------------------------------------------
{
  CVertex		*pNewVertex;
  CTriangle	*pNewTriangle;
  CEdge		*pEdge, *pNewEdge;
	vtkIdType				*pVertexIndex,*pEdgeIndex;

  //add a new vertex
  pNewVertex = new CVertex(pCoord);
  pNewVertex->Id = NumOfPatchVertex;
  PatchVertexes.push_back(pNewVertex);

  //old triangle
  pVertexIndex = pTriangle->AVertex;
  pEdgeIndex = pTriangle->AEdge;

  //add three new triangles
  pNewTriangle = new CTriangle(pVertexIndex[0],pVertexIndex[1],NumOfPatchVertex);
  pNewTriangle->SetEdge(pEdgeIndex[0],NumOfPatchEdge,NumOfPatchEdge+1);
  pNewTriangle->Id = NumOfPatchTriangle;
  PatchTriangles.push_back(pNewTriangle);

  pNewTriangle = new CTriangle(NumOfPatchVertex,pVertexIndex[1],pVertexIndex[2]);
  pNewTriangle->SetEdge(NumOfPatchEdge,pEdgeIndex[1],NumOfPatchEdge+2);
  pNewTriangle->Id = NumOfPatchTriangle+1;
  PatchTriangles.push_back(pNewTriangle);

  pNewTriangle = new CTriangle(pVertexIndex[0],NumOfPatchVertex,pVertexIndex[2]);
  pNewTriangle->SetEdge(NumOfPatchEdge+1,NumOfPatchEdge+2,pEdgeIndex[2]);
  pNewTriangle->Id = NumOfPatchTriangle+2;
  PatchTriangles.push_back(pNewTriangle);

  //add three new edges
  pNewEdge = new CEdge(pVertexIndex[1],NumOfPatchVertex,pVertexIndex[0],pVertexIndex[2]);
  pNewEdge->ATriangle[0] = NumOfPatchTriangle;
  pNewEdge->ATriangle[1] = NumOfPatchTriangle+1;
  pNewEdge->Id = NumOfPatchEdge;
  PatchEdges.push_back(pNewEdge);

  pNewEdge = new CEdge(NumOfPatchVertex,pVertexIndex[0],pVertexIndex[1],pVertexIndex[2]);
  pNewEdge->ATriangle[0] = NumOfPatchTriangle;
  pNewEdge->ATriangle[1] = NumOfPatchTriangle+2;
  pNewEdge->Id = NumOfPatchEdge+1;
  PatchEdges.push_back(pNewEdge);

  pNewEdge = new CEdge(pVertexIndex[2],NumOfPatchVertex,pVertexIndex[1],pVertexIndex[0]);
  pNewEdge->ATriangle[0] = NumOfPatchTriangle+1;
  pNewEdge->ATriangle[1] = NumOfPatchTriangle+2;
  pNewEdge->Id = NumOfPatchEdge+2;
  PatchEdges.push_back(pNewEdge);

  //modify three edges of old triangle
  for(int i=0; i<3; i++)
  {
    pEdge = PatchEdges[pEdgeIndex[i]];
    if( pEdge->AVertex[0] == pVertexIndex[i] )
    {
      pEdge->AVertex[2] = NumOfPatchVertex;
      pEdge->ATriangle[0] = NumOfPatchTriangle+i;
    }
    else
    {
      pEdge->AVertex[3] = NumOfPatchVertex;
      pEdge->ATriangle[1] = NumOfPatchTriangle+i;
    }
    pEdge->BMarked = false;
  }

  NumOfPatchVertex += 1;
  NumOfPatchTriangle += 3;
  NumOfPatchEdge += 3;
  pTriangle->BDeleted = true;

  return pNewVertex;
}

//----------------------------------------------------------------------------
// The edge is adjacent to two triangles
// Now, swap the edge to two non-mutual vertices of the triangles.
bool vtkALBAFillingHole::RelaxOneEdge(CEdge *pEdge)
//----------------------------------------------------------------------------
{
  int		pVertexIndex[4],i,j;
  double	*pCoord[4];//,dCentroid[3];

  CEdge		*pOldEdge;

  int *edgevertex;	
  int *edgetriangle;	

  edgevertex = pEdge->AVertex;
  for(i=0; i<4; i++)
  {
    pVertexIndex[i] = edgevertex[i];
    pCoord[i] = PatchVertexes[edgevertex[i]]->DCoord;
  }

  if (CircumCircleTest(pCoord[0], pCoord[1], pCoord[2], pCoord[3]))
    return false; //it means the vertex[2] is located outside of circum-sphere

  //we are supposed to swap the edge
  //check, if the swap is valid, i.e., if the new edge is no longer present in the
  //surrounding mesh - this may happen due to round-off errors
  int nHoleSize = (int)HolePointIDs.size();
  if (pVertexIndex[2] < nHoleSize && pVertexIndex[3] < nHoleSize)
  {
    //the new edge connects two boundary points
    int V0 = HolePointIDs[pVertexIndex[2]];
    int V1 = HolePointIDs[pVertexIndex[3]];

    CEdge* pCheckEdge = Edges[HoleEdgeIDs[pVertexIndex[2]]];    
    if (pCheckEdge->AVertex[2] == V1 || pCheckEdge->AVertex[3] == V1)
      return false; //the edge already exists

    pCheckEdge = Edges[HoleEdgeIDs[pVertexIndex[3]]];    
    if (pCheckEdge->AVertex[2] == V0 || pCheckEdge->AVertex[3] == V0)
      return false; //the edge already exists
  }
  

  edgetriangle = pEdge->ATriangle;

  CTriangle	*pLeftTriangle,*pRightTriangle;
  CTriangle	*pTopTriangle,*pBottomTriangle;

  //modify two old triangles to two new triangles.
  //modify four edges of two old triangles
  pLeftTriangle = PatchTriangles[edgetriangle[0]];
  pRightTriangle = PatchTriangles[edgetriangle[1]];

  pTopTriangle = new CTriangle(pVertexIndex[3],pVertexIndex[1],pVertexIndex[2]);
  pBottomTriangle = new CTriangle(pVertexIndex[0],pVertexIndex[3],pVertexIndex[2]);

  pTopTriangle->Id = pLeftTriangle->Id;
  pBottomTriangle->Id = pRightTriangle->Id;

  pTopTriangle->AEdge[2] = pEdge->Id;
  pBottomTriangle->AEdge[1] = pEdge->Id;

#if 0
#if defined(_FILLING_DBG) && defined(_MSC_VER)
  _RPT4(_CRT_WARN, "Relax Edge: #%d(%d,%d,%d),", 
    pLeftTriangle->id, pLeftTriangle->aVertex[0], 
    pLeftTriangle->aVertex[1], pLeftTriangle->aVertex[2]);
  _RPT4(_CRT_WARN, "#%d(%d,%d,%d) -> ", 
    pRightTriangle->id, pRightTriangle->aVertex[0], 
    pRightTriangle->aVertex[1], pRightTriangle->aVertex[2]);
  _RPT4(_CRT_WARN, "#%d(%d,%d,%d),", 
    pTopTriangle->id, pTopTriangle->aVertex[0], 
    pTopTriangle->aVertex[1], pTopTriangle->aVertex[2]);
  _RPT4(_CRT_WARN, "#%d(%d,%d,%d)\n", 
    pBottomTriangle->id, pBottomTriangle->aVertex[0], 
    pBottomTriangle->aVertex[1], pBottomTriangle->aVertex[2]);  

  int edges[4];
  for(i=0;i<3;i++)
  {
    if( pLeftTriangle->aVertex[i] != pVertexIndex[0] )	continue;
    edges[0] = pLeftTriangle->aEdge[(i+1)%3];
    edges[1] = pLeftTriangle->aEdge[(i+2)%3];
    break;
  }

  for(i=0;i<3;i++)
  {
    if( pRightTriangle->aVertex[i] != pVertexIndex[0] )	continue;
    edges[2] = pRightTriangle->aEdge[(i+1)%3];
    edges[3] = pRightTriangle->aEdge[(i+2)%3];
    break;
  }

  int tris[6];
  int outtris = 0;
  for (i = 0; i < 4; i++)
  {
    CEdge* pE = PatchEdges[edges[i]];
    for (int j = 0; j < 2; j++)
    {
      if (pE->aTriangle[j] == -1) continue;

      bool bAlreadyPrinted = false;
      for (int k = 0; k < outtris; k++)
      {
        if (tris[k] == pE->aTriangle[j]) {
          bAlreadyPrinted = true; break;
        }
      }
      if (bAlreadyPrinted) continue;
      tris[outtris] = pE->aTriangle[j]; outtris++;
      CTriangle* pTr = PatchTriangles[pE->aTriangle[j]];
      for (int k = 0; k < 4; k++)
      {
        CVertex* pV = PatchVertexes[pTr->aVertex[k % 3]];
        _RPT3(_CRT_WARN, "%d %f %f\n",
          pTr->aVertex[k % 3], pV->dCoord[0], pV->dCoord[1]/*, pV->dCoord[2]*/);
      }
    }
  }
#endif //_FILLING_DBG
#endif
  
  CEdge* changeEdges[4];    //edges to be modified
  int changePos[4];         //position which to change
  int changeNewVal[4];      //and values to write there

  //modify left triangle
  for(i=0;i<3;i++)
  {
    if( pLeftTriangle->AVertex[i] != pVertexIndex[0] )	continue;

    j = pLeftTriangle->AEdge[(i+1)%3];		
    pTopTriangle->AEdge[1] = j;
    changeEdges[0] = pOldEdge = PatchEdges[j];
    changeNewVal[0] = pVertexIndex[3];
    if( pOldEdge->AVertex[0] == pVertexIndex[1] )
      changePos[0] = 2;      
    else
      changePos[0] = 3;         

    j = pLeftTriangle->AEdge[(i+2)%3];
    pBottomTriangle->AEdge[2] = j;
    changeEdges[2] = pOldEdge = PatchEdges[j];
    changeNewVal[2] = pVertexIndex[3];
    if( pOldEdge->AVertex[0] == pVertexIndex[2] )
      changePos[2] = 2;
    else
      changePos[2] = 3;
   
    break;
  }
  //modify right triangle
  for(i=0;i<3;i++)
  {
    if( pRightTriangle->AVertex[i] != pVertexIndex[0] )	continue;

    j = pRightTriangle->AEdge[(i+1)%3];
    pTopTriangle->AEdge[0] = j;
    changeEdges[3] = pOldEdge = PatchEdges[j];
    changeNewVal[3] = pVertexIndex[2];
    if( pOldEdge->AVertex[0] == pVertexIndex[3] )
      changePos[3] = 2;
    else
      changePos[3] = 3;


    j = pRightTriangle->AEdge[i];
    pBottomTriangle->AEdge[0] = j;
    changeEdges[1] = pOldEdge = PatchEdges[j];
    changeNewVal[1] = pVertexIndex[2];
    if( pOldEdge->AVertex[0] == pVertexIndex[0] )
      changePos[1] = 2;
    else
      changePos[1] = 3;

    break;
  }
  
  //now check, if the swap is valid
  //this code was added to increase the robustness of the algorithm
  for (i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      if (changeEdges[i]->AVertex[j] == changeNewVal[i])
      {
#if defined(_DEBUG) && defined(_MSC_VER)
        _RPT1(_CRT_WARN, "Invalid swap operation for edge #%d\n", pEdge->Id);
#endif //_DEBUG        

        //swap is illegal
        delete pTopTriangle;
        delete pBottomTriangle;
        return false;
      }
    }
  }

  //and finally perform the change
  for (i = 0; i < 4; i++)
  {
    changeEdges[i]->AVertex[changePos[i]] = changeNewVal[i];
    changeEdges[i]->BMarked = false;
  }
  
  //changePos is 2 or 3
  changeEdges[2]->ATriangle[changePos[2] - 2] = pBottomTriangle->Id;
  changeEdges[3]->ATriangle[changePos[3] - 2] = pTopTriangle->Id;  

  PatchTriangles[edgetriangle[0]] = pTopTriangle;
  PatchTriangles[edgetriangle[1]] = pBottomTriangle;

  //modify an old edge to a new edge.
  edgevertex[0] = pVertexIndex[2];
  edgevertex[1] = pVertexIndex[3];
  edgevertex[2] = pVertexIndex[1];
  edgevertex[3] = pVertexIndex[0];

  delete pLeftTriangle;
  delete pRightTriangle;
  return true;
}

#ifdef _FILLING_DBG
void vtkALBAFillingHole::CheckPatchEdges(int& nStartEdgeId)
{
  nStartEdgeId = 0;

  vtkstd::map<long long, int> mapEdges;  
  typedef vtkstd::map<long long, int>::iterator EdgesIterator;

  while (nStartEdgeId < NumOfPatchEdge)
  {
    int* pVertexIndex = PatchEdges[nStartEdgeId]->aVertex;

    int nUnique = 4;
    for(int i=0; i<4; i++)
    {
      for (int j = 0; j < 4; j++) {
        if (i != j && pVertexIndex[i] == pVertexIndex[j])
          nUnique--;
      }
    }
    assert(nUnique == 4);
    
    int sv1 = pVertexIndex[0];
    int sv2 = pVertexIndex[1];    

    long long key;
    if (sv1 < sv2)      
      key = ((long long)sv1) << 32 | sv2;      
    else
      key = ((long long)sv2) << 32 | sv1;

    EdgesIterator iter = mapEdges.find(key);
    if (iter == mapEdges.end())
      mapEdges[key] = 0;          //new edge
    else if (iter->second == 0)   //already existing edge, but with just one neighbor
      iter->second++;
    else
    {
      assert(false);               //non-manifold, this edge already has two triangles
      break;
    }
    
    nStartEdgeId++;
  }

  mapEdges.clear();
  for (int i = 0; i < NumOfPatchTriangle; i++)
  {
    CTriangle* pTriangle = PatchTriangles[i];
    if (pTriangle->bDeleted) continue;

    for (int j = 0; j < 3; j++)
    {
      int sv1 = pTriangle->aVertex[j];
      int sv2 = pTriangle->aVertex[(j + 1) % 3];

      long long key;
      if (sv1 < sv2)      
        key = ((long long)sv1) << 32 | sv2;      
      else
        key = ((long long)sv2) << 32 | sv1;

      EdgesIterator iter = mapEdges.find(key);
      if (iter == mapEdges.end())
        mapEdges[key] = 0;          //new edge
      else if (iter->second == 0)   //already existing edge, but with just one neighbor
        iter->second++;
      else
      {
        assert(false);               //non-manifold, this edge already has two triangles
        break;
      }
    }
  }
}
#endif //_FILLING_DBG


//----------------------------------------------------------------------------
// divide all large triangles to smaller triangles
// then, relax all interior edges of the patching mesh
// repeat the above procedure until the patch match the surrounding mesh
void vtkALBAFillingHole::RefinePatch()
//----------------------------------------------------------------------------
{
  bool bFlag;
  int i,j;
  double dWeight[3],dTotalWeight,dCentroid[3],dLength;
  double *pCoord[3];
  int nVertex,nTriangle;

  CVertex		*pVertex,*pNewVertex;
  CTriangle *pTriangle;
  CEdge *pEdge;
	vtkIdType *pVertexIndex;
	vtkIdType *pEdgeIndex;

#if defined(_FILLING_DBG)
  int nCheckEdgePos = 0;
  CheckPatchEdges(nCheckEdgePos);
#endif

  double alpha = sqrt(2.0);
  while(true)
  {
    nVertex = NumOfPatchVertex;
    nTriangle = NumOfPatchTriangle;

    for(i=0; i<nTriangle; i++)
    {
      pTriangle = PatchTriangles[i];
      if(pTriangle->BDeleted == true ) continue;
      if(pTriangle->BMarked == true )	continue;   //confirm subdiving isn't necessary.

      pVertexIndex = pTriangle->AVertex;
      for( j=0; j<3; j++)
      {
        pVertex = PatchVertexes[pVertexIndex[j]];
        pCoord[j] = pVertex->DCoord;
        dWeight[j] = pVertex->DWeight;
      }
      dTotalWeight = (dWeight[0] + dWeight[1] + dWeight[2] )/ 3.0;
      dCentroid[0] = (pCoord[0][0] + pCoord[1][0] + pCoord[2][0])/3.0;
      dCentroid[1] = (pCoord[0][1] + pCoord[1][1] + pCoord[2][1])/3.0;
      dCentroid[2] = (pCoord[0][2] + pCoord[1][2] + pCoord[2][2])/3.0;

      //judge if creating new triangles
      for( j=0; j<3; j++)
      {
        dLength = VEC3_SQUAREDIST(dCentroid,pCoord[j]);
        dLength = alpha * sqrt(dLength);
        if( dLength > dWeight[j] && dLength > dTotalWeight ){
          bFlag = true;
        }
        else{
          bFlag = false;															
          break;
        }
      }
      if(bFlag == true)
      {
        pNewVertex = AddOnePointToTriangle(dCentroid,pTriangle);
        pNewVertex->DWeight = dTotalWeight;	
        //relax three edges of the old triangle.	
        pEdgeIndex = pTriangle->AEdge;
        for(j=0; j<3; j++)
        {
          pEdge = PatchEdges[pEdgeIndex[j]];
          if(pEdge->BBoundary == true)	continue;					
          RelaxOneEdge(pEdge);
        }

#if defined(_FILLING_DBG)
        CheckPatchEdges(nCheckEdgePos);
#endif
      }
      else
      {
        //the triangle wasn't need to subdivided.
        pTriangle->BMarked = true;	
      }		
    }//for triangle

    if( nVertex == NumOfPatchVertex ){
      //the only exit. It means that no new triangles were created.
      break;		
    }

    //here, it means creating some new triangles before.
    //now, relax all interior edges of the patch mesh.
    int nMaxAllowedRepeats = NumOfPatchEdge;

    do{
      bFlag = false;
      for( j=0; j<NumOfPatchEdge; j++)
      {
        pEdge  = PatchEdges[j];				
        if(pEdge->BBoundary == true)	continue;
        if(pEdge->BMarked == true)		continue;   //confirm the swapping edge isn't necessary.

        if( RelaxOneEdge(pEdge) == false ) {
          pEdge->BMarked = true;
        }
        else
          bFlag = true;        

#if defined(_FILLING_DBG)
        CheckPatchEdges(nCheckEdgePos);
#endif
      }
    }while(bFlag == true && (--nMaxAllowedRepeats) > 0);

#if defined(_DEBUG) && defined(_MSC_VER)
    if (bFlag) {
      _RPT0(_CRT_WARN, "Infinite swaps detected.\n");
    }
#endif //_DEBUG        
  }//for while

  //clear deleted triangles.
  vtkstd::vector<CTriangle*>::iterator	oldtriangle,newtriangle;

  nTriangle = NumOfPatchTriangle;	
  oldtriangle = newtriangle = PatchTriangles.begin();
  for(NumOfPatchTriangle=0,i=0; i<nTriangle; i++,oldtriangle++)
  {
    pTriangle = *oldtriangle;
    pEdgeIndex = pTriangle->AEdge;
    if( pTriangle->BDeleted != true )
    {
      pTriangle->Id = NumOfPatchTriangle++;
      pEdgeIndex[0] = pEdgeIndex[1] = pEdgeIndex[2] = -1;
      pTriangle->BMarked = false;

      *newtriangle = pTriangle;
      newtriangle++;
    }		
    else
    {
      delete pTriangle;
    }
  }
  PatchTriangles.resize(NumOfPatchTriangle);

  //clear all edges.
  for(i=0; i<NumOfPatchEdge; i++)
  {
    delete  PatchEdges[i];		
  }
  PatchEdges.clear();
  NumOfPatchEdge = 0;

  //clear vertex information.
  for(i=0; i<NumOfPatchVertex; i++)
  {
    pVertex = PatchVertexes[i];
    pVertex->DOneRingEdgeLength = 0;
    pVertex->DWeight = 0;
    pVertex->BBoundary = false;
    pVertex->OneRingTriangle.clear();
    pVertex->OneRingEdge.clear();
    pVertex->OneRingVertex.clear();
  }
}

//----------------------------------------------------------------------------
// extend the patch and include some surrounding vertices and triangles
// for smoothing the patch and making it match the curvature of the surrounding mesh
void vtkALBAFillingHole::ExtendPatch()
//----------------------------------------------------------------------------
{
  int i,step;
  int start,end;
  int id;
  CVertex *pVertex,*pNewVertex;
  CTriangle	*pTriangle,*pNewTriangle;
	vtkIdType		*pVertexIndex,*pNewVertexIndex;

  vtkstd::vector<int>::iterator	oneringend,onering;
  vtkstd::vector<int> surroundvertexes,surroundtriangles;

  //set the seed vertexes in original mesh
  end = HolePointIDs.size();
  for(i=0;i<end;i++)
  {
    start = HolePointIDs[i];
    surroundvertexes.push_back(start);
    pVertex = Vertexes[start];
    pVertex->BMarked = true;

    //temporary changing for recording mapping relationship between 
    //patch mesh vertex id and original mesh vertex id.
    pVertex->Id = i;
  }
  start = 0;

  //build a local surrounding mesh
  const static int nMinSteps = 5;
  int nSteps = 1;   //Thin Membrane needs only one additional ring of vertices
  
  if (FillingType == SmoothMembrane)
  {
    nSteps = ((int)sqrt((double)(NumOfPatchVertex - end))) / 2;
    if (nSteps < nMinSteps) 
      nSteps = nMinSteps;
  }

  for(step = 0; step < nSteps; step++)
  {
    //SmoothMembrane requires the ring, otherwise, it won't work 
    if (FillingType == SmoothMembrane && end > MAXPATCHVERTEX)
    {
      //need too much memory for store
      //we should have a out-of-core version.    
#ifdef _MSC_VER
      _RPT1(_CRT_WARN, "vtkALBAFillingHole::SmoothPatch - the maximum number of points "
        "for smoothing (%d) has been reached\n", end);
#endif

      vtkWarningMacro("vtkALBAFillingHole::SmoothPatch - the maximum number of points "
        "for smoothing has been reached");

      break;  
    }


    for(i=start;i<end;i++)
    {
      pVertex = Vertexes[surroundvertexes[i]];

      onering = pVertex->OneRingTriangle.begin();
      oneringend = pVertex->OneRingTriangle.end();
      for(;onering!=oneringend;onering++)
      {				
        id = *onering;
        if( Triangles[id]->BMarked == true ) continue;
        Triangles[id]->BMarked = true;
        surroundtriangles.push_back(id);
      }

      onering = pVertex->OneRingVertex.begin();
      oneringend = pVertex->OneRingVertex.end();
      for(;onering!=oneringend;onering++)
      {				
        id = *onering;
        if( Vertexes[id]->BMarked == true ) continue;
        Vertexes[id]->BMarked = true;
        surroundvertexes.push_back(id);
      }
    }
    start = end;
    end = surroundvertexes.size();
  }

  //extend the patch to local surrounding mesh
  //extend the patch mesh vertex
  start = HolePointIDs.size();
  end = surroundvertexes.size();
  for(i=start;i<end;i++)
  {
    id = surroundvertexes[i];
    pVertex = Vertexes[id];
    pNewVertex = new CVertex(pVertex->DCoord);
    pNewVertex->BMarked = true; //it is vertex in surround mesh
    pNewVertex->Id = id;

    PatchVertexes.push_back(pNewVertex);

    //temporary changing for recording mapping relationship between 
    //patch mesh vertex id and original mesh vertex id.
    pVertex->Id = NumOfPatchVertex++;
  }

  //extend the patch mesh triangle
  end = surroundtriangles.size();
  for(i=0;i<end;i++)
  {
    pTriangle = Triangles[surroundtriangles[i]];
    pNewTriangle = new CTriangle();
    pNewTriangle->Id = NumOfPatchTriangle++;
    pNewTriangle->BMarked = true;   //it is triangle in surround mesh
    pTriangle->BMarked = false;     //restore the original marked value.

    pVertexIndex = pTriangle->AVertex;
    pNewVertexIndex = pNewTriangle->AVertex;

    //using the recorded mapping relationship
    //computing the vertex position of the new triangle.
    pNewVertexIndex[0] = Vertexes[pVertexIndex[0]]->Id;
    pNewVertexIndex[1] = Vertexes[pVertexIndex[1]]->Id;
    pNewVertexIndex[2] = Vertexes[pVertexIndex[2]]->Id;

    PatchTriangles.push_back(pNewTriangle);
  }

  //restore the surrounding mesh vertex id.
  for(i=0;i<NumOfPatchVertex;i++)
  {
    pNewVertex = PatchVertexes[i];
    if( pNewVertex->BMarked == false )  continue;
    pVertex = Vertexes[pNewVertex->Id];
    pVertex->Id = pNewVertex->Id;
    pVertex->BMarked = false;
  }

  surroundtriangles.clear();
  surroundvertexes.clear();
}

//----------------------------------------------------------------------------
// Build a scale laplacian mesh for the patch
// Lij = -1, if i == j;
//        0, if there is no edge from vertex pi to pj
//        ||pi-pj|| / sum_j(||pi-pj||), otherwise  
//
// the matrix is stored efficiently in PatchLaplacian, where is N entries
// and every entry contains just an array of all non-zero values but -1
void vtkALBAFillingHole::BuildPatchLaplacian()
{
  int           i,j,nOneRingNum;  
  CLaplacian    *pVertexLaplaican;


  CVertex  *pVertex;
  CVertex  *pVertexFirst,*pVertexSecond;
  vtkstd::vector<int>::iterator	  first,second;
  vtkstd::vector<int>::iterator   end,start;

  //Build vertex two ring relationship
  for(i=0;i<NumOfPatchVertex;i++)
  {
    pVertex = PatchVertexes[i];
    pVertex->TwoRingVertex  = pVertex->OneRingVertex; 
  }
  for(i=0;i<NumOfPatchVertex;i++)
  {
    pVertex = PatchVertexes[i];		
    start =pVertex->OneRingVertex.begin();
    end = pVertex->OneRingVertex.end();
    for(first=start;first!=end;first++)
    {
      pVertexFirst = PatchVertexes[*first];
      for(second=first+1;second!=end;second++)
      {
        if( pVertexFirst->IsTwoRingVertex(*second) == true ) continue;
        pVertexFirst->TwoRingVertex.push_back(*second);
        pVertexSecond = PatchVertexes[*second];
        pVertexSecond->TwoRingVertex.push_back(*first);
      }
    }	
  }

  //Build Vertex Laplacian
  PatchLaplacian.resize(NumOfPatchVertex);
  for(i=0;i<NumOfPatchVertex;i++)
  {    
    nOneRingNum = PatchVertexes[i]->OneRingEdge.size();
    pVertexLaplaican = new CLaplacian(nOneRingNum);

    //Scale Laplacian
    double dblLenTotal = 0.0;
    for(j=0; j<nOneRingNum; j++)
    {
      int iEdge = PatchVertexes[i]->OneRingEdge[j];      
      pVertexLaplaican->ALaplacian[j] = Edges[iEdge]->DLength;
      dblLenTotal += Edges[iEdge]->DLength;
    }

    for(j=0; j<nOneRingNum; j++){
      pVertexLaplaican->ALaplacian[j] /= dblLenTotal;
    }
        
    ////Uniform laplacian
    //double omega = 1.0/nOneRingNum;
    //for(j=0; j<nOneRingNum; j++)
    //{
    //  pVertexLaplaican->aLaplacian[j] = omega;
    //}	
    
    pVertexLaplaican->DLaplacian = -1;
    PatchLaplacian[i] = pVertexLaplaican;
  }
}

//----------------------------------------------------------------------------
//Multiplies transpose L matrix and L matrix, i.e., A =  L^T*L 
void vtkALBAFillingHole::ComputeLTransposeLMatrix(double *A)
//----------------------------------------------------------------------------
{
  int i,j,index;
  int firstrow,indexrow,firstcol;
  double diagonal,dValue,dLapi;
  double	*pLaplacian;

  CVertex     *pVertex;
  CLaplacian    *pVertexLaplaican;

  vtkstd::vector<int>::iterator	  first,second;
  vtkstd::vector<int>::iterator   end,start;

  //A[i,j] = sum(k = 0..n-1)L[k,i]*L[k,j]
  //A[i,j] = sum(index = 0..n-1)L[index,i]*L[index,j]
  //A[i_pos,j_pos] = sum(index = 0..n-1)L[index,i_pos]*L[index,j_pos]
  //A[i_pos(i),j_pos] = sum(index = 0..n-1)L[index,i_pos(i)]*L[index,j_pos]
  //L[index, other index than from i_pos(*)] = 0
  memset(A,0,sizeof(double)*NumOfPatchVertex*NumOfPatchVertex);
  for(index=0,indexrow=0;index<NumOfPatchVertex;index++,indexrow += NumOfPatchVertex)
  {
    //load one row L[index,*]
    pVertex = PatchVertexes[index];
    pVertexLaplaican = PatchLaplacian[index];
    pLaplacian = pVertexLaplaican->ALaplacian;
    diagonal = pVertexLaplaican->DLaplacian;

    start = pVertex->OneRingVertex.begin();
    end = pVertex->OneRingVertex.end();

    //for every pair {i,j}, where i,j = 0..n-1, A[i,j] += L[index,i]*L[index,j]
    //value L[index, index] is stored in diagonal
    //other non-zero values are stored in pLaplacian
    //and their valid indices in pVertex->OneRingVertex

    //L[index,i==index]*L[index,j==index]
    A[indexrow + index] += diagonal*diagonal;

    //and now, we could compute the rest of L[index,index]*L[index,j]
    //however, we know that L^T*L is a symmetric matrix, so we can avoid this
    //double multiplication and process the rest of L[index,i]*L[index,j] directly
    for(i=0,first=start;first!=end;first++,i++)
    {
      //assert first and second are not equal index.
      dLapi = pLaplacian[i];

      firstcol = (*first);
      firstrow = firstcol*NumOfPatchVertex;
 
      //pLaplacian does not include value at L[index, j==index], which is diagonal
      //we need therefore to add it specially; we also exploit that the matrix is symmetric
      dValue = diagonal * dLapi;
      A[ indexrow + firstcol] += dValue;
      A[ firstrow + index] += dValue;

      //and now, the rest of values L[index,i]*L[index,j], i!=index && j!=index
      //compute L[index,i]*L[index,j] for i == j and then exploit the symmetry
      //to compute the rest of items (N.B. i!=index && j!=index condition is
      //ensured simply by the fact that pLaplacian does not store these values
      A[ firstrow + firstcol] += dLapi*dLapi; 
      for(j=i+1,second=first+1;second!=end;second++,j++)
      {
        //assert first isn't equal second.
        dValue = dLapi*pLaplacian[j];
        A[firstrow + (*second)] += dValue;
        A[(*second)*NumOfPatchVertex + firstcol] += dValue;
      }
    }
  }  
}

//----------------------------------------------------------------------------
// Multiplies transpose L matrix and vector, i.e., result = L^T*source
void vtkALBAFillingHole::LTransposeMatrixVector( double *source,double *result )
//------------------------------------------------------------------------
{
  int i,j;
  CVertex *pVertex;
  double	*pLaplacian,dSource;
  vtkstd::vector<int>::iterator	onering,end;

  //xi = sum(j = 1..n)bj*Lij = bi*Lii + sum(j = 1..n, j!=i)bj*Lij
  //Lij = 0 if there is no edge from vertex pi to pj =>
  //xi = bi*Lii + sum(j=one-ring(i))bj*Lij
  //b is source, x is result

  memset(result,0,sizeof(double)*NumOfPatchVertex);
  for(i=0;i<NumOfPatchVertex;i++)
  {
    pVertex = PatchVertexes[i];
    pLaplacian = PatchLaplacian[i]->ALaplacian;
    dSource = source[i];

    onering = pVertex->OneRingVertex.begin();
    end = pVertex->OneRingVertex.end();
    for(j=0;onering!=end;onering++,j++)
    {    
      //supposing that the laplacian is stored as the full matrix n x n
      //then actually j goes from 0 to NumOfPatchVertex and it is the index
      //then x[j] = sum(i)(L[i,j]*b[i]), thus in every step of this inner j-loop
      //we add just one element L[i,j]*b[i] to x[j] and as after i-loop
      //is finished, all of n elements are summed.
      //our laplacian is a sparse matrix => x[index(j)] = sum(i)(L[i,j]*b[i])      
      result[*onering] += pLaplacian[j]*dSource;
    }	
    // L[i,i] * source[i]
    result[i] += PatchLaplacian[i]->DLaplacian * dSource;
  }
}

//----------------------------------------------------------------------------
// Multiplies matrix A and vector xyz, i.e., result = A*xyz
// N.B. A is L^T*L + some constraints on diagonal
void vtkALBAFillingHole::LTransposeLMatrixVector( double *A,double *xyz,double *result )
//------------------------------------------------------------------------
{
  int i,j,index;
  double	dValue;
  CVertex    *pVertex;

  vtkstd::vector<int>::iterator	tworing,end;
  vtkstd::vector<CVertex*>::iterator	vertexiterator;

  vertexiterator = PatchVertexes.begin();
  for(index=0,i=0;i<NumOfPatchVertex;i++,index += NumOfPatchVertex,vertexiterator++)
  {
    pVertex = *vertexiterator;		

    //result[i] = sum(j=1..n)A[i,j]*xyz[j]
    //many A[i,j] items are zero, avoid their multiplications to speed up
    //the calculation -> when j is two-ring(i), the A[i,j] != 0
    //Proof: A = L^T*L + weights on diagonal => without weight consideration that
    //A[i,j] = L[1,i]*L[1,j] + L[2,i]*L[2,j] + ... L[n,i]*L[n,j] 
    //=> A[i,j] = sum(k=1..n)L[k,i]*L[k,j]
    //L[k,i] = 0 <==> if there is no edge from the vertex pk to pi && pk != pi
    //L[k,j] = 0 <==> if there is no edge from the vertex pk to pj && pk != pj
    //thus L[k,i]*L[k,j] != 0 <==> 
    //1) if there is an edge from pk to pi and from pk to pj => 
    //vertices pi and pj are connected via the vertex pk => 
    //pj must be in the two-ring of pi
    //2) if k == i or pj and there is direct edge from pi to pj =>
    //pj must be in the one-ring of pi, which is a subset of two-ring of pi
    tworing = pVertex->TwoRingVertex.begin();
    end = pVertex->TwoRingVertex.end();
    for(dValue=0;tworing!=end;tworing++)
    {
      j = *tworing;
      dValue += A[index+j] * xyz[j];
    }
    //include A[i,i]
    result[i] = dValue + A[index+i]*xyz[i];
  }
}

//----------------------------------------------------------------------------
//Solves system of linear equations A*x = b using iterative conjugate gradient
//numerical method - see http://en.wikipedia.org/wiki/Conjugate_gradient
//A = L^T*L (+ some weights), xyz = coordinates (initial guess of the solution),
//b = L^T*b0 (+ some weights)
//N.B. A must be symmetric and positive definite, otherwise, the method may 
//not produce good results (it may not converge)
void vtkALBAFillingHole::LTransposeLConjugateGradient(double *A,double *xyz,double *b)
//----------------------------------------------------------------------------
{
//Pseudocode:
//1.  r_0 = b - A*x_0;
//2.  p_0 = r_0
//3.  k = 0
//4.  repeat 
//5.    alpha_k = (r_k^T*r_k) / (p_k^T*A*p_k)
//6.    x_k+1 = x_k + alpha_k*p_k
//7.    r_k+1 = r_k - alpha_k*A*p_k
//8.    if rk+1 is "sufficiently small" then exit loop
//9.    beta_k = (r_k+1^T*r_k+1) / (r_k^T*r_k)
//10.    p_k+1 = r_k+1 + beta_k*p_k
//11.    k+=1
//12.  end repeat 
//13.  the result is  x_k+1


  double	dTolerance=0.01;
  int		step = 1000;
  double	r_r_new;
  double	alpha;
  double	r_r=0;
  int		k,i;

  double *Ap = new double[NumOfPatchVertex];
  double *p = new double[NumOfPatchVertex];
  double *r = new double[NumOfPatchVertex];

  //1.  r_0 = b - A*x_0;
  //compute Ap = A*xyz (i.e., Ap = (L^T*L)*xyz
  //and initial r_0 (vector of resisum) and measure the total error
  LTransposeLMatrixVector(A,xyz,Ap);	
  for(i=0;i<NumOfPatchVertex;i++)
  {	
    alpha = r[i] = b[i] - Ap[i];
    r_r += alpha*alpha;     //r_r = (r_k^T*r_k)
  }

  //3.  k = 0
  k=0;
  dTolerance *= dTolerance;

  while( k < step && r_r > dTolerance) 
  {
    k++;
    r_r_new = r_r;
    if(k == 1)	
    {
      //2.  p_0 = r_0
      for(i=0;i<NumOfPatchVertex;i++){	p[i] = r[i];	}
    }
    else 
    {
      //9.    beta_k = (r_k+1^T*r_k+1) / (r_k^T*r_k)
      //10.    p_k+1 = r_k+1 + beta_k*p_k
      for(r_r_new=0,i=0;i<NumOfPatchVertex;i++){	r_r_new += r[i]*r[i];	}
      alpha = (r_r_new/r_r);
      for(i=0;i<NumOfPatchVertex;i++){	p[i] = r[i] + alpha*p[i];	}
    }

    //5.    alpha_k = (r_k^T*r_k) / (p_k^T*A*p_k)
    LTransposeLMatrixVector(A, p, Ap);
    for(alpha=0,i=0;i<NumOfPatchVertex;i++){	alpha += p[i]*Ap[i];	}
    alpha = (r_r_new/alpha);

    //6.    x_k+1 = x_k + alpha_k*p_k
    //7.    r_k+1 = r_k - alpha_k*A*p_k
    for(i=0;i<NumOfPatchVertex;i++)
    {	
      xyz[i] += alpha*p[i];	
      r[i] -= alpha*Ap[i];	
    }
    r_r = r_r_new;
  }

  //BES: 18.6.2008 - display a warning to the user that the result may be incorrect
  if (k == step)
  {
#ifdef _MSC_VER
    _RPT1(_CRT_WARN, "vtkALBAFillingHole::LTransposeLConjugateGradient - "
                     "the method does not converge, error: %.2f", r_r);
#endif

    vtkWarningMacro("vtkALBAFillingHole::LTransposeLConjugateGradient - the method does not converge, error:"  << r_r);    
  }

  delete Ap;
  delete p;
  delete r;
}

//----------------------------------------------------------------------------
// smooth a patch and make its curvature match the surrounding mesh
void vtkALBAFillingHole::SmoothPatch()
//----------------------------------------------------------------------------
{  
  if( FillingType == Flat )   
    return;    //flat filling holes.

  //we will smooth the patch using uniform laplacian
  //where every vertex except for inner vertices will be fixed,
  //so check if we have any inner vertex (otherwise we cannot do it)  
  if (NumOfPatchVertex - (int)HolePointIDs.size() == 0)
    return;   //nothing to smooth  

  //Extend the patch to include also neighbours
  //they will determine the shape of smoothing  
  ExtendPatch();  
  
  //1) Smoothing by the minimization of membrane energy is defined as follows.
  //Smoothing without any constraints (i.e., no vertex has fixed coordinates)
  //is the task of solving the system of linear equations:
  //L*X = 0, where L is the laplacian matrix n x n where n is number
  //of points to be smoothed and
  //
  //if we have constraints (and we have them, indeed) then we have 
  //system of m equations for n unknowns:
  //A'*X = b', where A' is m x n matrix consisting of two blocks - L and F
  //F is n x n matrix in which 
  //  F[i,j] = 0, if the vertex pi is not fixed one, or i != j
  //           W (weight), if the vertex pi is the fixed one and i == j      
  //b' = n x 1 vector consisting of n zeroes and n fixed vertex coordinates
  //scaled by the same weight (W) as in F
  //
  //This system of linear equations can be solved if we actually solve:
  //A'^T*A'*X = A'^T*b' => A*X = b where A is a matrix n x n and b is vector n x 1  

  //2) Smoothing by the minimization of thin plate energy is defined as follows.
  //Kobbelt L, Campagna S, Vorsatz J, Seidel HP: Interactive Multi-Resolution
  //Modeling on Arbitrary Meshes. SIGGRAPH 98 Conference Proceedings.
  //
  //Smoothing is the task of solving the system of N linear equations such that:
  //U2(pi) = 0, where pi is patch vertex (NOT from extended patch) and U2 is 
  //second derivative of the umbrella operator U(pi). 
  //U(pi) = -pi + sum_j( ||pi-pj||*pj ) / sum_j( ||pi-pj|| ); pi and pj are connected
  //=> U(pi) = -pi + sum_j(L[i,j]*pj), where L is the Laplacian - see above
  //U2(pi) = -U(pi) + sum_j(L[i,j]*U(pj))
  
  //create internal structures
  BuildPatch();

  if (FillingType == SmoothMembrane)
  {

    //create the laplacian matrix L
    BuildPatchLaplacian();  

    MembraneSmoothing();
  }
  else  
  {
    //RELEASE NOTE: ThinPlateSmoothing does not use Laplacian
    ThinPlateSmoothing();    
  }
}

//------------------------------------------------------------------------
//The core of smoothing by the minimization of membrane energy
void vtkALBAFillingHole::MembraneSmoothing()
//------------------------------------------------------------------------
{
  const static double Wp = 3.0;  
  CVertex* pVertex;

  double	*A = new double[NumOfPatchVertex*NumOfPatchVertex];
  double	*b_temp = new double[NumOfPatchVertex];
  double	*b = new double[NumOfPatchVertex];
  double	*xyz = new double[NumOfPatchVertex];  

  //compute A = A'^T*A', thus A[i,j] = sum(k=1..2n)A'[k,i]*A'[k,j] = 
  //sum(k=1..n)L[k,i]*L[k,j] + sum(k=n+1..2n)A'[k,i]*A'[k,j]
  //first, compute L^T*L (i.e., the first sum)
  ComputeLTransposeLMatrix(A);

  //next, add the second sum, so 
  //add a weight for every fixed vertex (i.e., vertices from original mesh)
  //to minimize their movement during the computation

  //as matrix F contains only one value in one row and column =>
  //F^T*F contains non-zero values only on diagonal    
  for(int i = 0; i < NumOfPatchVertex; i++)
  {    
    pVertex = PatchVertexes[i];
    if( pVertex->BMarked == false)  
      continue; //not fixed one

    A[i*NumOfPatchVertex+i] += Wp*Wp;
  }

  //for x, y and z coordinates (index) do
  for(int index=0;index<3;index++)
  {
    //set b_temp value.
    //b = A'^T*b' => b[i] = sum(k=1..2n)A'[k,i]*b'[i] =>
    //b[i] = sum(k=1..n)L[k,i]*b'[i] + sum(k=n+1..2n)A'[k,i]*b'[i]

    for(int i=0;i<NumOfPatchVertex;i++)
    {
      //BES: 18.6.2008 - currently dLaplacianCoord is always 0
      //but this may change in the future, so I leave it heare
      b_temp[i] = PatchLaplacian[i]->DLaplacianCoord[index];
      xyz[i] = PatchVertexes[i]->DCoord[index];
    }

    //first, compute b = L^T*b_temp, i.e., the first sum
    LTransposeMatrixVector(b_temp,b);

    //next, add weights for every fixed vertex, i.e., the second sum
    //due to the character of submatrix F, A'[k,i] != 0 
    //<==> k == i + n and the vertex pi is fixed one
    //=> b[i] += A'[i+n,i]*b'[i]
    for(int i=0;i<NumOfPatchVertex;i++)
    {
      pVertex = PatchVertexes[i];
      if( pVertex->BMarked == false)
        continue;

      b[i] += Wp*(pVertex->DCoord[index]*Wp);     
    }

    //solve the value of x,y,z for the system of linear equations A*xyz = b
    //using a numeric method with xyz initial solution
    LTransposeLConjugateGradient(A,xyz,b);

    //save the coordinate of each vertex.
    for(int i=0;i<NumOfPatchVertex;i++)
    {
      PatchVertexes[i]->DCoord[index] = xyz[i];			
    }
  }

  delete A;
  delete xyz;
  delete b;
  delete b_temp;
}

//------------------------------------------------------------------------
//The core of smoothing by the minimization of thin plate energy
void vtkALBAFillingHole::ThinPlateSmoothing()
//------------------------------------------------------------------------
{
  //Brute Force approach implemented according to: Leif Kobbelt, Swen
  //Campagna, Jens Vorsatz, and Hans-Peter Seidel. Interactive
  //Multi-Resolution Modeling on Arbitrary Meshes. SIGGRAPH 98 Conference
	//Proceedings.

  //It works with uniform weights and very often requires
  //very large number of repetitions (nevertheless, it works
  //unlike the unstable matrix method (and does not need memory))

  double* valences = new double[NumOfPatchVertex];
  memset(valences, 0, sizeof(double)*NumOfPatchVertex);
  for (int i = 0; i < NumOfPatchVertex; i++)
  {
    CVertex* pi = PatchVertexes[i];
    if (pi->BMarked)
      continue; //vertex is on the boundary => it has known coordinates

    int nCount = (int)pi->OneRingVertex.size();
    for (int m = 0; m < nCount; m++)
    { 
      int j = pi->OneRingVertex[m];            

      CVertex* pj = PatchVertexes[j];
      int nCount2 = (int)pj->OneRingVertex.size();
      valences[i] += 1.0 / nCount2; 
    }

    valences[i] = 1.0 / (1 + (1.0/nCount)*valences[i]);
  }
  
  for (int nStep = 0; nStep < SmoothThinPlateSteps; nStep++)
  {
    double dblRes = 0.0;
    for (int i = 0; i < NumOfPatchVertex; i++)
    {
      CVertex* pi = PatchVertexes[i];
      if (pi->BMarked)  
        continue; //vertex is on the boundary => it has known coordinates
      
      double U_pi[3] = {0, 0, 0};
      double U2_pi[3] = {0, 0, 0};

      int nCount = (int)pi->OneRingVertex.size();
      for (int m = 0; m < nCount; m++)
      {        
        CVertex* pj = PatchVertexes[pi->OneRingVertex[m]];
        for (int k = 0; k < 3; k++){ 
          U_pi[k] += pj->DCoord[k];
        }

        double U_pj[3] = {0, 0, 0};
        int nCount2 = (int)pj->OneRingVertex.size();
        for (int n = 0; n < nCount2; n++)
        {            
          CVertex* pk = PatchVertexes[pj->OneRingVertex[n]];
          for (int k = 0; k < 3; k++){ 
            U_pj[k] += pk->DCoord[k];
          }            
        }

        for (int k = 0; k < 3; k++)
        { 
          U_pj[k] = -pj->DCoord[k] + U_pj[k] / nCount2;
          U2_pi[k] += U_pj[k];
        }
      }//end for m

      for (int k = 0; k < 3; k++)
      { 
        U_pi[k] = -pi->DCoord[k] + U_pi[k] / nCount;
        U2_pi[k] = -U_pi[k] + U2_pi[k] / nCount;

        pi->DCoord[k] -= valences[i]*U2_pi[k];
      }    
    }
  }

  delete[] valences;

/*
  double	*A = new double[NumOfPatchVertex*NumOfPatchVertex];  
  double	*b = new double[NumOfPatchVertex];
  double	*xyz = new double[NumOfPatchVertex]; 

  double** A_tmp = new double*[NumOfPatchVertex];
  
  //U(pi) = -pi + sum_j(L[i,j]*pj), where L is the Laplacian
  //U2(pi) = -U(pi) + sum_j(L[i,j]*U(pj)) =>
  //U2(pi) = pi - sum_j(L[i,j]*pj) + sum_j(L[i,j]* (-pj + sum_k(L[j,k]*pk)) =>
  //U2(pi) = pi - 2*sum_j(L[i,j]*pj) + sum_j(L[i,j]*sum_k(L[j,k]*pk)) = 0

  //compute solution matrix A with rows U2
  memset(A, 0, NumOfPatchVertex*NumOfPatchVertex);  
  memset(b, 0, NumOfPatchVertex*sizeof(double));

  double* A_row = A;
  for (int i = 0; i < NumOfPatchVertex; i++, A_row += NumOfPatchVertex)
  {
    A_tmp[i] = A_row;

    A_row[i] = 1.0;   //diagonal contains A    

    CVertex* pi = PatchVertexes[i];
    if (pi->bMarked)  
      continue; //vertex is on the boundary => it has known coordinates

    CLaplacian* Lij = PatchLaplacian[i];

    int nCount = (int)pi->OneRingVertex.size();
    for (int m = 0; m < nCount; m++)
    { 
      int j = pi->OneRingVertex[m];      
      A_row[j] += -2*(1.0 / nCount);//Lij->aLaplacian[m];      

      CVertex* pj = PatchVertexes[j];
      CLaplacian* Ljk = PatchLaplacian[j];

      int nCount2 = (int)pj->OneRingVertex.size();
      for (int n = 0; n < nCount2; n++)
      {
        int k = pj->OneRingVertex[n];
        A_row[k] += 1.0 / (nCount * nCount2);
          //Lij->aLaplacian[m]*Ljk->aLaplacian[n];
      }
    }         
  }  

  //for x, y and z coordinates (index) do
  for(int index=0;index<3;index++)
  {    
    for(int i=0;i<NumOfPatchVertex;i++)
    {
      CVertex* pi = PatchVertexes[i];
      if (pi->bMarked)
        b[i] = pi->dCoord[index];

      xyz[i] = pi->dCoord[index];
    }

    ////solve the value of x,y,z for the system of linear equations A*xyz = b
    ////using a numeric method with xyz initial solution
    //LTransposeLConjugateGradient(A,xyz,b);

    memcpy(xyz, b, NumOfPatchVertex*sizeof(double));
    vtkMath::SolveLinearSystem(A_tmp, xyz, NumOfPatchVertex);

    //save the coordinate of each vertex.
    for(int i=0;i<NumOfPatchVertex;i++){
      PatchVertexes[i]->dCoord[index] = xyz[i];			
    }
  }

  delete A;
  delete xyz;
  delete b;  
*/
}
//----------------------------------------------------------------------------
// Merge the patch for a hole to origin mesh
void vtkALBAFillingHole::MergePatch()
//----------------------------------------------------------------------------
{
  CVertex	*pVertex,*pNewVertex;
  CTriangle	*pNewTriangle;
  int v0,v1,v2;
	vtkIdType *pVertexIndex;
  int NumOfNewTriangle;

  vtkstd::vector<CVertex*>::iterator	vertex,vertexstart;
  vtkstd::vector<CVertex*>::iterator	vertexend;
  vtkstd::vector<CTriangle*>::iterator	triangle;
  vtkstd::vector<CTriangle*>::iterator	triangleend;

  vertexstart = PatchVertexes.begin();
  vertexend = PatchVertexes.end();
  for(vertex=vertexstart;vertex!=vertexend;vertex++)
  {
    pVertex = *vertex;
    if( pVertex->BMarked == true) continue;

    //record the changing for merge triangle 
    pVertex->Id = NumOfVertex;	

    // the vertex in patch mesh is n't in original mesh
    // create a new vertex for the original mesh.
    pNewVertex = new CVertex(pVertex->DCoord);
    pNewVertex->Id = NumOfVertex++;
    Vertexes.push_back(pNewVertex);
  }

  triangle = PatchTriangles.begin();
  triangleend = PatchTriangles.end();
  NumOfNewTriangle = 0;
  for(;triangle!=triangleend;triangle++)
  {
    if( (*triangle)->BMarked == true )  continue;
    pVertexIndex = (*triangle)->AVertex;

    v0 = vertexstart[pVertexIndex[0]]->Id;
    v1 = vertexstart[pVertexIndex[1]]->Id;
    v2 = vertexstart[pVertexIndex[2]]->Id;

    pNewTriangle = new CTriangle(v0,v1,v2);
    pNewTriangle->Id = NumOfTriangle++;
    NumOfNewTriangle++;
    Triangles.push_back(pNewTriangle);
  }
  UpdateMesh(NumOfTriangle-NumOfNewTriangle);
}

//----------------------------------------------------------------------------
void vtkALBAFillingHole::ClearMesh()
//----------------------------------------------------------------------------
{
  int i;
  for(i=0;i<NumOfEdge;i++)
  {
    delete  Edges[i];
  }
  for(i=0;i<NumOfTriangle;i++)
  {
    delete  Triangles[i];
  }
  for(i=0;i<NumOfVertex;i++)
  {
    delete  Vertexes[i];
  }
  Vertexes.clear();
  Triangles.clear();
  Edges.clear();

  NumOfVertex = 0;
  NumOfTriangle = 0;
  NumOfEdge = 0;
}
/*
//----------------------------------------------------------------------------
//Build internal mesh structure filtering out invalid, non-manifold triangles
void vtkALBAFillingHole::InitManifoldMesh()
//----------------------------------------------------------------------------
{
  //create initial mesh
  InitMesh();

  //Detect, if this is manifold    
  int i,j, t, tmp;
  int sv1, sv2, dv1, dv2;    
  bool bflag;

  CTriangle	*pTriangle;

  vtkstd::map<long long, int> mapEdges;  
  typedef vtkstd::map<long long, int>::iterator EdgesIterator;

  for (bflag = false, t = 0; 
    bflag ==false && t < NumOfTriangle; t++)
  {
    pTriangle = Triangles[t];
    for (i = 0; i < 3; i++)
    {
      sv1 = pTriangle->aVertex[i];
      sv2 = pTriangle->aVertex[(i + 1) % 3];

      long long key;
      if (sv1 < sv2)      
        key = ((long long)sv1) << 32 | sv2;      
      else
        key = ((long long)sv2) << 32 | sv1;
      
      EdgesIterator iter = mapEdges.find(key);
      if (iter == mapEdges.end())
        mapEdges[key] = 0;          //new edge
      else if (iter->second == 0)   //already existing edge, but with just one neighbor
        iter->second++;
      else
      {
        bflag = true;               //non-manifold, this edge already has two triangles
        break;
      }
    }   
  }

  if (!bflag)
    return; //it is manifold mesh


  //The algorithm:
  //1. Create Vertex - Triangles association
  //2. For every vertex do the following
  //3. Starting with the first valid associated triangle, detect 
  //   every fan (full or open) of continuous vertices where no 
  //   vertex is used more than once
  //4. If there is just one fan, it is manifold, otherwise
  //   delete (mark as invalid) every associate triangle not 
  //   enlisted in the largest fan  
        
  CVertex		*pVertex;  
    
  int nMaxDegree = 0;
  int nNewNumOfTriangle = NumOfTriangle;  
  for (t = 0; t < NumOfTriangle; t++)
  {
    int* pVertexIndex = Triangles[t]->aVertex;
    //each triangle has three vertex;
    for(i=0;i<3;i++)
    {
      pVertex = Vertexes[pVertexIndex[i]];
      //add the triangle to the vertex as an first degree ring triangle.
      pVertex->OneRingTriangle.push_back(t);

      int nDegree = pVertex->OneRingTriangle.size();
      if (nDegree > nMaxDegree)
        nMaxDegree = nDegree;   //update max vertex degree      
    }
  }

  //buffer for numbers of elements in FansIdx fro the vertex
  int* FansCounts = new int[nMaxDegree];

  //buffer for indices of fans in the format V1,T1,V2,T2,V3 ....Tn
  //where Vi and Vi+1 are vertices of the triangle Ti
  int* FansIdx = new int[nMaxDegree*nMaxDegree*2];

  //check every vertex
  bool bWrongOrient = false;  
  for (sv1 = 0; sv1 < NumOfVertex; sv1++)
  {
    //mark all triangles around the vertex v as unprocessed
    CVertex	*pVertex = Vertexes[sv1];    
    int nTriCount = (int)pVertex->OneRingTriangle.size();

    int nToProcess = 0;
    for (int k = 0; k < nTriCount; k++) 
    {
      pTriangle = Triangles[pVertex->OneRingTriangle[k]];            
      if (false == (pTriangle->BMarked = pTriangle->bDeleted))
        nToProcess++; //increment number of valid triangles around the current vertex
    }

    int nFans = 0;    //number of detected fans    
    while (nToProcess > 0)
    {
      int nFanPos = nFans*nMaxDegree;

      //start from the first not marked triangle      
      for (int k = 0; k < nTriCount; k++) 
      {
        pTriangle = Triangles[pVertex->OneRingTriangle[k]];
        if (!pTriangle->BMarked)
          break;  //we found it       
      }

      //find the position of vertex sv1
      for (j = 0; j < 3; j++) {
        if (pTriangle->aVertex[j] == sv1)
          break;  //we found it      
      }

      sv2 = pTriangle->aVertex[(j + 1) % 3];
      FansIdx[nFanPos] = sv2;
      FansCounts[nFans] = 1;

      bool bSecondTrack = false;
      while (true) 
      {
        //search for the edge sv1,sv2 in the list of triangles
        int neight;
        bool bFound = false;        
        for (int k = 0; k < nTriCount; k++) 
        {
          neight = pVertex->OneRingTriangle[k];
          pTriangle = Triangles[neight];      
          if (!pTriangle->BMarked)
          {
            //we have a valid triangle
            for (j = 0; j < 3; j++) 
            {
              if (pTriangle->aVertex[j] == sv1) {
                bFound = true; break;  //we found it
              }
            }

            if (!bSecondTrack)
            {
              dv1 = pTriangle->aVertex[(j + 1) % 3];
              dv2 = pTriangle->aVertex[(j + 2) % 3];
            }
            else
            {
              dv1 = pTriangle->aVertex[(j + 2) % 3];
              dv2 = pTriangle->aVertex[(j + 1) % 3];
            }

            if (dv2 == sv2)
            {
              //this triangle has inconsistent orientation              
              tmp = dv1; dv1 = dv2; dv2 = tmp;        
              bWrongOrient = true;
            }

            //the edge sv1,sv2 = sv1,dv1
            //the next triangle will continue with the edge sv1,dv2
            if (dv1 == sv2) {
              bFound = true; break;  //found it
            }
          }
        }//end for

        if (bFound)
        {
          //we found some triangle that may be suitable for continuing
          //search in the list of vertices of the current fan whether dv2 is unique
          bflag = false;
          for (j = 0; j < FansCounts[nFans]; j += 2)
          {
            if (FansIdx[nFanPos + j] == dv2)
            {
              bflag = true;
              break;
            }
          }

          if (bflag)
          {
            //if we returned back to some already processed vertex, then it is
            //either the first vertex, where we started our search, or
            //the surface intersect itself (it is not a manifold)
            //at any rate, our search ends here
            if (dv2 == FansIdx[nFanPos])
            {
              FansIdx[nFanPos + FansCounts[nFans]] = neight; //triangle ID
              FansCounts[nFans] += 1;

              pTriangle->BMarked = true;
              nToProcess--;
              break;
            }

            //we may continue there
            //neight = end;
          }
          else
          {
            //pTriangle is the triangle where we will continue
            FansIdx[nFanPos + FansCounts[nFans]] = neight; //triangle ID
            FansIdx[nFanPos + FansCounts[nFans] + 1] = dv2;
            FansCounts[nFans] += 2;

            pTriangle->BMarked = true;
            nToProcess--;
            sv2 = dv2;
            continue;
          }
        } //end if (neight != end)
        
        //assert(neight == end);
          
        //there is no triangle to continue in this direction,
        //which means that the current edge sv1, sv2 is the boundary edge
        if (bSecondTrack) {
          break;  //this fan is completed, the fan is open
        }
        else
        {
          //if we did not search yet from the other direction, do it now
          //reverse FansIdx and continue the search with the edge sv1,sv_end
          sv2 = FansIdx[nFanPos]; //sv_end
          for (j = 0; j < FansCounts[nFans] / 2; j++)
          {
            tmp = FansIdx[nFanPos + j];
            FansIdx[nFanPos + j] = FansIdx[nFanPos + FansCounts[nFans] - j - 1];
            FansIdx[nFanPos + FansCounts[nFans] - j - 1] = tmp;              
          }

          bSecondTrack = true;
          continue;
        }
      } //end while (true)

      nFans++;
    } //end while
   
    //now we know how many fans do we have there
    if (nFans > 1)
    {
      //so, the mesh is not manifold
      //we will keep only the largest fan, the rest will be deleted
      //which will create a hole that can be later filled

      int iLargestPos = 0;      
      for (j = 1; j < nFans; j++)
      {
        if (FansCounts[j] > FansCounts[iLargestPos])                
          iLargestPos = j;  //update largest        
      }

      int nFansPos = 0;
      for (j = 0; j < nFans; j++, nFansPos += nMaxDegree)
      {
        if (j == iLargestPos)
          continue; //let this untouched
              
        //go through every triangle in the fan
        for (i = 1; i < FansCounts[j]; i += 2)
        {
          pTriangle = Triangles[FansIdx[nFansPos + i]];
          pTriangle->bDeleted = true;
          nNewNumOfTriangle--;
        }
      }
    }

    //release OneRingTriangle - it will be created in BuildMesh
    pVertex->OneRingTriangle.clear();
  } //end for (vertices)

  delete[] FansIdx;
  delete[] FansCounts;

  if (nNewNumOfTriangle != NumOfTriangle)
  {
    //delete invalid triangles physically    
    vtkstd::vector< CTriangle* > newTriangles(nNewNumOfTriangle);
    nNewNumOfTriangle = 0;

    for (t = 0; t < NumOfTriangle; t++)
    {
      pTriangle = Triangles[t];
      if (pTriangle->bDeleted)
        delete pTriangle;
      else
      {
        pTriangle->BMarked = false;
        pTriangle->id = nNewNumOfTriangle++;
        
        newTriangles[pTriangle->id] = pTriangle;
      }
      
    } //end for

    NumOfTriangle = nNewNumOfTriangle;
    Triangles.assign(newTriangles.begin(), newTriangles.end());
  }  
}  
*/

//----------------------------------------------------------------------------
// Initialize the internal relationship of a mesh
void vtkALBAFillingHole::BuildMesh()
//----------------------------------------------------------------------------
{
  int i,j,t;
  int sv1,sv2;
  int dv1,dv2;
  bool bflag;

  double dLength;
	vtkIdType *pVertexIndex,*pVertexIndex2,*pEdgeIndex;
  
  CEdge		*pEdge;
  CTriangle	*pTriangle, *pNeighTriangle;
  CVertex		*pVertex,*pVertex1,*pVertex2;
  
  for( t=0; t<NumOfTriangle; t++)
  {
    pVertexIndex = Triangles[t]->AVertex;
    //each triangle has three vertex;
    for(i=0;i<3;i++)
    {
      pVertex = Vertexes[pVertexIndex[i]];
      //add the triangle to the vertex as an first degree ring triangle.
      pVertex->OneRingTriangle.push_back(t);
    }
  }

  NumOfEdge = 0;
  Edges.resize(3*NumOfTriangle);
  for( t=0; t<NumOfTriangle; t++)
  {
    pTriangle = Triangles[t];		
    pVertexIndex = pTriangle->AVertex;
    pEdgeIndex = pTriangle->AEdge;
    //each triangle has three vertex;
    for(i=0;i<3;i++)
    {
      if( pEdgeIndex[i] >= 0 )				continue;

      pEdgeIndex[i] = NumOfEdge;

      sv1 = pVertexIndex[i];
      sv2 = pVertexIndex[(i+1)%3];

      pVertex1 = Vertexes[sv1];
      pVertex2 = Vertexes[sv2];

      pVertex1->OneRingVertex.push_back(sv2);
      pVertex2->OneRingVertex.push_back(sv1);
      pVertex1->OneRingEdge.push_back(NumOfEdge);
      pVertex2->OneRingEdge.push_back(NumOfEdge);

      dLength = VEC3_SQUAREDIST(pVertex1->DCoord,pVertex2->DCoord);
      dLength = sqrt(dLength);
      pVertex1->DOneRingEdgeLength += dLength;
      pVertex2->DOneRingEdgeLength += dLength;

      pEdge = new CEdge(sv1,sv2);
      pEdge->Id = NumOfEdge;
      pEdge->AVertex[2] = pVertexIndex[(i+2)%3];
      pEdge->ATriangle[0] = t;
      pEdge->DLength = dLength;
      Edges[NumOfEdge] = pEdge;

      bflag = false;
      int tricount = (int)pVertex1->OneRingTriangle.size();      
      for(int k = 0; k < tricount; k++ )
      {
        int neight = pVertex1->OneRingTriangle[k];
        //as triangles are ascending ordered in vertices
        //any neighbouring triangle associated with pVertex1 
        //having lower id was already processed previously
        if( neight <= t) continue;
        pNeighTriangle = Triangles[neight];	
        pVertexIndex2 = pNeighTriangle->AVertex;

        for(j=0; j<3; j++)
        {
          // when find the edge of the neighbor triangle was computed, continue find 
          // next edge of the neighbor triangle.
          if( pNeighTriangle->AEdge[j] >=0 ) continue;	

          //N.B. orientation of triangles should be consistent
          dv1 = pVertexIndex2[j];
          dv2 = pVertexIndex2[(j+1)%3];

          if (sv2 == dv2 && sv1 == dv1)
          {
            //the mesh is not properly oriented, the neighbouring triangle has orientation different
            //from the currently processed triangle => hot fix to prevent holes
            vtkWarningMacro(<< "inconsistent orientation of triangles\n");
            
            bflag = true;
            break;
          }

          if( sv2 == dv1 && sv1==dv2 )
          {
            pNeighTriangle->AEdge[j] = NumOfEdge;
            if( pEdge->ATriangle[1] < 0 )
            {
              // if the mesh is manifold, each edge only have two adjacent triangles.
              pEdge->AVertex[3] = pVertexIndex2[(j+2)%3];
              pEdge->ATriangle[1] = neight;
            }
            else
            {
              // if the mesh is non-manifold, each edge have more two adjacent triangles.
              // cout << "it isn't an manifold, one edge  have more two adjacent triangles\n";
              vtkWarningMacro(<< "it isn't a manifold, one edge  have more two adjacent triangles\n");
              //assert(false);              
            }
            bflag = true;
            break;						
          }          
        }
      }
      if( bflag == false )
      {
        //find a boundary edge and vertex
        pEdge->BBoundary = true;
        pVertex1->BBoundary = true;
        pVertex2->BBoundary = true;
      }

      NumOfEdge++;
    }
  }
  Edges.resize(NumOfEdge);
}

//----------------------------------------------------------------------------
// update the internal relationship of a mesh
void vtkALBAFillingHole::UpdateMesh(int id)
//----------------------------------------------------------------------------
{
  int i,j,t;
  int sv1,sv2;
  int dv1,dv2;
  int nEdge;
  bool bflag;

  double dLength;
	vtkIdType *pVertexIndex,*pVertexIndex2;

  CVertex		*pVertex,*pVertex1,*pVertex2;
  CTriangle	*pTriangle, *pNeighTriangle;
  CEdge		*pEdge;

  vtkstd::vector<int>::iterator	neight,end;

  for(t=id; t<NumOfTriangle; t++)
  {
    pTriangle = Triangles[t];		
    pVertexIndex = pTriangle->AVertex;
    //each triangle has three vertex;
    for(i=0;i<3;i++)
    {
      pVertex = Vertexes[pVertexIndex[i]];
      //add the triangle to the vertex as an first degree ring triangle.      
      pVertex->OneRingTriangle.push_back(t);
    }
  }

  //Euler formula
  Edges.reserve(NumOfVertex+NumOfTriangle-1);		
  for(t=id; t<NumOfTriangle; t++)
  {
    pTriangle = Triangles[t];		
    pVertexIndex = pTriangle->AVertex;
    //each triangle has three vertex;
    for(i=0;i<3;i++)
    {
      if( pTriangle->AEdge[i] >= 0 )				continue;

      sv1 = pVertexIndex[i];
      sv2 = pVertexIndex[(i+1)%3];

      pVertex1 = Vertexes[sv1];
      pVertex2 = Vertexes[sv2];

      pEdge = new CEdge(sv1,sv2);			

      bflag = false;
      nEdge = -1;
      end = pVertex1->OneRingTriangle.end();
      neight = pVertex1->OneRingTriangle.begin();
      for( ;neight != end&&bflag!=true; neight++ )
      {
        if( *neight == t) continue;
        pNeighTriangle = Triangles[*neight];	
        pVertexIndex2 = pNeighTriangle->AVertex;

        for(j=0; j<3; j++)
        {
          dv1 = pVertexIndex2[j];
          dv2 = pVertexIndex2[(j+1)%3];
          if( sv2 == dv1 && sv1==dv2 )
          {
            nEdge = pNeighTriangle->AEdge[j];
            if( nEdge<0 )
            {
              pNeighTriangle->AEdge[j] = NumOfEdge;
              pEdge->AVertex[3] = pVertexIndex2[(j+2)%3];
              pEdge->ATriangle[1] = *neight;
            }
            bflag = true;
            break;						
          }
        }
      }
      if( nEdge < 0 )
      {		
        pTriangle->AEdge[i] = NumOfEdge;

        pVertex1->OneRingVertex.push_back(sv2);
        pVertex2->OneRingVertex.push_back(sv1);			
        pVertex1->OneRingEdge.push_back(NumOfEdge);
        pVertex2->OneRingEdge.push_back(NumOfEdge);

        dLength = VEC3_SQUAREDIST(pVertex1->DCoord,pVertex2->DCoord);
        dLength = sqrt(dLength);
        pVertex1->DOneRingEdgeLength += dLength;
        pVertex2->DOneRingEdgeLength += dLength;

        pEdge->Id = NumOfEdge;
        pEdge->AVertex[2] = pVertexIndex[(i+2)%3];
        pEdge->ATriangle[0] = t;
        pEdge->DLength = dLength;
        Edges.push_back(pEdge);

        NumOfEdge++;
      }
      else
      {
        delete pEdge;

        pTriangle->AEdge[i] = nEdge;
        pEdge = Edges[nEdge];
        // if the mesh is manifold, each edge only have two adjacent triangles.
        pEdge->AVertex[3] = pVertexIndex[(i+2)%3];
        pEdge->ATriangle[1] = t;
        if( pEdge->BBoundary==true )
        {
          //find a boundary edge and vertex in original mesh
          //now change it as an internal edge and vertex
          pEdge->BBoundary = false;
          pVertex1->BBoundary = false;
          pVertex2->BBoundary = false;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
void vtkALBAFillingHole::SetFillAHole(int id)
//----------------------------------------------------------------------------
{
  //filling a hole
  FillingHoles = 1;  
  BorderPointID = id;

  Modified();
}

//----------------------------------------------------------------------------
void vtkALBAFillingHole::SetFillAllHole()
//----------------------------------------------------------------------------
{
  //filling all holes
  FillingHoles = 0;  
}

//----------------------------------------------------------------------------
bool vtkALBAFillingHole::FindAHole()
//----------------------------------------------------------------------------
{
  int *pEdgeVertex;
  bool bFlag;

  CEdge *pEdge;
  CVertex *pVertex, *pStartVertex;
  vtkstd::vector<int>::iterator ringedge;	

  if( BorderPointID >= NumOfVertex) 
    return false;
  pVertex = Vertexes[BorderPointID];
  if(pVertex->BBoundary == false)	
    return false;	//it is a interior vertex or a alone vertex
  if(pVertex->BMarked == true )   
    return false;

  pStartVertex = pVertex;
  //start clock-wise search boundary vertexes and edges.
  //maybe count-clock-wise search is better.
  do{
    bFlag = false;
    for( ringedge = pStartVertex->OneRingEdge.begin(); ringedge != pStartVertex->OneRingEdge.end(); ringedge++ )
    {
      pEdge = Edges[*ringedge];
      if( pEdge->BBoundary == false)	continue;	
      pEdgeVertex = pEdge->AVertex;
      //only the edge is (pStartVertex,nextVertex).
      if( pEdgeVertex[0] == pStartVertex->Id ) 
      {
        //find the next vertex, if it was marked, the boundary isn't correct.
        if( pStartVertex->BMarked == true ) 
          break;
        HolePointIDs.push_back(pStartVertex->Id);
        HoleEdgeIDs.push_back(pEdge->Id);
        pStartVertex->BMarked = true;

        pStartVertex = Vertexes[pEdgeVertex[1]];
        bFlag = true;
        break;
      }
    }
    //when the mesh isn't a manifold,we can't find the next vertex.
    //should stop the cycle.
  }while(pStartVertex->Id != pVertex->Id && bFlag == true);
  if( bFlag == false )
  {
    //encounter a single vertex.
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------
bool vtkALBAFillingHole::FindNextHole()
//----------------------------------------------------------------------------
{
  int i;
  CVertex *pVertex;
  bool flag=false;

  for(i=BorderPointID; i<NumOfVertex; i++)
  {
    pVertex = Vertexes[i];
    if(pVertex->BBoundary == false)	continue;	//it is a interior vertex or a alone vertex
    BorderPointID = i;

    HoleEdgeIDs.clear();
    HolePointIDs.clear();

    flag = FindAHole();

    if( flag == false )     continue;
    else break;
  }
  return flag;
}

//----------------------------------------------------------------------------
void vtkALBAFillingHole::InitMesh()
//----------------------------------------------------------------------------
{
  int i;
  int v0,v1,v2;
  double pCoord[3];
  CVertex *pVertex;
  CTriangle *pTriangle;
  vtkIdList *ptids;

  InputMesh = this->GetInput();
  OutputMesh = this->GetOutput();

  NumOfTriangle = InputMesh->GetNumberOfCells();
  NumOfVertex = InputMesh->GetNumberOfPoints();

  Vertexes.resize(NumOfVertex);
  Triangles.resize(NumOfTriangle);
  
  for(i=0;i<NumOfVertex;i++)
  {
    InputMesh->GetPoint(i,pCoord);
    pVertex = new CVertex(pCoord);
    pVertex->Id = i;

    //BES: 31.7.2008 - the vertex degree is usually 6 => avoid reallocate
    //in push_back (it is very slow)
    pVertex->OneRingTriangle.reserve(6);
    pVertex->OneRingEdge.reserve(6);
    pVertex->OneRingVertex.reserve(6);
    pVertex->TwoRingVertex.reserve(12);

    Vertexes[i] = pVertex;
  }
  InputMesh->BuildCells();
  for(i=0; i<NumOfTriangle; i++)
  {
    ptids = InputMesh->GetCell(i)->GetPointIds();
    v0 = ptids->GetId(0);            
    v1 = ptids->GetId(1);            
    v2 = ptids->GetId(2);            
    pTriangle = new CTriangle(v0,v1,v2);
    pTriangle->Id = i;
    Triangles[i] = pTriangle;
  }
}

//----------------------------------------------------------------------------
void vtkALBAFillingHole::DoneMesh()
//----------------------------------------------------------------------------
{
  int i;

  // set up polydata object and data arrays
  vtkPoints *points = vtkPoints::New() ;
  vtkCellArray *triangles = vtkCellArray::New() ;

  // insert data into point array
  // InsertPoint() can allocate its own memory if not enough pre-allocated
  points->Allocate(NumOfVertex) ;       
  for (i = 0 ;  i < NumOfVertex ;  i++)
    points->InsertPoint(i, Vertexes[i]->DCoord) ; 

  // insert data into cell array
  triangles->Allocate(NumOfTriangle) ;   
  for (i = 0 ;  i < NumOfTriangle ;  i++)
    triangles->InsertNextCell(3, Triangles[i]->AVertex) ;

  OutputMesh->SetPoints(points) ;
  OutputMesh->SetPolys(triangles) ;

  points->Delete();
  triangles->Delete();
}
void vtkALBAFillingHole::BuildPatchOutput()
//----------------------------------------------------------------------------
{
  int i;

  LastPatch->Initialize();
  // set up polydata object and data arrays
  vtkPoints *points = vtkPoints::New() ;
  vtkCellArray *triangles = vtkCellArray::New() ;

  // insert data into point array
  // InsertPoint() can allocate its own memory if not enough pre-allocated
  int NumOfVertexPatch = PatchVertexes.size();
  points->Allocate(NumOfVertexPatch) ;       
  for (i = 0 ;  i < NumOfVertexPatch ;  i++)
    points->InsertPoint(i, PatchVertexes[i]->DCoord) ; 

  // insert data into cell array
  int PatchNumOfTriangle = PatchTriangles.size();
  triangles->Allocate(PatchNumOfTriangle) ;   
  for (i = 0 ;  i < PatchNumOfTriangle ;  i++)
    triangles->InsertNextCell(3, PatchTriangles[i]->AVertex) ;

  LastPatch->SetPoints(points) ;
  LastPatch->SetPolys(triangles) ;

  points->Delete();
  triangles->Delete();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vtkALBAFillingHole::Execute()
//----------------------------------------------------------------------------
{  
  //InitManifoldMesh();
  InitMesh();
  BuildMesh();
  if(FillingHoles == 0)
  {
    //filling all holes.
    while(FindNextHole()==true)
    {
      NumOfPatchVertex = HolePointIDs.size();
      NumOfPatchTriangle = 0;
      NumOfPatchEdge = 0;
      CreatePatch();
      RefinePatch();
      SmoothPatch();
      MergePatch();
      ClearPatch();
    }
  }
  else //if(FillingHoles == 1)
  {
    //filling a hole 
    if( FindAHole()==true )
    {     
        NumOfPatchVertex = HolePointIDs.size();
        NumOfPatchTriangle = 0;
        NumOfPatchEdge = 0;
        CreatePatch();
        RefinePatch();
        SmoothPatch();
        MergePatch();  
        BuildPatchOutput();
    }
#if defined(_DEBUG) && defined(_MSC_VER)
    else
    {
      _RPT1(_CRT_WARN, "FindAHole has failed. BorderPointID = %d.\n", BorderPointID);
    }
#endif
  }
  DoneMesh();
  ClearMesh();
}