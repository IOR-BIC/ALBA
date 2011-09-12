

#include "vtkObjectFactory.h"
#include "vtkObject.h"
#include "vtkPolyData.h"
#include "vtkIdType.h"
#include "vtkIdList.h"
#include "vtkCell.h"
#include "vtkCellArray.h"

#include "vtkMEDPolyDataNavigator.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkCleanPolyData.h"
#include "vtkMath.h"

#include <vector>
#include <map>
#include <set>
#include <ostream>
#include <assert.h>



//------------------------------------------------------------------------------
// standard macros
vtkCxxRevisionMacro(vtkMEDPolyDataNavigator, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(vtkMEDPolyDataNavigator);
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
vtkMEDPolyDataNavigator::vtkMEDPolyDataNavigator()
{
}


//------------------------------------------------------------------------------
// Destructor
vtkMEDPolyDataNavigator::~vtkMEDPolyDataNavigator()
//------------------------------------------------------------------------------
{
}





//------------------------------------------------------------------------------
// Insert id into list before location i.
// Analagous to insert() method on std::vector.
void vtkMEDPolyDataNavigator::InsertIdIntoList(int i, int id, vtkIdList *idList) const
//------------------------------------------------------------------------------
{
  assert (i < idList->GetNumberOfIds()) ;

  vtkIdList *tempList = vtkIdList::New() ;

  for (int j = 0 ;  j < i ;  j++)
    tempList->InsertNextId(idList->GetId(j)) ;

  tempList->InsertNextId(id) ;

  for (int j = i ;  j < idList->GetNumberOfIds() ;  j++)
    tempList->InsertNextId(idList->GetId(j)) ;

  idList->DeepCopy(tempList) ;

  tempList->Delete() ;
}




//------------------------------------------------------------------------------
// Delete last entry in id list.
// Crazy but there's no other way to do it.
void vtkMEDPolyDataNavigator::DeleteLastId(vtkIdList *idList) const
//------------------------------------------------------------------------------
{
  vtkIdList *tempList = vtkIdList::New() ;
  tempList->Initialize() ;

  // copy all but the last id to the temp list
  for (int i = 0 ;  i < idList->GetNumberOfIds() - 1 ;  i++)
    tempList->InsertNextId(idList->GetId(i)) ;

  // copy id's back again
  idList->Reset() ;
  for (int i = 0 ;  i < tempList->GetNumberOfIds() ;  i++)
    idList->InsertNextId(tempList->GetId(i)) ;

  idList->DeepCopy(tempList) ;

  tempList->Delete() ;
}




//------------------------------------------------------------------------------
// Append id list: idList = idList + addList
void vtkMEDPolyDataNavigator::AppendIdList(vtkIdList *idList,  vtkIdList *addList,  bool includeFirstId,  bool includeLastId) const
//------------------------------------------------------------------------------
{
  int istart, ilast ;

  if (includeFirstId)
    istart = 0 ;
  else
    istart = 1 ;

  if (includeLastId)
    ilast = addList->GetNumberOfIds() - 1 ;
  else
    ilast = addList->GetNumberOfIds() - 2 ;

  // copy addList to the end of idList
  for (int i = istart ;  i <= ilast ;  i++)
    idList->InsertNextId(addList->GetId(i)) ;
}




//------------------------------------------------------------------------------
// Get the id's which are in both lists
void vtkMEDPolyDataNavigator::GetIdsInBothLists(vtkIdList *idlist0,  vtkIdList *idlist1,  vtkIdList *idlist_out) const
//------------------------------------------------------------------------------
{
  idlist_out->Initialize() ;

  for (int i = 0 ;  i < idlist0->GetNumberOfIds() ;  i++){
    bool found = false ;
    for (int j = 0 ;  j < idlist1->GetNumberOfIds() && !found ;  j++){
      if (idlist1->GetId(j) == idlist0->GetId(i)){
        idlist_out->InsertNextId(idlist1->GetId(j)) ;
        found = true ;
      }
    }
  }
}



//------------------------------------------------------------------------------
// Find the index of the id.
// If istart is set, the search range is istart to n-1 inclusive.
// Use istart for finding a next occurrence, or whether a second occurrence exists.
// Returns -1 if not found
int vtkMEDPolyDataNavigator::FindIdInList(vtkIdList *idList,  int id,  int istart) const
//------------------------------------------------------------------------------
{
  for (int i = istart ;  i < idList->GetNumberOfIds() ;  i++){
    if (idList->GetId(i) == id)
      return i ;
  }

  // id not found
  return -1 ;
}






//------------------------------------------------------------------------------
// Replace idold with idnew in idList. \n
// Returns index of id, -1 if not found.
int vtkMEDPolyDataNavigator::FindAndReplaceIdInList(vtkIdList *idList,  int idold,  int idnew) const 
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < idList->GetNumberOfIds() ;  i++){
    if (idList->GetId(i) == idold){
      idList->SetId(i, idnew) ;
      return i ;
    }
  }

  // id not found
  return -1 ;
}



//------------------------------------------------------------------------------
// Find the index of a consecutive pair of id's in a list.
// Returns the index of the first of the pair to be found.
// Returns -1 if not found
int vtkMEDPolyDataNavigator::FindIdPairFirstId(vtkIdList *idList,  int id0,  int id1) const
//------------------------------------------------------------------------------
{
  int n = idList->GetNumberOfIds() ;
  for (int i = 0 ;  i < n ;  i++){
    int ii = (i+1)%n ;
    if ((idList->GetId(i) == id0) && (idList->GetId(ii) == id1))
      return i ;
    if ((idList->GetId(i) == id1) && (idList->GetId(ii) == id0))
      return i ;
  }

  // id's not found
  return -1 ;
}



//------------------------------------------------------------------------------
// Find the index of a consecutive pair of id's in a list.
// Returns the index of the second of the pair to be found.
// Returning the second id is most convenient if you want to insert an id between the pair.
// Returns -1 if not found
int vtkMEDPolyDataNavigator::FindIdPairSecondId(vtkIdList *idList,  int id0,  int id1) const
//------------------------------------------------------------------------------
{
  int n = idList->GetNumberOfIds() ;
  for (int i = 0 ;  i < n ;  i++){
    int ii = (i+1)%n ;
    if ((idList->GetId(i) == id0) && (idList->GetId(ii) == id1))
      return ii ;
    if ((idList->GetId(i) == id1) && (idList->GetId(ii) == id0))
      return ii ;
  }

  // id's not found
  return -1 ;
}



//------------------------------------------------------------------------------
// Insert id between consecutive pair of id's in a list. \n
// Returns -1 if not found
int vtkMEDPolyDataNavigator::InsertIdBetweenIds(vtkIdList *idList,  int id0,  int id1, int idNew) const
//------------------------------------------------------------------------------
{
  int i = FindIdPairSecondId(idList, id0, id1) ;
  if (i == -1)
    return -1 ;

  InsertIdIntoList(i, idNew, idList) ;
  return i ;
}




//------------------------------------------------------------------------------
// Get the part of idList from indices i0 to i1 inclusive.
// The list is assumed to be circular, so n wraps around to 0.
//
// Forwards direction:
// If i0 <= i1, you get i0...i1.
// If i0 > i1 , you get i0...n-1, 0,1...i1
//
// Backwards direction:
// If i0 >= i1, you get i0...i1, counting backwards.
// If i0 < i1,  you get i0...0, n-1...i1.
void vtkMEDPolyDataNavigator::GetPartOfIdList(vtkIdList *idList,  vtkIdList *idListOut,  int i0,  int i1,  bool forwards) const
//------------------------------------------------------------------------------
{
  idListOut->Initialize() ;
  int n = idList->GetNumberOfIds() ;

  if (forwards){
    if (i0 <= i1){
      for (int i = i0 ;  i <= i1 ;  i++)
        idListOut->InsertNextId(idList->GetId(i)) ;
    }
    else{
      for (int i = i0 ;  i < n ;  i++)
        idListOut->InsertNextId(idList->GetId(i)) ;

      for (int i = 0 ;  i <= i1 ;  i++)
        idListOut->InsertNextId(idList->GetId(i)) ;
    }
  }
  else{
    if (i0 >= i1){
      for (int i = i0 ;  i >= i1 ;  i--)
        idListOut->InsertNextId(idList->GetId(i)) ;
    }
    else{
      for (int i = i0 ;  i >= 0 ;  i--)
        idListOut->InsertNextId(idList->GetId(i)) ;

      for (int i = n-1 ;  i >= i1 ;  i--)
        idListOut->InsertNextId(idList->GetId(i)) ;
    }
  }
}



//------------------------------------------------------------------------------
// Initialize list to 3 triangle ids
void vtkMEDPolyDataNavigator::InitListToTriangle(vtkIdList *list, int id0, int id1, int id2) const
//------------------------------------------------------------------------------
{
  list->Initialize() ;
  list->InsertNextId(id0) ;
  list->InsertNextId(id1) ;
  list->InsertNextId(id2) ;
}



//------------------------------------------------------------------------------
// Copy id map to vtkIdList
void vtkMEDPolyDataNavigator::CopyIdSetToList(IdSet idSet, vtkIdList *idList) const
//------------------------------------------------------------------------------
{
  idList->Initialize() ;

  IdSet::iterator pos ;
  for (pos = idSet.begin() ;  pos != idSet.end() ;  pos++)
    idList->InsertNextId(*pos) ;
}



//------------------------------------------------------------------------------
// Cyclic shift id list
void vtkMEDPolyDataNavigator::CyclicShiftIdList(vtkIdList *idList, int nsteps) const
//------------------------------------------------------------------------------
{
  vtkIdList *temp = vtkIdList::New() ;

  int n = idList->GetNumberOfIds() ;

  for (int i = 0 ;  i < n ;  i++){
    int ii = Modulo(i-nsteps, n) ;
    temp->InsertNextId(idList->GetId(ii)) ;
  }

  // copy back to original list
  idList->DeepCopy(temp) ;

  temp->Delete() ;
}




//------------------------------------------------------------------------------
// Print id list
void vtkMEDPolyDataNavigator::PrintIdList(vtkIdList *idList, ostream& os) const
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < idList->GetNumberOfIds() ;  i++){
    os << idList->GetId(i) << " " ;

    if (i%10 == 9)
      os << std::endl ;
  }

  os << std::endl ;
}




//------------------------------------------------------------------------------
// Add edge to list if it is not already in the list
// NB This is stupendously inefficient if there are a lot of edges !
void vtkMEDPolyDataNavigator::AddUniqueEdge(const Edge& edge, EdgeVector& edgeList) const
//------------------------------------------------------------------------------
{
  // search list and return if the requested edge is found
  for (int i = 0 ;  i < (int)edgeList.size() ;  i++){
    if (edgeList.at(i) == edge)
      return ;
  }

  // not found so add the edge
  edgeList.push_back(edge) ;
}


//------------------------------------------------------------------------------
// Add edge to multimap if not already present.
void vtkMEDPolyDataNavigator::AddUniqueEdge(const Edge& edge, EdgeMultiMap& edgeMap) const
//------------------------------------------------------------------------------
{
  typedef EdgeMultiMap::iterator Mit ;
  typedef std::pair<Mit,Mit> MitRange ;

  int id0 = edge.GetId0() ;
  int id1 = edge.GetId1() ;
  int key = id0*id0 + id1*id1 ; // key is not quite unique, but collisions should be rare
  bool found = false ;

  Mit pos ;
  MitRange& range = edgeMap.equal_range(key) ;
  for (pos = range.first ;  pos != range.second && !found ;  pos++){
    found = (pos->second == edge) ;
  }

  if (!found)
    pos = edgeMap.insert(std::make_pair(key,edge)) ;
}




//------------------------------------------------------------------------------
// Copy edge multimap to vector
void vtkMEDPolyDataNavigator::CopyEdgeMapToVector(EdgeMultiMap& edgeMap, EdgeVector& edges) const
//------------------------------------------------------------------------------
{
  edges.clear() ;

  EdgeMultiMap::iterator pos ;
  for (pos = edgeMap.begin() ;  pos != edgeMap.end() ;  pos++){
    edges.push_back(pos->second) ;
  }
}




//------------------------------------------------------------------------------
// Get number of points on cell
int vtkMEDPolyDataNavigator::GetNumberOfPointsOnCell(vtkPolyData *polydata, int cellId) const
//------------------------------------------------------------------------------
{
  vtkIdList *ids = vtkIdList::New() ;
  polydata->GetCellPoints(cellId, ids) ;
  int n = ids->GetNumberOfIds() ;
  ids->Delete() ;
  return n ;
}



//------------------------------------------------------------------------------
// Get edges of cell
void vtkMEDPolyDataNavigator::GetCellEdges(vtkPolyData *polydata, int cellId, EdgeVector& edges) const
//------------------------------------------------------------------------------
{
  edges.clear() ;

  vtkIdList *ptIds = vtkIdList::New() ;
  polydata->GetCellPoints(cellId, ptIds) ;

  // Put the points in the list of edges
  int n = ptIds->GetNumberOfIds() ;
  for (int i = 0 ;  i < n ;  i++){
    int id0 = ptIds->GetId(i) ;
    int id1 = ptIds->GetId((i+1)%n) ;
    edges.push_back(Edge(id0,id1)) ;
  }

  ptIds->Delete() ;
}







//-----------------------------------------------------------------------------
// Get list of cells on edge
void vtkMEDPolyDataNavigator::GetCellNeighboursOfEdge(vtkPolyData *polydata, const Edge& edge, vtkIdList *idList) const
//-----------------------------------------------------------------------------
{
  idList->Initialize() ;

  // list cell neighbours of both endpoints of edge 
  vtkIdList *CellsOnPt0 = vtkIdList::New() ;
  vtkIdList *CellsOnPt1 = vtkIdList::New() ;
  polydata->GetPointCells(edge.GetId0(), CellsOnPt0) ; // list of cells on point id0
  polydata->GetPointCells(edge.GetId1(), CellsOnPt1) ; // list of cells on point id1

  // get the cells which are in both lists
  GetIdsInBothLists(CellsOnPt0, CellsOnPt1, idList) ;

  CellsOnPt0->Delete() ;
  CellsOnPt1->Delete() ;
}




//------------------------------------------------------------------------------
/// Get number of cells on an edge
int vtkMEDPolyDataNavigator::GetNumberOfCellsOnEdge(vtkPolyData *polydata, const Edge& edge) const
//------------------------------------------------------------------------------
{
  // list cell neighbours of both endpoints of edge 
  vtkIdList *CellsOnPt0 = vtkIdList::New() ;
  vtkIdList *CellsOnPt1 = vtkIdList::New() ;
  polydata->GetPointCells(edge.GetId0(), CellsOnPt0) ; // list of cells on point id0
  polydata->GetPointCells(edge.GetId1(), CellsOnPt1) ; // list of cells on point id1

  vtkIdList *idList = vtkIdList::New() ;
  GetIdsInBothLists(CellsOnPt0, CellsOnPt1, idList) ;
  int icount = idList->GetNumberOfIds() ;

  idList->Delete() ;
  CellsOnPt0->Delete() ;
  CellsOnPt1->Delete() ;

  return icount ;
}




//-----------------------------------------------------------------------------
// Get adjacent points on a cell, ie those joined to it by edges
void vtkMEDPolyDataNavigator::GetAdjacentPointsOnCell(vtkPolyData *polydata, int cellId, int ptId, int& ptId0, int& ptId1) const
//-----------------------------------------------------------------------------
{
  vtkIdList *ptsOnCell = vtkIdList::New() ;
  polydata->GetCellPoints(cellId, ptsOnCell) ;

  int n = ptsOnCell->GetNumberOfIds() ;

  // which point is input point
  int ifound ;
  bool found = false ;
  for (int i = 0 ;  i < n && !found ;  i++){
    if (ptsOnCell->GetId(i) == ptId){
      ifound = i ;
      found = true ;
    }
  }

  // check that input id was actually found
  assert (found) ;

  if (n < 2){
    ptId0 = -1 ;
    ptId1 = -1 ;
  }
  else if (n == 2){
    ptId0 = ptsOnCell->GetId(Modulo(ifound+1, n)) ;
    ptId1 = -1 ;
  }
  else{
    ptId0 = ptsOnCell->GetId(Modulo(ifound-1, n)) ;
    ptId1 = ptsOnCell->GetId(Modulo(ifound+1, n)) ;
  }

  ptsOnCell->Delete() ;
}




//-----------------------------------------------------------------------------
// Get all points on cells around a point.
void vtkMEDPolyDataNavigator::GetPointsOnCellNeighbours(vtkPolyData *polydata, int ptId, vtkIdList *idList) const
//-----------------------------------------------------------------------------
{
  idList->Initialize() ;

  vtkIdList *cellList = vtkIdList::New() ;
  vtkIdList *ptsOnCell = vtkIdList::New() ;

  // get cells on point
  polydata->GetPointCells(ptId, cellList) ;

  for (int i = 0 ;  i < cellList->GetNumberOfIds() ;  i++){
    // get points on cell
    polydata->GetCellPoints(cellList->GetId(i), ptsOnCell) ;

    // copy points to output list
    for (int j = 0 ;  j < ptsOnCell->GetNumberOfIds() ;  j++){
      if (ptsOnCell->GetId(j) != ptId)
        idList->InsertUniqueId(ptsOnCell->GetId(j)) ;
    }
  }

  cellList->Delete() ;
  ptsOnCell->Delete() ;
}



//-----------------------------------------------------------------------------
// Get point neighbours of point which are joined by edges.
// If the cells are triangles, this is the same as GetPointsOnCellNeighbours().
void vtkMEDPolyDataNavigator::GetPointNeighboursOfPoint(vtkPolyData *polydata, int ptId, vtkIdList *idList) const
//-----------------------------------------------------------------------------
{
  idList->Initialize() ;

  vtkIdList *cellList = vtkIdList::New() ;
  vtkIdList *ptsOnCell = vtkIdList::New() ;

  // get cells on point
  polydata->GetPointCells(ptId, cellList) ;

  for (int i = 0 ;  i < cellList->GetNumberOfIds() ;  i++){
    // get adjacent points on cell
    int ptId0, ptId1 ;
    GetAdjacentPointsOnCell(polydata, cellList->GetId(i), ptId, ptId0, ptId1) ;

    // copy points to output list
    if ((ptId0 != -1) && (ptId0 != ptId))
      idList->InsertUniqueId(ptId0) ;
    if ((ptId1 != -1) && (ptId1 != ptId))
      idList->InsertUniqueId(ptId1) ;
  }

  cellList->Delete() ;
  ptsOnCell->Delete() ;
}



//------------------------------------------------------------------------------
// Get the edges of the cells around a point
void vtkMEDPolyDataNavigator::GetEdgesAroundPointInclusive(vtkPolyData *polydata, int ptId, EdgeVector& edges) const
//------------------------------------------------------------------------------
{
  edges.clear() ;

  vtkIdList *cellList = vtkIdList::New() ;
  EdgeVector edgesOnCellList ;

  polydata->GetPointCells(ptId, cellList) ;

  for (int i = 0 ;  i < cellList->GetNumberOfIds() ;  i++){
    edgesOnCellList.clear() ;
    GetCellEdges(polydata, cellList->GetId(i), edgesOnCellList) ;

    // add edges to output list
    for (int j = 0 ;  j < (int)edgesOnCellList.size() ;  j++)
      AddUniqueEdge(edgesOnCellList.at(j), edges) ;
  }

  cellList->Delete() ;
}



//------------------------------------------------------------------------------
// Get the edges of the cells around a point, excluding those which touch the point itself
void vtkMEDPolyDataNavigator::GetEdgesAroundPointExclusive(vtkPolyData *polydata, int ptId, EdgeVector& edges) const
//------------------------------------------------------------------------------
{
  edges.clear() ;

  vtkIdList *cellList = vtkIdList::New() ;
  EdgeVector edgesOnCellList ;

  polydata->GetPointCells(ptId, cellList) ;

  for (int i = 0 ;  i < cellList->GetNumberOfIds() ;  i++){
    edgesOnCellList.clear() ;
    GetCellEdges(polydata, cellList->GetId(i), edgesOnCellList) ;

    // add edges to output list, if they do not contain the input point
    for (int j = 0 ;  j < (int)edgesOnCellList.size() ;  j++){
      if (!(edgesOnCellList.at(j)).ContainsPoint(ptId))
        AddUniqueEdge(edgesOnCellList.at(j), edges) ;
    }
  }

  cellList->Delete() ;
}



//------------------------------------------------------------------------------
// Get the points in the cells around the edge, excluding those in the edge
void vtkMEDPolyDataNavigator::GetPointsAroundEdgeExclusive(vtkPolyData *polydata, const Edge& edge, vtkIdList *idList) const
//------------------------------------------------------------------------------
{
  idList->Initialize() ;

  vtkIdList *cellIds = vtkIdList::New() ;
  vtkIdList *ptsOnCell = vtkIdList::New() ;
  GetCellNeighboursOfEdge(polydata, edge, cellIds) ;

  for (int i = 0 ;  i < cellIds->GetNumberOfIds() ;  i++){
    polydata->GetCellPoints(cellIds->GetId(i), ptsOnCell) ;

    for (int j = 0 ;  j < ptsOnCell->GetNumberOfIds() ;  j++){
      int tryId = ptsOnCell->GetId(j) ;
      if (!edge.ContainsPoint(tryId))
        idList->InsertUniqueId(tryId) ;
    }
  }

  cellIds->Delete() ;
  ptsOnCell->Delete() ;

}




//------------------------------------------------------------------------------
// Get the edges on the cells on each side of the edge.
// This does not consider edges which are joined only through the end vertices.
void vtkMEDPolyDataNavigator::GetEdgesAroundEdge(vtkPolyData *polydata, const Edge& edge, EdgeVector& edges) const
//------------------------------------------------------------------------------
{
  vtkIdList *cellIds = vtkIdList::New() ;
  EdgeVector edgesOnCellList ;

  GetCellNeighboursOfEdge(polydata, edge, cellIds) ;

  for (int i = 0 ;  i < cellIds->GetNumberOfIds() ;  i++){
    edgesOnCellList.clear() ;
    GetCellEdges(polydata, cellIds->GetId(i), edgesOnCellList) ;

    // add edges to output list, if edge is not the same as the input edge
    for (int j = 0 ;  j < (int)edgesOnCellList.size() ;  j++){
      if (edgesOnCellList.at(j) != edge)
        AddUniqueEdge(edgesOnCellList.at(j), edges) ;
    }
  }

  cellIds->Delete() ;

}




//------------------------------------------------------------------------------
// Get list of all edges in polydata
void vtkMEDPolyDataNavigator::GetAllEdges(vtkPolyData *polydata, EdgeVector& edges) const
//------------------------------------------------------------------------------
{
  edges.clear() ;

  EdgeVector edgesOnCell ;

  // get edges in multimap form
  EdgeMultiMap edgeMap ;
  GetAllEdges(polydata, edgeMap) ;
  CopyEdgeMapToVector(edgeMap, edges) ;
}





//------------------------------------------------------------------------------
// Get list of all edges in polydata, in the form of a multimap for quick search
void vtkMEDPolyDataNavigator::GetAllEdges(vtkPolyData *polydata, EdgeMultiMap& edges) const
//------------------------------------------------------------------------------
{
  edges.clear() ;

  EdgeVector edgesOnCell ;

  for (int i = 0 ;  i < polydata->GetNumberOfCells() ;  i++){
    GetCellEdges(polydata, i, edgesOnCell) ;
    for (int j = 0 ;  j < (int)edgesOnCell.size() ;  j++)
      AddUniqueEdge(edgesOnCell.at(j), edges) ;
  }
}





//------------------------------------------------------------------------------
// Get list of all edges on list of cells
void vtkMEDPolyDataNavigator::GetEdgesOnListOfCells(vtkPolyData *polydata, vtkIdList *cellList, EdgeVector& edges) const
//------------------------------------------------------------------------------
{
  edges.clear() ;

  // get edges in multimap form and copy to vector
  EdgeMultiMap edgeMap ;
  GetEdgesOnListOfCells(polydata, cellList, edgeMap) ;
  CopyEdgeMapToVector(edgeMap, edges) ;
}





//------------------------------------------------------------------------------
// Get list of all edges on list of cells, in the form of a multimap for quick search
void vtkMEDPolyDataNavigator::GetEdgesOnListOfCells(vtkPolyData *polydata, vtkIdList *cellList, EdgeMultiMap& edges) const
//------------------------------------------------------------------------------
{
  edges.clear() ;

  EdgeVector edgesOnCell ;

  for (int i = 0 ;  i < cellList->GetNumberOfIds() ;  i++){
    int id = cellList->GetId(i) ;
    GetCellEdges(polydata, id, edgesOnCell) ;
    for (int j = 0 ;  j < (int)edgesOnCell.size() ;  j++)
      AddUniqueEdge(edgesOnCell.at(j), edges) ;
  }
}




//------------------------------------------------------------------------------
// Get list of all cells on list of edges
void vtkMEDPolyDataNavigator::GetCellsOnListOfEdges(vtkPolyData *polydata, EdgeVector& edgeList, vtkIdList* cellList) const
//------------------------------------------------------------------------------
{
  cellList->Initialize() ;

  // get cells in set form and copy to list
  IdSet cellSet ;
  GetCellsOnListOfEdges(polydata, edgeList, cellSet) ;
  CopyIdSetToList(cellSet, cellList) ;
}



//------------------------------------------------------------------------------
// Get list of all cells on list of edges, in the form of a set for quick search
void vtkMEDPolyDataNavigator::GetCellsOnListOfEdges(vtkPolyData *polydata, EdgeVector& edgelist, IdSet& cellSet) const
//------------------------------------------------------------------------------
{
  cellSet.clear() ;

  vtkIdList* cellNeighbours = vtkIdList::New() ;

  for (int i = 0 ;  i < (int)edgelist.size() ;  i++){
    Edge edge = edgelist.at(i) ;
    GetCellNeighboursOfEdge(polydata, edge, cellNeighbours) ;
    for (int j = 0 ;  j < cellNeighbours->GetNumberOfIds() ;  j++)
      AddUniqueIdToSet(cellSet, cellNeighbours->GetId(j)) ;
  }

  cellNeighbours->Delete() ;
}



//------------------------------------------------------------------------------
// Get the edge connected neighbours of a cell
void vtkMEDPolyDataNavigator::GetCellsOnCell_EdgeConnected(vtkPolyData *polydata, int cellId,  vtkIdList *idList) const
//------------------------------------------------------------------------------
{
  idList->Initialize() ;

  // get the edges on the cell
  EdgeVector edgesOnCell ;
  GetCellEdges(polydata, cellId, edgesOnCell) ;

  vtkIdList *cellsOnEdge = vtkIdList::New() ;

  for (int i = 0 ;  i < (int)edgesOnCell.size() ;  i++){
    Edge edge = edgesOnCell.at(i) ;
    GetCellNeighboursOfEdge(polydata, edge, cellsOnEdge) ;

    for (int j = 0 ;  j < cellsOnEdge->GetNumberOfIds() ;  j++){
      int id = cellsOnEdge->GetId(j) ;
      if (id != cellId)
        idList->InsertUniqueId(id) ;      
    }
  }

  cellsOnEdge->Delete() ;
}





//------------------------------------------------------------------------------
// Is point on a cell
bool vtkMEDPolyDataNavigator::IsPointOnCell(vtkPolyData *polydata, int cellId, int ptId) const
//------------------------------------------------------------------------------
{

  vtkIdList *idlist = vtkIdList::New() ;

  polydata->GetCellPoints(cellId, idlist) ;
  bool inlist = InList(ptId, idlist) ;

  idlist->Delete() ;
  return inlist ;
}



//------------------------------------------------------------------------------
// Is edge on a cell
bool vtkMEDPolyDataNavigator::IsEdgeOnCell(vtkPolyData *polydata, int cellId, const Edge& edge) const
//------------------------------------------------------------------------------
{
  EdgeVector cellEdges ;
  GetCellEdges(polydata, cellId, cellEdges) ;
  for (int i = 0 ;  i < (int)cellEdges.size() ;  i++){
    if (cellEdges.at(i) == edge)
      return true ;
  }

  return false ;
}



//------------------------------------------------------------------------------
// Find point on cell which is opposite to the edge.
// If cell is not a triangle, returns the first non-edge point found.
// Returns -1 if none found.
int vtkMEDPolyDataNavigator::GetPointOnCellOppositeToEdge(vtkPolyData *polydata, int cellId, const Edge& edge) const
//------------------------------------------------------------------------------
{
  int id = -1 ;

  vtkIdList *cellPts = vtkIdList::New() ;
  polydata->GetCellPoints(cellId, cellPts) ;

  for (int i = 0 ;  i < cellPts->GetNumberOfIds() && id != -1 ;  i++){
    if (!edge.ContainsPoint(cellPts->GetId(i)))
      id = cellPts->GetId(i) ;
  }

  cellPts->Delete() ;
  return id ;
}




//------------------------------------------------------------------------------
// Get cell which containing both edges
int vtkMEDPolyDataNavigator::GetCellWithTwoEdges(vtkPolyData *polydata, const Edge& edge0,  const Edge& edge1) const
//------------------------------------------------------------------------------
{
  int id ;

  vtkIdList *cellsOnEdge0 = vtkIdList::New() ;
  vtkIdList *cellsOnEdge1 = vtkIdList::New() ;
  vtkIdList *cellsOnBoth = vtkIdList::New() ;

  GetCellNeighboursOfEdge(polydata, edge0, cellsOnEdge0) ;
  GetCellNeighboursOfEdge(polydata, edge1, cellsOnEdge1) ;
  GetIdsInBothLists(cellsOnEdge0, cellsOnEdge1, cellsOnBoth) ;

  // There should only be one common cell.
  int n = cellsOnBoth->GetNumberOfIds() ;
  if (n == 1)
    id = cellsOnBoth->GetId(0) ;
  else
    id = -1 ;

  cellsOnEdge0->Delete() ;
  cellsOnEdge1->Delete() ;
  cellsOnBoth->Delete() ;

  return id ;
}



//------------------------------------------------------------------------------
// Get cell containing edge and point
int vtkMEDPolyDataNavigator::GetCellWithEdgeAndPoint(vtkPolyData *polydata, const Edge& edge, int ptId) const
//------------------------------------------------------------------------------
{
  int id ;

  vtkIdList *cellsOnEdge = vtkIdList::New() ;
  vtkIdList *cellsOnPoint = vtkIdList::New() ;
  vtkIdList *cellsOnBoth = vtkIdList::New() ;

  GetCellNeighboursOfEdge(polydata, edge, cellsOnEdge) ;
  polydata->GetPointCells(ptId, cellsOnPoint) ;
  GetIdsInBothLists(cellsOnEdge, cellsOnPoint, cellsOnBoth) ;

  // There should only be one common cell.
  int n = cellsOnBoth->GetNumberOfIds() ;
  if (n == 1)
    id = cellsOnBoth->GetId(0) ;
  else
    id = -1 ;

  cellsOnEdge->Delete() ;
  cellsOnPoint->Delete() ;
  cellsOnBoth->Delete() ;

  return id ;
}





//------------------------------------------------------------------------------
/// Get cell containing all three points
/// Returns -1 if other than one cell found
int vtkMEDPolyDataNavigator::GetCellWithThreePoints(vtkPolyData *polydata, int ptId0, int ptId1, int ptId2) const
//------------------------------------------------------------------------------
{
  int id ;

  vtkIdList *cellsOnPoint0 = vtkIdList::New() ;
  vtkIdList *cellsOnPoint1 = vtkIdList::New() ;
  vtkIdList *cellsOnPoint2 = vtkIdList::New() ;
  vtkIdList *cellsOnBoth01 = vtkIdList::New() ;
  vtkIdList *cellsOnAll = vtkIdList::New() ;

  polydata->GetPointCells(ptId0, cellsOnPoint0) ;
  polydata->GetPointCells(ptId1, cellsOnPoint1) ;
  polydata->GetPointCells(ptId2, cellsOnPoint2) ;

  GetIdsInBothLists(cellsOnPoint0, cellsOnPoint1, cellsOnBoth01) ;
  GetIdsInBothLists(cellsOnBoth01, cellsOnPoint2, cellsOnAll) ;

  int n = cellsOnAll->GetNumberOfIds() ;
  if (n == 1)
    id = cellsOnAll->GetId(0) ;
  else
    id = -1 ;

  cellsOnPoint0->Delete() ;
  cellsOnPoint1->Delete() ;
  cellsOnPoint2->Delete() ;
  cellsOnBoth01->Delete() ;
  cellsOnAll->Delete() ;

  return id ;
}




//------------------------------------------------------------------------------
// List cells in polydata with a given number of points
void vtkMEDPolyDataNavigator::GetCellsWithNumberOfPoints(vtkPolyData *polydata,  int n,  vtkIdList *cellIds) const
//------------------------------------------------------------------------------
{
  cellIds->Initialize() ;

  vtkIdList *idsOnCell = vtkIdList::New() ;

  for (int i = 0 ;  i < polydata->GetNumberOfCells() ;  i++){
    polydata->GetCellPoints(i, idsOnCell) ;
    if (idsOnCell->GetNumberOfIds() == n)
      cellIds->InsertNextId(i) ;
  }

  idsOnCell->Delete() ;
}




//------------------------------------------------------------------------------
// Get center of cell
void vtkMEDPolyDataNavigator::GetCenterOfCell(vtkPolyData *polydata, int cellId, double x[3]) const
//------------------------------------------------------------------------------
{
  vtkIdList *idsOnCell = vtkIdList::New() ;
  polydata->GetCellPoints(cellId, idsOnCell) ;

  for (int j = 0 ;  j < 3 ;  j++)
    x[j] = 0.0 ;

  for (int i = 0 ;  i < idsOnCell->GetNumberOfIds() ;  i++){
    int ptId = idsOnCell->GetId(i) ;
    double *xpt = polydata->GetPoint(ptId) ;
    for (int j = 0 ;  j < 3 ;  j++)
      x[j] += xpt[j] ;
  }

  for (int j = 0 ;  j < 3 ;  j++)
    x[j] /= (double)(idsOnCell->GetNumberOfIds()) ;

  idsOnCell->Delete() ;
}




//------------------------------------------------------------------------------
// Get points on cell which are colinear with the sides of the shape,
// ie the points which are on the straight sides, but not the corners.
// Useful for triangulating a cell.
// tol is the tolerance for colinearity in degrees.
// NB The output is the indices of the points on the cell, not the polydata id's.
void vtkMEDPolyDataNavigator::GetPointsOnStraightSidesOfCell(vtkPolyData *polydata, int cellId, vtkIdList *ptIdsOnCell, double tol) const
//------------------------------------------------------------------------------
{
  double tolRad = 3.14159/180.0 * tol ;

  // list all the points
  vtkIdList *allIds = vtkIdList::New() ;
  polydata->GetCellPoints(cellId, allIds) ;

  double xa[3], xb[3], xc[3] ;

  // Test each point for colinearity with the adjacent points
  // and copy the colinear ones to the output list
  int n = allIds->GetNumberOfIds() ;
  ptIdsOnCell->Initialize() ;

  for (int ib = 0 ;  ib < n ;  ib++){
    int ia = Modulo(ib-1, n) ;
    int ic = Modulo(ib+1, n) ;

    int ida = allIds->GetId(ia) ;
    int idb = allIds->GetId(ib) ;
    int idc = allIds->GetId(ic) ;

    polydata->GetPoint(ida, xa) ;
    polydata->GetPoint(idb, xb) ;
    polydata->GetPoint(idc, xc) ;

    // calculate cos(theta) = (b-a).(b-c) / |b-a| |b-c|
    double ba[3] ;
    double bc[3] ;
    for (int j = 0 ;  j < 3 ;  j++){
      ba[j] = xb[j] - xa[j] ;
      bc[j] = xb[j] - xc[j] ;
    }
    double dot = vtkMath::Dot(ba,bc) ;
    double modba = vtkMath::Norm(ba) ;
    double modbc = vtkMath::Norm(bc) ;
    double costheta = dot/(modba*modbc) ;

    // correct for any rounding errors
    if (costheta < -1.0)
      costheta = -1.0 ;
    if (costheta > 1.0)
      costheta = 1.0 ;

    double sintheta = sqrt(1.0-costheta*costheta) ;

    bool colin = (sintheta <= tolRad) ; 

    if (colin)
      ptIdsOnCell->InsertNextId(ib) ;
  }
}




//------------------------------------------------------------------------------
// Get points with a given scalar value. \n
// Useful for listing labelled cells.
void vtkMEDPolyDataNavigator::GetPointsWithScalarValue(vtkPolyData *polydata, char* scalarName, int component, double value, vtkIdList *pts)
//------------------------------------------------------------------------------
{
  pts->Initialize() ;

  // find array with given name
  vtkDataArray *array = polydata->GetPointData()->GetScalars(scalarName) ;
  if (array == NULL)
    return ;

  for (int i = 0 ;  i < array->GetNumberOfTuples() ;  i++){
    double thisValue = array->GetComponent(i, component) ;
    if (thisValue == value)
      pts->InsertNextId(i) ;
  }
}




//------------------------------------------------------------------------------
// PrintCell.
// NB Needs BuildCells().
void vtkMEDPolyDataNavigator::PrintCell(vtkPolyData *polydata, int cellId, ostream& os) const
//------------------------------------------------------------------------------
{
  os << "cell " << cellId << ": " ;

  if (polydata->GetCellType(cellId) == VTK_EMPTY_CELL){
    os << "VTK_EMPTY_CELL" << std::endl ;
    return ;
  }

  vtkIdList *ptsOnCell = vtkIdList::New() ;
  polydata->GetCellPoints(cellId, ptsOnCell) ; // Needs BuildCells() to be valid


  int n = ptsOnCell->GetNumberOfIds() ;
  if (n == 0 ){
    os << "empty" << std::endl ;
    return ;
  }
  else{
    for (int i = 0 ;  i < n ;  i++){
      int ptId = ptsOnCell->GetId(i) ;
      os << ptId << " " ;
    }
    for (int i = 0 ;  i < n ;  i++){
      int ptId = ptsOnCell->GetId(i) ;
      double x[3] ;
      polydata->GetPoint(ptId, x) ;
      os << std::setprecision(3) << "(" << x[0] << "," << x[1] << "," << x[2] << ")" << " " ;
    }
  }

  os << std::endl ;

  ptsOnCell->Delete() ;
}




//------------------------------------------------------------------------------
// Print all cells in polydata.
void vtkMEDPolyDataNavigator::PrintCells(vtkPolyData *polydata, ostream& os)  const
//------------------------------------------------------------------------------
{
  os << "number of cells = " << polydata->GetNumberOfCells() << std::endl ;
  for (int i = 0 ;  i < polydata->GetNumberOfCells() ;  i++)
    PrintCell(polydata, i, os) ;
  os << std::endl ;
}




//------------------------------------------------------------------------------
// Print attribute structure of polydata
void vtkMEDPolyDataNavigator::PrintAttributeData(vtkPolyData *polydata, ostream& os,  bool printTuples)  const
//------------------------------------------------------------------------------
{
  //----------------------------------------------------------------------------
  // Point data
  //----------------------------------------------------------------------------
  vtkPointData *PD = polydata->GetPointData() ;
  int napd = PD->GetNumberOfArrays() ;
  os << "Point data" << std::endl ;
  os << "no. of arrays = " << napd << std::endl ;

  for (int i = 0 ;  i < napd ;  i++){
    os << "  array " << i << std::endl ;

    vtkDataArray *DA = PD->GetArray(i) ;

    const char *name = DA->GetName() ;
    if (name != NULL)
      os << "  name = " << name << std::endl ;

    int nc = DA->GetNumberOfComponents() ;
    os << "  no. of components = " << nc << std::endl ;

    int dtype = DA->GetDataType() ;
    os << "  data type = " << vtkImageScalarTypeNameMacro(dtype) << std::endl ;

    int ntup = DA->GetNumberOfTuples() ;
    os << "  no. of tuples = " << ntup << std::endl ;

    double range[2] ;
    PD->GetArray(i)->GetRange(range) ;
    os << "  range = " << range[0] << " " << range[1] << std::endl ;

    os << std::endl ;
  }


  //----------------------------------------------------------------------------
  // Cell data
  //----------------------------------------------------------------------------
  vtkCellData *CD = polydata->GetCellData() ;
  int nacd = CD->GetNumberOfArrays() ;
  os << "Cell data" << std::endl ;
  os << "no. of arrays = " << nacd << std::endl ;

  for (int i = 0 ;  i < nacd ;  i++){
    os << "  array " << i << std::endl ;

    vtkDataArray *DA = CD->GetArray(i) ;

    const char *name = DA->GetName() ;
    if (name != NULL)
      os << "  name = " << name << std::endl ;

    int nc = DA->GetNumberOfComponents() ;
    os << "  no. of components = " << nc << std::endl ;

    int dtype = DA->GetDataType() ;
    os << "  data type = " << vtkImageScalarTypeNameMacro(dtype) << std::endl ;

    int ntup = DA->GetNumberOfTuples() ;
    os << "  no. of tuples = " << ntup << std::endl ;

    double range[2] ;
    DA->GetRange(range) ;
    os << "  range = " << range[0] << " " << range[1] << std::endl ;

    os << std::endl ;
  }

  os << std::endl ;


  //----------------------------------------------------------------------------
  // Point and cell attributes
  //----------------------------------------------------------------------------

  if (printTuples){
    // Point attributes
    for (int i = 0 ;  i < napd ;  i++){
      os << "point data array " << i << std::endl ;

      vtkDataArray *DA = PD->GetArray(i) ;

      int nc = DA->GetNumberOfComponents() ;

      for (int j = 0 ;  j < DA->GetNumberOfTuples() ;  j++){
        double tuple[9] ;
        DA->GetTuple(j, tuple) ;
        os << j << ": " ;
        for (int k = 0 ;  k < nc ;  k++)
          os << tuple[k] << " " ;
        os << std::endl ;
      }
      os << std::endl ;
    }

    // Cell attributes
    for (int i = 0 ;  i < nacd ;  i++){
      os << "cell data array " << i << std::endl ;

      vtkDataArray *DA = CD->GetArray(i) ;

      int nc = DA->GetNumberOfComponents() ;

      for (int j = 0 ;  j < DA->GetNumberOfTuples() ;  j++){
        double tuple[9] ;
        DA->GetTuple(j, tuple) ;
        os << j << ": " ;
        for (int k = 0 ;  k < nc ;  k++)
          os << tuple[k] << " " ;
        os << std::endl ;
      }
      os << std::endl ;
    }
    os << std::endl ;
  }
}





//------------------------------------------------------------------------------
// Copy point data to the end of the attribute list,
// increasing the no. of tuples by one.
// The attributes are copied from the point id0.
void vtkMEDPolyDataNavigator::CopyPointData(vtkPolyData *polydata,  int id0) const
//------------------------------------------------------------------------------
{
  vtkPointData *pd = polydata->GetPointData() ;
  int numberOfPtArrays = pd->GetNumberOfArrays() ;

  // copy data to the new tuple
  for (int i = 0 ;  i < numberOfPtArrays ;  i++){
    vtkDataArray *da = pd->GetArray(i) ;
    int numberOfComponents = da->GetNumberOfComponents() ;

    double tuple[9] ;
    for (int j = 0 ;  j < numberOfComponents ;  j++)
      tuple[j] = da->GetComponent(id0, j) ;

    da->InsertNextTuple(tuple) ;
  }
}


//------------------------------------------------------------------------------
// Copy point data to the end of the attribute list,
// increasing the no. of tuples by one.
// The attributes are interpolated between the points id0 and id1.
void vtkMEDPolyDataNavigator::CopyPointData(vtkPolyData *polydata,  int id0, int id1, double lambda) const
//------------------------------------------------------------------------------
{
  vtkPointData *pd = polydata->GetPointData() ;
  int numberOfPtArrays = pd->GetNumberOfArrays() ;

  // copy data to the new tuple
  for (int i = 0 ;  i < numberOfPtArrays ;  i++){
    vtkDataArray *da = pd->GetArray(i) ;
    int numberOfComponents = da->GetNumberOfComponents() ;

    double tuple[9] ;
    for (int j = 0 ;  j < numberOfComponents ;  j++){
      double c0 = da->GetComponent(id0, j) ;
      double c1 = da->GetComponent(id1, j) ;
      tuple[j] = (1.0-lambda)*c0 + lambda*c1 ;
    }

    da->InsertNextTuple(tuple) ;
  }
}




//------------------------------------------------------------------------------
// Copy cell data to the end of the attribute list,
// increasing the no. of tuples by one.
void vtkMEDPolyDataNavigator::CopyCellData(vtkPolyData *polydata,  int cellId) const
//------------------------------------------------------------------------------
{
  vtkCellData *cd = polydata->GetCellData() ;
  int numberOfCellArrays = cd->GetNumberOfArrays() ;

  // copy data to the new tuple
  for (int i = 0 ;  i < numberOfCellArrays ;  i++){
    vtkDataArray *da = cd->GetArray(i) ;
    int numberOfComponents = da->GetNumberOfComponents() ;
    double tuple[9] ;
    for (int j = 0 ;  j < numberOfComponents ;  j++)
      tuple[j] = da->GetComponent(cellId, j) ;

    da->InsertNextTuple(tuple) ;
  }
}





//------------------------------------------------------------------------------
// Delete tuples from cell attributes.
void vtkMEDPolyDataNavigator::DeleteCellTuples(vtkPolyData *polydata, vtkIdList *cellIds)  const
//------------------------------------------------------------------------------
{
  // copy cell data to new array and empty it of existing tuples.
  vtkCellData *newCellData = vtkCellData::New() ;
  newCellData->DeepCopy(polydata->GetCellData()) ;
  newCellData->SetNumberOfTuples(0) ;

  vtkCellData *oldCellData = polydata->GetCellData() ;
  int numberOfCellArrays = oldCellData->GetNumberOfArrays() ;

  // copy all tuples to new list which are not in the list of cells
  for (int j = 0 ;  j < oldCellData->GetNumberOfTuples() ;  j++){
    if (this->NotInList(j, cellIds)){

      for (int i = 0 ;  i < numberOfCellArrays ;  i++){
        vtkDataArray *daOld = oldCellData->GetArray(i) ;
        vtkDataArray *daNew = newCellData->GetArray(i) ;

        int numberOfComponents = daOld->GetNumberOfComponents() ;
        double tuple[9] ;
        for (int k = 0 ;  k < numberOfComponents ;  k++)
          tuple[k] = daOld->GetComponent(j, k) ;

        daNew->InsertNextTuple(tuple) ;
      }
    }
  }

  oldCellData->DeepCopy(newCellData) ;
}



//------------------------------------------------------------------------------
// Set cell to degenerate empty cell
void vtkMEDPolyDataNavigator::SetCellToEmpty(vtkPolyData *polydata, int cellId) const
//------------------------------------------------------------------------------
{
  polydata->ReplaceCell(cellId, 0, NULL) ;
}




//------------------------------------------------------------------------------
// Delete list of cells. \n
// cf polydata->DeleteCell(), which labels the cell, but does not actually remove it.
/// The corresponding attribute data is also deleted.
void vtkMEDPolyDataNavigator::DeleteCells(vtkPolyData *polydata, vtkIdList *idList) const
//------------------------------------------------------------------------------
{
  // This needs valid links, and doesn't build them itself.
  polydata->BuildCells() ;
  polydata->BuildLinks() ;


  //----------------------------------------------------------------------------
  // First "delete" the cells
  // This marks cells as VTK_EMPTY_CELL
  //----------------------------------------------------------------------------
  for (int i = 0 ;  i < idList->GetNumberOfIds() ;  i++){
    int id = idList->GetId(i) ;
    polydata->DeleteCell(id) ;
  }


  //----------------------------------------------------------------------------
  // Copy the unmarked cells from the polydata to a new cell array
  //----------------------------------------------------------------------------
  vtkCellArray *copyCells = vtkCellArray::New() ;
  vtkIdList *tmp = vtkIdList::New() ;

  for (int i = 0 ;  i < polydata->GetNumberOfCells() ;  i++){
    if (polydata->GetCellType(i) != VTK_EMPTY_CELL){
      polydata->GetCellPoints(i, tmp) ;
      copyCells->InsertNextCell(tmp) ;   
    }
  }

  tmp->Delete() ;


  //----------------------------------------------------------------------------
  // Replace the polydata cells with the new array
  //----------------------------------------------------------------------------
  polydata->SetPolys(copyCells) ;
  copyCells->Delete() ;


  //----------------------------------------------------------------------------
  // Delete the attributes of the deleted cells
  //----------------------------------------------------------------------------
  DeleteCellTuples(polydata, idList) ;


  //----------------------------------------------------------------------------
  // Delete the invalid links and cells
  //----------------------------------------------------------------------------
  polydata->DeleteCells() ;
  polydata->DeleteLinks() ;
}





//------------------------------------------------------------------------------
// Create a new point, adding tuples to the attribute data if necessary.
// The position and scalars are copied from id0.
// Returns the id of the created point.
// This only creates a point - it does not add it to a cell.
// This can be called successively, but you must delete or rebuild the cells and links when finished.
int vtkMEDPolyDataNavigator::CreateNewPoint(vtkPolyData *polydata,  int id0) const
//------------------------------------------------------------------------------
{
  // add new point
  double x0[3] ;
  polydata->GetPoint(id0, x0) ;

  int idnew = polydata->GetPoints()->InsertNextPoint(x0) ;


  // add new attributes
  CopyPointData(polydata, id0) ;


  return idnew ;
}



//------------------------------------------------------------------------------
// Create a new point, adding tuples to the attribute data if necessary.
// The position and scalars are interpolated between id0 and id1.
// Returns the id of the created point.
// Lambda is the interpolation weight, where 0 <= lambda <= 1.
// This only creates a point - it does not add it to a cell.
// This can be called successively, but you must delete or rebuild the cells and links when finished.
int vtkMEDPolyDataNavigator::CreateNewPoint(vtkPolyData *polydata,  int id0, int id1, double lambda) const
//------------------------------------------------------------------------------
{
  // add new point
  double x0[3], x1[3], x[3] ;
  polydata->GetPoint(id0, x0) ;
  polydata->GetPoint(id1, x1) ;
  for (int j = 0 ;  j < 3 ;  j++)
    x[j] = (1.0-lambda)*x0[j] + lambda*x1[j] ;

  int idnew = polydata->GetPoints()->InsertNextPoint(x) ;


  // add new attributes
  CopyPointData(polydata, id0, id1, lambda) ;


  return idnew ;
}




//------------------------------------------------------------------------------
// Create a new cell, adding tuples to the attribute data if necessary.
/// The attribute data is copied from cellId.
// Returns the id of the created cell.
// This can be called successively, but you must delete or rebuild the cells and links when finished.
int vtkMEDPolyDataNavigator::CreateNewCell(vtkPolyData *polydata,  int copyScalarsCellId,  vtkIdList *ids) const
//------------------------------------------------------------------------------
{
  int idnew = polydata->GetPolys()->InsertNextCell(ids) ; // insert new cell
  CopyCellData(polydata, copyScalarsCellId) ;             // copy attribute data

  return idnew ;
}




//------------------------------------------------------------------------------
/// Copy cells to the end of the polydata. \n
/// The cell attributes are also copied.
void vtkMEDPolyDataNavigator::CopyCells(vtkPolyData *polydata,  vtkIdList *cellIds,  vtkIdList *newCellIds)  const
//------------------------------------------------------------------------------
{
  // create array of id lists to store the point ids
  int n = cellIds->GetNumberOfIds() ;
  vtkIdList** ptIds = new  vtkIdList * [n] ;
  for (int i = 0 ;  i < n ;  i++)
    ptIds[i] = vtkIdList::New() ;

  // Get the list of id's for each cell
  // Note that we finish reading the polydata before we change it.
  for (int i = 0 ;  i < n ;  i++){
    int id = cellIds->GetId(i) ;
    polydata->GetCellPoints(id, ptIds[i]) ;
  }

  // copy each cell to a new cell at the end
  newCellIds->Initialize() ;
  for (int i = 0 ;  i < n ;  i++){
    int id = cellIds->GetId(i) ;
    int idnew = CreateNewCell(polydata, id, ptIds[i]) ;
    newCellIds->InsertNextId(idnew) ;
  }

  // free allocated memory
  for (int i = 0 ;  i < n ;  i++)
    ptIds[i]->Delete() ;
  delete [] ptIds ;

  // delete invalid links and cells
  polydata->DeleteCells() ;
  polydata->DeleteLinks() ;
}





//------------------------------------------------------------------------------
// Add new points to edges.
// This creates a new point on the midpoint of each edge.  
// The id's of the new points are returned in newPtIds.
// All cells on each edge will gain the new point, so triangles will become quads.
void vtkMEDPolyDataNavigator::AddPointsToEdges(vtkPolyData *polydata, EdgeVector edges, vtkIdList *newPtIds)  const
//------------------------------------------------------------------------------
{
  //----------------------------------------------------------------------------
  // Get all the cells which use the edges
  // and list the id's of the cells as editable vtkIdList's 
  //----------------------------------------------------------------------------
  vtkIdList *allCellsOnEdges = vtkIdList::New() ;
  GetCellsOnListOfEdges(polydata, edges, allCellsOnEdges) ;

  int ncells = allCellsOnEdges->GetNumberOfIds() ;
  vtkIdList **idsOfPtsOnCell = new (vtkIdList * [ncells]) ;
  for (int i = 0 ;  i < ncells ; i++)
    idsOfPtsOnCell[i] = vtkIdList::New() ;

  for (int i = 0 ;  i < ncells ; i++)
    polydata->GetCellPoints(allCellsOnEdges->GetId(i), idsOfPtsOnCell[i]) ;



  //----------------------------------------------------------------------------
  // Create new points on the edges
  //----------------------------------------------------------------------------
  newPtIds->Initialize() ;
  int nedges = (int)edges.size() ;
  for (int i = 0 ;  i < nedges ;  i++){
    int id0 = edges.at(i).GetId0() ;
    int id1 = edges.at(i).GetId1() ;
    int idNew = CreateNewPoint(polydata, id0, id1, 0.5) ;
    newPtIds->InsertNextId(idNew) ;
  }


  //----------------------------------------------------------------------------
  // For each edge, add the new point id to the editable cells.
  //----------------------------------------------------------------------------
  vtkIdList *cellsOnThisEdge = vtkIdList::New() ;
  for (int i = 0 ;  i < nedges ;  i++){
    GetCellNeighboursOfEdge(polydata, edges.at(i), cellsOnThisEdge) ;

    int id0 = edges.at(i).GetId0() ;
    int id1 = edges.at(i).GetId1() ;
    int idnew = newPtIds->GetId(i) ;

    for (int j = 0 ;  j < cellsOnThisEdge->GetNumberOfIds() ;  j++){
      int cellId = cellsOnThisEdge->GetId(j) ;

      // find the array id of this cell id
      bool found = false ;
      int k = -1 ;
      for (int ii = 0 ;  ii < ncells && !found ;  ii++){
        int thisId = allCellsOnEdges->GetId(ii) ;
        if (thisId == cellId){
          k = ii ;
          found = true ;
        }
      }
      assert(found) ;

      this->InsertIdBetweenIds(idsOfPtsOnCell[k], id0, id1, idnew) ;
    }
  }
  cellsOnThisEdge->Delete() ;




  //----------------------------------------------------------------------------
  // Copy the modified editable cells to new polydata cells
  // and delete original cells
  //----------------------------------------------------------------------------
  for (int i = 0 ;  i < ncells ; i++)
    CreateNewCell(polydata, allCellsOnEdges->GetId(i), idsOfPtsOnCell[i]) ;
  
  this->DeleteCells(polydata, allCellsOnEdges) ;



  // Free allocated memory
  allCellsOnEdges->Delete() ;
  for (int i = 0 ;  i < ncells ; i++)
    idsOfPtsOnCell[i]->Delete() ;
  delete [] idsOfPtsOnCell ;


  // rebuild the links because we have changed the polydata
  polydata->BuildCells() ;
  polydata->BuildLinks() ;

}





//------------------------------------------------------------------------------
// Change a point id in a cell.
// This searches for idold in the cell and replaces it with idnew.
// This can (probably) be called successively, but you must delete or rebuild the cells and links when finished.
// NB The methods ReplaceCellPoint() and ReplaceLinkedCell() don't seem to work.
void vtkMEDPolyDataNavigator::ChangePointIdInCell(vtkPolyData *polydata, int cellId,  int idold, int idnew) const
//------------------------------------------------------------------------------
{
  vtkCell *cell = polydata->GetCell(cellId) ; // probably very inefficient !
  int n = cell->GetNumberOfPoints() ;

  int *ptIds = new int[n] ;
  for (int i = 0 ;  i < n ;  i++){
    int id = cell->GetPointId(i) ;
    if (id == idold)
      ptIds[i] = idnew ;
    else
      ptIds[i] = id ;
  }

  polydata->ReplaceCell(cellId, n, ptIds) ;

  delete [] ptIds ;
}




//------------------------------------------------------------------------------
// Split cells.
// The edges are the point-pairs which must be connected across each cell.
// The cells must be quads or greater.
void vtkMEDPolyDataNavigator::SplitCells(vtkPolyData *polydata,  vtkIdList *cellIds,  EdgeVector edges) const
//------------------------------------------------------------------------------
{
  vtkIdList *cellPts = vtkIdList::New() ;
  vtkIdList *cellPts0 = vtkIdList::New() ;
  vtkIdList *cellPts1 = vtkIdList::New() ;

  for (int i = 0 ;  i < cellIds->GetNumberOfIds() ;  i++){
    // get the points on the cell
    polydata->GetCellPoints(cellIds->GetId(i), cellPts) ;

    int n = cellPts->GetNumberOfIds() ;
    assert (n >=  4) ;

    // locate the positions of the ends of the split on the cell
    int id0 = edges.at(i).GetId0() ;
    int id1 = edges.at(i).GetId1() ;
    int ii0 = this->FindIdInList(cellPts, id0) ;
    int ii1 = this->FindIdInList(cellPts, id1) ;

    // split the cell into two lists of points
    this->GetPartOfIdList(cellPts, cellPts0, ii0, ii1, true) ;
    this->GetPartOfIdList(cellPts, cellPts1, ii1, ii0, true) ;

    // create two new cells from the lists
    CreateNewCell(polydata, cellIds->GetId(i), cellPts0) ;
    CreateNewCell(polydata, cellIds->GetId(i), cellPts1) ; 
  }

  // delete the invalid links
  polydata->DeleteCells() ;
  polydata->DeleteLinks() ;

  // delete the original cells
  this->DeleteCells(polydata, cellIds) ;

  // free allocated memory
  cellPts->Delete() ;
  cellPts0->Delete() ;
  cellPts1->Delete() ;

}



//------------------------------------------------------------------------------
// Split quads into triangles.
// Each cell is split along a new edge from the given point to the opposite point.
// Quads will be split into two triangles.
// Cells greater than quads will be split in half.
// The cells must be quads or greater.
void vtkMEDPolyDataNavigator::SplitCells(vtkPolyData *polydata,  vtkIdList *cellIds,  vtkIdList *ptIds) const
//------------------------------------------------------------------------------
{
  EdgeVector edges ;

  vtkIdList *cellPts = vtkIdList::New() ;

  // calculate the edges for splitting each cell
  for (int i = 0 ;  i < cellIds->GetNumberOfIds() ;  i++){
    // get the points on the cell
    polydata->GetCellPoints(cellIds->GetId(i), cellPts) ;

    int n = cellPts->GetNumberOfIds() ;
    assert (n >=  4) ;

    // create edge from given point to opposite point
    int id0 = ptIds->GetId(i) ;                   // id0 is given point
    int ii0 = this->FindIdInList(cellPts, id0) ;  // index of id0 in cell
    int ii1 = (ii0 + n/2) % n ;                   // index of opposite point ii1 = ii0+n/2
    int id1 = cellPts->GetId(ii1) ;               // id1 is id of opposite point

    edges.push_back(Edge(id0,id1)) ;
  }

  // split all the cells along the edges
  SplitCells(polydata, cellIds, edges) ;

  // free allocated memory
  cellPts->Delete() ;
}




//------------------------------------------------------------------------------
// Subdivide triangular cells. \n
// Each listed cell is split into 4, \n
// and the neighbours of the listed cells are triangulated as required.
// The list of input cellId's is overwritten by the list of new cells.
//------------------------------------------------------------------------------
void vtkMEDPolyDataNavigator::SubdivideCells(vtkPolyData *polydata,  vtkIdList *cellIds) const
{
  //----------------------------------------------------------------------------
  // get all the edges and add points to them
  //----------------------------------------------------------------------------
  EdgeVector edges ;
  vtkIdList *newPtIds = vtkIdList::New() ;
  GetEdgesOnListOfCells(polydata, cellIds, edges) ;
  AddPointsToEdges(polydata, edges, newPtIds) ;

  // delete the invalid links
  polydata->DeleteCells() ;
  polydata->DeleteLinks() ;

  newPtIds->Delete() ;



  //----------------------------------------------------------------------------
  // relocate the modified cells
  //----------------------------------------------------------------------------
  cellIds->Initialize() ;
  for (int i = 0 ;  i < polydata->GetNumberOfCells() ;  i++){
    if (GetNumberOfPointsOnCell(polydata, i) > 3)
      cellIds->InsertNextId(i) ;
  }



  //----------------------------------------------------------------------------
  // Subdivide every cell in the list
  //----------------------------------------------------------------------------
  vtkIdList *allPtsOnCell = vtkIdList::New() ;
  vtkIdList *sidePtsOnCell = vtkIdList::New() ;

  // allocate output lists for modified cells
  vtkIdList **newCells = new (vtkIdList * [4]) ;
  for (int i = 0 ;  i < 4 ;  i++)
    newCells[i] = vtkIdList::New() ;

  vtkIdList *subvidedCells = vtkIdList::New() ;
  vtkIdList *idsOfNewCells = vtkIdList::New() ;
 
  for (int i = 0 ;  i < cellIds->GetNumberOfIds() ;  i++){
    int cellId = cellIds->GetId(i) ;

    // get points on cell
    polydata->GetCellPoints(cellId, allPtsOnCell) ;

    // Get the list of points added to the sides of the original triangle
    // (These points are indices on the cell, not polydata id's)
    sidePtsOnCell->Initialize() ;
    GetPointsOnStraightSidesOfCell(polydata, cellId, sidePtsOnCell, 10.0) ;

    int nPts = allPtsOnCell->GetNumberOfIds() ;
    int nSidePts = sidePtsOnCell->GetNumberOfIds() ;

    int numberOfNewCells = 0 ;

    if ((nPts == 6) && (nSidePts == 3)){
      // split the cell into 4 triangles
      // if the first side point is a, then the triangles are:
      // a + (0,1,2), (2,3,4), (0,4,5), (0,2,4), converted of course to polydata id's
      //
      //        a+1
      //         /\
      //        /  \
      //      a+2 - a
      //      / \  / \
      //     /   \/   \
      //    a+3--a+4--a+5
      //

      int a = sidePtsOnCell->GetId(0) ;
      int id0 = allPtsOnCell->GetId(a) ;
      int id1 = allPtsOnCell->GetId((a+1) % nPts) ;
      int id2 = allPtsOnCell->GetId((a+2) % nPts) ;
      int id3 = allPtsOnCell->GetId((a+3) % nPts) ;
      int id4 = allPtsOnCell->GetId((a+4) % nPts) ;
      int id5 = allPtsOnCell->GetId((a+5) % nPts) ;

      numberOfNewCells = 4 ;
      InitListToTriangle(newCells[0], id0, id1, id2) ;
      InitListToTriangle(newCells[1], id2, id3, id4) ;
      InitListToTriangle(newCells[2], id0, id4, id5) ;
      InitListToTriangle(newCells[3], id0, id2, id4) ; 

      subvidedCells->InsertNextId(cellId) ;
    }
    else if ((nPts == 5) && (nSidePts == 2)){
      // split the cell into 3 triangles
      // if the first side points is a, then the triangles are:
      // a + (0,1,2), (0,2,3), (0,3,4)
      //
      //        a+1                       a+1
      //         /\                       /\
      //        /  \                     /  \ 
      //      a+2 - a                   /    a 
      //      /    / \                 /    /|\
      //     /   /    \               /   /  | \
      //    / /        \             / /     |  \
      //  a+3----------a+4         a+2-----a+3--a+4
      //

      int a = sidePtsOnCell->GetId(0) ;
      int id0 = allPtsOnCell->GetId(a) ;
      int id1 = allPtsOnCell->GetId((a+1) % nPts) ;
      int id2 = allPtsOnCell->GetId((a+2) % nPts) ;
      int id3 = allPtsOnCell->GetId((a+3) % nPts) ;
      int id4 = allPtsOnCell->GetId((a+4) % nPts) ;

      numberOfNewCells = 3 ;
      InitListToTriangle(newCells[0], id0, id1, id2) ;
      InitListToTriangle(newCells[1], id0, id2, id3) ;
      InitListToTriangle(newCells[2], id0, id3, id4) ;

      subvidedCells->InsertNextId(cellId) ;
    }
    else if ((nPts == 4) && (nSidePts == 1)){
      // split the cell into 2 triangles
      // if the first side point is a, then the triangles are:
      // a + (0,1,2), (0,2,3)
      //
      //        a+1
      //         /\
      //        /  \
      //       /    a
      //      /    / \
      //     /   /    \
      //    / /        \
      //  a+2----------a+3
      //

      int a = sidePtsOnCell->GetId(0) ;
      int id0 = allPtsOnCell->GetId(a) ;
      int id1 = allPtsOnCell->GetId((a+1) % nPts) ;
      int id2 = allPtsOnCell->GetId((a+2) % nPts) ;
      int id3 = allPtsOnCell->GetId((a+3) % nPts) ;

      numberOfNewCells = 2 ;
      InitListToTriangle(newCells[0], id0, id1, id2) ;
      InitListToTriangle(newCells[1], id0, id2, id3) ;

      subvidedCells->InsertNextId(cellId) ;
    }
    else{
      // This cell was not subdivided
    }

    // create new cells from the lists
    for (int k = 0 ;  k < numberOfNewCells ;  k++){
      int id = CreateNewCell(polydata, cellId, newCells[k]) ;
      idsOfNewCells->InsertNextId(id) ; // note the id of each new cell
    }
  }


  //----------------------------------------------------------------------------
  // Tidy up
  //----------------------------------------------------------------------------
  // Delete the original cells which got subdivided
  int numberOfDeletedCells = subvidedCells->GetNumberOfIds() ;
  DeleteCells(polydata, subvidedCells) ;

  // Overwrite the input list with the id's of the new cells
  cellIds->Initialize() ;
  for (int i = 0 ;  i < idsOfNewCells->GetNumberOfIds() ;  i++){
    int id = idsOfNewCells->GetId(i) - numberOfDeletedCells ;
    cellIds->InsertNextId(id) ;
  }

  // delete the invalid links
  polydata->DeleteCells() ;
  polydata->DeleteLinks() ;


  // free memory
  allPtsOnCell->Delete() ;
  sidePtsOnCell->Delete() ;

  for (int i = 0 ;  i < 4 ;  i++)
    newCells[i]->Delete() ;
  delete [] newCells ;

  subvidedCells->Delete() ;
  idsOfNewCells->Delete() ;
}




//------------------------------------------------------------------------------
// Merge points.
// The points in the input lists are merged and assigned the new ids in the output list.
// Lambda controls the interpolation:
//  0 or 1 sets the new point equal to one or other of the input points.
//  0.5 sets the new point to half-way between the two.
//------------------------------------------------------------------------------
void vtkMEDPolyDataNavigator::MergePoints(vtkPolyData *polydata, vtkIdList *idsIn0,  vtkIdList *idsIn1,  
                                           vtkIdList *idsOut,  double lambda) const
{
  //----------------------------------------------------------------------------
  // Check the input data
  //----------------------------------------------------------------------------

  // check lengths of list are the same
  assert(idsIn0->GetNumberOfIds() == idsIn1->GetNumberOfIds()) ;



  //----------------------------------------------------------------------------
  // Create the new interpolated points
  //----------------------------------------------------------------------------

  idsOut->Initialize() ;

  // create new points at the end of the list
  for (int i = 0 ;  i < idsIn0->GetNumberOfIds() ;  i++){
    int id0 = idsIn0->GetId(i) ;
    int id1 = idsIn1->GetId(i) ;

    int idnew = CreateNewPoint(polydata, id0, id1, lambda) ;
    idsOut->InsertNextId(idnew) ;
  }



  //----------------------------------------------------------------------------
  // find the cells which need modifying
  //----------------------------------------------------------------------------

  vtkIdList *cellsToModify = vtkIdList::New() ;
  vtkIdList *cellsOnPt = vtkIdList::New() ;
  for (int i = 0 ;  i < idsIn0->GetNumberOfIds() ;  i++){
    // get the cells on the point idsIn0[i]
    int id0 = idsIn0->GetId(i) ;
    polydata->GetPointCells(id0, cellsOnPt) ;

    // loop over all the cells on the point
    for (int j = 0 ;  j < cellsOnPt->GetNumberOfIds() ;  j++){
      int cellId = cellsOnPt->GetId(j) ;
      cellsToModify->InsertUniqueId(cellId) ;
    }

    // get the cells on the point idsIn1[i]
    int id1 = idsIn1->GetId(i) ;
    polydata->GetPointCells(id1, cellsOnPt) ;

    // loop over all the cells on the point
    for (int j = 0 ;  j < cellsOnPt->GetNumberOfIds() ;  j++){
      int cellId = cellsOnPt->GetId(j) ;
      cellsToModify->InsertUniqueId(cellId) ;
    }
  }

  cellsOnPt->Delete() ;



  //----------------------------------------------------------------------------
  // modify cells and copy to end of polydata
  //----------------------------------------------------------------------------

  vtkIdList *ptsOnCell = vtkIdList::New() ;
  vtkIdList *newCellIds = vtkIdList::New() ;

  // modify all the cells which contain the ids in list 0
  for (int i = 0 ;  i < cellsToModify->GetNumberOfIds() ;  i++){
    int cellId = cellsToModify->GetId(i) ;

    // get the points on the cell
    polydata->GetCellPoints(cellId, ptsOnCell) ;

    // change any points which are in input list 0
    for (int j = 0 ;  j < ptsOnCell->GetNumberOfIds() ;  j++){
      int idOfCellPoint = ptsOnCell->GetId(j) ;
      int k = FindIdInList(idsIn0, idOfCellPoint) ; // k = position in input list
      if (k >= 0){
        int newId = idsOut->GetId(k) ;  // newId = corresponding id in output list
        ptsOnCell->SetId(j, newId) ; // change id in list of call points
      }
    }

    // change any points which are in input list 1
    for (int j = 0 ;  j < ptsOnCell->GetNumberOfIds() ;  j++){
      int idOfCellPoint = ptsOnCell->GetId(j) ;
      int k = FindIdInList(idsIn1, idOfCellPoint) ; // k = position in input list
      if (k >= 0){
        int newId = idsOut->GetId(k) ;  // newId = corresponding id in output list
        ptsOnCell->SetId(j, newId) ; // change id in list of call points
      }
    }

    // make a modified copy of the cell at the end of the polydata
    int newCellId = CreateNewCell(polydata, cellId, ptsOnCell) ;
    newCellIds->InsertNextId(newCellId) ;
  }

  ptsOnCell->Delete() ;



  //----------------------------------------------------------------------------
  // delete old cells
  //----------------------------------------------------------------------------

  DeleteCells(polydata, cellsToModify) ;

  // use vtkCleanPolydata to remove unused points
  vtkCleanPolyData *clean = vtkCleanPolyData::New() ;
  clean->SetInput(polydata) ;
  clean->GetOutput()->Update() ;
  polydata->DeepCopy(clean->GetOutput()) ;
  clean->Delete() ;


  //----------------------------------------------------------------------------
  // free memory
  //----------------------------------------------------------------------------
  cellsToModify->Delete() ;

}



//------------------------------------------------------------------------------
// Modulo operator, same as % but works correctly on negative values of n as well
int vtkMEDPolyDataNavigator::Modulo(int n, int m) const
//------------------------------------------------------------------------------
{
  if (n >= 0)
    return n % m ;
  else
    return m - (-n % m) ;
}



//------------------------------------------------------------------------------
// Equals operator for Edge helper class
bool vtkMEDPolyDataNavigator::Edge::operator==(const Edge& edge) const
//------------------------------------------------------------------------------
{
  return (
    ((edge.Id0 == Id0) && (edge.Id1 == Id1)) || 
    ((edge.Id0 == Id1) && (edge.Id1 == Id0))
    );
} 


//------------------------------------------------------------------------------
// Not equals operator for Edge helper class
bool vtkMEDPolyDataNavigator::Edge::operator!=(const Edge& edge) const
//------------------------------------------------------------------------------
{
  return !(
    ((edge.Id0 == Id0) && (edge.Id1 == Id1)) || 
    ((edge.Id0 == Id1) && (edge.Id1 == Id0))
    );
} 


