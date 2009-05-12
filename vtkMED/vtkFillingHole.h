/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkFillingHole.h,v $
Language:  C++
Date:      $Date: 2009-05-12 08:33:17 $
Version:   $Revision: 1.1.2.1 $
Authors:   Fuli Wu, Josef Kohout
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __vtkFillingHole_h
#define __vtkFillingHole_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "vtkstd/vector"
#include "vtkCellArray.h"
#include "vtkPointLocator.h"
#include "vtkPolyDataToPolyDataFilter.h"
#include <cmath>
#include <algorithm>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
//#define _FILLING_DBG

//----------------------------------------------------------------------------
// vtkFillingHole class 
//----------------------------------------------------------------------------
class VTK_GRAPHICS_EXPORT vtkFillingHole : public vtkPolyDataToPolyDataFilter
{
public:
#pragma region Nested classes
  //----------------------------------------------------------------------------
  /** Nested Vertex class 
  This is a list of the triangles, edges and vertices which are joined to this vertex.*/
  //----------------------------------------------------------------------------
  class CVertex
    {
    public:
      double  dCoord[3];
      int     id;
      double  dWeight,dOneRingEdgeLength;
      bool    bMarked;
      bool    bBoundary;
      vtkstd::vector<int> OneRingTriangle;
      vtkstd::vector<int> OneRingEdge;
      vtkstd::vector<int> OneRingVertex;
      vtkstd::vector<int> TwoRingVertex;

      CVertex(double *pCoord);
      ~CVertex();
      bool IsTwoRingVertex(int id);
    };

  //----------------------------------------------------------------------------
  /** Nested Triangle class 
  Each triangle has three edges and three vertices.*/
  //----------------------------------------------------------------------------
  class  CTriangle
    {
    public:
      bool  bDeleted;
      bool  bMarked;
      int   aVertex[3];
      int   aEdge[3];
      int   id;
    public:
      CTriangle();
      CTriangle(int v0,int v1,int v2);
      void SetEdge(int e0,int e1,int e2);
    };

  //----------------------------------------------------------------------------
  /** Nested Edge class 
  Normally, each edge has two neighbor triangles. Two vertices consist of an edge.*/
  //----------------------------------------------------------------------------
  class CEdge
    {
    public:
      bool  bBoundary;
      bool  bMarked;
      double  dLength;
      int   aVertex[4];       //first second, left right
      int   aTriangle[2];     //left right
      int   id;
      CEdge(); 
      CEdge(int v0,int v1);
      CEdge(int v0,int v1,int v2,int v3);
      void SetTriangle(int t0,int t1);
    };

  //----------------------------------------------------------------------------
  /** Nested Laplacian class
  Each vertex has a Laplacian. There is a Laplaican between the vertex and each neighbor vertices*/
  //----------------------------------------------------------------------------
  class CLaplacian
    {
    public:
      double  *aLaplacian;
      double  dLaplacian;
      double  dLaplacianCoord[3];
      CLaplacian(int size);  
      ~CLaplacian();
    };
#pragma endregion Nested classes

public:
  typedef enum FillingTypes
  {
    Flat,               //no fairing is applied
    SmoothMembrane,     //fairing is applied based on the minimization of membrane energy (prefers smaller surface area)
    SmoothThinPlate,    //fairing is applied based on the minimization of thin plate energy (punishes strong bending)
  };

public:
  //-----------------------------------------------------------------------------
  // public methods
  //-----------------------------------------------------------------------------
  static vtkFillingHole *New();

  vtkTypeRevisionMacro(vtkFillingHole,vtkPolyDataToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetFillAllHole();      ///< Set filling all holes on the mesh.

  /** Set filling a specific hole on the mesh. 
  The id is a vertex id on the border of the hole.
  */
  void SetFillAHole(int id);  


  /** Sets Flat filling of hole patch */
  inline void SetFlatFill(){
    SetFillingType(Flat);
  }

  /** Set the smooth filling mode. 
  If bThinPlate is true, smoothing is done by the minimization of thin plate energy,
  which is good for cases when patch should preserve curvature of surrounding mesh. 
  If the hole is end of blood vessel, it is not recommended option. */
  inline void SetSmoothFill(bool bThinPlate = true) {
    SetFillingType((bThinPlate ? SmoothThinPlate : SmoothMembrane));
  }

  /** Sets the filling type */
  inline void SetFillingType(FillingTypes type) {
    FillingType = type;
  }

  /** Sets number of steps for thin plate smoothing. */
  inline void SetSmoothThinPlateSteps(int nSteps) {
    SmoothThinPlateSteps = nSteps;
  }

protected:
  vtkFillingHole();           
  ~vtkFillingHole();

  int		**Lambda;
  
  vtkstd::vector<CVertex*>    PatchVertexes;        ///< list of vertices of a patch for one hole
  vtkstd::vector<CTriangle*>  PatchTriangles;       ///< list of triangles of a patch for one hole
  vtkstd::vector<CEdge*>      PatchEdges;           ///< list of edges of a patch for one hole
  vtkstd::vector<CLaplacian*> PatchLaplacian;       ///< list of laplacian of a patch for one hole

  vtkstd::vector<CVertex*>    Vertexes;             ///< list of vertices of the mesh
  vtkstd::vector<CTriangle*>  Triangles;            ///< list of triangles of the mesh
  vtkstd::vector<CEdge*>      Edges;                ///< list of edge of the mesh

  vtkstd::vector<int>         HolePointIDs;         ///< list of vertices of a hole in order
  vtkstd::vector<int>         HoleEdgeIDs;          ///< list of edges of a hole in order

  int NumOfPatchVertex;         ///< the number of all vertices of a patch for one hole  
  int NumOfPatchTriangle;       ///< the number of triangles of a patch for one hole
  int NumOfPatchEdge;           ///< the number of edges of a patch for one hole

  int NumOfVertex;              ///< the number of vertices of the mesh
  int NumOfTriangle;            ///< the number of triangles of the mesh
  int NumOfEdge;                ///< the number of edges of the mesh

  int SmoothThinPlateSteps;     ///< the number of smoothing steps

  void    CreatePatch();
  void    RefinePatch();
  void    Trace(int i,int k);
  bool    RelaxOneEdge(CEdge *pEdge);
  CVertex* AddOnePointToTriangle(double *pCoord, CTriangle *pTriangle);

  /** Multiplies L transpose matrix and L matrix, i.e., A =  L^T*L */
  void  ComputeLTransposeLMatrix(double *A);

  /** Multiplies matrix A and vector xyz, i.e., result = A*xyz 
  N.B. A is L^T*L + some constraints on diagonal */
  void LTransposeLMatrixVector(double *A,double *xyz,double *result);
  
  /** Multiplies transpose L matrix and vector, i.e., result = L^T*source */
  void LTransposeMatrixVector(double *source,double *result);
  void  LTransposeLConjugateGradient(double *A,double *xyz,double *b);

  /** Computes the normalized normal of the triangle. 
  The triangle is defined by the three given vertices */  
  void ComputeNormal(const double *v1, const double *v2, const double *v3, double *n);

  /** Computes the dihedral angle between two planes.
  Planes are defined by their normal vectors (n1, n2). The function does not 
  return the angle in degrees but a value ranging from -1 to 3
  that is proportional to this angle */
  double ComputeDihedralAngleF(const double* n1, const double* n2);

  double TriangleArea(double *pCoordv0,double *pCoordv1,double *pCoordv2);

  /** 
  Returns true, if the point v4 lies outside the circum-circle subscribed
  to the triangle formed by points v1, v2 and v3. 
  N.B. points v2, v1 and v4 must form the adjacent triangle */
  bool CircumCircleTest(double *v1,double *v2,double *v3, double *v4);  

protected:  
  void  BuildMesh();
  void  UpdateMesh(int id);
  void  ClearMesh();
  void  MergePatch();

  void  BuildPatch();
  void BuildPatchLaplacian();
  void  ClearPatch();
  void  ExtendPatch();
  void  SmoothPatch();

  /** The core of smoothing by the minimization of membrane energy */
  void MembraneSmoothing();

  /** The core of smoothing by the minimization of thin plate energy */
  void ThinPlateSmoothing();

  bool FindNextHole();          ///< detect next hole on the surface
  bool FindAHole();             ///< detect a hole on the surface

#if defined(_FILLING_DBG)
  void CheckPatchEdges(int& nStartEdgeId);
#endif //_DEBUG

protected:

  int FillingType;              /// smooth filling or flat filling (see enum FillingType)
  int FillingHoles;             /// filling all holes or one specific hole
  int BorderPointID;            /// if filling one hole, input a vertex id on the border of the hole

  vtkPolyData *InputMesh,*OutputMesh;

  ///** Build internal mesh structure filtering out invalid, non-manifold triangles */
  //void InitManifoldMesh();

  /** Build internal mesh. It suppose that the input mesh is manifold */
  void InitMesh();
  void Execute();
  void DoneMesh();

private:
  vtkFillingHole(const vtkFillingHole&);  // Not implemented.
  void operator=(const vtkFillingHole&);  // Not implemented.
};

#endif