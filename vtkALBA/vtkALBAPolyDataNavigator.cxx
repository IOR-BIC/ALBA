/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkALBAPolyDataNavigator.cxx,v $
Language:  C++
Date:      $Date: 2009-10-08 13:44:29 $
Version:   $Revision: 1.1.2.5 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2012
University of Bedfordshire
=========================================================================*/


#include "vtkObjectFactory.h"
#include "vtkObject.h"
#include "vtkPolyData.h"
#include "vtkIdType.h"
#include "vtkIdList.h"
#include "vtkCell.h"
#include "vtkCellArray.h"

#include "vtkALBAPolyDataNavigator.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCleanPolyData.h"
#include "vtkMath.h"

#include "vtkDataArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"

#include <vector>
#include <map>
#include <set>
#include <ostream>
#include <algorithm>
#include <assert.h>
#include <fstream>



//------------------------------------------------------------------------------
// standard macros
vtkStandardNewMacro(vtkALBAPolyDataNavigator);
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
vtkALBAPolyDataNavigator::vtkALBAPolyDataNavigator()
{
}



//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
vtkALBAPolyDataNavigator::~vtkALBAPolyDataNavigator()
{
}



//------------------------------------------------------------------------------
// Is id in set
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::InSet(int id,  const IdSet& idSet) const
{
  IdSet::const_iterator pos = idSet.find(id) ;
  return pos != idSet.end() ;
}



//------------------------------------------------------------------------------
// Is id not in set
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::NotInSet(int id,  const IdSet& idSet) const
{
  IdSet::const_iterator pos = idSet.find(id) ;
  return pos == idSet.end() ;

}



//------------------------------------------------------------------------------
// Insert id into list before location i.
// Analagous to insert() method on std::vector.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::InsertIdIntoList(int i, int id, vtkIdList *idList) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::DeleteLastId(vtkIdList *idList) const
{
  vtkIdList *tempList = vtkIdList::New() ;
  tempList->Initialize() ;

  // copy all but the last id to the temp list
  for (int i = 0 ;  i < idList->GetNumberOfIds() - 1 ;  i++)
    tempList->InsertNextId(idList->GetId(i)) ;

  // copy id's back again
  idList->DeepCopy(tempList) ;

  tempList->Delete() ;
}



//------------------------------------------------------------------------------
// Reverse entries in id list
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::ReverseIdList(vtkIdList *idList) const
{
  int n = idList->GetNumberOfIds() ;

  // copy backwards to temp list 
  vtkIdList *tempList = vtkIdList::New() ;
  tempList->Initialize() ;
  for (int i = n-1 ;  i >= 0 ;  i--)
    tempList->InsertNextId(idList->GetId(i)) ;

  // copy id's back again
  idList->DeepCopy(tempList) ;

  tempList->Delete() ;
}




//------------------------------------------------------------------------------
// Append id list: idList = idList + addList
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::AppendIdList(vtkIdList *idList,  vtkIdList *addList,  bool includeFirstId,  bool includeLastId) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetIdsInBothLists(vtkIdList *idlist0,  vtkIdList *idlist1,  vtkIdList *idlist_out) const
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
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::FindIdInList(vtkIdList *idList,  int id,  int istart) const
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
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::FindAndReplaceIdInList(vtkIdList *idList,  int idold,  int idnew) const 
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
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::FindIdPairFirstId(vtkIdList *idList,  int id0,  int id1) const
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
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::FindIdPairSecondId(vtkIdList *idList,  int id0,  int id1) const
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
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::InsertIdBetweenIds(vtkIdList *idList,  int id0,  int id1, int idNew) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetPartOfIdList(vtkIdList *idList,  vtkIdList *idListOut,  int i0,  int i1,  bool forwards) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::InitListToTriangle(vtkIdList *list, int id0, int id1, int id2) const
{
  list->Initialize() ;
  list->InsertNextId(id0) ;
  list->InsertNextId(id1) ;
  list->InsertNextId(id2) ;
}




//------------------------------------------------------------------------------
// Copy vtkIdList to id set
// NB if an id appears twice in the id list, it will only copy once to the map.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CopyListToIdSet(vtkIdList *idList, IdSet& idSet) const
{
  idSet.clear() ;
  AddListToIdSet(idList, idSet) ;
}



//------------------------------------------------------------------------------
// Add vtkIdList to id set
// NB if an id appears twice in the id list, it will only copy once to the map.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::AddListToIdSet(vtkIdList *idList, IdSet& idSet) const
{
  int n = idList->GetNumberOfIds() ;
  for (int i = 0 ;  i < n ;  i++){
    int id = idList->GetId(i) ;
    this->AddUniqueIdToSet(idSet, id) ;
  }
}


//------------------------------------------------------------------------------
// Copy id set to vtkIdList
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CopyIdSetToList(const IdSet& idSet, vtkIdList *idList) const
{
  idList->Initialize() ;

  IdSet::iterator pos ;
  for (pos = idSet.begin() ;  pos != idSet.end() ;  pos++)
    idList->InsertNextId(*pos) ;
}



//------------------------------------------------------------------------------
// Get ids in the range (0,n-1) which are not in the list.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetIdsNotInList(vtkIdList* idsIn, vtkIdList* idsOut, int n)
{
  // copy list to set for efficient searching
  IdSet idSetIn ;
  CopyListToIdSet(idsIn, idSetIn) ;

  idsOut->Reset() ;
  for (int i = 0 ;  i < n ;  i++){
    IdSet::iterator pos ;
    pos = idSetIn.find(i) ;
    if (pos == idSetIn.end())
      idsOut->InsertNextId(i) ;
  }
}



//------------------------------------------------------------------------------
// Cyclic shift id list
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CyclicShiftIdList(vtkIdList *idList, int nsteps) const
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
// Copy vector to id list
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CopyVectorToList(const std::vector<int>& vlist, vtkIdList* idList, bool append) const
{
  if (!append)
    idList->Reset() ;

  int n = (int)vlist.size() ;
  for (int i = 0 ;  i < n ;  i++)
    idList->InsertNextId(vlist.back()) ;
}



//------------------------------------------------------------------------------
// Copy id list to vector
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CopyListToVector(vtkIdList* idList, std::vector<int>& vlist, bool append) const
{
  if (!append)
    vlist.clear() ;

  int n = idList->GetNumberOfIds() ;
  for (int i = 0 ;  i < n ;  i++)
    vlist.push_back(idList->GetId(i)) ;
}



//------------------------------------------------------------------------------
// Set id list to all points (0,1,2...npts-1)
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::SetIdListToAllPoints(vtkPolyData *polydata, vtkIdList *list) const
{
  int n = polydata->GetPoints()->GetNumberOfPoints() ;
  list->SetNumberOfIds(n) ;
  for (int i = 0 ;  i < n-1 ;  i++)
    list->SetId(i,i) ;
}



//------------------------------------------------------------------------------
// Set id list to all cells (0,1,2...ncells-1)
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::SetIdListToAllCells(vtkPolyData *polydata, vtkIdList *list) const
{
  int n = polydata->GetNumberOfCells() ;
  list->SetNumberOfIds(n) ;
  for (int i = 0 ;  i < n-1 ;  i++)
    list->SetId(i,i) ;
}



//------------------------------------------------------------------------------
// Get min id in list
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::GetMinIdInList(vtkIdList *list) const
{
  int n = list->GetNumberOfIds() ;
  if (n == 0)
    return -1 ;

  int idmin = list->GetId(0) ;
  for (int i = 0 ;  i < n ;  i++){
    int id = list->GetId(i) ;
    if (id < idmin)
      idmin = id ;
  }

  return idmin ;
}



//------------------------------------------------------------------------------
// Get max id in list
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::GetMaxIdInList(vtkIdList *list) const
{
  int n = list->GetNumberOfIds() ;
  if (n == 0)
    return -1 ;

  int idmax = list->GetId(0) ;
  for (int i = 0 ;  i < n ;  i++){
    int id = list->GetId(i) ;
    if (id > idmax)
      idmax = id ;
  }

  return idmax ;
}



//------------------------------------------------------------------------------
// Print id list
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::PrintIdList(vtkIdList *idList, ostream& os) const
{
  for (int i = 0 ;  i < idList->GetNumberOfIds() ;  i++){
    os << idList->GetId(i) << " " ;

    if (i%10 == 9)
      os << std::endl ;
  }

  os << std::endl ;
}



//------------------------------------------------------------------------------
// Print id list
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::PrintIdListCSV(vtkIdList *idList, ostream& os) const
{
  for (int i = 0 ;  i < idList->GetNumberOfIds() ;  i++){
    os << idList->GetId(i) << ", " ;

    if (i%10 == 9)
      os << std::endl ;
  }

  os << std::endl ;
}



//------------------------------------------------------------------------------
// Add edge to list if it is not already in the list
// NB This is stupendously inefficient if there are a lot of edges !
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::AddUniqueEdge(const Edge& edge, EdgeVector& edgeList) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::AddUniqueEdge(const Edge& edge, EdgeMultiMap& edgeMap) const
{
  typedef EdgeMultiMap::iterator Mit ;
  typedef std::pair<Mit,Mit> MitRange ;

  int key = edge.GenerateKey() ;

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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CopyEdgeMapToVector(EdgeMultiMap& edgeMap, EdgeVector& edges) const
{
  edges.clear() ;

  EdgeMultiMap::iterator pos ;
  for (pos = edgeMap.begin() ;  pos != edgeMap.end() ;  pos++){
    edges.push_back(pos->second) ;
  }
}



//------------------------------------------------------------------------------
// Copy edge vector to edge multimap
// Multiple occurrences of edges are only copied once into the map.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CopyVectorToEdgeMap(const EdgeVector& edges, EdgeMultiMap& edgeMap) const
{
  edgeMap.clear() ;

  for (int i = 0 ;  i < (int)edges.size() ;  i++)
    AddUniqueEdge(edges[i], edgeMap) ;
}



//------------------------------------------------------------------------------
// Is edge in edge map
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::IsEdgeInMap(const EdgeMultiMap& edgeMap, const Edge& edge) const
{
  typedef EdgeMultiMap::const_iterator Mit ;
  int key = edge.GenerateKey() ;
  Mit pos = edgeMap.find(key) ;

  return (pos != edgeMap.end()) ;
}



//------------------------------------------------------------------------------
// Get number of points on cell
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::GetNumberOfPointsOnCell(vtkPolyData *polydata, int cellId) const
{
  vtkIdList *ids = vtkIdList::New() ;
  polydata->GetCellPoints(cellId, ids) ;
  int n = ids->GetNumberOfIds() ;
  ids->Delete() ;
  return n ;
}



//------------------------------------------------------------------------------
// Get points on cell with coords (cf polydata->GetCellPoints())
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellPointsWithCoords(vtkPolyData *polydata, int cellId, vtkIdList* ptIds, double (*coords)[3])
{
  polydata->GetCellPoints(cellId, ptIds) ;
  for (int i = 0 ;  i < ptIds->GetNumberOfIds() ;  i++){
    int id = ptIds->GetId(i) ;
    polydata->GetPoint(id, coords[i]) ;
  }
}



//------------------------------------------------------------------------------
// Get edges of cell
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellEdges(vtkPolyData *polydata, int cellId, EdgeVector& edges) const
{
  edges.clear() ;

  vtkIdList *ptIds = vtkIdList::New() ;
  polydata->GetCellPoints(cellId, ptIds) ;

  // Put the points in the list of edges
  int n = ptIds->GetNumberOfIds() ;
  int cellType = polydata->GetCellType(cellId) ;
  switch(cellType){  
  case VTK_EMPTY_CELL:
  case VTK_VERTEX:
    return ;
  case VTK_LINE:
  case VTK_POLY_LINE:
    for (int i = 0 ;  i < n-1 ;  i++){
      int id0 = ptIds->GetId(i) ;
      int id1 = ptIds->GetId(i+1) ;
      edges.push_back(Edge(id0,id1)) ;
    }
    break ;
  case VTK_TRIANGLE:
  case VTK_POLYGON:
    for (int i = 0 ;  i < n ;  i++){
      int id0 = ptIds->GetId(i) ;
      int id1 = ptIds->GetId((i+1)%n) ;
      edges.push_back(Edge(id0,id1)) ;
    }
    break ;
  default:
    return ;
  }

  ptIds->Delete() ;
}







//-----------------------------------------------------------------------------
// Get list of cells on edge
//-----------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellNeighboursOfEdge(vtkPolyData *polydata, const Edge& edge, vtkIdList *idList) const
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
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::GetNumberOfCellsOnEdge(vtkPolyData *polydata, const Edge& edge) const
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
//-----------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetAdjacentPointsOnCell(vtkPolyData *polydata, int cellId, int ptId, int& ptId0, int& ptId1) const
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
//-----------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetPointsOnCellNeighbours(vtkPolyData *polydata, int ptId, vtkIdList *idList) const
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
//-----------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetPointNeighboursOfPoint(vtkPolyData *polydata, int ptId, vtkIdList *idList) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetEdgesAroundPointInclusive(vtkPolyData *polydata, int ptId, EdgeVector& edges) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetEdgesAroundPointExclusive(vtkPolyData *polydata, int ptId, EdgeVector& edges) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetPointsAroundEdgeExclusive(vtkPolyData *polydata, const Edge& edge, vtkIdList *idList) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetEdgesAroundEdge(vtkPolyData *polydata, const Edge& edge, EdgeVector& edges) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetAllEdges(vtkPolyData *polydata, EdgeVector& edges) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetAllEdges(vtkPolyData *polydata, EdgeMultiMap& edges) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetEdgesOnListOfCells(vtkPolyData *polydata, vtkIdList *cellList, EdgeVector& edges) const
{
  edges.clear() ;

  // get edges in multimap form and copy to vector
  EdgeMultiMap edgeMap ;
  GetEdgesOnListOfCells(polydata, cellList, edgeMap) ;
  CopyEdgeMapToVector(edgeMap, edges) ;
}





//------------------------------------------------------------------------------
// Get list of all edges on list of cells, in the form of a multimap for quick search
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetEdgesOnListOfCells(vtkPolyData *polydata, vtkIdList *cellList, EdgeMultiMap& edges) const
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
// Get list of all points on list of edges
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetPointsOnListOfEdges(vtkPolyData *polydata, const EdgeVector& edgeList, vtkIdList* ptList) const
{
  ptList->Initialize() ;

  // get points in set form and copy to list
  IdSet ptSet ;
  GetPointsOnListOfEdges(polydata, edgeList, ptSet) ;
  CopyIdSetToList(ptSet, ptList) ;
}



//------------------------------------------------------------------------------
// Get list of all points on list of edges, in the form of a set for quick search
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetPointsOnListOfEdges(vtkPolyData *polydata, const EdgeVector& edgelist, IdSet& ptSet) const
{
  ptSet.clear() ;

  for (int i = 0 ;  i < (int)edgelist.size() ;  i++){
    Edge edge = edgelist[i] ;
    AddUniqueIdToSet(ptSet, edge.GetId0()) ;
    AddUniqueIdToSet(ptSet, edge.GetId1()) ;
  }
}



//------------------------------------------------------------------------------
// Get list of all cells on list of edges
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellsOnListOfEdges(vtkPolyData *polydata, const EdgeVector& edgeList, vtkIdList* cellList) const
{
  cellList->Initialize() ;

  // get cells in set form and copy to list
  IdSet cellSet ;
  GetCellsOnListOfEdges(polydata, edgeList, cellSet) ;
  CopyIdSetToList(cellSet, cellList) ;
}



//------------------------------------------------------------------------------
// Get list of all cells on list of edges, in the form of a set for quick search
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellsOnListOfEdges(vtkPolyData *polydata, const EdgeVector& edgelist, IdSet& cellSet) const
{
  cellSet.clear() ;

  vtkIdList* cellNeighbours = vtkIdList::New() ;

  for (int i = 0 ;  i < (int)edgelist.size() ;  i++){
    Edge edge = edgelist[i] ;
    GetCellNeighboursOfEdge(polydata, edge, cellNeighbours) ;
    for (int j = 0 ;  j < cellNeighbours->GetNumberOfIds() ;  j++)
      AddUniqueIdToSet(cellSet, cellNeighbours->GetId(j)) ;
  }

  cellNeighbours->Delete() ;
}



//------------------------------------------------------------------------------
// Get list of all cells on list of points
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellsOnListOfPoints(vtkPolyData *polydata, vtkIdList *ptIds, vtkIdList* cellList) const
{
  cellList->Initialize() ;

  // get cells in set form and copy to list
  IdSet cellSet ;
  GetCellsOnListOfPoints(polydata, ptIds, cellSet) ;
  CopyIdSetToList(cellSet, cellList) ;
}



//------------------------------------------------------------------------------
// Get list of all cells on list of points, in the form of a set for quick search
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellsOnListOfPoints(vtkPolyData *polydata, vtkIdList *ptIds, IdSet& cellSet) const
{
  cellSet.clear() ;

  vtkIdList* cellNeighbours = vtkIdList::New() ;

  for (int i = 0 ;  i < ptIds->GetNumberOfIds() ;  i++){
    polydata->GetPointCells(ptIds->GetId(i), cellNeighbours) ;
    for (int j = 0 ;  j < cellNeighbours->GetNumberOfIds() ;  j++)
      AddUniqueIdToSet(cellSet, cellNeighbours->GetId(j)) ;
  }

  cellNeighbours->Delete() ;
}



//------------------------------------------------------------------------------
// Get the edge connected neighbours of a cell
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellsOnCell_EdgeConnected(vtkPolyData *polydata, int cellId,  vtkIdList *idList) const
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
// Get edge connected neighbours of list of cells
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellsOnListOfCells_EdgeConnected(vtkPolyData *polydata, vtkIdList* cellsIn, vtkIdList* cellsOut)
{
  IdSet cellsInSet, cellsOutSet ;
  CopyListToIdSet(cellsIn, cellsInSet) ;

  GetCellsOnListOfCells_EdgeConnected(polydata, cellsInSet, cellsOutSet) ;

  CopyIdSetToList(cellsOutSet, cellsOut) ;
}



//------------------------------------------------------------------------------
// Get edge connected neighbours of list of cells
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellsOnListOfCells_EdgeConnected(vtkPolyData *polydata, IdSet& cellsIn, IdSet& cellsOut)
{
  cellsOut.clear() ;

  vtkIdList* tmp = vtkIdList::New() ;

  IdSet::iterator pos ;
  for (pos = cellsIn.begin() ;  pos != cellsIn.end() ;  pos++){
    int cellId = *pos ;

    GetCellsOnCell_EdgeConnected(polydata, cellId, tmp) ;
    for (int j = 0 ;  j < tmp->GetNumberOfIds() ;  j++){
      int newCellId = tmp->GetId(j) ;

      IdSet::iterator posFound = cellsIn.find(newCellId) ;
      if (posFound == cellsIn.end())
        AddUniqueIdToSet(cellsOut, newCellId) ;
    }
  }

  tmp->Delete() ;
}



//------------------------------------------------------------------------------
// Find edge in list.  Returns -1 if not found
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::FindEdgeInList(Edge& edge, EdgeVector& edgeList) const
{
  for (int i = 0 ;  i < (int)edgeList.size() ;  i++){
    if (edge == edgeList[i])
      return i ;
  }
  return -1 ;
}




//------------------------------------------------------------------------------
// Find point on a cell
// Returns -1 if not found
int vtkALBAPolyDataNavigator::FindIndexOfPointOnCell(vtkPolyData *polydata, int cellId, int ptId) const
  //------------------------------------------------------------------------------
{
  vtkIdList *idlist = vtkIdList::New() ;

  polydata->GetCellPoints(cellId, idlist) ;
  int index = FindIdInList(idlist, ptId) ;

  idlist->Delete() ;
  return index ;
}



//------------------------------------------------------------------------------
// Is point on a cell
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::IsPointOnCell(vtkPolyData *polydata, int cellId, int ptId) const
{
  vtkIdList *idlist = vtkIdList::New() ;

  polydata->GetCellPoints(cellId, idlist) ;
  bool inlist = InList(ptId, idlist) ;

  idlist->Delete() ;
  return inlist ;
}



//------------------------------------------------------------------------------
// Is edge on a cell
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::IsEdgeOnCell(vtkPolyData *polydata, int cellId, const Edge& edge) const
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
// Is proposed edge a point-pair across this cell, joining non-adjacent points.
// Can only be true for quads or higher.
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::IsEdgeAcrossCell(vtkPolyData *polydata, int cellId, const Edge& edge) const
{
  vtkIdList *cellPts = vtkIdList::New() ;
  polydata->GetCellPoints(cellId, cellPts) ;
  int n = cellPts->GetNumberOfIds() ;
  int index0 = FindIdInList(cellPts, edge.GetId0()) ;
  int index1 = FindIdInList(cellPts, edge.GetId1()) ;
  cellPts->Delete() ;

  if (n < 4)
    return false ;

  if ((index0 == -1) || (index1 == -1))
    return false ;

  if ((index1 == index0) || (index1 == Modulo(index0+1, n)) || (index1 == Modulo(index0-1, n)))
    return false ;

  return true ;
}




//------------------------------------------------------------------------------
// Is point in list of edges
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::IsPointInEdgeList(int ptId, EdgeVector& edges)
{
  bool found = false ;
  for (int i = 0 ;  i < (int)edges.size() && !found ;  i++)
    found = edges[i].ContainsPoint(ptId) ;
  return found ;
}


//------------------------------------------------------------------------------
// Is point in list of edges
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::IsPointInEdgeList(int ptId, EdgeVector& edges, int& whichEdge)
{
  bool found = false ;
  whichEdge = -1 ;
  for (int i = 0 ;  i < (int)edges.size() && !found ;  i++){
    if (edges[i].ContainsPoint(ptId)){
      whichEdge = i ;
      found = true ;
    }
  }

  return found ;
}



//------------------------------------------------------------------------------
// Find point on cell which is opposite to the edge.
// If cell is not a triangle, returns the first non-edge point found.
// Returns -1 if none found.
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::GetPointOnCellOppositeToEdge(vtkPolyData *polydata, int cellId, const Edge& edge) const
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
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::GetCellWithTwoEdges(vtkPolyData *polydata, const Edge& edge0,  const Edge& edge1) const
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
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::GetCellWithEdgeAndPoint(vtkPolyData *polydata, const Edge& edge, int ptId) const
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
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::GetCellWithThreePoints(vtkPolyData *polydata, int ptId0, int ptId1, int ptId2) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellsWithNumberOfPoints(vtkPolyData *polydata,  int n,  vtkIdList *cellIds) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCenterOfCell(vtkPolyData *polydata, int cellId, double x[3]) const
{
  vtkIdList *idsOnCell = vtkIdList::New() ;
  polydata->GetCellPoints(cellId, idsOnCell) ;
  int n = idsOnCell->GetNumberOfIds() ;

  for (int j = 0 ;  j < 3 ;  j++)
    x[j] = 0.0 ;

  for (int i = 0 ;  i < n ;  i++){
    int ptId = idsOnCell->GetId(i) ;
    double *xpt = polydata->GetPoint(ptId) ;
    for (int j = 0 ;  j < 3 ;  j++)
      x[j] += xpt[j] ;
  }

  for (int j = 0 ;  j < 3 ;  j++)
    x[j] /= (double)n ;

  idsOnCell->Delete() ;
}




//------------------------------------------------------------------------------
// Get points on cell which are colinear with the sides of the shape,
// ie the points which are on the straight sides, but not the corners.
// Useful for triangulating a cell.
// tol is the tolerance for colinearity in degrees.
// NB The output is the indices of the points on the cell, not the polydata id's.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetPointsOnStraightSidesOfCell(vtkPolyData *polydata, int cellId, vtkIdList *ptIdsOnCell, double tol) const
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

  allIds->Delete() ;
}



//------------------------------------------------------------------------------
// Get edges on boundary of polydata
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetEdgesOnBoundary(vtkPolyData *polydata, EdgeVector& edges) const
{
  edges.clear() ;

  EdgeVector allEdges ;
  GetAllEdges(polydata, allEdges) ;

  for (int i = 0 ;  i < (int)allEdges.size() ;  i++){
    Edge edge = allEdges[i] ;
    int n = GetNumberOfCellsOnEdge(polydata, edge) ;
    if (n == 1)
      edges.push_back(edge) ;
  }
}



//------------------------------------------------------------------------------
// Get points on boundary of polydata
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetPointsOnBoundary(vtkPolyData *polydata, vtkIdList* ptIds) const
{
  EdgeVector edges ;
  GetEdgesOnBoundary(polydata, edges) ;
  GetPointsOnListOfEdges(polydata, edges, ptIds) ;
}



//------------------------------------------------------------------------------
// Get cells on boundary of polydata
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellsOnBoundary(vtkPolyData *polydata, vtkIdList* cellIds) const
{
  EdgeVector edges ;
  GetEdgesOnBoundary(polydata, edges) ;
  GetCellsOnListOfEdges(polydata, edges, cellIds) ;
}



//------------------------------------------------------------------------------
// Get points with a given scalar value. \n
// Useful for listing labelled points.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetPointsWithScalarValue(vtkPolyData *polydata, char* scalarName, int component, double value, vtkIdList *pts)
{
  pts->Reset() ;

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
// Get cells with a given scalar value. \n
// Useful for listing labelled cells.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetCellsWithScalarValue(vtkPolyData *polydata, char* scalarName, int component, double value, vtkIdList *pts)
{
  pts->Reset() ;

  // find array with given name
  vtkDataArray *array = polydata->GetCellData()->GetScalars(scalarName) ;
  if (array == NULL)
    return ;

  for (int i = 0 ;  i < array->GetNumberOfTuples() ;  i++){
    double thisValue = array->GetComponent(i, component) ;
    if (thisValue == value)
      pts->InsertNextId(i) ;
  }
}



//------------------------------------------------------------------------------
// Set list of points to a given scalar value. \n
// Useful for labelling points.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::SetPointsToScalarValue(vtkPolyData *polydata, char* scalarName, int component, double value, vtkIdList *pts)
{
  // find array with given name
  vtkDataArray *array = polydata->GetPointData()->GetScalars(scalarName) ;
  if (array == NULL)
    return ;

  for (int i = 0 ;  i < pts->GetNumberOfIds() ;  i++){
    int id = pts->GetId(i) ;
    array->SetComponent(id, component, value) ;
  }

  array->Modified() ; // Essential because SetComponent() does not do it.
}



//------------------------------------------------------------------------------
// Set list of cells to a given scalar value. \n
// Useful for labelling cells.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::SetCellsToScalarValue(vtkPolyData *polydata, char* scalarName, int component, double value, vtkIdList* cells)
{
  // find array with given name
  vtkDataArray *array = polydata->GetCellData()->GetScalars(scalarName) ;
  if (array == NULL)
    return ;

  for (int i = 0 ;  i < cells->GetNumberOfIds() ;  i++){
    int id = cells->GetId(i) ;
    array->SetComponent(id, component, value) ;
  }

  array->Modified() ; // Essential because SetComponent() does not do it.
}



//------------------------------------------------------------------------------
// Get points with no cell neighbours
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::GetPointsWithNoCells(vtkPolyData *polydata, vtkIdList *pts) const
{
  pts->Initialize() ;
  vtkIdList *cellNeighs = vtkIdList::New() ;
  int n = polydata->GetPoints()->GetNumberOfPoints() ;
  for (int i = 0 ;  i < n ;  i++){
    polydata->GetPointCells(i, cellNeighs) ;
    if (cellNeighs->GetNumberOfIds() == 0)
      pts->InsertNextId(i) ;
  }
  cellNeighs->Delete() ;
}



//------------------------------------------------------------------------------
// Get length of edge
//------------------------------------------------------------------------------
double vtkALBAPolyDataNavigator::GetLengthOfEdge(vtkPolyData *polydata, const Edge edge) const
{
  int id0 = edge.GetId0() ;
  int id1 = edge.GetId1() ;

  double x0[3], x1[3] ;
  polydata->GetPoint(id0, x0) ;
  polydata->GetPoint(id1, x1) ;

  double dx = x1[0] - x0[0] ;
  double dy = x1[1] - x0[1] ;
  double dz = x1[2] - x0[2] ;
  double r = sqrt(dx*dx + dy*dy + dz*dz) ;
  return r ;
}



//------------------------------------------------------------------------------
// PrintCell.
// NB Needs BuildCells().
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::PrintCell(vtkPolyData *polydata, int cellId, ostream& os) const
{
  os << "cell " << cellId << ": " ;

  int cellType = polydata->GetCellType(cellId) ;
  switch(cellType){
  case VTK_EMPTY_CELL:
    os << "Empty Cell\n" ;
    return ;
  case VTK_VERTEX:
    os << "vertex " ;
    break ;
  case VTK_LINE:
    os << "line " ;
    break ;
  case VTK_POLY_LINE:
    os << "polyline " ;
    break ;
  case VTK_TRIANGLE:
    os << "triangle " ;
    break ;
  case VTK_POLYGON:
    os << "poly " ;
    break ;
  default:
    os << "type " << cellType << " " ;
    break ;
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
// PrintCell.
// NB Needs BuildCells().
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::PrintCellCSV(vtkPolyData *polydata, int cellId, ostream& os) const
{
  os << cellId << ", " ;

  int cellType = polydata->GetCellType(cellId) ;
  switch(cellType){
  case VTK_EMPTY_CELL:
    os << "Empty Cell\n" ;
    return ;
  case VTK_VERTEX:
    os << "vertex, " ;
    break ;
  case VTK_LINE:
    os << "line, " ;
    break ;
  case VTK_POLY_LINE:
    os << "polyline, " ;
    break ;
  case VTK_TRIANGLE:
    os << "triangle, " ;
    break ;
  case VTK_POLYGON:
    os << "poly, " ;
    break ;
  default:
    os << "type, " << cellType << ", " ;
    break ;
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
      os << ptId << ", " ;
    }
    os << ", " ;

    for (int i = 0 ;  i < n ;  i++){
      int ptId = ptsOnCell->GetId(i) ;
      double x[3] ;
      polydata->GetPoint(ptId, x) ;
      os << std::setprecision(3) << x[0] << ", " << x[1] << ", " << x[2] << ",, " ;
    }
  }

  os << std::endl ;

  ptsOnCell->Delete() ;
}



//------------------------------------------------------------------------------
// Print all cells in polydata.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::PrintCells(vtkPolyData *polydata, ostream& os)  const
{
  os << "number of points = " << polydata->GetPoints()->GetNumberOfPoints() << "\n\n" ;

  os << "number of cells = " << polydata->GetNumberOfCells() << "\n" ;
  for (int i = 0 ;  i < polydata->GetNumberOfCells() ;  i++)
    PrintCell(polydata, i, os) ;
  os << "\n" ;

  vtkIdList *ptsNoCells = vtkIdList::New() ;
  GetPointsWithNoCells(polydata, ptsNoCells) ;
  os << "points with no cells\n" ;
  PrintIdList(ptsNoCells, os) ;
  os << "\n" ;
  ptsNoCells->Delete() ;
}



//------------------------------------------------------------------------------
// Print all cells in polydata.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::PrintCellsCSV(vtkPolyData *polydata, ostream& os)  const
{
  os << "number of points, " << polydata->GetPoints()->GetNumberOfPoints() << "\n\n" ;

  os << "number of cells, " << polydata->GetNumberOfCells() << "\n" ;
  for (int i = 0 ;  i < polydata->GetNumberOfCells() ;  i++)
    PrintCellCSV(polydata, i, os) ;
  os << "\n" ;

  vtkIdList *ptsNoCells = vtkIdList::New() ;
  GetPointsWithNoCells(polydata, ptsNoCells) ;
  os << "points with no cells\n" ;
  PrintIdListCSV(ptsNoCells, os) ;
  os << "\n" ;
  ptsNoCells->Delete() ;
}



//------------------------------------------------------------------------------
// Print attribute structure of polydata
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::PrintAttributeData(vtkPolyData *polydata, ostream& os,  bool printTuples)  const
{
  //----------------------------------------------------------------------------
  // Points and cells
  //----------------------------------------------------------------------------
  os << "no of pts " << polydata->GetPoints()->GetNumberOfPoints() << "\n" ;
  os << "no of cells " << polydata->GetNumberOfCells() << "\n" ;


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
// Print bounds of polydata
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::PrintBounds(vtkPolyData *polydata, ostream& os) const
{
  double b[6] ;
  polydata->GetBounds(b) ;

  os << "bounds\n" ;
  os << b[0] << " " << b[1] << "\t" << "(size " << b[1]-b[0] << ")" << "\n" ;
  os << b[2] << " " << b[3] << "\t" << "(size " << b[3]-b[2] << ")" << "\n" ;
  os << b[4] << " " << b[5] << "\t" << "(size " << b[5]-b[4] << ")" << "\n" ;
  os << "\n" ;
}



//------------------------------------------------------------------------------
// Copy point data to the end of the attribute list,
// increasing the no. of tuples by one.
// The attributes are copied from the point id0.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CopyPointData(vtkPolyData *polydata,  int id0) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CopyPointData(vtkPolyData *polydata,  int id0, int id1, double lambda) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CopyCellData(vtkPolyData *polydata,  int cellId) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::DeleteCellTuples(vtkPolyData *polydata, vtkIdList *cellIds)  const
{
  vtkCellData *oldCellData = polydata->GetCellData() ;
  int numberOfCellArrays = oldCellData->GetNumberOfArrays() ;
  int ncellsOld = oldCellData->GetNumberOfTuples() ;
  int ncellsToDelete = cellIds->GetNumberOfIds() ;

  if (numberOfCellArrays == 0)
    return ;

  // make array labelling cells to keep
  int *keep = new int[ncellsOld] ;
  for (int i = 0 ;  i < ncellsOld ;  i++)
    keep[i] = 1 ;
  for (int i = 0 ;  i < ncellsToDelete ;  i++){
    int id = cellIds->GetId(i) ;
    assert(id < ncellsOld) ;
    keep[id] = 0 ;
  }

  // copy cell data to new array and empty it of existing tuples.
  vtkCellData *newCellData = vtkCellData::New() ;
  newCellData->DeepCopy(polydata->GetCellData()) ;
  newCellData->SetNumberOfTuples(0) ;

  // copy all tuples to new list which are to keep
  for (int j = 0 ;  j < ncellsOld ;  j++){
    if (keep[j] == 1){
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
  delete [] keep ;
  newCellData->Delete() ;
}




//------------------------------------------------------------------------------
// Delete tuples from point attributes.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::DeletePointTuples(vtkPolyData *polydata, vtkIdList *ptIds)  const
{
  vtkPointData *oldPtData = polydata->GetPointData() ;
  int numberOfPtArrays = oldPtData->GetNumberOfArrays() ;
  int nptsOld = oldPtData->GetNumberOfTuples() ;
  int nptsToDelete = ptIds->GetNumberOfIds() ;

  if (numberOfPtArrays == 0)
    return ;

  // make array labelling pts to keep
  int *keep = new int[nptsOld] ;
  for (int i = 0 ;  i < nptsOld ;  i++)
    keep[i] = 1 ;
  for (int i = 0 ;  i < nptsToDelete ;  i++){
    int id = ptIds->GetId(i) ;
    assert(id < nptsOld) ;
    keep[id] = 0 ;
  }

  // copy cell data to new array and empty it of existing tuples.
  vtkPointData *newPtData = vtkPointData::New() ;
  newPtData->DeepCopy(polydata->GetPointData()) ;
  newPtData->SetNumberOfTuples(0) ;

  // copy all tuples to new list which are to keep
  for (int j = 0 ;  j < nptsOld ;  j++){
    if (keep[j] == 1){
      for (int i = 0 ;  i < numberOfPtArrays ;  i++){
        vtkDataArray *daOld = oldPtData->GetArray(i) ;
        vtkDataArray *daNew = newPtData->GetArray(i) ;

        int numberOfComponents = daOld->GetNumberOfComponents() ;
        double tuple[9] ;
        for (int k = 0 ;  k < numberOfComponents ;  k++)
          tuple[k] = daOld->GetComponent(j, k) ;

        daNew->InsertNextTuple(tuple) ;
      }
    }
  }

  oldPtData->DeepCopy(newPtData) ;
  delete [] keep ;
  newPtData->Delete() ;
}



//------------------------------------------------------------------------------
// Create new attribute array
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CreatePointDataArray(vtkPolyData *polydata, char* name, int numberOfComponents, int dataType) const
{
  vtkDataArray* scalars ;

  switch(dataType){
  case VTK_UNSIGNED_CHAR:
    {
      vtkUnsignedCharArray* array = vtkUnsignedCharArray::New() ;
      scalars = array ;
      break ;
    }
  case VTK_INT:
    {
      vtkIntArray* array = vtkIntArray::New() ;
      scalars = array ;
      break ;
    }
  case VTK_DOUBLE:
    {
      vtkDoubleArray* array = vtkDoubleArray::New() ;
      scalars = array ;
      break ;
    }
  case VTK_FLOAT:
    {
      vtkFloatArray* array = vtkFloatArray::New() ;
      scalars = array ;
      break ;
    }
  default:
    return ;
  }

  int n = polydata->GetPoints()->GetNumberOfPoints() ;
  int m = numberOfComponents ;

  scalars->SetNumberOfComponents(m) ;
  scalars->SetNumberOfTuples(n) ;  
  scalars->SetName(name) ;

  double* tuple = new double[m] ;
  for (int j = 0 ;  j < m ;  j++)
    tuple[j] = 255.0 ;

  for (int i = 0 ;  i < n ;  i++)
    scalars->SetTuple(i,tuple) ;

  polydata->GetPointData()->SetScalars(scalars) ;
  scalars->Delete() ;

  delete [] tuple ;
}







//------------------------------------------------------------------------------
// Create new attribute array
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CreateCellDataArray(vtkPolyData *polydata, char* name, int numberOfComponents, int dataType) const
{
  //  vtkDataArray* scalars ;

  /* switch(dataType){
  case VTK_UNSIGNED_CHAR:
  { */
  vtkUnsignedCharArray* scalars = vtkUnsignedCharArray::New() ;
  //scalars = array ;
  /*      break ;
  }
  case VTK_INT:
  {
  vtkIntArray* array = vtkIntArray::New() ;
  scalars = array ;
  break ;
  }
  case VTK_DOUBLE:
  {
  vtkDoubleArray* array = vtkDoubleArray::New() ;
  scalars = array ;
  break ;
  }
  case VTK_FLOAT:
  {
  vtkFloatArray* array = vtkFloatArray::New() ;
  scalars = array ;
  break ;
  }
  default:
  return ;
  }
  */
  int n = polydata->GetNumberOfCells() ;
  int m = numberOfComponents ;

  scalars->SetNumberOfComponents(m) ;
  scalars->SetNumberOfTuples(n) ;  
  scalars->SetName(name) ;

  double* tuple = new double[m] ;
  for (int j = 0 ;  j < m ;  j++)
    tuple[j] = 255.0 ;

  for (int i = 0 ;  i < n ;  i++)
    scalars->SetTuple(i,tuple) ;

  polydata->GetCellData()->SetScalars(scalars) ;
  scalars->Delete() ;

  delete [] tuple ;
}






//------------------------------------------------------------------------------
// Set cell to degenerate empty cell
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::SetCellToEmpty(vtkPolyData *polydata, int cellId) const
{
  polydata->ReplaceCell(cellId, 0, NULL) ;
}




//------------------------------------------------------------------------------
// Delete list of cells.
// cf polydata->DeleteCell(), which labels the cell, but does not actually remove it.
// The corresponding attribute data is also deleted.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::DeleteCells(vtkPolyData *polydata, vtkIdList *idList) const
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
  vtkCellArray *copyPolys = vtkCellArray::New() ;
  vtkCellArray *copyLines = vtkCellArray::New() ;
  vtkCellArray *copyVerts = vtkCellArray::New() ;
  vtkIdList *tmp = vtkIdList::New() ;

  for (int i = 0 ;  i < polydata->GetNumberOfCells() ;  i++){
    int cellType = polydata->GetCellType(i) ;
    if (cellType != VTK_EMPTY_CELL){
      polydata->GetCellPoints(i, tmp) ;
      switch(cellType){
      case VTK_VERTEX:
      case VTK_POLY_VERTEX:
        copyVerts->InsertNextCell(tmp) ;
        break ;
      case VTK_LINE:
      case VTK_POLY_LINE:
        copyLines->InsertNextCell(tmp) ;
        break ;
      case VTK_POLYGON:
      case VTK_TRIANGLE:
      default:
        // assume cell is poly by default
        copyPolys->InsertNextCell(tmp) ;
        break ;
      }
    }
  }

  tmp->Delete() ;


  //----------------------------------------------------------------------------
  // Replace the polydata cells with the new arrays
  //----------------------------------------------------------------------------
  if (copyVerts->GetNumberOfCells() > 0)
    polydata->SetVerts(copyVerts) ;
  if (copyLines->GetNumberOfCells() > 0)
    polydata->SetLines(copyLines) ;
  if (copyPolys->GetNumberOfCells() > 0)
    polydata->SetPolys(copyPolys) ;

  copyVerts->Delete() ;
  copyLines->Delete() ;
  copyPolys->Delete() ;


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
// Delete list of points.
// The corresponding attribute data is also deleted.
// NB Only use this for points which form a block at the end of the polydata,
// and which are not members of cells, else cells will contain invalid id's.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::DeletePoints(vtkPolyData *polydata, vtkIdList *idList)  const
{
  int n = polydata->GetPoints()->GetNumberOfPoints() ;
  int m = idList->GetNumberOfIds() ;

  // check that id's are a contiguous block at the end: n-m to n-1
  for (int i = 0 ;  i < m ;  i++)
    assert(idList->GetId(i) == n-m+i) ;

  // copy the points without the end block
  vtkPoints* newPts = vtkPoints::New() ;
  for (int i = 0 ;  i < n-m ;  i++){
    double x[3] ;
    polydata->GetPoint(i, x) ;
    newPts->InsertNextPoint(x) ;
  }
  polydata->SetPoints(newPts) ;

  // delete the attribute data
  DeletePointTuples(polydata, idList) ;

  // Delete the invalid links and cells
  polydata->DeleteCells() ;
  polydata->DeleteLinks() ;
}




//------------------------------------------------------------------------------
// Swap the id's of two points. \n
// Can call consecutively but must delete or rebuild links when finished.
// (Actually the points don't move but we exchange their data).
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::SwapPointIds(vtkPolyData *polydata, int id0, int id1) const
{
  // swap the positions
  double x0[3], x1[3] ;
  polydata->GetPoint(id0, x0) ;
  polydata->GetPoint(id1, x1) ;
  polydata->GetPoints()->SetPoint(id0, x1) ;
  polydata->GetPoints()->SetPoint(id1, x0) ;


  // swap the point attribute data
  vtkPointData *pd = polydata->GetPointData() ;
  int numberOfPtArrays = pd->GetNumberOfArrays() ;
  for (int i = 0 ;  i < numberOfPtArrays ;  i++){
    vtkDataArray *da = pd->GetArray(i) ;
    int numberOfComponents = da->GetNumberOfComponents() ;

    double t0[9], t1[9] ;
    for (int j = 0 ;  j < numberOfComponents ;  j++){
      t0[j] = da->GetComponent(id0, j) ;
      t1[j] = da->GetComponent(id1, j) ;
      da->SetComponent(id0, j, t1[j]) ;
      da->SetComponent(id1, j, t0[j]) ;
    }
  }


  // change the id's on any cells belonging to the points
  vtkIdList* idlist = vtkIdList::New() ;

  polydata->GetPointCells(id0, idlist) ;
  for (int i = 0 ;  i < idlist->GetNumberOfIds() ;  i++){
    int cellId = idlist->GetId(i) ;
    ChangePointIdInCell(polydata, cellId, id0, id1) ;
  }

  polydata->GetPointCells(id1, idlist) ;
  for (int i = 0 ;  i < idlist->GetNumberOfIds() ;  i++){
    int cellId = idlist->GetId(i) ;
    ChangePointIdInCell(polydata, cellId, id1, id0) ;
  }

  idlist->Delete() ;
}



//------------------------------------------------------------------------------
// Move id's of points to end of polydata. \n
// Corresponding cells and attributes are also changed.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::MovePointIdsToEnd(vtkPolyData *polydata, vtkIdList *idList)  const
{
  int n = polydata->GetPoints()->GetNumberOfPoints() ;
  int m = idList->GetNumberOfIds() ;

  // Problem is to move m id's into the range n-m to n-1 incl.
  // id's which are outside this range must move in.
  // id's inside the range but not in the list must move out.
  // We assume that the list contains no repeats.
  int idmin = GetMinIdInList(idList) ;
  int idmax = GetMaxIdInList(idList) ;
  if ((idmin == n-m) && (idmax == n-1)){
    // no need to do anything
    return ;
  }

  // list id's which are to move in
  std::vector<int> idsInBound ;
  for (int i = 0 ;  i < m ;  i++){
    int id = idList->GetId(i) ;
    if (id < n-m)
      idsInBound.push_back(id) ;
  }

  // list id's which are to move out
  // note that we copy the list to a set for efficient searching
  IdSet idset ;
  std::vector<int> idsOutBound ;
  CopyListToIdSet(idList, idset) ;
  for (int id = n-m ;  id < n ;  id++){
    if (this->NotInSet(id, idset))
      idsOutBound.push_back(id) ;
  }

  // check that inbound and outbound lists are same size
  int numInBound = (int)idsInBound.size() ;
  int numOutBound = (int)idsOutBound.size() ;
  if (numInBound != numOutBound)
    assert(false) ;

  // swap lists in pairs
  for (int i = 0 ;  i < numInBound ;  i++)
    SwapPointIds(polydata, idsInBound[i], idsOutBound[i]) ;

  // list is no longer valid to set points to n-m to n-1
  for (int i = 0 ;  i < m ;  i++)
    idList->SetId(i, i+n-m) ;

  // Delete the invalid links and cells
  polydata->DeleteCells() ;
  polydata->DeleteLinks() ;
}



//------------------------------------------------------------------------------
// Create a new point, adding tuples to the attribute data if necessary.
// The position and scalars are copied from id0.
// Returns the id of the created point.
// This only creates a point - it does not add it to a cell.
// This can be called successively, but you must delete or rebuild the cells and links when finished.
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::CreateNewPoint(vtkPolyData *polydata,  int id0) const
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
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::CreateNewPoint(vtkPolyData *polydata,  int id0, int id1, double lambda) const
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
// The attribute data is copied from cellId.
// Returns the id of the created cell.
// This can be called successively, but you must delete or rebuild the cells and links when finished.
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::CreateNewCell(vtkPolyData *polydata,  int copyScalarsCellId,  vtkIdList *ids) const
{
  int cellType = polydata->GetCellType(copyScalarsCellId) ;
  int idnew ;
  switch(cellType){
  case VTK_VERTEX:
  case VTK_POLY_VERTEX:
    idnew = polydata->GetVerts()->InsertNextCell(ids) ;
    break ;
  case VTK_LINE:
  case VTK_POLY_LINE:
    idnew = polydata->GetLines()->InsertNextCell(ids) ;
    break ;
  default:
    idnew = polydata->GetPolys()->InsertNextCell(ids) ;
    break ;
  }

  CopyCellData(polydata, copyScalarsCellId) ;             // copy attribute data

  return idnew ;
}




//------------------------------------------------------------------------------
// Copy cells to the end of the polydata. \n
// The cell attributes are also copied.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::CopyCells(vtkPolyData *polydata,  vtkIdList *cellIds,  vtkIdList *newCellIds)  const
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
// Add new points to edges. \n
// A single point is placed at the midpoint of each edge. \n
// Each edge should occur only once in the list. \n
// The id's of the new points are returned in newPtIds. \n
// All cells on each edge will gain the new point, so triangles will become quads. \n
// This method does not change the id's of the cells.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::AddPointsToEdges(vtkPolyData *polydata, const EdgeVector& edges, vtkIdList *newPtIds)  const
{
  //----------------------------------------------------------------------------
  // Get all the cells and list the id's as editable vtkIdList's 
  //----------------------------------------------------------------------------
  int ncells = polydata->GetNumberOfCells() ;
  vtkIdList **allCells = new (vtkIdList * [ncells]) ;
  for (int i = 0 ;  i < ncells ; i++)
    allCells[i] = vtkIdList::New() ;

  for (int i = 0 ;  i < ncells ; i++)
    polydata->GetCellPoints(i, allCells[i]) ;


  //----------------------------------------------------------------------------
  // Get all the cells which use the edges
  //----------------------------------------------------------------------------
  vtkIdList *allCellsOnEdges = vtkIdList::New() ;
  GetCellsOnListOfEdges(polydata, edges, allCellsOnEdges) ;


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
  // For each edge, add the new point id to the corresponding editable cells.
  //----------------------------------------------------------------------------
  vtkIdList *cellsOnThisEdge = vtkIdList::New() ;
  for (int i = 0 ;  i < nedges ;  i++){
    GetCellNeighboursOfEdge(polydata, edges.at(i), cellsOnThisEdge) ;

    int id0 = edges.at(i).GetId0() ;
    int id1 = edges.at(i).GetId1() ;
    int idnew = newPtIds->GetId(i) ;

    for (int j = 0 ;  j < cellsOnThisEdge->GetNumberOfIds() ;  j++){
      int cellId = cellsOnThisEdge->GetId(j) ;
      InsertIdBetweenIds(allCells[cellId], id0, id1, idnew) ;
    }
  }
  cellsOnThisEdge->Delete() ;


  //----------------------------------------------------------------------------
  // Create modified cells and delete originals
  //----------------------------------------------------------------------------

  for (int i = 0 ;  i < ncells ; i++)
    CreateNewCell(polydata, i, allCells[i]) ;

  vtkIdList* orgIds = vtkIdList::New() ;
  for (int i = 0 ;  i < ncells ;  i++)
    orgIds->InsertNextId(i) ;
  this->DeleteCells(polydata, orgIds) ;
  orgIds->Delete() ;


  // Free allocated memory
  allCellsOnEdges->Delete() ;
  for (int i = 0 ;  i < ncells ; i++)
    allCells[i]->Delete() ;
  delete [] allCells ;

  // rebuild the links because we have changed the polydata
  polydata->DeleteCells() ;
  polydata->DeleteLinks() ;
}



//------------------------------------------------------------------------------
// Add new points to edges. \n
// Each point is placed on edge[i] at interpolated position lambda[i][j]. \n
// Each edge should occur only once in the list. \n
// The id's of the new points are returned in newPtIds. \n
// All cells on each edge will gain the new points, so triangles will become quads. \n
// This method does not change the id's of the cells.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::AddPointsToEdges(
  vtkPolyData *polydata, const EdgeVector& edges, 
  const std::vector<std::vector<double> >& lambda, 
  std::vector<std::vector<int> >& newPtIds)  const
{
  //----------------------------------------------------------------------------
  // Get all the cells and list the id's as editable vtkIdList's 
  //----------------------------------------------------------------------------
  int ncells = polydata->GetNumberOfCells() ;
  vtkIdList **allCells = new (vtkIdList * [ncells]) ;
  for (int i = 0 ;  i < ncells ; i++)
    allCells[i] = vtkIdList::New() ;

  for (int i = 0 ;  i < ncells ; i++)
    polydata->GetCellPoints(i, allCells[i]) ;


  //----------------------------------------------------------------------------
  // Get all the cells which use the edges
  //----------------------------------------------------------------------------
  vtkIdList *allCellsOnEdges = vtkIdList::New() ;
  GetCellsOnListOfEdges(polydata, edges, allCellsOnEdges) ;


  //----------------------------------------------------------------------------
  // int the output vector of new pt ids
  //----------------------------------------------------------------------------
  int nedges = (int)edges.size() ;
  newPtIds.reserve(nedges) ;
  newPtIds.resize(nedges) ;
  for (int i = 0 ;  i < nedges ;  i++)
    newPtIds[i].clear() ;


  //----------------------------------------------------------------------------
  // Create new points on the edges
  //----------------------------------------------------------------------------
  for (int i = 0 ;  i < nedges ;  i++){
    int id0 = edges.at(i).GetId0() ;
    int id1 = edges.at(i).GetId1() ;
    for (int j = 0 ;  j < (int)lambda[i].size() ;  j++){
      int idNew = CreateNewPoint(polydata, id0, id1, lambda[i][j]) ;
      newPtIds[i].push_back(idNew) ;
    }
  }


  //----------------------------------------------------------------------------
  // For each edge, add the new point id to the corresponding editable cells.
  //----------------------------------------------------------------------------
  vtkIdList *cellsOnThisEdge = vtkIdList::New() ;
  for (int i = 0 ;  i < nedges ;  i++){
    GetCellNeighboursOfEdge(polydata, edges.at(i), cellsOnThisEdge) ;

    int id0 = edges.at(i).GetId0() ;
    int id1 = edges.at(i).GetId1() ;

    for (int k = 0 ;  k < cellsOnThisEdge->GetNumberOfIds() ;  k++){
      int cellId = cellsOnThisEdge->GetId(k) ;
      for (int j = 0;  j < (int)newPtIds[i].size() ;  j++){
        int idnew = newPtIds[i][j] ;
        if (j == 0)
          InsertIdBetweenIds(allCells[cellId], id0, id1, idnew) ; // insert first pt between id0 and id1
        else{
          int idprev = newPtIds[i][j-1] ;
          InsertIdBetweenIds(allCells[cellId], idprev, id1, idnew) ; // insert others between last pt and id1.
        }
      }
    }
  }
  cellsOnThisEdge->Delete() ;


  //----------------------------------------------------------------------------
  // Create modified cells and delete originals
  //----------------------------------------------------------------------------

  for (int i = 0 ;  i < ncells ; i++)
    CreateNewCell(polydata, i, allCells[i]) ;

  vtkIdList* orgIds = vtkIdList::New() ;
  for (int i = 0 ;  i < ncells ;  i++)
    orgIds->InsertNextId(i) ;
  this->DeleteCells(polydata, orgIds) ;
  orgIds->Delete() ;


  // Free allocated memory
  allCellsOnEdges->Delete() ;
  for (int i = 0 ;  i < ncells ; i++)
    allCells[i]->Delete() ;
  delete [] allCells ;

  // rebuild the links because we have changed the polydata
  polydata->DeleteCells() ;
  polydata->DeleteLinks() ;
}


//------------------------------------------------------------------------------
// Add new points to edges. \n
// Each point i is placed on edge[i] at interpolated position lambda[i]. \n
// This might not work if edges occur more than once in the list. \n
// The id's of the new points are returned in newPtIds. \n
// All cells on each edge will gain the new points, so triangles will become quads. \n
// This method does not change the id's of the cells.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::AddPointsToEdges(vtkPolyData *polydata, const EdgeVector& edges, const std::vector<double>& lambda, vtkIdList *newPtIds)  const
{
  //----------------------------------------------------------------------------
  // Get all the cells and list the id's as editable vtkIdList's 
  //----------------------------------------------------------------------------
  int ncells = polydata->GetNumberOfCells() ;
  vtkIdList **allCells = new (vtkIdList * [ncells]) ;
  for (int i = 0 ;  i < ncells ; i++)
    allCells[i] = vtkIdList::New() ;

  for (int i = 0 ;  i < ncells ; i++)
    polydata->GetCellPoints(i, allCells[i]) ;


  //----------------------------------------------------------------------------
  // Get all the cells which use the edges
  //----------------------------------------------------------------------------
  vtkIdList *allCellsOnEdges = vtkIdList::New() ;
  GetCellsOnListOfEdges(polydata, edges, allCellsOnEdges) ;


  //----------------------------------------------------------------------------
  // Create new points on the edges
  //----------------------------------------------------------------------------
  newPtIds->Initialize() ;
  int nedges = (int)edges.size() ;
  for (int i = 0 ;  i < nedges ;  i++){
    int id0 = edges.at(i).GetId0() ;
    int id1 = edges.at(i).GetId1() ;
    int idNew = CreateNewPoint(polydata, id0, id1, lambda[i]) ;
    newPtIds->InsertNextId(idNew) ;
  }


  //----------------------------------------------------------------------------
  // For each edge, add the new point id to the corresponding editable cells.
  //----------------------------------------------------------------------------
  vtkIdList *cellsOnThisEdge = vtkIdList::New() ;
  for (int i = 0 ;  i < nedges ;  i++){
    GetCellNeighboursOfEdge(polydata, edges.at(i), cellsOnThisEdge) ;

    int id0 = edges.at(i).GetId0() ;
    int id1 = edges.at(i).GetId1() ;
    int idnew = newPtIds->GetId(i) ;

    for (int j = 0 ;  j < cellsOnThisEdge->GetNumberOfIds() ;  j++){
      int cellId = cellsOnThisEdge->GetId(j) ;
      InsertIdBetweenIds(allCells[cellId], id0, id1, idnew) ;
    }
  }
  cellsOnThisEdge->Delete() ;


  //----------------------------------------------------------------------------
  // Create modified cells and delete originals
  //----------------------------------------------------------------------------

  for (int i = 0 ;  i < ncells ; i++)
    CreateNewCell(polydata, i, allCells[i]) ;

  vtkIdList* orgIds = vtkIdList::New() ;
  for (int i = 0 ;  i < ncells ;  i++)
    orgIds->InsertNextId(i) ;
  this->DeleteCells(polydata, orgIds) ;
  orgIds->Delete() ;


  // Free allocated memory
  allCellsOnEdges->Delete() ;
  for (int i = 0 ;  i < ncells ; i++)
    allCells[i]->Delete() ;
  delete [] allCells ;

  // rebuild the links because we have changed the polydata
  polydata->DeleteCells() ;
  polydata->DeleteLinks() ;
}



//------------------------------------------------------------------------------
// Change a point id in a cell.
// This searches for idold in the cell and replaces it with idnew.
// Returns true if a change was made.
// This can be called successively, but you must delete or rebuild the cells and links when finished.
// NB The methods ReplaceCellPoint() and ReplaceLinkedCell() don't seem to work.
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::ChangePointIdInCell(vtkPolyData *polydata, int cellId,  int idold, int idnew) const
{
  bool changedCell = false ;

  vtkCell *cell = polydata->GetCell(cellId) ; // probably very inefficient !
  int n = cell->GetNumberOfPoints() ;

	vtkIdType *ptIds = new vtkIdType[n] ;
  for (int i = 0 ;  i < n ;  i++){
    int id = cell->GetPointId(i) ;
    if (id == idold){
      ptIds[i] = idnew ;
      changedCell = true ;
    }
    else
      ptIds[i] = id ;
  }

  if (changedCell)
    polydata->ReplaceCell(cellId, n, ptIds) ;

  delete [] ptIds ;
  return changedCell ;
}



//------------------------------------------------------------------------------
// Split cells.
// The edges are the point-pairs which must be connected across each cell.
// The cells must be quads or greater.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::SplitCells(vtkPolyData *polydata,  vtkIdList *cellIds,  const EdgeVector& edges) const
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
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::SplitCells(vtkPolyData *polydata,  vtkIdList *cellIds,  vtkIdList *ptIds) const
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
// Split cells. 
// Many cutting lines per cell, defined by list of point pairs. 
// This does not create new points nor affect neighbouring cells. 
// The edges are the point-pairs which must be connected across each cell. 
// The cells must be quads or greater.  For triangles, add the extra points first, then split.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::SplitCells(vtkPolyData *polydata,  vtkIdList *cellIds,  std::vector<EdgeVector>& edgeLists) const
{
  const int MaxSubCells = 10 ;

  // allocate list of subcells
  std::vector<vtkIdList*> subCells ;
  for (int i = 0 ;  i < MaxSubCells ;  i++){
    vtkIdList* tmp = vtkIdList::New() ;
    tmp->Initialize() ;
    subCells.push_back(tmp) ;
  }

  for (int i = 0 ;  i < cellIds->GetNumberOfIds() ;  i++){
    int numSubCells ;
    bool ok = FindSubCellsOfCell(polydata, i, edgeLists[i], subCells, MaxSubCells, numSubCells) ;
    assert(ok) ;

    // create new cells from the list of subcells
    for (int j = 0 ;  j < numSubCells ;  j++)
      CreateNewCell(polydata, cellIds->GetId(i), subCells[j]) ;
  }

  // delete the invalid links
  polydata->DeleteCells() ;
  polydata->DeleteLinks() ;

  // delete the original cells
  this->DeleteCells(polydata, cellIds) ;

  // free allocated memory
  for (int i = 0 ;  i < MaxSubCells ;  i++)
    subCells[i]->Delete() ;
}



//------------------------------------------------------------------------------
// Find the subcells of a cell which is transected by a list of point pairs. \n
// Subcells returned as a vector of vtkIdList pointers. (Must be pre-allocated). \n
// This does not affect the polydata.
// Returns true if sucessful.
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::FindSubCellsOfCell(
  vtkPolyData *polydata, int cellId, 
  const EdgeVector& edges, 
  std::vector<vtkIdList*>& outputCells, 
  int maxSubCells, int& numOutputCells) const
{
  numOutputCells = 0 ;

  //----------------------------------------------
  // init list of input cell points
  //----------------------------------------------
  vtkIdList* inputCellIds = vtkIdList::New() ;

  if ((int)edges.size() == 0)
    return false ;

  polydata->GetCellPoints(cellId, inputCellIds) ;

  int n = inputCellIds->GetNumberOfIds() ;
  assert(n >= 4) ;


  //---------------------------------------------------------------------
  // empty output lists of any previous data
  //---------------------------------------------------------------------
  for (int i = 0 ;  i < maxSubCells ;  i++)
    outputCells[i]->Reset() ;


  //---------------------------------------------------------------------
  // Go round cell, counting how many edges are on each point
  //---------------------------------------------------------------------
  int *count = new int[n] ;
  for (int i = 0 ;  i < n ;  i++){
    count[i] = 0 ;
    int ptId = inputCellIds->GetId(i) ;
    for (int j = 0 ;  j < (int)edges.size() ;  j++){
      if (edges[j].ContainsPoint(ptId))
        count[i]++ ;
    }
  }

  // Set start point to first which is not joined to an edge
  int startPtCellIndex = -1 ;
  bool found = false ;
  for (int i = 0 ;  i < n && !found ;  i++){
    if (count[i] == 0){
      startPtCellIndex = i ;
      found = true ;
    }
  }

  if (!found){
    // couldn't find start point - possibly due to crossed edges
    inputCellIds->Delete() ;
    delete [] count ;
    return false ;
  }


  //---------------------------------------------------------------------
  // Go round the input cell in opposite directions, left and right, from the start point.
  // Each list stops when it reaches a point connected to an edge.
  // The left and right points should now match an edge in the list, asuming the edges don't cross.
  // The left and right lists then form a subcell.
  // Continue until the lists meet.
  //---------------------------------------------------------------------

  vtkIdList* leftList = vtkIdList::New() ;
  vtkIdList* rightList = vtkIdList::New() ;
  leftList->Initialize() ;
  rightList->Initialize() ;

  bool ok = true ;
  bool finished = false ;

  // these variables mark where each side has got up to
  int leftIndex = Modulo(startPtCellIndex,n) ;
  int rightIndex = Modulo(startPtCellIndex-1,n) ;

  for (int subCellId = 0 ;  !finished && ok && subCellId < maxSubCells ;  subCellId++){
    int i, j, leftPtId, rightPtId ;
    bool istop, jstop ;

    // create the left list
    leftList->Reset() ;
    for (i = 0, istop = false ;  i < n && !istop && !finished;  i++){
      int ii = Modulo(leftIndex+i,n) ;
      leftList->InsertNextId(ii) ;

      if ((count[ii] > 0) || (ii == rightIndex)){
        leftIndex = ii ;
        leftPtId = inputCellIds->GetId(ii) ;
        count[ii]-- ;
        istop = true ;
      }
    }

    finished = (leftIndex == rightIndex) ;

    // create the right list 
    rightList->Reset() ;
    for (j = 0, jstop = false ;  j < n && !jstop && !finished ;  j++){
      int jj = Modulo(rightIndex-j,n) ;
      rightList->InsertNextId(jj) ;

      if ((count[jj] > 0)|| (jj == leftIndex)){
        rightIndex = jj ;
        rightPtId = inputCellIds->GetId(jj) ;
        count[jj]-- ;
        jstop = true ;
      }
    }

    finished = (leftIndex == rightIndex) ;

    // error check - should not exceed max no. of subcells
    ok &= (subCellId < maxSubCells) ;

    // error check - ends should belong to an edge which is actually in the list
    if (!finished){
      Edge testEdge(leftPtId, rightPtId) ;
      bool foundTestEdge = false ;
      for (int i = 0 ;  i < (int)edges.size() && !foundTestEdge ;  i++)
        foundTestEdge = edges[i] == testEdge ;

      ok &= foundTestEdge ;
    }

    if (ok){
      // copy the left and right lists to the current sub cell
      vtkIdList* outputSubCell = outputCells[subCellId] ;
      ReverseIdList(rightList) ;
      outputSubCell->Initialize() ;
      this->AppendIdList(outputSubCell, leftList) ;
      this->AppendIdList(outputSubCell, rightList) ;
      numOutputCells++ ;
    }
  }

  inputCellIds->Delete() ;
  delete [] count ;
  leftList->Delete() ;
  rightList->Delete() ;
  return ok ;
}



//------------------------------------------------------------------------------
// Subdivide triangular cells. \n
// Each listed cell is split into 4, \n
// and the neighbours of the listed cells are triangulated as required.
// The list of input cellId's is overwritten by the list of new cells.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::SubdivideCells(vtkPolyData *polydata,  vtkIdList *cellIds) const
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
    if (GetNumberOfPointsOnCell(polydata, i) > 3){
      int n = GetNumberOfPointsOnCell(polydata, i) ;
      cellIds->InsertNextId(i) ;
    }
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
    // Note the angular tolerance - cell mustn't be too sharp
    sidePtsOnCell->Initialize() ;
    GetPointsOnStraightSidesOfCell(polydata, cellId, sidePtsOnCell, 0.001) ;

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
// Subdivide all cells in mesh (triangles only)
// Each cell is split into 4.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::SubdivideAllCells(vtkPolyData *polydata) const
{
  vtkIdList *cellIds = vtkIdList::New() ;
  SetIdListToAllCells(polydata, cellIds) ;
  SubdivideCells(polydata, cellIds) ;
  cellIds->Delete() ;
}



//------------------------------------------------------------------------------
// Merge points.
// The points in the input lists are merged and assigned the new ids in the output list.
// Lambda controls the interpolation:
//  0 or 1 sets the new point equal to one or other of the input points.
//  0.5 sets the new point to half-way between the two.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::MergePoints(vtkPolyData *polydata, vtkIdList *idsIn0,  vtkIdList *idsIn1,  
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
  clean->SetInputData(polydata) ;
  clean->Update();
  polydata->DeepCopy(clean->GetOutput()) ;
  clean->Delete() ;


  //----------------------------------------------------------------------------
  // free memory
  //----------------------------------------------------------------------------
  cellsToModify->Delete() ;

}


//------------------------------------------------------------------------------
// Remove isolated points which are not part of cells.
// This does not remove points which are vertex cells.
//------------------------------------------------------------------------------
void vtkALBAPolyDataNavigator::RemovePointsWithNoCells(vtkPolyData *polydata) const
{
  vtkIdList *ptsNoCells = vtkIdList::New() ;
  GetPointsWithNoCells(polydata, ptsNoCells) ;
  MovePointIdsToEnd(polydata, ptsNoCells) ; // DeletePoints() needs points at the end
  DeletePoints(polydata, ptsNoCells) ;
  ptsNoCells->Delete() ;

  // Delete the invalid links and cells
  polydata->DeleteCells() ;
  polydata->DeleteLinks() ;
}




//------------------------------------------------------------------------------
// Modulo operator, same as % but works correctly on negative values of n as well
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::Modulo(int i, int n) const
{
  if (i >= 0)
    return i % n ;
  else
    return (n-1) - (-(i+1) % n) ;
}





//------------------------------------------------------------------------------
// Edge:
// Are edges joined by at least one point
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::Edge::IsJoined(const Edge& edge) const
{
  return (
    ((edge.m_Id0 == m_Id0) || (edge.m_Id1 == m_Id1)) || 
    ((edge.m_Id0 == m_Id1) || (edge.m_Id1 == m_Id0))
    );
} 


//------------------------------------------------------------------------------
// Edge:
// Equals operator
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::Edge::operator==(const Edge& edge) const
{
  return (
    ((edge.m_Id0 == m_Id0) && (edge.m_Id1 == m_Id1)) || 
    ((edge.m_Id0 == m_Id1) && (edge.m_Id1 == m_Id0))
    );
} 


//------------------------------------------------------------------------------
// Edge:
// Not equals operator
//------------------------------------------------------------------------------
bool vtkALBAPolyDataNavigator::Edge::operator!=(const Edge& edge) const
{
  return !(
    ((edge.m_Id0 == m_Id0) && (edge.m_Id1 == m_Id1)) || 
    ((edge.m_Id0 == m_Id1) && (edge.m_Id1 == m_Id0))
    );
} 



//------------------------------------------------------------------------------
// Edge:
// Calculate a key for locating edge in a map
//------------------------------------------------------------------------------
int vtkALBAPolyDataNavigator::Edge::GenerateKey() const
{
  // key =10*max(id0,id1)^2 + min(id0,id1)^2
  int key ;
  if (m_Id0 >= m_Id1)
    key = 10*m_Id0*m_Id0 + m_Id1*m_Id1 ; // key is not quite unique, but collisions should be very rare
  else
    key = 10*m_Id1*m_Id1 + m_Id0*m_Id0 ;

  return key ;
}


