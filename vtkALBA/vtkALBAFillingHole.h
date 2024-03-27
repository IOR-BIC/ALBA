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

#ifndef __vtkALBAFillingHole_h
#define __vtkALBAFillingHole_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaConfigure.h"
#include "vtkCellArray.h"
#include "vtkPointLocator.h"
#include "vtkPolyDataAlgorithm.h"
#include <cmath>
#include <algorithm>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
//#define _FILLING_DBG

/**
  class name: vtkALBAFillingHole
  Filter which fill holes of a vtkPolydata
*/
class ALBA_EXPORT vtkALBAFillingHole : public vtkPolyDataAlgorithm
{
public:
#pragma region Nested classes
  /** 
    class name: CVertex 
    Nested Vertex class 
    This is a list of the triangles, edges and vertices which are joined to this vertex.*/
  class CVertex
    {
    public:
      double  DCoord[3];
      int     Id;
      double  DWeight,DOneRingEdgeLength;
      bool    BMarked;
      bool    BBoundary;
      std::vector<int> OneRingTriangle;
      std::vector<int> OneRingEdge;
      std::vector<int> OneRingVertex;
      std::vector<int> TwoRingVertex;

      /** constructor */
      CVertex(double *pCoord);
      /** destructor */
      ~CVertex();
      /** check if the vertex is two ring vertex */
      bool IsTwoRingVertex(int id);
    };

  /** 
    class name: CTriangle
    Nested Triangle class 
    Each triangle has three edges and three vertices.
    */
  class ALBA_EXPORT CTriangle
    {
    public:
      bool  BDeleted;
      bool  BMarked;
      int   AVertex[3];
      int   AEdge[3];
      int   Id;
    public:
      /** constructor */
      CTriangle();
      /** destructor */
      CTriangle(int v0,int v1,int v2);
      /** set the edge */
      void SetEdge(int e0,int e1,int e2);
    };

  
  /**  
  class name: CEdge
  Nested Edge class 
  Normally, each edge has two neighbor triangles. Two vertices consist of an edge.
  */
  class ALBA_EXPORT CEdge
    {
    public:
      bool  BBoundary;
      bool  BMarked;
      double  DLength;
      int   AVertex[4];       //first second, left right
      int   ATriangle[2];     //left right
      int   Id;
      /** constructor */
      CEdge();
      /** overloaded constructor */ 
      CEdge(int v0,int v1);
      /** verloaded constructor */
      CEdge(int v0,int v1,int v2,int v3);
      /** Set Triangle to the edge */
      void SetTriangle(int t0,int t1);
    };

  
  /** 
  class name : CLaplacian
  Nested Laplacian class
  Each vertex has a Laplacian. There is a Laplaican between the vertex and each neighbor vertices
  */
  
  class CLaplacian
    {
    public:
      double  *ALaplacian;
      double  DLaplacian;
      double  DLaplacianCoord[3];
      /** constructor */
      CLaplacian(int size);  
      /** destructor */
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
  /** retrieve instance of the class */
  static vtkALBAFillingHole *New();

  /** RTTI macro */
  vtkTypeMacro(vtkALBAFillingHole,vtkPolyDataAlgorithm);
  /** print information of the class */
  void PrintSelf(ostream& os, vtkIndent indent);

  /** Set filling all holes on the mesh. */
  void SetFillAllHole();      

  /** Set filling a specific hole on the mesh.  The id is a vertex id on the border of the hole. */
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
  /**Get the latest created patch*/
vtkPolyData*  GetLastPatch(){return LastPatch;};  


protected:
  /** constructor */
  vtkALBAFillingHole();           
  /** destructor */
  ~vtkALBAFillingHole();

  int		**Lambda;
  
  std::vector<CVertex*>    PatchVertexes;        ///< list of vertices of a patch for one hole
  std::vector<CTriangle*>  PatchTriangles;       ///< list of triangles of a patch for one hole
  std::vector<CEdge*>      PatchEdges;           ///< list of edges of a patch for one hole
  std::vector<CLaplacian*> PatchLaplacian;       ///< list of laplacian of a patch for one hole

  std::vector<CVertex*>    Vertexes;             ///< list of vertices of the mesh
  std::vector<CTriangle*>  Triangles;            ///< list of triangles of the mesh
  std::vector<CEdge*>      Edges;                ///< list of edge of the mesh

  std::vector<int>         HolePointIDs;         ///< list of vertices of a hole in order
  std::vector<int>         HoleEdgeIDs;          ///< list of edges of a hole in order

  int NumOfPatchVertex;         ///< the number of all vertices of a patch for one hole  
  int NumOfPatchTriangle;       ///< the number of triangles of a patch for one hole
  int NumOfPatchEdge;           ///< the number of edges of a patch for one hole

  int NumOfVertex;              ///< the number of vertices of the mesh
  int NumOfTriangle;            ///< the number of triangles of the mesh
  int NumOfEdge;                ///< the number of edges of the mesh

  int SmoothThinPlateSteps;     ///< the number of smoothing steps

  /** Build a patch for the hole on the surface */
  void    CreatePatch();
  /** divide all large triangles to smaller triangles
     then, relax all interior edges of the patching mesh */
  void    RefinePatch();
  /** Filling Gaps in the Boundary of a Polyhedron */
  void    Trace(int i,int k);
  /** swap the edge to two non-mutual vertices of the triangles */
  bool    RelaxOneEdge(CEdge *pEdge);
  /** build the patch */
  void    BuildPatchOutput();

  /** add one point to a trinagle*/
  CVertex* AddOnePointToTriangle(double *pCoord, CTriangle *pTriangle);

  /** Multiplies L transpose matrix and L matrix, i.e., A =  L^T*L */
  void  ComputeLTransposeLMatrix(double *A);

  /** Multiplies matrix A and vector xyz, i.e., result = A*xyz 
  N.B. A is L^T*L + some constraints on diagonal */
  void LTransposeLMatrixVector(double *A,double *xyz,double *result);
  
  /** Multiplies transpose L matrix and vector, i.e., result = L^T*source */
  void LTransposeMatrixVector(double *source,double *result);
  /** Solves system of linear equations A*x = b using iterative conjugate gradient */
  void  LTransposeLConjugateGradient(double *A,double *xyz,double *b);

  /** Computes the normalized normal of the triangle. 
  The triangle is defined by the three given vertices */  
  void ComputeNormal(const double *v1, const double *v2, const double *v3, double *n);

  /** Computes the dihedral angle between two planes.
  Planes are defined by their normal vectors (n1, n2). The function does not 
  return the angle in degrees but a value ranging from -1 to 3
  that is proportional to this angle */
  double ComputeDihedralAngleF(const double* n1, const double* n2);

  /** calculate triangle area*/
  double TriangleArea(double *pCoordv0,double *pCoordv1,double *pCoordv2);

  /** 
  Returns true, if the point v4 lies outside the circum-circle subscribed
  to the triangle formed by points v1, v2 and v3. 
  N.B. points v2, v1 and v4 must form the adjacent triangle */
  bool CircumCircleTest(double *v1,double *v2,double *v3, double *v4);  

protected:
  /** build mesh */
  void  BuildMesh();
  /** update mesh */
  void  UpdateMesh(int id);
  /** clear mesh */
  void  ClearMesh();
  /** merge patch */
  void  MergePatch();

  /** build patch */
  void  BuildPatch();
  /** Build a scale laplacian mesh for the patch */
  void BuildPatchLaplacian();
  /** clear patch */
  void  ClearPatch();
  /**  extend the patch and include some surrounding vertices and triangles */
  void  ExtendPatch();
  /** smooth a patch and make its curvature match the surrounding mesh */
  void  SmoothPatch();

  /** The core of smoothing by the minimization of membrane energy */
  void MembraneSmoothing();

  /** The core of smoothing by the minimization of thin plate energy */
  void ThinPlateSmoothing();
  /** detect next hole on the surface */
  bool FindNextHole();
  /**detect a hole on the surface */      
  bool FindAHole();

#if defined(_FILLING_DBG)
  /** check patch edges*/
  void CheckPatchEdges(int& nStartEdgeId);
#endif //_DEBUG

protected:

  int FillingType;              /// smooth filling or flat filling (see enum FillingType)
  int FillingHoles;             /// filling all holes or one specific hole
  int BorderPointID;            /// if filling one hole, input a vertex id on the border of the hole

  vtkPolyData *InputMesh,*OutputMesh, *LastPatch;

  ///** Build internal mesh structure filtering out invalid, non-manifold triangles */
  //void InitManifoldMesh();

  /** Build internal mesh. It suppose that the input mesh is manifold */
  void InitMesh();
  /** filter execution */
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);
  /** mesh allocation */
  void DoneMesh();

private:
  /** copy constructor not implemented */
  vtkALBAFillingHole(const vtkALBAFillingHole&); 
  /** operator= not implemented */
  void operator=(const vtkALBAFillingHole&);  
};

#endif