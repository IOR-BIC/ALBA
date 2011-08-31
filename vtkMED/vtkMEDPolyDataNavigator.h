#ifndef __vtkMEDPolyDataNavigator_h
#define __vtkMEDPolyDataNavigator_h

#include "vtkObject.h"
#include "vtkPolyData.h"
#include "vtkIdType.h"
#include "vtkIdList.h"
#include <vector>
#include <map>
#include <set>
#include <ostream>
#include <assert.h>




//------------------------------------------------------------------------------
/// vtkMEDPolyDataNavigator. \n
/// Useful methods for navigating and editing vtkPolyData and vtkIdList. \n\n
///
/// Methods already provided by vtkPolyData are: \n
/// GetCellPoints(cellid, idlist) to get the points on a cell \n
/// GetPointCells(ptid, idlist) to get the cells around a point \n
/// GetCellEdgeNeighbours(cellId, id0, id1, idlist) to get the cell neighbours across a cell edge. \n\n
///
/// Formats of lists: \n
/// Edges are listed as EdgeVector = std::vector<Edge>, \n
/// and Id's of points and cells as vtkIdList. \n
/// However we also use std::multimap and std::set if efficiency demands.
//
// For Developers:
//
// polydata->GetCellPoints() is much faster than polydata->GetCell()->GetCellIds(),
// which has only serial access to the cells.
//
// Methods which use GetCellPoints() need BuildCells().
// Methods which use GetPointCells() need BuildLinks().
// All the point, edge and cell methods depend on at least one of these.
// GetCellPoints() and GetPointCells() will build the links themselves if they are not already built,
// but will NOT update existing links if they are invalid, causing a crash.
// Therefore methods which change the polydata should finish by deleting the invalid links,
// or carry a warning that the user should do it.
//
// Version: Nigel McFarlane 8.7.11
//------------------------------------------------------------------------------
class VTK_GRAPHICS_EXPORT vtkMEDPolyDataNavigator : public vtkObject
{
public:
  static vtkMEDPolyDataNavigator *New();
  vtkTypeRevisionMacro(vtkMEDPolyDataNavigator, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) {} ;



  //----------------------------------------------------------------------------
  /// Definitions
  //----------------------------------------------------------------------------
  class Edge ;
  typedef std::set<int> IdSet ;
  typedef std::vector<Edge> EdgeVector ;
  typedef std::multimap<int,Edge> EdgeMultiMap ;


  //----------------------------------------------------------------------------
  /// id list methods
  //----------------------------------------------------------------------------

  /// Modulo operator, same as % but works correctly on negative values of n as well
  int Modulo(int n, int m) const ;

  /// Is id in list
  inline bool InList(int id,  vtkIdList *idlist) const {return (idlist->IsId(id) >= 0) ;}

  /// Is id not in list
  inline bool NotInList(int id,  vtkIdList *idlist) const {return (idlist->IsId(id) < 0) ;}

  /// Insert id into list before location i. \n
  /// Analogous to insert() method on std::vector.
  void InsertIdIntoList(int i, int id, vtkIdList *idList) const ;

  /// Delete last entry in id list
  void DeleteLastId(vtkIdList *idList) const ;

  /// Append id list: idList = idList + addList. \n
  /// Note the flags which allow you to omit the first and last ids.
  void AppendIdList(vtkIdList *idList,  vtkIdList *addList,  bool includeFirstId = true,  bool includeLastId = true) const ;

  /// Get the id's which are in both lists
  void GetIdsInBothLists(vtkIdList *idlist0,  vtkIdList *idlist1,  vtkIdList *idlist_out) const ;

  /// Find the index of the id. \n
  /// If istart is set, the search range is istart to n-1 inclusive. \n
  /// Use istart to find the next occurrence, or whether a next occurrence exists. \n
  /// Returns -1 if not found.
  int FindIdInList(vtkIdList *idList,  int id,  int istart = 0) const ;

  /// Replace idold with idnew in idList. \n
  /// Returns index of id, -1 if not found.
  int FindAndReplaceIdInList(vtkIdList *idList,  int idold,  int idnew) const ;

  /// Find the index of a consecutive pair of id's in a list. \n
  /// Returns the index of the first of the pair to be found. \n
  /// Returns -1 if not found
  int FindIdPairFirstId(vtkIdList *idList,  int id0,  int id1) const ;

  /// Find the index of a consecutive pair of id's in a list. \n
  /// Returns the index of the second of the pair to be found. \n
  /// Returning the second id is most convenient if you want to insert an id between the pair. \n
  /// Returns -1 if not found
  int FindIdPairSecondId(vtkIdList *idList,  int id0,  int id1) const ;

  /// Insert id between consecutive pair of id's in a list. \n
  /// Returns -1 if not found
  int InsertIdBetweenIds(vtkIdList *idList,  int id0,  int id1, int idNew) const ;

  /// Get the part of idList from indices i0 to i1 inclusive. \n
  /// The list is assumed to be circular, so n wraps around to 0. \n\n
  /// Forwards direction: \n
  /// If i0 <= i1, you get i0...i1. \n
  /// If i0 > i1 , you get i0...n-1, 0,1...i1. \n\n
  /// Backwards direction: \n
  /// If i0 >= i1, you get i0...i1, counting backwards. \n
  /// If i0 < i1,  you get i0...0, n-1...i1.
  void GetPartOfIdList(vtkIdList *idList,  vtkIdList *idListOut,  int i0,  int i1,  bool forwards = true) const ;

  /// Initialize list to 3 triangle ids
  void InitListToTriangle(vtkIdList *list, int id0, int id1, int id2) const ;

  /// Add unique id to set.  Cf. vtkIdList::InsertUniqueId(). \n
  /// This is efficient if there are a lot of id's.
  inline void AddUniqueIdToSet(IdSet& idSet, int id) const {idSet.insert(id);}

  /// Copy id map to vtkIdList
  void CopyIdSetToList(IdSet idSet, vtkIdList *idList) const ;

  /// Cyclic shift id list
  void CyclicShiftIdList(vtkIdList *idList, int nsteps) const ;

  /// Print id list
  void PrintIdList(vtkIdList *idList, ostream& os) const ;



  //----------------------------------------------------------------------------
  /// point, edge and cell methods
  //----------------------------------------------------------------------------

  /// Add edge to list if not already present. \n
  /// NB This is stupendously inefficient if there are a lot of edges ! \n
  void AddUniqueEdge(const Edge& edge, EdgeVector& edgeList) const ; 

  /// Add edge to multimap if not already present. \n
  /// This is efficient if there are a lot of edges.
  void AddUniqueEdge(const Edge& edge, EdgeMultiMap& edgeMap) const ; 

  /// Copy edge multimap to vector
  void CopyEdgeMapToVector(EdgeMultiMap& edgeMap, EdgeVector& edges) const ;

  /// Get number of points on cell
  /// This is not very efficient, so not recommended to call a large number of times.
  int GetNumberOfPointsOnCell(vtkPolyData *polydata, int cellId) const ;

  /// Get edges of cell.
  void GetCellEdges(vtkPolyData *polydata, int cellId, EdgeVector& edges) const ;

  /// Get list of cells on edge
  void GetCellNeighboursOfEdge(vtkPolyData *polydata, const Edge& edge, vtkIdList *idList) const ;

  /// Get number of cells on an edge
  int GetNumberOfCellsOnEdge(vtkPolyData *polydata, const Edge& edge) const ;

  /// Get adjacent points on a cell, ie those joined to it by edges
  void GetAdjacentPointsOnCell(vtkPolyData *polydata, int cellId, int ptId, int& ptId0, int& ptId1) const ;

  /// Get all points on cells around a point.
  void GetPointsOnCellNeighbours(vtkPolyData *polydata, int ptId, vtkIdList *idList) const ;

  /// Get point neighbours of point which are joined by edges. \n
  /// If the cells are triangles, this is the same as GetPointsOnCellNeighbours().
  void GetPointNeighboursOfPoint(vtkPolyData *polydata, int ptId, vtkIdList *idList) const ;

  /// Get the edges of the cells around a point, including those connected to the point itself
  void GetEdgesAroundPointInclusive(vtkPolyData *polydata, int ptId, EdgeVector& edges) const ;

  /// Get the edges of the cells around a point, excluding those connected to the point itself
  void GetEdgesAroundPointExclusive(vtkPolyData *polydata, int ptId, EdgeVector& edges) const ;

  /// Get the points in the cells around the edge, excluding those in the edge
  void GetPointsAroundEdgeExclusive(vtkPolyData *polydata, const Edge& edge, vtkIdList *idList) const ;

  /// Get the edges on the cells on each side of the edge. \n
  /// This does not consider edges which are joined only through the end vertices.
  void GetEdgesAroundEdge(vtkPolyData *polydata, const Edge& edge, EdgeVector& edges) const ;

  /// Get list of all edges in polydata
  void GetAllEdges(vtkPolyData *polydata, EdgeVector& edges) const ;

  /// Get list of all edges in polydata, in the form of a multimap for quick search
  void GetAllEdges(vtkPolyData *polydata, EdgeMultiMap& edges) const ;

  /// Get list of all edges on list of cells
  void GetEdgesOnListOfCells(vtkPolyData *polydata, vtkIdList *cellList, EdgeVector& edges) const ;

  /// Get list of all edges on list of cells, in the form of a multimap for quick search
  void GetEdgesOnListOfCells(vtkPolyData *polydata, vtkIdList *cellList, EdgeMultiMap& edges) const ;

  /// Get list of all cells on list of edges
  void GetCellsOnListOfEdges(vtkPolyData *polydata, EdgeVector& edgeList, vtkIdList* cellList) const ;

  /// Get list of all cells on list of edges, in the form of a set for quick search
  void GetCellsOnListOfEdges(vtkPolyData *polydata, EdgeVector& edgelist, IdSet& cellSet) const ;

  /// Get the edge connected neighbours of a cell
  void GetCellsOnCell_EdgeConnected(vtkPolyData *polydata, int cellId,  vtkIdList *idList) const ;

  /// Is point on a cell
  bool IsPointOnCell(vtkPolyData *polydata, int cellId, int ptId) const ;

  /// Is edge on a cell
  bool IsEdgeOnCell(vtkPolyData *polydata, int cellId, const Edge& edge) const ;

  /// Find point on cell which is opposite to the edge. \n
  /// If cell is not a triangle, returns the first non-edge point found. \n
  /// Returns -1 if none found.
  int GetPointOnCellOppositeToEdge(vtkPolyData *polydata, int cellId, const Edge& edge) const ;

  /// Get cell which containing both edges
  /// Returns -1 if other than one cell found
  int GetCellWithTwoEdges(vtkPolyData *polydata, const Edge& edge0,  const Edge& edge1)  const ;

  /// Get cell containing edge and point
  /// Returns -1 if other than one cell found
  int GetCellWithEdgeAndPoint(vtkPolyData *polydata, const Edge& edge, int ptId)  const ;

  /// Get cell containing all three points
  /// Returns -1 if other than one cell found
  int GetCellWithThreePoints(vtkPolyData *polydata, int ptId0, int ptId1, int ptId2)  const ;

  /// List cells in polydata with a given number of points 
  void GetCellsWithNumberOfPoints(vtkPolyData *polydata,  int n,  vtkIdList *cellIds) const ;

  /// Get center of cell
  void GetCenterOfCell(vtkPolyData *polydata, int cellId, double x[3]) const ;

  /// Get points on cell which are colinear with the straight sides of the shape, \n
  /// ie the points which are on the straight sides, but not the corners. \n
  /// Useful for triangulating a cell. \n
  /// tol is the tolerance for colinearity in degrees. \n
  /// NB The output is the indices of the points on the cell, not the polydata id's.
  void GetPointsOnStraightSidesOfCell(vtkPolyData *polydata, int cellId, vtkIdList *ptIds, double tol) const ;

  /// Get points with a given scalar value. \n
  /// Useful for listing labelled cells.
  void GetPointsWithScalarValue(vtkPolyData *polydata, char* scalarName, int component, double value, vtkIdList *pts) ;

  /// PrintCell. \n
  /// NB This depends on BuildCells()
  void PrintCell(vtkPolyData *polydata, int cellId, ostream& os)  const ;

  /// Print all cells in polydata. \n
  void PrintCells(vtkPolyData *polydata, ostream& os)  const ;

  /// Print attribute structure of polydata
  void PrintAttributeData(vtkPolyData *polydata, ostream& os, bool printTuples = false)  const ;


  //----------------------------------------------------------------------------
  /// Methods which add or delete attribute data
  //----------------------------------------------------------------------------

  /// Copy point data to new entry at the end of the attribute list, \n
  /// increasing the no. of tuples by one. \n
  /// The attributes are copied from id0.
  void CopyPointData(vtkPolyData *polydata,  int id0) const ;

  /// Copy point data to new entry at the end of the attribute list, \n
  /// increasing the no. of tuples by one. \n
  /// The attributes are interpolated between the points id0 and id1.
  void CopyPointData(vtkPolyData *polydata,  int id0, int id1, double lambda) const ;

  /// Copy cell data to new entry at the end of the attribute list, \n
  /// increasing the no. of tuples by one.
  void CopyCellData(vtkPolyData *polydata,  int cellId) const ;

  /// Delete tuples from cell attributes. \n
  void DeleteCellTuples(vtkPolyData *polydata, vtkIdList *cellIds)  const ;



  //----------------------------------------------------------------------------
  /// Methods which change the polydata
  //----------------------------------------------------------------------------

  /// Set cell to degenerate empty cell
  void SetCellToEmpty(vtkPolyData *polydata, int cellId) const ;

  /// Delete list of cells. \n
  /// cf polydata->DeleteCell(), which labels the cell, but does not actually remove it. \n
  /// The corresponding attribute data is also deleted.
  void DeleteCells(vtkPolyData *polydata, vtkIdList *idList)  const ;

  /// Create a new point, adding tuples to the attribute data if necessary. \n
  /// The position and scalars are copied from id0. \n
  /// Returns the id of the created point. \n
  /// This only creates a point - it does not add it to a cell. \n
  /// This can be called successively, but you must delete or rebuild the cells and links when finished.
  int CreateNewPoint(vtkPolyData *polydata,  int id0) const ;

  /// Create a new point, adding tuples to the attribute data if necessary. \n
  /// The position and scalars are interpolated between id0 and id1. \n
  /// Returns the id of the created point. \n
  /// Lambda is the interpolation weight, where 0 <= lambda <= 1. \n
  /// This only creates a point - it does not add it to a cell. \n
  /// This can be called successively, but you must delete or rebuild the cells and links when finished.
  int CreateNewPoint(vtkPolyData *polydata,  int id0, int id1, double lambda) const ;

  /// Create a new cell, adding tuples to the attribute data if necessary. \n
  /// The attribute data is copied from cellId. \n
  /// Returns the id of the created cell. \n
  /// This can be called successively, but you must delete or rebuild the cells and links when finished.
  int CreateNewCell(vtkPolyData *polydata,  int copyScalarsCellId,  vtkIdList *ids) const ;

  /// Copy cells to the end of the polydata. \n
  /// The cell attributes are also copied. \n
  /// The id's of the new cells are returned in newCellIds.
  void CopyCells(vtkPolyData *polydata,  vtkIdList *cellIds,  vtkIdList *newCellIds)  const ;

  /// Add new points to edges. \n
  /// This creates a new point on the midpoint of each edge. \n
  /// The id's of the new points are returned in newPtIds. \n
  /// All cells on each edge will gain the new point, so triangles will become quads.
  void AddPointsToEdges(vtkPolyData *polydata, EdgeVector edges, vtkIdList *newPtIds)  const ;

  /// Change a point id in a cell. \n
  /// This searches for idold in the cell and replaces it with idnew. \n
  /// This can (probably) be called successively, but you must delete or rebuild the cells and links when finished.
  void ChangePointIdInCell(vtkPolyData *polydata, int cellId,  int idold, int idnew) const ;

  /// Split cells. \n
  /// This does not create new points nor affect neighbouring cells. \n
  /// The edges are the point-pairs which must be connected across each cell. \n
  /// The cells must be quads or greater.
  void SplitCells(vtkPolyData *polydata,  vtkIdList *cellIds,  EdgeVector edges) const ;

  /// Split cells. \n
  /// This does not create new points nor affect neighbouring cells. \n
  /// Each cell is split along a new edge from the given point to the opposite point. \n
  /// Quads will be split into two triangles. \n
  /// Cells greater than quads will be split in half. \n
  /// The cells must be quads or greater.
  void SplitCells(vtkPolyData *polydata,  vtkIdList *cellIds,  vtkIdList *ptIds) const ;

  /// Subdivide triangular cells. \n
  /// Each listed cell is split into 4, \n
  /// and the neighbours of the listed cells are triangulated as required. \n
  /// The list of input cellId's is overwritten by the list of new cells.
  void SubdivideCells(vtkPolyData *polydata,  vtkIdList *cellIds) const ;

  /// Merge points. \n
  /// The points in the input lists are merged and assigned the new ids in the output list. \n
  /// Lambda controls the interpolation: \n
  ///  0 or 1 sets the new point equal to one or other of the input points. \n
  ///  0.5 sets the new point to half-way between the two.
  void MergePoints(vtkPolyData *polydata, vtkIdList *idsIn0,  vtkIdList *idsIn1,  vtkIdList *idsOut,  double lambda) const ;



  //----------------------------------------------------------------------------
  /// protected methods
  //----------------------------------------------------------------------------

  protected:
    vtkMEDPolyDataNavigator() ; ///< constructor
    ~vtkMEDPolyDataNavigator() ; ///< deconstructor

} ;





//------------------------------------------------------------------------------
/// Edge helper class. \n
/// This contains the id's of the endpoints. \n
/// Edges are equal if they contain the same id's, regardless of direction.
//------------------------------------------------------------------------------
class vtkMEDPolyDataNavigator::Edge
{
public:
  Edge(int id0, int id1) : m_Id0(id0), m_Id1(id1) {} ///< constructor
  int GetId0() const {return m_Id0 ;} ///< get id0
  int GetId1() const {return m_Id1 ;} ///< get id1
  void SetId0(int id0) {m_Id0 = id0 ;} ///< set id0
  void SetId1(int id1) {m_Id1 = id1 ;} ///< set id1
  int GetOtherId(int id) const {if (id == m_Id0) return m_Id1 ; else if (id == m_Id1) return m_Id0 ; else return -1 ;}
  bool ContainsPoint(int id) const {return ((m_Id0 == id) || (m_Id1 == id)) ;} ///< does edge contain point
  bool IsSameDirection(const Edge& edge) const {return (m_Id0 == edge.m_Id0) && (m_Id1 == edge.m_Id1) ;} ///< is edge same direction as this one
  bool IsReversed(const Edge& edge) const {return (m_Id0 == edge.m_Id1) && (m_Id1 == edge.m_Id0) ;} ///< is edge reverse of this one
  void Reverse() {std::swap(m_Id0,m_Id1) ;} ///< reverse direction (swap end points)
  void PrintSelf(ostream& os, vtkIndent indent) const {os << m_Id0 << " " << m_Id1 << std::endl ;} ///< print self
  bool operator==(const Edge& edge) const ; ///< equals operator
  bool operator!=(const Edge& edge) const ; ///< not equals operator
private:
  vtkIdType m_Id0 ;   ///< id of endpoint 0
  vtkIdType m_Id1 ;   ///< id of endpoint 1
};







#endif