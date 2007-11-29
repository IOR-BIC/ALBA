/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFMeshCutter.h,v $
Language:  C++
Date:      $Date: 2007-11-29 09:18:41 $
Version:   $Revision: 1.4 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2007 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/


#ifndef __vtkMAFMeshCutter_h
#define __vtkMAFMeshCutter_h

#include "vtkUnstructuredGridToPolyDataFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPlane.h"
#include "vtkPolyData.h"
#include "vtkIdType.h"
#include "vtkIdList.h"
#include "vtkMAFConfigure.h"
#include "mafObject.h"
#include <ostream>
#include <vector>


/** vtkMAFMeshCutter - vtk filter which cuts a finite element mesh (unstructured grid) with a plane.
This is very similar to vtkCutter().

Differences from vtkCutter():
1) vtkMAFMeshCutter() returns polygons which correspond to the mesh cells, eg a slice
through the center of a cube might return a square.  In vtkCutter() this would
be subdivided into triangles.
2) Public methods have been provided to map the output points and cells back to the input mesh.
3) Integer scalars are rounded, not truncated to next-lowest integer.
4) There is no contouring, ie multiple slices.

Special cases:
1) It is possible for a slice through a non-convex cell to create a hole.
The class cannot currently deal with this, and will return an error.
2) Edges which are exactly in the plane are not included in the list of
edges cut by the plane, although their endpoints are.
3) No cells of lower order than triangles are created.
Therefore if the plane cuts exactly through an isolated edge or vertex, the output
polydata will contain the points, but no cell will be created.*/
class VTK_vtkMAF_EXPORT vtkMAFMeshCutter : public vtkUnstructuredGridToPolyDataFilter
{
public:
  vtkTypeRevisionMacro(vtkMAFMeshCutter,vtkUnstructuredGridToPolyDataFilter);
  static vtkMAFMeshCutter *New() ;
  void PrintSelf(ostream& os, vtkIndent indent);                                ///< print self

   /** Overload standard modified time function. If cut function is modified,
  then this object is modified as well. */
  unsigned long GetMTime();

  void SetCutFunction(vtkPlane *P) ;                                            ///< Set the cutting plane (but does not register the object)
  vtkPlane* GetCutFunction() ;                                                  ///< Get the cutting plane

  /** Get the output point idout corresponding to the input edge (id0,id1). 
  Lambda returns the fractional distance between id0 and id1. 
  Returns true if the edge exists and was cut by the plane. 
  NB this will not find edges which lie exactly in the plane. */
  bool GetOutputPointWhichCutsEdge(vtkIdType id0, vtkIdType id1, vtkIdType *idout, double *lambda) const ;

  /** Get the output point idout corresponding to the single input point id0. 
  Returns true if the input point exists and was intersected exactly by the plane. */
  bool GetOutputPointWhichCutsPoint(vtkIdType id0, vtkIdType *idout) const ;

  /** Get the input edge (id0,id1) corresponding to the point idout. 
  Lambda returns the fractional distance between id0 and id1. */
  bool GetInputEdgeCutByPoint(vtkIdType idout, vtkIdType *id0, vtkIdType *id1, double *lambda) const ;

  /** Get the single input point id0 corresponding to the point idout. */
  bool GetInputPointCutByPoint(vtkIdType idout, vtkIdType *id0) const ;

  /** Get the input cell corresponding to the output cell. */
  vtkIdType GetInputCellCutByOutputCell(vtkIdType idout) ;

  /** Get the output cell corresponding to the input cell.
  Returns true if the input id was found. */
  bool GetOutputCellWhichCutsInputCell(vtkIdType idin, vtkIdType *idout) ;


  /** initialize the cutter */
  void Initialize() ;

protected:
  vtkMAFMeshCutter() ;                                                              ///< constructor
  ~vtkMAFMeshCutter() ;                                                             ///< destructor

  void Execute();                                                               ///< execute method


  // edge described by id's of endpoints
  typedef struct{
    vtkIdType id0 ;   // id's of endpoints
    vtkIdType id1 ;
  } Edge ;

  /** type of intersection made by cutting line with plane */
  enum intersection_type {
    NO_INTERSECTION = 0,    // no intersection
    INTERSECTS_LINE,        // single intersection with line
    INTERSECTS_POINT0,      // intersects line at endpoint 0 (ie lambda = 0)
    INTERSECTS_POINT1,      // intersects line at endpoint 1 (ie lambda = 1)
    LINE_IN_PLANE          // line lies in plane
  } ;

  /** type of mapping between output point and input mesh */
  enum mapping_type {
    NO_MAPPING = 0,
    POINT_TO_EDGE,
    POINT_TO_POINT
  } ;

  // constant for undefined id's
  static const vtkIdType undefinedId = -1 ;

  /** Add set of id's to the edge mapping table.
  This is a POINT_TO_EDGE mapping
  This maps output point idout to input edge */
  void AddMapping(vtkIdType idout, const Edge& edge, double lambda) ;

  /** Add set of id's to the edge mapping table.
  This is a POINT_TO_POINT mapping
  This maps output point idout to single input point id0 */
  void AddMapping(vtkIdType idout, vtkIdType id0, double lambda) ;

  /** Get intersection of line with plane
  Line is defined by coords of endpoints (p0, p1).
  Plane is defined by origin and normal.
  Returns intersection type (see enum)
  lambda returns fractional distance of interpolated point along edge (0 <= lambda <= 1). */
  int GetIntersectionOfLineWithPlane(const double *p0, const double *p1, const double *origin, const double *norm, 
    double *coords, double *lambda) const ;

  /** Get intersection of edge with plane
  Returns intersection type (see enum)
  lambda returns fractional distance of interpolated point along edge (0 <= lambda <= 1). */
  int GetIntersectionOfEdgeWithPlane(const Edge& edge, double *coords, double *lambda) const ;

  /** Get list of cells which are adjoined to point */
  void GetCellNeighboursOfPoint(vtkIdType idpt, vtkIdList *list) const ;

  /** Get list of cells which are adjoined to edge */
  void GetCellNeighboursOfEdge(const Edge& edge, vtkIdList *list) const ;

  /** Find where the edges cross the plane and create points */
  void FindPointsInPlane() ;

  /** Get the list of input cells which are attached to the output point */
  void GetInputCellsOnOutputPoint(vtkIdType idout, vtkIdList *cellids) const ;

  /** Create and fill table of cells.
  // There is an entry for every input cell.
  // Entry i is the list of output points in input cell i */
  void AssignPointsToCells() ;

  /** Are all the point ids in the list members of this face
  Undefined id's are ignored, eg if the list is {1, 3, -1} and the face is {1, 3, 10, 11}, the result is true */
  bool AllIdsInFace(vtkCell *face, vtkIdList *idlist) ;

  /** Find a face containing all id's in the list
  Undefined id's are ignored, ie the result can still be true if some of the id's are undefined */
  bool FindFaceContainingAllIds(vtkCell *cell, vtkIdList *idlist, vtkIdType *faceid) ;

  /** Construct the polygonal slice through the cell */
  bool ConstructCellSlicePolygon(vtkIdType cellid, vtkIdList *idlist) ;

  /** calculate the normal of the output polygon, given the list of point id's */
  void CalculatePolygonNormal(vtkIdList *idlist, double *norm) ;

  /** Interpolate two scalar tuples
  tupout = (1-lambda)*tup0 + lambda*tup1 */
  void InterpolateScalars(double *tup0, double *tup1, double *tupout, double lambda, int ncomponents, int dattype) ;

  /** transfer the scalars by interpolation from input to output */
  void TransferScalars() ;

  /** initialize the cutter */
  //void Initialize() ;

  /** do the whole thing */
  void CreateSlice() ;

  // This table maps each output point to the input points or point which created it.
  // if mtype = POINT_TO_POINT this maps the point idout to the intersected point id0
  // if   "   = POINT_TO_EDGE   "   "     "    "   idout  "  "  intersected edge (id0,id1)
  typedef struct{
    vtkIdType idout ;    // id of point in output polydata
    vtkIdType id0 ;      // id's of corresponding input points
    vtkIdType id1 ;     
    double lambda ;      // fractional distance of interpolated point along edge
    mapping_type mtype ; // mapping is point-to-edge or point-to-point
  } EdgeMapping ;
  std::vector<EdgeMapping>m_edgeMapping ;

  // This is a list of the input cells which have been touched by the plane.
  // NB this includes some grazing incidence cells which do not create any output polygons,
  // so don't use it to map input and output cells.
  std::vector<vtkIdType> m_intersectedCells ;

  // This is a list of the output point id's in EVERY input cell, including all the empty ones.
  // It is a list of lists.
  typedef std::vector<vtkIdType> IdList;
  std::vector<IdList> m_pointsInCells ;

  // This is a mapping from the output cells to the input cells: 
  // cellid_in = m_cellMapping[cellid_out]
  // It is set in CreateSlice()
  std::vector<vtkIdType>m_cellMapping ;

  // cutting function
  vtkPlane *m_cutFunction ;

  // input and output
  vtkUnstructuredGrid *m_unstructGrid ;
  vtkPolyData *m_polydata ;
} ;

#endif
