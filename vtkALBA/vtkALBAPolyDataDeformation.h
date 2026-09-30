/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkALBAPolyDataDeformation.h,v $ 
  Language: C++ 
  Date: $Date: 2011-06-28 09:31:46 $ 
  Version: $Revision: 1.1.2.15 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
  vtkALBAPolyDataDeformation performs a skeleton based deformation
  of the input poly data set (mesh). The deformation is driven by 
  two skeletons, here aka the original and the deformed curves. 
  The original curve(skeleton) corresponds to the input (original)
  mesh and it actually defines its structure. The filter parametrizes
  every vertex of the mesh using this skeleton. The deformed curve
  corresponds to the desired output mesh. The correspondence between
  both curves can be either specified (fully or partially) or it
  is computed automatically. The movement of skeleton nodes then
  defines how the mesh should be deformed - parametrized vertices
  are converted into Cartesian coordinates. Note that the mesh may
  have more than one skeleton (this may be useful for complex 
  models that cannot be described by only one 1D skeleton in 
  the requested accuracy.

  The deformation is based on:

  Blanco FR, Oliveira MM: Instant mesh deformation.
  In: Proceedings of the 2008 symposium on Interactive 3D graphics and games,
  Redwood City, California, 2008, pp. 71-78

  The correspondence detection between skeletons is primarily based on:

  Rahmati M, Mirzaei A: Shape recognition by clustering and matching of skeletons.
  Journal of Computers 2008, Academy Publisher, May 2008, 3(5):24-33

  and

  The Hungarian algorithm (http://en.wikipedia.org/wiki/Hungarian_algorithm)
*/
#ifndef vtkALBAPolyDataDeformation_h__
#define vtkALBAPolyDataDeformation_h__

#include "albaConfigure.h"

#include <vector>
#include <list>

// #include "albaDllMacros.h"

#pragma once

#pragma warning(push)
#pragma warning(disable:4996)
#include "vtkPolyDataAlgorithm.h"
#pragma warning(pop)

//#define DEBUG_vtkALBAPolyDataDeformation

class vtkUnstructuredGrid;
class vtkPolyData;
class vtkIdList;
class vtkCellLocator;

// class CSkeletonEdge;
// class CSkeletonVertex;
// EXPORT_STL_VECTOR(ALBA_EXPORT,CSkeletonEdge*);
// EXPORT_STL_VECTOR(ALBA_EXPORT,CSkeletonVertex*);

class ALBA_EXPORT vtkALBAPolyDataDeformation : public vtkPolyDataAlgorithm
{

public:
  static vtkALBAPolyDataDeformation *New();

  vtkTypeMacro(vtkALBAPolyDataDeformation, vtkPolyDataAlgorithm);  

  friend class CMatrixTest;
  friend class CSkeletonEdgeTest;
  friend class CSkeletonVertexTest;

protected:
  vtkALBAPolyDataDeformation();           
  virtual ~vtkALBAPolyDataDeformation();

protected:
#pragma region //Nested Classes

  class CSkeletonEdge;
  class CSkeletonVertex;
    
  //internal data structure describing vertex of a mesh
  class CMeshVertexParametrization
  {
  public:
    CSkeletonEdge* PEdge;   //<the governing edge      
    double PCoords[3];      //<a,b,c parametric coordinates
    int NOriginPos;         //<0 or 1 to define the origin of LF
    double DblRm;           //<elongation factor, 0.0, if there is no elongation    
    double DblWeight;       //<weighting factor

  public:
    CMeshVertexParametrization() {
      memset(this, 0, sizeof(CMeshVertexParametrization));
    }
  };

  typedef std::vector< CMeshVertexParametrization > CMeshVertex;

  //internal structure for one vertex in the skeleton
  class ALBA_EXPORT CSkeletonVertex
  {
  public:
    int Id;             //<ID of this point
    double Coords[3];   //<coordinates
  
    typedef struct LOCAL_FRAME
    {      
      double u[3];      //<base vector 1
      double v[3];      //<base vector 2    
      double w[3];      //<base vector 3  
    } LOCAL_FRAME;

    LOCAL_FRAME LF;             //<local frame system
    CSkeletonVertex* PMatch;    //<matched vertex

    std::vector< CSkeletonEdge* > OneRingEdges;    //<edges around this vertex
    std::vector< CSkeletonVertex* > JoinedVertices;//<vertices of other curves with the same coordinates   

    double WT;          //<topology weight
    int NMark;          //<vertex tag for internal use
  public:
  public:
    CSkeletonVertex() 
    {
			Id = -1;
      memset(Coords, 0, sizeof(Coords));
			memset(&LF, 0, sizeof(LF));
			PMatch = NULL;      
			WT = 0.0;
			NMark = 0;
    }

    CSkeletonVertex(double coords[3]) 
    {      
      Id = -1;      
			memset(&LF, 0, sizeof(LF));
			PMatch = NULL;
			WT = 0.0;
			NMark = 0;

      Coords[0] = coords[0];
      Coords[1] = coords[1];
      Coords[2] = coords[2];
    }    

    /** Gets the degree of this vertex */
    inline int GetDegree() {
      return (int)OneRingEdges.size();
    }

    /** Gets the number of vertices having the same coordinate */
    inline int GetNumberOfJoinedVertices() {
      return (int)JoinedVertices.size();
    }

    /** 
    Returns true, if the given point lies in the direction LF.u of
    the half-space defined by the LF.v, LF.w and m_Coords  */
    inline bool IsInPositiveHalfspace(double coords[3]) {
      return LF.u[0]*(coords[0] - Coords[0]) +
        LF.u[1]*(coords[1] - Coords[1]) + 
        LF.u[2]*(coords[2] - Coords[2]) >= 0;      
    }
  };
  
  //internal structure for one edge in the skeleton
  class ALBA_EXPORT CSkeletonEdge
  {
  public:
    int Id;                     //<ID of this edge
    CSkeletonVertex* Verts[2];  //<end points (may be NULL)

    CSkeletonEdge* PMatch;      //<matched edge
    int NMark;                  //<edge tag for internal use        

  public:
    CSkeletonEdge() 
    {
      memset(this, 0, sizeof(CSkeletonEdge));
      Id = -1;       
    }

    CSkeletonEdge(CSkeletonVertex* pV1, CSkeletonVertex* pV2)
    {
      memset(this, 0, sizeof(CSkeletonEdge));
      Id = -1; 

      Verts[0] = pV1; Verts[1] = pV2;
    }

  public:
    /** Returns the number of edges connected to one of end points of this edge */
    inline int GetNumberOfConnectedEdges() {
      return Verts[0]->GetDegree() + Verts[1]->GetDegree() - 2;
    }

    /** Returns true, if the edge is internal */
    inline bool IsInternal() {
      return Verts[0]->GetDegree() != 1 && Verts[1]->GetDegree() != 1;
    }    

    /** Returns length of the edge. 
    If the edge does not have both vertices defined, it returns 0.0 */
    double GetLength();
  };

  //Internal data structure to encapsulates one skeleton
  class ALBA_EXPORT CSkeleton
  {
  public:

    std::vector< CSkeletonVertex* > Vertices;
    std::vector< CSkeletonEdge* > Edges;

  public:
    ~CSkeleton();

    /** Computes topology weights for vertices */
    void ComputeTW();

    /** Computes the bounding box */
    void GetBoundingBox(double bnds[6]);

  protected:
    /** Computes the weight for the given edge.
    The computation is recursive in the given direction. */
    int ComputeEdgeWeight(CSkeletonEdge* pEdge, int iDir);    
  };

  class CSuperSkeleton
  {
  public:
    CSkeleton* POCSkel;    //<original skeleton (it is linked to deformed)
    CSkeleton* PDCSkel;    //<deformed skeleton (it is linked to original)

    int* PSkelPositions;    //<stores where in m_pxC_Skel starts next skeleton (not curves)
  public:
    CSuperSkeleton() {
      POCSkel = new CSkeleton();
      PDCSkel = new CSkeleton();
      PSkelPositions = NULL;
    }

    ~CSuperSkeleton() {
      delete POCSkel;
      delete PDCSkel;
      delete PSkelPositions;
    }
  };

#pragma region Munkres
  //This class was adopted from John Weaver code (GNU - see below) and thoroughly modified 
  template <class T>
  class ALBA_EXPORT CMatrix 
  {
  public:
    /** Constructor. */
    CMatrix(int rows, int columns);    

    /** Destructor. */
    ~CMatrix();
        
    /** Get min size between num of columns and num of rows */
    inline int GetMinSize() {
      return ((NumOfRows < NNumOfCols) ? NumOfRows : NNumOfCols);
    }

    /** Return the number of  columns */
    inline int GetNumberOfColumns() {
      return NNumOfCols;
    }

    /** Return the number of rows */
    inline int GetNumberOfRows() {
      return NumOfRows;
    }
    
    /** Return the control of the element (x,y) */
    inline T& operator ()(int x, int y) {  
        return Matrix[x][y];
    }

  private:
    T** Matrix;
    int NumOfRows;
    int NNumOfCols;
  };

  /*
  *   Copyright (c) 2007 John Weaver, Modified by Josef Kohout
  */
  class CMunkres {
  public:
    void Solve(CMatrix< double >* matrix, CMatrix< int >* matches);

  private:
    inline bool FindUncoveredInMatrix(double,int&,int&);
    inline bool PairInList(const std::pair<int,int> &, const std::list<std::pair<int,int> > &);
    int Step1(void);
    int Step2(void);
    int Step3(void);
    int Step4(void);
    int Step5(void);
    int Step6(void);

    CMatrix<int>* MaskMatrix;
    CMatrix<double>* Matrix;

    bool *RowMask;
    bool *ColMask;
    int Saverow, Savecol;
  };
#pragma endregion //Munkres
#pragma endregion //Nested Classes    


  typedef struct CONTROL_SKELETON
  {  
    vtkPolyData* pPolyLines[2];    //<polyline curves (0 = original, 1 = deformed)
    vtkIdList* pCCList;            //<list of correspondences between curves

    bool RSOValid[2];              //<specifies, if RSO is valid
    double RSO[2][3];              //<RSO point
  } CONTROL_SKELETON;

  CONTROL_SKELETON* Skeletons;   //<input array of skeletons
  int NumberOfSkeletons;         //<number of skeletons in this array

  CSuperSkeleton* SuperSkeleton;  //<dynamical data structure describing super skeleton

  double MatchGeometryWeight;       //<weight (0-1) for geometry matching of skeletons
  double MatchTopologyWeight;       //<weight (0-1) for topology matching of skeletons
  double MatchTolerance;            //<weight (0-1) for the matching of vertices of curves

  int DivideSkeletonEdges;          //<1 if large skeleton edges should be divided  
  int PreserveVolume;               //<1, if volume should be preserved

  CMeshVertex* MeshVertices;      //<internal data structure describing the mesh  

public:  
  /** Gets the weight using to match geometry of skeletons */
  vtkGetMacro(MatchGeometryWeight, double);

  /** Sets the weight using to match geometry of skeletons
  The value must be in the interval (0,1) */
  vtkSetMacro(MatchGeometryWeight, double);

  /** Gets the weight using to match topology of skeletons */
  vtkGetMacro(MatchTopologyWeight, double);

  /** Sets the weight using to match topology of skeletons
  The value must be in the interval (0,1) */
  vtkSetMacro(MatchTopologyWeight, double);

  /** Gets the tolerance used to match vertices of two curves.
  The tolerance is relative to the length of shortest edge of these curves. */
  vtkGetMacro(MatchTolerance, double);

  /** Sets the tolerance used to match vertices of two curves
  The tolerance is relative to the length of shortest edge of these curves.
  The value must be in the interval (0,1). */
  vtkSetMacro(MatchTolerance, double);

  /** Returns 1, if large skeleton edges should be divided */
  vtkGetMacro(DivideSkeletonEdges, int);

  /** Specifies whether large skeleton edges should be divided */
  vtkSetMacro(DivideSkeletonEdges, int);

  /** Specifies whether large skeleton edges should be divided */
  vtkBooleanMacro(DivideSkeletonEdges, int);

  /** Returns 1, if simple volume preservation technique is to be used */
  vtkGetMacro(PreserveVolume, int);

  /** Specifies whether simple volume preservation technique is to be used */
  vtkSetMacro(PreserveVolume, int);

  /** Specifies whether simple volume preservation technique is to be used */
  vtkBooleanMacro(PreserveVolume, int);

  /** Get the number of control curves. */
  inline virtual int GetNumberOfSkeletons() {
    return NumberOfSkeletons;
  }

  /** Sets the number of control skeletons.  
  Old skeletons are copied (and preserved) */
  virtual void SetNumberOfSkeletons(int nCount);

  /** Specifies the n-th control skeleton. 
  If RSO points are specified, they are used during the computation of LFs
  of curves of both skeletons. A local fame is defined by its origin point 
  and three vectors u, v and w. Vector u is the tangent vector (it goes in
  the direction of polyline) and vectors v,w are perpendicular to this vector.
  As there is infinite number of u,v,w configurations, the algorithm uses the
  given RSO point to get a unique one (v lies in the plane defined by u and RSO). 
  If RSO is not specified, v is chosen to lie in the plane closest to the u vector.
  When RSO points are not specified (or they are specified incorrectly), 
  the deformed object might be unrealistically rotated against other objects 
  in the scene, if the skeleton of object to deform tends to rotate (simple edge, 
  or only one skeleton for object). */
  virtual void SetNthSkeleton(int idx, vtkPolyData* original, 
    vtkPolyData* modified, vtkIdList* correspondence = NULL, 
    double* original_rso = NULL, double* modified_rso = NULL);

  /** Return this object's modified time. */  
  /*virtual*/ vtkMTimeType GetMTime();
protected:
  /** 
  By default, UpdateInformation calls this method to copy information
  unmodified from the input to the output.*/
  /*virtual*/int RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);

  /**
  This method is the one that should be used by subclasses, right now the 
  default implementation is to call the backwards compatibility method */
  /*virtual*/void ExecuteData(vtkDataObject *output);
protected:
  /** Creates a single skeleton that describes the deformation.
  Returns false, if the construction failed. */  
  bool CreateSuperSkeleton();  

  /** Destroys the super skeleton */
  inline void DestroySuperSkeleton() {
    delete SuperSkeleton;
    SuperSkeleton = NULL;
  }

  /** Creates a single super skeleton for the given control skeleton.
  It combines both skeletons together, matching their vertices and
  creating new vertices as needed. It also computes local frames.
  The combined information is then appended into super skeleton.
  If the super skeleton does not exist, it is created. */  
  void CreateSuperSkeleton(vtkPolyData* pOC, 
    vtkPolyData* pDC, vtkIdList* pCC, double dblEdgeFactor);    

  /** Creates the internal skeleton data structure for the given polydata */
  CSkeleton* CreateSkeleton(vtkPolyData* pPoly);

  /** Releases memory allocated for the given skeleton (in CreateSkeleton) */
  inline void DestroySkeleton(CSkeleton* pSkel) {
    delete pSkel;
  }

  //this structure is used in MatchCurves
  typedef struct CURVE_VERTEX;

  /** Compute the best match for skeleton vertices.
  Only junctions and terminal nodes are matched. The caller can optionally 
  specify some (or all) correspondences. The routine returns the list containing
  pairs of corresponding vertices. */
  vtkIdList* MatchSkeletons(CSkeleton* pOC, CSkeleton* pDC, vtkIdList* pCC);

  /** Traces the curve in the given direction starting from the given vertex.
  Starting from the given vertex, the routine moves in the given direction
  (which actually defines the edge which to go) storing the visited vertices
  of the graph into the output buffer. The buffer must be capable to hold 
  all vertices of the graph. The traversal process ends when a "junction" 
  or end-point is reached (those vertices have mark >= 0). The routine 
  returns the number of vertices stored in the buffer. 
  N.B. this routine is supposed to be called from CreateSuperSkeleton */
  int TraceSkeletonCurve(CSkeletonVertex* pStartVertex, int iDir, CSkeletonVertex** pOutBuf);

  /** Marks every edge of the given skeleton curve */
  void MarkCurveEdges(CSkeletonVertex** pCurve, int nCount);

  /** Matches two curves defined by two arrays of vertices.
  It creates at least max(nOCVerts, nDCVerts) new vertices and those 
  vertices are stored in pOC and pDC buffers. Note: both array buffers 
  must be capable enough to hold nOCVerts + nDCVerts vertices.
  Returns the number of vertices in matched curves. 
  It also creates edges between vertices and establishes links between
  both vertices and edges. The caller is responsible for deletion of all 
  objects when they are no longer needed.
  N.B. the original vertices are not destroyed, the routine damages buffers only*/
  int MatchCurves(CSkeletonVertex** pOC, int nOCVerts, CSkeletonVertex** pDC, int nDCVerts);

  /** Create edges for the given array of vertices. */
  void CreateCurveEdges(CSkeletonVertex** pVerts, int nVerts);  

  /** Refines the given curve (and its corresponding one) by adding more vertices.
  Every skeleton edge larger than sqrt(dblEdgeFactor) is split recursively into 
  two shorter edges. The matching curve is split appropriately.
  N.B. the routine is intended to be called after CreateCurveEdges */
  void RefineCurve(CSkeletonVertex* pCurve, double dblEdgeFactor);

  /** Stores vertices and edges from the given curve and the matched one into the superskeleton 
  The routine also constructs automatically joints for end-points of curves.
  N.B. both curves must be compatible with curves constructed by CreateCurveEdges.  */
  void AddCurveToSuperSkeleton(CSkeletonVertex* pOCCurve);

  /** Computes the average length of edges of the input mesh */
  double ComputeInputMeshAvgEdgeLength();  

  /** Gets the previous vertex on the curve.
  Returns NULL, if the given vertex is the first one. */
  inline CSkeletonVertex* GetPreviousCurveVertex(CSkeletonVertex* pCurve) {
    return (pCurve->GetDegree() == 1) ? NULL : 
      pCurve->OneRingEdges[1]->Verts[0];      
  }

  /** Gets the next vertex on the curve.
  Returns NULL, if the given vertex is the last one. */
  inline CSkeletonVertex* GetNextCurveVertex(CSkeletonVertex* pCurve) 
  {
    CSkeletonVertex* pRet = pCurve->OneRingEdges[0]->Verts[1]; 
    return (pRet == pCurve) ? NULL : pRet;
  }

  /** Gets the previous edge on the curve.
  Returns NULL, if the given vertex is the first one. */
  inline CSkeletonEdge* GetPreviousCurveEdge(CSkeletonEdge* pCurve) 
  {
    if (pCurve->Verts[0] == NULL || pCurve->Verts[0]->GetDegree() == 1)
      return NULL;  //there is no previous edge    

    return pCurve->Verts[0]->OneRingEdges[1];
  }

  /** Gets the next edge on the curve.
  Returns NULL, if the given vertex is the last one. */
  inline CSkeletonEdge* GetNextCurveEdge(CSkeletonEdge* pCurve) 
  {
    if (pCurve->Verts[1] == NULL || pCurve->Verts[1]->GetDegree() == 1)
      return NULL;  //there is no next edge

    return pCurve->Verts[1]->OneRingEdges[0];    
  }

  /** Computes the local frame systems for both curves.  
  N.B. both curves must be compatible with curves constructed by CreateCurveEdges
  and the links must be established between them

  The algorithm is based on the paper: Blanco FR, Oliveira MM: Instant mesh deformation.
  In: Proceedings of the 2008 symposium on Interactive 3D graphics and games,
  Redwood City, California, 2008, pp. 71-78 
  
  ROS_OC and ROS_DC defines the plane to compute the first LF - see SetNthSkeleton*/
  void ComputeLFS(CSkeletonVertex* pOC, double* ROS_OC = NULL, double* ROS_DC = NULL);  

  /** Computes an approximate geodesic distance between two points.
  If the straight line between both points does not intersect the input mesh,
  the returned distance is the Euclidian distance between those two points;
  otherwise it is the sum of the length of the shortest (surface) path from 
  nPtStartId to the surface point closest to the intersection and the distance
  that returned by GetDistance with the first parameter to be the surface 
  point closest to the intersection. If the total distance during the 
  computation exceeds the given dblMaxDist, the algorithm stops (and returns
  the distance measured so far) - this is to speed up the process. 
  N.B. cellLocator must be initialized with the input mesh. */
  double GetDistance(vtkIdType nPtStartId, double ptEnd[3], 
    vtkCellLocator* cellLocator, double dblMaxDist);

  //this structure is used in GetPathLength
  typedef struct DIJKSTRA_ITEM;

  /** Computes the length of the path between nPtFrom to nPtTo vertices.
  The computation is not precise (because of speed). If there is no path, or the
  path would be too long, the routine returns dblMaxDist. 
  N.B. the found path may not be the shortest one! */
  double GetPathLength(vtkIdType nPtFrom, vtkIdType nPtTo, double dblMaxDist);

  /** Gets the number of vertices belonging to the given curve.
  N.B. curve must be compatible with curves constructed by CreateCurveEdges.*/
  int GetNumberOfCurveVertices(CSkeletonVertex* pCurve);

  /** Constructs the matrix for rotation of vectors around the vector r by the angle theta.
  The angle is given indirectly as cos(theta). 
  N.B. vector r must be normalized. */
  void BuildGeneralRotationMatrix(double r[3], double cos_theta, double M[3][3]);

  /** Parametrize the input mesh using the super-skeleton.
  N.B. edges ROI must be build and refined before this routine may be called. */
  void ComputeMeshParametrization();
  
  //void ComputeParametrization(CSkeletonEdge* pNextEdge, 
  //   CSkeletonEdge* pEdge, int iWSkelEdge);

  /** Computes new position of vertices in the output mesh.
  In this last step, positions of vertices are modified according to their
  parametrization in respect to the superskeleton. 
  
  N.B. the given output polydata must be compatible with the input polydata */
  void DeformMesh(vtkPolyData* output);
  
  /** Creates polydata from the given skeleton.   */
  void CreatePolyDataFromSkeleton(CSkeleton* pSkel, vtkPolyData* output); 

private:
  vtkALBAPolyDataDeformation(const vtkALBAPolyDataDeformation&);  // Not implemented.
  void operator = (const vtkALBAPolyDataDeformation&);  // Not implemented.  

#ifdef DEBUG_vtkALBAPolyDataDeformation
public:
  vtkPolyData* m_MATCHED_POLYS[2];  //new polys for the first skeleton
  vtkIdList* m_MATCHED_FULLCC;      //and their correspondences
  vtkIdList* m_MATCHED_CC;          //first skeleton correspondences
protected:  
  /** Debug routine that creates polydata from superskeleton */
  void CreatePolyDataFromSuperskeleton();

  /** Destroys the debug data*/
  void DestroyMATCHEDData();  
#endif   
};

#pragma region //Munkres INLINES
inline bool vtkALBAPolyDataDeformation::
CMunkres::FindUncoveredInMatrix(double item, int &row, int &col) 
{
  for ( row = 0 ; row < Matrix->GetNumberOfRows() ; row++ )
  {
    if ( !RowMask[row] )
    {
      for ( col = 0 ; col < Matrix->GetNumberOfColumns() ; col++ )
      {
        if ( !ColMask[col] && (*Matrix)(row,col) == item )
          return true;
      }
    }
  }

  return false;
}

inline bool vtkALBAPolyDataDeformation::
CMunkres::PairInList(const std::pair<int,int> &needle, 
                       const std::list<std::pair<int,int> > &haystack) 
{
  for (std::list<std::pair<int,int> >::const_iterator i = haystack.begin(); i != haystack.end() ; i++)
  {
    if ( needle == *i )
      return true;
  }

  return false;
}
#pragma endregion //Munkres INLINES

#endif // vtkALBAPolyDataDeformation_h__
