/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkALBAPolyDataNavigator.h,v $
Language:  C++
Date:      $Date: 2009-03-20 16:39:48 $
Version:   $Revision: 1.1.2.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2012
University of Bedfordshire
=========================================================================*/


#ifndef __vtkALBAPolyDataNavigator_h
#define __vtkALBAPolyDataNavigator_h

#include "albaConfigure.h"
#include "vtkObject.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include <vector>
#include <map>
#include <set>
#include <ostream>
#include <assert.h>




//------------------------------------------------------------------------------
/// vtkALBAPolyDataNavigator. \n
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
// Version: Nigel McFarlane 2.4.14
//------------------------------------------------------------------------------
class ALBA_EXPORT vtkALBAPolyDataNavigator : public vtkObject
{
public:
  static vtkALBAPolyDataNavigator *New();
  vtkTypeMacro(vtkALBAPolyDataNavigator, vtkObject);
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

  /// Modulo operator, same as % but works correctly on negative values of i as well
  int Modulo(int i, int n) const ;

  /// Is id in list
  inline bool InList(int id,  vtkIdList *idlist) const {return (idlist->IsId(id) >= 0) ;}

  /// Is id not in list
  inline bool NotInList(int id,  vtkIdList *idlist) const {return (idlist->IsId(id) < 0) ;}

  /// Is id in set.
  inline bool InSet(int id,  const IdSet& idSet) const ;

  /// Is id not in set
  inline bool NotInSet(int id,  const IdSet& idSet) const ;

  /// Insert id into list before location i. \n
  /// Analogous to insert() method on std::vector.
  void InsertIdIntoList(int i, int id, vtkIdList *idList) const ;

  /// Delete last entry in id list
  void DeleteLastId(vtkIdList *idList) const ;

  /// Reverse entries in id list
  void ReverseIdList(vtkIdList *idList) const ;

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

  /// Copy vtkIdList to id set. \n
  /// NB if an id appears twice in the id list, it will only copy once to the map.
  void CopyListToIdSet(vtkIdList *idList, IdSet& idSet) const ;

  /// Add vtkIdList to id set. \n
  /// NB if an id appears twice in the id list, it will only copy once to the map. \n
  /// This is a good way to remove duplicate ids from a list.
  void AddListToIdSet(vtkIdList *idList, IdSet& idSet) const ;

  /// Copy id set to vtkIdList
  void CopyIdSetToList(const IdSet& idSet, vtkIdList *idList) const ;

  /// Get ids in the range (0,n-1) which are not in the list.
  void GetIdsNotInList(vtkIdList* idsIn, vtkIdList* idsOut, int n) ;

  /// Cyclic shift id list
  void CyclicShiftIdList(vtkIdList *idList, int nsteps) const ;

  /// Copy vector to id list
  void CopyVectorToList(const std::vector<int>& vlist, vtkIdList* idList, bool append) const ;

  /// Copy id list to vector
  void CopyListToVector(vtkIdList* idList, std::vector<int>& vlist, bool append) const ;

  /// Set id list to all points (0,1,2...npts-1)
  void SetIdListToAllPoints(vtkPolyData *polydata, vtkIdList *list) const ;

  /// Set id list to all cells (0,1,2...ncells-1)
  void SetIdListToAllCells(vtkPolyData *polydata, vtkIdList *list) const ;

  /// Get min id in list
  int GetMinIdInList(vtkIdList *list) const ;

  /// Get max id in list
  int GetMaxIdInList(vtkIdList *list) const ;

  /// Print id list
  void PrintIdList(vtkIdList *idList, ostream& os) const ;

  /// Print id list
  void PrintIdListCSV(vtkIdList *idList, ostream& os) const ;



  //----------------------------------------------------------------------------
  /// point, edge and cell methods
  //----------------------------------------------------------------------------

  /// Add edge to list if not already present. \n
  /// NB This is stupendously inefficient if there are a lot of edges !
  void AddUniqueEdge(const Edge& edge, EdgeVector& edgeList) const ; 

  /// Add edge to multimap if not already present. \n
  /// This is efficient if there are a lot of edges.
  void AddUniqueEdge(const Edge& edge, EdgeMultiMap& edgeMap) const ; 

  /// Copy edge multimap to vector
  // (NB Can't declare const EdgeMultiMap& for some reason)
  void CopyEdgeMapToVector(EdgeMultiMap& edgeMap, EdgeVector& edges) const ;

  /// Copy edge vector to edge multimap. \n
  /// Multiple occurrences of edges are only copied once into the map.
  void CopyVectorToEdgeMap(const EdgeVector& edges, EdgeMultiMap& edgeMap) const ;

  /// Is edge in edge map
  bool IsEdgeInMap(const EdgeMultiMap& edgeMap, const Edge& edge) const ;

  /// Get number of points on cell. \n
  /// This is not very efficient, so not recommended to call a large number of times.
  int GetNumberOfPointsOnCell(vtkPolyData *polydata, int cellId) const ;

  /// Get points on cell with coords (cf polydata->GetCellPoints())
  void GetCellPointsWithCoords(vtkPolyData *polydata, int cellId, vtkIdList* ptIds, double (*coords)[3]) ;

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

  /// Get list of all points on list of edges
  void GetPointsOnListOfEdges(vtkPolyData *polydata, const EdgeVector& edgeList, vtkIdList* ptList) const ;

  /// Get list of all points on list of edges, in the form of a set for quick search
  void GetPointsOnListOfEdges(vtkPolyData *polydata, const EdgeVector& edgelist, IdSet& ptSet) const ;

  /// Get list of all cells on list of edges
  void GetCellsOnListOfEdges(vtkPolyData *polydata, const EdgeVector& edgeList, vtkIdList* cellList) const ;

  /// Get list of all cells on list of edges, in the form of a set for quick search
  void GetCellsOnListOfEdges(vtkPolyData *polydata, const EdgeVector& edgelist, IdSet& cellSet) const ;

  /// Get list of all cells on list of points
  void GetCellsOnListOfPoints(vtkPolyData *polydata, vtkIdList *ptIds, vtkIdList* cellList) const ;

  /// Get list of all cells on list of points, in the form of a set for quick search
  void GetCellsOnListOfPoints(vtkPolyData *polydata, vtkIdList *ptIds, IdSet& cellSet) const ;

  /// Get the edge connected neighbours of a cell
  void GetCellsOnCell_EdgeConnected(vtkPolyData *polydata, int cellId,  vtkIdList *idList) const ;

  /// Get edge connected neighbours of list of cells
  void GetCellsOnListOfCells_EdgeConnected(vtkPolyData *polydata, vtkIdList* cellsIn, vtkIdList* cellsOut) ;

  /// Get edge connected neighbours of list of cells
  void GetCellsOnListOfCells_EdgeConnected(vtkPolyData *polydata, IdSet& cellsIn, IdSet& cellsOut) ;

  /// Find edge in list.  Returns -1 if not found.
  int FindEdgeInList(Edge& edge, EdgeVector& edgeList) const ;

  /// Find index of point on a cell
  /// Returns - 1 if not found
  int FindIndexOfPointOnCell(vtkPolyData *polydata, int cellId, int ptId) const ;

  /// Is point on a cell
  bool IsPointOnCell(vtkPolyData *polydata, int cellId, int ptId) const ;

  /// Is edge on a cell
  bool IsEdgeOnCell(vtkPolyData *polydata, int cellId, const Edge& edge) const ;

  /// Is proposed edge a point-pair across this cell, joining non-adjacent points. \n
  /// Can only be true for quads or higher.
  bool IsEdgeAcrossCell(vtkPolyData *polydata, int cellId, const Edge& edge) const ;

  /// Is point in list of edges
  bool IsPointInEdgeList(int ptId, EdgeVector& edges) ;

  /// Is point in list of edges
  bool IsPointInEdgeList(int ptId, EdgeVector& edges, int& whichEdge) ;

  /// Find point on cell which is opposite to the edge. \n
  /// If cell is not a triangle, returns the first non-edge point found. \n
  /// Returns -1 if none found.
  int GetPointOnCellOppositeToEdge(vtkPolyData *polydata, int cellId, const Edge& edge) const ;

  /// Get cell which contains both edges. \n
  /// Returns -1 if other than one cell found
  int GetCellWithTwoEdges(vtkPolyData *polydata, const Edge& edge0,  const Edge& edge1)  const ;

  /// Get cell containing edge and point. \n
  /// Returns -1 if other than one cell found
  int GetCellWithEdgeAndPoint(vtkPolyData *polydata, const Edge& edge, int ptId)  const ;

  /// Get cell containing all three points \n
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

  /// Get edges on boundary of polydata
  void GetEdgesOnBoundary(vtkPolyData *polydata, EdgeVector& edges) const ;

  /// Get points on boundary of polydata
  void GetPointsOnBoundary(vtkPolyData *polydata, vtkIdList* ptIds) const ;

  /// Get cells on boundary of polydata
  void GetCellsOnBoundary(vtkPolyData *polydata, vtkIdList* cellIds) const ;

  /// Get points with a given scalar value. \n
  /// Useful for listing labelled points.
  void GetPointsWithScalarValue(vtkPolyData *polydata, char* scalarName, int component, double value, vtkIdList *pts) ;

  /// Get cells with a given scalar value. \n
  /// Useful for listing labelled cells.
  void GetCellsWithScalarValue(vtkPolyData *polydata, char* scalarName, int component, double value, vtkIdList *cells) ;

  /// Set list of points to a given scalar value. \n
  /// Useful for labelling points.
  void SetPointsToScalarValue(vtkPolyData *polydata, char* scalarName, int component, double value, vtkIdList *pts) ;

  /// Set list of cells to a given scalar value. \n
  /// Useful for labelling cells.
  void SetCellsToScalarValue(vtkPolyData *polydata, char* scalarName, int component, double value, vtkIdList *cells) ;

  /// Get points with no cell neighbours
  void GetPointsWithNoCells(vtkPolyData *polydata, vtkIdList *pts) const ;

  /// Get length of edge
  double GetLengthOfEdge(vtkPolyData *polydata, const Edge edge) const ;

  /// PrintCell. \n
  /// NB This depends on BuildCells()
  void PrintCell(vtkPolyData *polydata, int cellId, ostream& os)  const ;

  /// PrintCell. \n
  /// NB This depends on BuildCells()
  void PrintCellCSV(vtkPolyData *polydata, int cellId, ostream& os)  const ;

  /// Print all cells in polydata. \n
  void PrintCells(vtkPolyData *polydata, ostream& os)  const ;

  /// Print all cells in polydata. \n
  void PrintCellsCSV(vtkPolyData *polydata, ostream& os)  const ;

  /// Print attribute structure of polydata
  void PrintAttributeData(vtkPolyData *polydata, ostream& os, bool printTuples = false)  const ;

  /// Print bounds of polydata
  void PrintBounds(vtkPolyData *polydata, ostream& os) const ;


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

  /// Delete tuples from point attributes. \n
  void DeletePointTuples(vtkPolyData *polydata, vtkIdList *ptIds)  const ;

  /// Create new attribute array
  void CreatePointDataArray(vtkPolyData *polydata, char* name, int numberOfComponents, int dataType) const ;

  /// Create new attribute array
  void CreateCellDataArray(vtkPolyData *polydata, char* name, int numberOfComponents, int dataType) const ;


  //----------------------------------------------------------------------------
  /// Methods which change the polydata
  //----------------------------------------------------------------------------

  /// Set cell to degenerate empty cell
  void SetCellToEmpty(vtkPolyData *polydata, int cellId) const ;

  /// Delete list of cells. \n
  /// Supports cell types verts, lines and polys. \n
  /// cf polydata->DeleteCell(), which labels the cell, but does not actually remove it. \n
  /// The corresponding attribute data is also deleted.
  void DeleteCells(vtkPolyData *polydata, vtkIdList *idList)  const ;

  /// Delete list of points. \n
  /// The corresponding attribute data is also deleted. \n
  /// NB Only use this for points which form a block at the end of the polydata, \n
  /// and which are not members of cells, else cells will contain invalid id's.
  void DeletePoints(vtkPolyData *polydata, vtkIdList *idList)  const ;

  /// Swap the id's of two points. \n
  /// Can call consecutively but must delete or rebuild links when finished.
  void SwapPointIds(vtkPolyData *polydata, int id0, int id1) const ;

  /// Move id's of points to end of polydata. \n
  /// Corresponding cells and attributes are also changed.
  void MovePointIdsToEnd(vtkPolyData *polydata, vtkIdList *idList)  const ;

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
  /// A single point is placed at the midpoint of each edge. \n
  /// Each edge should occur only once in the list. \n
  /// The id's of the new points are returned in newPtIds. \n
  /// All cells on each edge will gain the new point, so triangles will become quads. \n
  /// This method does not change the id's of the cells.
  void AddPointsToEdges(vtkPolyData *polydata, const EdgeVector& edges, vtkIdList *newPtIds)  const ;

  /// Add new points to edges. \n
  /// Each point is placed on edge[i] at interpolated position lambda[i][j]. \n
  /// Each edge should occur only once in the list. \n
  /// The id's of the new points are returned in newPtIds. \n
  /// All cells on each edge will gain the new points, so triangles will become quads. \n
  /// This method does not change the id's of the cells.
  void AddPointsToEdges(
    vtkPolyData *polydata, const EdgeVector& edges, 
    const std::vector<std::vector<double> >& lambda, 
    std::vector<std::vector<int> >& newPtIds)  const ;

  /// Add new points to edges. \n
  /// Each point i is placed on edge[i] at interpolated position lambda[i]. \n
  /// This might not work if edges occur more than once in the list. \n
  /// The id's of the new points are returned in newPtIds. \n
  /// All cells on each edge will gain the new points, so triangles will become quads. \n
  /// This method does not change the id's of the cells.
  void AddPointsToEdges(vtkPolyData *polydata, const EdgeVector& edges, const std::vector<double>& lambda, vtkIdList *newPtIds)  const ;

  /// Change a point id in a cell. \n
  /// This searches for idold in the cell and replaces it with idnew. \n
  /// Returns true if a change was made. \n
  /// This can be called successively, but you must delete or rebuild the cells and links when finished.
  bool ChangePointIdInCell(vtkPolyData *polydata, int cellId,  int idold, int idnew) const ;

  /// Split cells. \n
  /// One cutting line per cell, defined by two given points. \n
  /// This does not create new points nor affect neighbouring cells. \n
  /// The edges are the point-pairs which must be connected across each cell. \n
  /// The cells must be quads or greater.  For triangles, add the extra points first, then split.
  void SplitCells(vtkPolyData *polydata,  vtkIdList *cellIds,  const EdgeVector& edges) const ;

  /// Split cells. \n
  /// One cutting line per cell, defined by a single given point to the opposite point. \n
  /// This does not create new points nor affect neighbouring cells. \n
  /// Each cell is split along a new edge from the given point to the opposite point. \n
  /// Quads will be split into two triangles. \n
  /// Cells greater than quads will be split in half. \n
  /// The cells must be quads or greater.  For triangles, add the extra points first, then split.
  void SplitCells(vtkPolyData *polydata,  vtkIdList *cellIds,  vtkIdList *ptIds) const ;

  /// Split cells. \n
  /// Many cutting lines per cell, defined by list of point pairs. \n
  /// This does not create new points nor affect neighbouring cells. \n
  /// The edges are the point-pairs which must be connected across each cell. \n
  /// The cells must be quads or greater.  For triangles, add the extra points first, then split.
  /// (This has not been tested)
  void SplitCells(vtkPolyData *polydata,  vtkIdList *cellIds,  std::vector<EdgeVector>& edgeLists) const ;

  /// Subdivide triangular cells. \n
  /// Each listed cell is split into 4, \n
  /// and the neighbours of the listed cells are triangulated as required. \n
  /// The list of input cellId's is overwritten by the list of new cells.
  void SubdivideCells(vtkPolyData *polydata,  vtkIdList *cellIds) const ;

  /// Subdivide all cells in mesh (triangles only). \n
  /// Each cell is split into 4.
  void SubdivideAllCells(vtkPolyData *polydata) const ;

  /// Merge points. \n
  /// The points in the input lists are merged and assigned the new ids in the output list. \n
  /// Lambda controls the interpolation: \n
  ///  0 or 1 sets the new point equal to one or other of the input points. \n
  ///  0.5 sets the new point to half-way between the two.
  void MergePoints(vtkPolyData *polydata, vtkIdList *idsIn0,  vtkIdList *idsIn1,  vtkIdList *idsOut,  double lambda) const ;

  /// Remove isolated points which are not part of cells. \n
  /// This does not remove points which are vertex cells.
  void RemovePointsWithNoCells(vtkPolyData *polydata) const ;



  //----------------------------------------------------------------------------
  /// protected methods
  //----------------------------------------------------------------------------

protected:
  vtkALBAPolyDataNavigator() ; ///< constructor
  ~vtkALBAPolyDataNavigator() ; ///< deconstructor

  /// Find the subcells of a cell which is transected by a list of point pairs. \n
  /// Subcells returned as a vector of vtkIdList pointers. (Must be pre-allocated). \n
  /// This does not affect the polydata.
  /// Returns true if sucessful.
  bool FindSubCellsOfCell(vtkPolyData *polydata, int cellId, const EdgeVector& edges, 
    std::vector<vtkIdList*>& outputCells, 
    int maxSubCells, int& numOutputCells) const ;


} ;





//------------------------------------------------------------------------------
/// Edge helper class. \n
/// This contains the id's of the endpoints. \n
/// Edges are equal if they contain the same id's, regardless of direction.
//------------------------------------------------------------------------------
class vtkALBAPolyDataNavigator::Edge
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
  bool IsJoined(const Edge& edge) const ; ///< Are edges joined by at least one point
  bool IsValid() const {return ((m_Id0>=0)&&(m_Id1>=0));} ///< Does edge contain valid data
  void Reverse() {std::swap(m_Id0,m_Id1) ;} ///< reverse direction (swap end points)
  int GenerateKey() const ; /// Calculate a key for locating edge in a map
  void PrintSelf(ostream& os, vtkIndent indent) const {os << m_Id0 << " " << m_Id1 << std::endl ;} ///< print self
  bool operator==(const Edge& edge) const ; ///< equals operator
  bool operator!=(const Edge& edge) const ; ///< not equals operator
private:
  vtkIdType m_Id0 ;   ///< id of endpoint 0
  vtkIdType m_Id1 ;   ///< id of endpoint 1
};







#endif
