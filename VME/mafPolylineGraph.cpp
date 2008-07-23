/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPolylineGraph.cpp,v $
Language:  C++
Date:      $Date: 2008-07-23 12:21:05 $
Version:   $Revision: 1.7 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafPolylineGraph.h"
#include "vtkSystemIncludes.h"
#include "vtkPolydata.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkCell.h"
#include "vtkIdType.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "mafIndent.h"
#include "wx/wx.h"
#include <ostream>
#include <vector>


//-------------------------------------------------------------------------
// CONTENTS
//
// Vertex::Vertex()
// Vertex::Vertex(coords)
// Vertex::~Vertex()
// Vertex::GetDegree()
// Vertex::GetEdgeId()
// Vertex::SetEdgeId()
// Vertex::AddEdgeId()
// Vertex::FindEdgeId()
// Vertex::DeleteEdgeId()
// Vertex::GetVertexId()
// Vertex::SetVertexId()
// Vertex::AddVertexId()
// Vertex::FindVertexId()
// Vertex::DeleteVertexId()
// Vertex::DeleteVerticesIdList()
// Vertex::GetCoords()
// Vertex::SetCoords()
// Vertex::Clear()
// Vertex::SelfCheck()
// Vertex::PrintSelf()
// 
// Edge::Edge()
// Edge::Edge(v0,v1)
// Edge::Edge(Edge)
// Edge::~Edge()
// Edge::operator=()
// Edge::GetVertexId()
// Edge::SetVertexId()
// Edge::SetVertexIds()
// Edge::GetVerticesIdList()
// Edge::GetOtherEndVertexId()
// Edge::GetBranchId()
// Edge::SetBranchId()
// Edge::IsVertexPair()
// Edge::IsVertexPairOrdered()
// Edge::SetDirected()
// Edge::IsDirected()
// Edge::ReverseDirection()
// Edge::GetWeight()
// Edge::SetWeight()
// Edge::GetMappingToOutputPolydata()
// Edge::SetMappingToOutputPolydata()
// Edge::SelfCheck()
// Edge::PrintSelf()
// 
// Branch::Branch()
// Branch::Branch(v)
// Branch::~Branch()
// Branch::GetNumberOfVertices()
// Branch::GetNumberOfEdges()
// Branch::GetName(*name)
// Branch::GetName()
// Branch::SetName()
// Branch::UnsetName()
// Branch::GetEdgeId()
// Branch::SetEdgeId()
// Branch::AddEdgeId()
// Branch::FindEdgeId()
// Branch::InsertEdgeId()
// Branch::GetLastEdgeId()
// Branch::GetVertexId()
// Branch::SetVertexId()
// Branch::AddVertexId()
// Branch::FindVertexId()
// Branch::InsertVertexId()
// Branch::GetLastVertexId()
// Branch::DeletelastVertex()
// Branch::GetVerticesIdList()
// Branch::ReverseDirection()
// Branch::GetMappingToOutputPolydata() 
// Branch::SetMappingToOutputPolydata()
// Branch::Clear()
// Branch::SelfCheck()
// Branch::PrintSelf()
// 
// mafPolylineGraph::mafPolylineGraph()
// mafPolylineGraph::~mafPolylineGraph()
// mafPolylineGraph::GetNumberOfVertices()
// mafPolylineGraph::GetNumberOfEdges()
// mafPolylineGraph::GetNumberOfBranches()
// mafPolylineGraph::GetMaxVertexId()
// mafPolylineGraph::GetMaxEdgeId()
// mafPolylineGraph::GetMaxBranchId()
// mafPolylineGraph::GetVertexCoords()
// mafPolylineGraph::SetVertexCoords()
// mafPolylineGraph::GetBranchName()
// mafPolylineGraph::SetBranchName()
// mafPolylineGraph::UnsetBranchName()
// mafPolylineGraph::FindBranchName()
// mafPolylineGraph::IsEdgeDirected()
// mafPolylineGraph::SetEdgeDirected()
// mafPolylineGraph::ReverseEdge()
// mafPolylineGraph::GetEdgeWeight()
// mafPolylineGraph::SetEdgeWeight()
// mafPolylineGraph::AddNewEdge()
// mafPolylineGraph::AddNewVertex()
// mafPolylineGraph::AddNewVertex(v0)
// mafPolylineGraph::AddNewBranch()
// mafPolylineGraph::AddNewBranch(v0)
// mafPolylineGraph::CopyFromPolydata()
// mafPolylineGraph::CopyToPolydata()
// mafPolylineGraph::GetOutputCellCorrespondingToEdge()
// mafPolylineGraph::GetEdgeCorrespondingToOutputCell()
// mafPolylineGraph::SplitBranchAtEdge()
// mafPolylineGraph::SplitBranchAtVertex()
// mafPolylineGraph::AddNewVertexToBranch()
// mafPolylineGraph::AddExistingVertexToBranch()
// mafPolylineGraph::AddExistingEdgeToBranch()
// mafPolylineGraph::ReverseBranch()
// mafPolylineGraph::DeleteEdge()
// mafPolylineGraph::DeleteVertex()
// mafPolylineGraph::DeleteBranch()
// mafPolylineGraph::IsConnected()
// mafPolylineGraph::IsConnectedTree()
// mafPolylineGraph::Clear() 
// mafPolylineGraph::SelfCheck() 
// mafPolylineGraph::PrintSelf()
// mafPolylineGraph::GetConstVertexPtr()
// mafPolylineGraph::GetConstEdgePtr()
// mafPolylineGraph::GetConstBranchPtr()
// mafPolylineGraph::AllocateVertices()
// mafPolylineGraph::AllocateEdges()
// mafPolylineGraph::AllocateBranches()
// mafPolylineGraph::SwapVertexIndices()
// mafPolylineGraph::SwapEdgeIndices()
// mafPolylineGraph::SwapBranchIndices()
// mafPolylineGraph::DeleteLastEdge()
// mafPolylineGraph::DeleteLastVertex()
// mafPolylineGraph::DeleteLastBranch()
// mafPolylineGraph::GetVertexPtr()
// mafPolylineGraph::GetEdgePtr()
// mafPolylineGraph::GetBranchPtr()
// 
//-------------------------------------------------------------------------



//-------------------------------------------------------------------------
// Vertex Constructor
mafPolylineGraph::Vertex::Vertex()
//-------------------------------------------------------------------------
{
  m_coords[0] = 0.0 ;
  m_coords[1] = 0.0 ;
  m_coords[2] = 0.0 ;
}


//-------------------------------------------------------------------------
// Vertex Constructor
mafPolylineGraph::Vertex::Vertex(double *coords)
//-------------------------------------------------------------------------
{
  m_coords[0] = coords[0] ;
  m_coords[1] = coords[1] ;
  m_coords[2] = coords[2] ;
}

//-------------------------------------------------------------------------
// Vertex Destructor
mafPolylineGraph::Vertex::~Vertex()
//-------------------------------------------------------------------------
{
  edgeId.clear() ;
  vertexId.clear() ;
}


//-------------------------------------------------------------------------
// Get degree of vertex
int mafPolylineGraph::Vertex::GetDegree() const
//-------------------------------------------------------------------------
{
  return (int)edgeId.size() ;
}


//-------------------------------------------------------------------------
// Get ith edge
vtkIdType mafPolylineGraph::Vertex::GetEdgeId(int i) const
//-------------------------------------------------------------------------
{
  return edgeId.at(i) ;
}


//-------------------------------------------------------------------------
// Set ith edge
void mafPolylineGraph::Vertex::SetEdgeId(int i, vtkIdType e)
//-------------------------------------------------------------------------
{
  edgeId.at(i) = e ;
}

//-------------------------------------------------------------------------
// Add new vertex id to list
void mafPolylineGraph::Vertex::AddEdgeId(vtkIdType e)
//-------------------------------------------------------------------------
{
  edgeId.push_back(e) ;
}


//-------------------------------------------------------------------------
// Find position of edge in list
int mafPolylineGraph::Vertex::FindEdgeId(vtkIdType e) const
//-------------------------------------------------------------------------
{
  for (int i = 0 ;  i < GetDegree() ;  i++){
    if (GetEdgeId(i) == e)
      return i ;  // found - return position
  }
  return UndefinedInt ;     // not found - return UndefinedInt
}



//-------------------------------------------------------------------------
// Delete this edge index and the associated vertex from the vertex
bool mafPolylineGraph::Vertex::DeleteEdgeId(vtkIdType e)
//-------------------------------------------------------------------------
{
  // find the position of the edge on the vertex
  int j = FindEdgeId(e) ;
  if (j == UndefinedInt){
    mafLogMessage("Vertex::DeleteEdgeId() can't find edge index %d", e) ;
    return false ;
  }

  // copy the end of the list to position j and delete the end of the list
  edgeId.at(j) = edgeId.back() ;
  edgeId.pop_back() ;

  vertexId.at(j) = vertexId.back() ;
  vertexId.pop_back() ;

  return true ;
}


//-------------------------------------------------------------------------
// Get ith vertex
vtkIdType mafPolylineGraph::Vertex::GetVertexId(int i) const
//-------------------------------------------------------------------------
{
  return vertexId.at(i) ;
}


//-------------------------------------------------------------------------
// Set ith vertex
void mafPolylineGraph::Vertex::SetVertexId(int i, vtkIdType v)
//-------------------------------------------------------------------------
{
  vertexId.at(i) = v ;
}


//-------------------------------------------------------------------------
// Add new vertex id to list
void mafPolylineGraph::Vertex::AddVertexId(vtkIdType v)
//-------------------------------------------------------------------------
{
  vertexId.push_back(v) ;
}



//-------------------------------------------------------------------------
// Find position of vertex in list
int mafPolylineGraph::Vertex::FindVertexId(vtkIdType v) const
//-------------------------------------------------------------------------
{
  for (int i = 0 ;  i < GetDegree() ;  i++){
    if (GetVertexId(i) == v)
      return i ;  // found - return position
  }

  return UndefinedInt ;     // not found - return UndefinedInt
}




//-------------------------------------------------------------------------
// Delete this vertex index and the associated edge from the vertex
bool mafPolylineGraph::Vertex::DeleteVertexId(vtkIdType v)
//-------------------------------------------------------------------------
{
  // find the position of the edge on the vertex
  int j = FindVertexId(v) ;
  if (j == UndefinedInt){
    mafLogMessage("Vertex::DeleteVertexId() can't find vertex index %d", v) ;
    return false ;
  }

  // copy the end of the list to position j and delete the end of the list
  edgeId.at(j) = edgeId.back() ;
  edgeId.pop_back() ;

  vertexId.at(j) = vertexId.back() ;
  vertexId.pop_back() ;

  return true ;
}


//-------------------------------------------------------------------------
// return vertices as a vtkIdlist
void mafPolylineGraph::Vertex::GetVerticesIdList(vtkIdList *idlist) const
//-------------------------------------------------------------------------
{
  idlist->Initialize() ;

  for (int i = 0 ;  i < GetDegree() ;  i++)
    idlist->InsertNextId(GetVertexId(i)) ;
}



//-------------------------------------------------------------------------
// Get vertex coords
void mafPolylineGraph::Vertex::GetCoords(double *coords) const
//-------------------------------------------------------------------------
{
  coords[0] = m_coords[0] ;
  coords[1] = m_coords[1] ;
  coords[2] = m_coords[2] ;
}


//-------------------------------------------------------------------------
// Set vertex coords
void mafPolylineGraph::Vertex::SetCoords(const double* coords)
//-------------------------------------------------------------------------
{
  m_coords[0] = coords[0] ;
  m_coords[1] = coords[1] ;
  m_coords[2] = coords[2] ;
}


//-------------------------------------------------------------------------
// Clear data from vertex
void mafPolylineGraph::Vertex::Clear()
//-------------------------------------------------------------------------
{
  edgeId.clear() ;
  vertexId.clear() ;
}


//-------------------------------------------------------------------------
// Test internal consistency of Vertex
// This currently does not allow repeated vertices on a vertex
bool mafPolylineGraph::Vertex::SelfCheck() const
//-------------------------------------------------------------------------
{
  // arrays should be of the same length
  if (edgeId.size() != vertexId.size()){
    mafLogMessage("mismatched list sizes in vertex %d %d", edgeId.size(), vertexId.size()) ;
    return false ;
  }

  // edges on vertex must not contain repeats
  for (int i = 0 ;  i < GetDegree()-1 ;  i++){
    for (int j = i+1 ;  j < GetDegree() ;  j++){
      if (GetEdgeId(i) == GetEdgeId(j)){
        mafLogMessage("repeated edge %d at %d and %d on vertex", GetEdgeId(i), i, j) ;
        return false ;
      }
    }
  }

  // vertices on vertex must not contain repeats
  for (int i = 0 ;  i < GetDegree()-1 ;  i++){
    for (int j = i+1 ;  j < GetDegree() ;  j++){
      if (GetVertexId(i) == GetVertexId(j)){
        mafLogMessage("repeated vertex %d at %d and %d on vertex", GetVertexId(i), i, j) ;
        return false ;
      }
    }
  }

  return true ;
}


//-------------------------------------------------------------------------
// Print vertex
void mafPolylineGraph::Vertex::PrintSelf(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  int i ;

  mafIndent indent(tabs);

  os << indent << "edges " ;
  for (i = 0 ;  i < GetDegree() ;  i++){
    os << GetEdgeId(i) << " " ;
  }

  os << "  vertices " ;
  for (i = 0 ;  i < GetDegree() ;  i++){
    os << GetVertexId(i) << " " ;
  }

  double coords[3] ;
  GetCoords(coords) ;
  os << " coords " << coords[0] << " " << coords[1] << " " << coords[2] ;

  os << std::endl ;
}




//-------------------------------------------------------------------------
// constructor (sets default values)
mafPolylineGraph::Edge::Edge() : directed(false), weight(1.0), branchId(UndefinedId), m_OutputPolydataCell(UndefinedId), m_OutputPolydataCellIndex(UndefinedId)
//-------------------------------------------------------------------------
{
  vertexId[0] = UndefinedId ;
  vertexId[1] = UndefinedId ;
}


//-------------------------------------------------------------------------
// constructor with end vertices
mafPolylineGraph::Edge::Edge(vtkIdType v0, vtkIdType v1) : directed(false), weight(1.0), branchId(UndefinedId), m_OutputPolydataCell(UndefinedId), m_OutputPolydataCellIndex(UndefinedId)
//-------------------------------------------------------------------------
{
  vertexId[0] = v0 ;
  vertexId[1] = v1 ;
}


//-------------------------------------------------------------------------
// copy constructor for edge
mafPolylineGraph::Edge::Edge(const mafPolylineGraph::Edge& e)                      
//-------------------------------------------------------------------------
{
  vertexId[0] = e.vertexId[0] ;
  vertexId[1] = e.vertexId[1] ;
  directed = e.directed ;
  weight = e.weight ;
  branchId = e.branchId ;
}


//-------------------------------------------------------------------------
// destructor
mafPolylineGraph::Edge::~Edge()
//-------------------------------------------------------------------------
{
}


//-------------------------------------------------------------------------
// assignment operator for edge
mafPolylineGraph::Edge& mafPolylineGraph::Edge::operator=(const mafPolylineGraph::Edge& e)                      
//-------------------------------------------------------------------------
{
  vertexId[0] = e.vertexId[0] ;
  vertexId[1] = e.vertexId[1] ;
  directed = e.directed ;
  weight = e.weight ;
  branchId = e.branchId ;

  return *this ;
}

//-------------------------------------------------------------------------
// get index of vertex i, where i = 0 or 1
vtkIdType mafPolylineGraph::Edge::GetVertexId(int i) const
//-------------------------------------------------------------------------
{
  return vertexId[i] ;
}

//-------------------------------------------------------------------------
// set index of vertex, where i = 0 or 1
void mafPolylineGraph::Edge::SetVertexId(int i, vtkIdType v)
//-------------------------------------------------------------------------
{
  vertexId[i] = v ;
}

//-------------------------------------------------------------------------
// set indices of vertices
void mafPolylineGraph::Edge::SetVertexIds(vtkIdType v0, vtkIdType v1)
//-------------------------------------------------------------------------
{
  vertexId[0] = v0 ;
  vertexId[1] = v1 ;
}


//-------------------------------------------------------------------------
// return vertices as a vtkIdlist
void mafPolylineGraph::Edge::GetVerticesIdList(vtkIdList *idlist) const
//-------------------------------------------------------------------------
{
  idlist->Initialize() ;

  for (int i = 0 ;  i < 2 ;  i++)
    idlist->InsertNextId(GetVertexId(i)) ;
}



//-------------------------------------------------------------------------
// get vertex id at other end of edge
vtkIdType mafPolylineGraph::Edge::GetOtherEndVertexId(vtkIdType v) const
//-------------------------------------------------------------------------
{
  if (GetVertexId(0) == v)
    return GetVertexId(1) ;
  else if (GetVertexId(1) == v)
    return GetVertexId(0) ;
  else
    return UndefinedId ;
}


//-------------------------------------------------------------------------
// get branch which edge belongs to
vtkIdType mafPolylineGraph::Edge::GetBranchId() const
//-------------------------------------------------------------------------
{
  return branchId ;
}

//-------------------------------------------------------------------------
// set branch which edge belongs to
void mafPolylineGraph::Edge::SetBranchId(vtkIdType b)
//-------------------------------------------------------------------------
{
  branchId = b ;
}

//-------------------------------------------------------------------------
// is the edge equal to these vertices (order doesn't matter)
bool mafPolylineGraph::Edge::IsVertexPair(vtkIdType v0, vtkIdType v1) const
//-------------------------------------------------------------------------
{
  return (vertexId[0] == v0 && vertexId[1] == v1) || (vertexId[0] == v1 && vertexId[1] == v0) ;
}


//-------------------------------------------------------------------------
// is the edge equal to these vertices in this order
bool mafPolylineGraph::Edge::IsVertexPairOrdered(vtkIdType v0, vtkIdType v1) const
//-------------------------------------------------------------------------
{
  return (vertexId[0] == v0 && vertexId[1] == v1) ;
}


//-------------------------------------------------------------------------
// Set/unset directed property
void mafPolylineGraph::Edge::SetDirected(bool dir)
//-------------------------------------------------------------------------
{
  directed = dir ;
}


//-------------------------------------------------------------------------
// is the edge directed
bool mafPolylineGraph::Edge::IsDirected() const
//-------------------------------------------------------------------------
{
  return directed ;
}



//-------------------------------------------------------------------------
// reverse direction of edge by exchanging vertices
void mafPolylineGraph::Edge::ReverseDirection()
//-------------------------------------------------------------------------
{
  vtkIdType v0, v1 ;
  v0 = GetVertexId(0) ;
  v1 = GetVertexId(1) ;
  SetVertexId(0, v1) ;
  SetVertexId(1, v0) ;
}


//-------------------------------------------------------------------------
// get weight
double mafPolylineGraph::Edge::GetWeight() const
//-------------------------------------------------------------------------
{
  return weight ;
}

//-------------------------------------------------------------------------
// set weight
void mafPolylineGraph::Edge::SetWeight(double w)
//-------------------------------------------------------------------------
{
  weight = w ;
}



//-------------------------------------------------------------------------
// get mapping from edge to location in output
void mafPolylineGraph::Edge::GetMappingToOutputPolydata(vtkIdType *cellid, vtkIdType *index) const
//-------------------------------------------------------------------------
{
  *cellid = m_OutputPolydataCell ;
  *index = m_OutputPolydataCellIndex ;
}


//-------------------------------------------------------------------------
// set mapping from edge to location in output
void mafPolylineGraph::Edge::SetMappingToOutputPolydata(vtkIdType cellid, vtkIdType index)
//-------------------------------------------------------------------------
{
  m_OutputPolydataCell = cellid ;
  m_OutputPolydataCellIndex = index ;
}



//-------------------------------------------------------------------------
// check self consistency of edge
bool mafPolylineGraph::Edge::SelfCheck() const                  
//-------------------------------------------------------------------------
{
  // edge must not be empty
  if ((vertexId[0] == UndefinedId) || (vertexId[1] == UndefinedId)){
    mafLogMessage("undefined vertex or vertices %d %d on edge", vertexId[0], vertexId[1]) ;
    return false ;
  }

  return true ;
}


//-------------------------------------------------------------------------
// Print edge
void mafPolylineGraph::Edge::PrintSelf(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "vertices= " << GetVertexId(0) << " " << GetVertexId(1) ;

  if (GetBranchId() == UndefinedId)
    os << " branch= " << "undef" ;
  else
    os << " branch= " << GetBranchId() ;

  os << " directed = " << IsDirected() << " weight= " << GetWeight() << std::endl ;
}




//-------------------------------------------------------------------------
// Branch Constructor
// Empty branch with optional name
mafPolylineGraph::Branch::Branch(const wxString *name) : m_OutputPolydataCell(UndefinedId)
//-------------------------------------------------------------------------
{
  if (name != NULL)
    m_name = *name ;
  else
    m_name = UndefinedName ;
}


//-------------------------------------------------------------------------
// Branch Constructor with initial vertex and optional name
mafPolylineGraph::Branch::Branch(vtkIdType v, const wxString *name) : m_OutputPolydataCell(UndefinedId)
//-------------------------------------------------------------------------
{
  // insert first vertex id
  vertexId.push_back(v) ;

  if (name != NULL)
    m_name = *name ;
  else
    m_name = UndefinedName ;
}



//-------------------------------------------------------------------------
// Branch Destructor
mafPolylineGraph::Branch::~Branch()
//-------------------------------------------------------------------------
{
  edgeId.clear() ;
  vertexId.clear() ;
}


//-------------------------------------------------------------------------
// Get number of vertices
int mafPolylineGraph::Branch::GetNumberOfVertices() const
//-------------------------------------------------------------------------
{
  return vertexId.size() ;
}

//-------------------------------------------------------------------------
// Get number of edges
int mafPolylineGraph::Branch::GetNumberOfEdges() const
//-------------------------------------------------------------------------
{
  return edgeId.size() ;
}


//-------------------------------------------------------------------------
// Get name
void mafPolylineGraph::Branch::GetName(wxString *name) const
//-------------------------------------------------------------------------
{
  *name = m_name ;
}

//-------------------------------------------------------------------------
// Get name
const wxString* mafPolylineGraph::Branch::GetName() const
//-------------------------------------------------------------------------
{
  return &m_name ;
}


//-------------------------------------------------------------------------
// Set or reset name
void mafPolylineGraph::Branch::SetName(const wxString *name)
//-------------------------------------------------------------------------
{
  if (name == NULL){
    mafLogMessage("SetName() called with NULL argument") ;
    assert(false) ;
  }

  m_name = *name ;
}

//-------------------------------------------------------------------------
// Set name to undefined
void mafPolylineGraph::Branch::UnsetName()
//-------------------------------------------------------------------------
{
  m_name = UndefinedName ;
}


//-------------------------------------------------------------------------
// Get ith edge
vtkIdType mafPolylineGraph::Branch::GetEdgeId(int i) const
//-------------------------------------------------------------------------
{
  return edgeId.at(i) ;
}


//-------------------------------------------------------------------------
// Set ith edge
void mafPolylineGraph::Branch::SetEdgeId(int i, vtkIdType e)
//-------------------------------------------------------------------------
{
  edgeId.at(i) = e ;
}


//-------------------------------------------------------------------------
// Add new edge id to end of list
void mafPolylineGraph::Branch::AddEdgeId(vtkIdType e)
//-------------------------------------------------------------------------
{
  edgeId.push_back(e) ;
}

//-------------------------------------------------------------------------
// Find position of edge in list
int mafPolylineGraph::Branch::FindEdgeId(vtkIdType e) const
//-------------------------------------------------------------------------
{
  for (int i = 0 ;  i < GetNumberOfEdges() ;  i++){
    if (GetEdgeId(i) == e)
      return i ;  // found - return position
  }
  return UndefinedInt ;     // not found - return UndefinedInt
}

//-------------------------------------------------------------------------
// Insert edge before position i
void mafPolylineGraph::Branch::InsertEdgeId(int i, vtkIdType e)
//-------------------------------------------------------------------------
{
  edgeId.insert(edgeId.begin()+i, e) ;
}


//-------------------------------------------------------------------------
// Get id of last edge
vtkIdType mafPolylineGraph::Branch::GetLastEdgeId() const
//-------------------------------------------------------------------------
{
  int n = GetNumberOfEdges() ;
  return GetEdgeId(n-1) ;
}


//-------------------------------------------------------------------------
// Get ith vertex
vtkIdType mafPolylineGraph::Branch::GetVertexId(int i) const
//-------------------------------------------------------------------------
{
  return vertexId.at(i) ;
}


//-------------------------------------------------------------------------
// Set ith vertex
void mafPolylineGraph::Branch::SetVertexId(int i, vtkIdType v)
//-------------------------------------------------------------------------
{
  vertexId.at(i) = v ;
}



//-------------------------------------------------------------------------
// Add new vertex id to end of list
void mafPolylineGraph::Branch::AddVertexId(vtkIdType v)
//-------------------------------------------------------------------------
{
  vertexId.push_back(v) ;
}


//-------------------------------------------------------------------------
// Find position of vertex in list
int mafPolylineGraph::Branch::FindVertexId(vtkIdType v) const
//-------------------------------------------------------------------------
{
  for (int i = 0 ;  i < GetNumberOfVertices() ;  i++){
    if (GetVertexId(i) == v)
      return i ;  // found - return position
  }
  return UndefinedInt ;     // not found - return UndefinedInt
}


//-------------------------------------------------------------------------
// Insert vertex before position i
void mafPolylineGraph::Branch::InsertVertexId(int i, vtkIdType v)
//-------------------------------------------------------------------------
{
  vertexId.insert(vertexId.begin()+i, v) ;
}



//-------------------------------------------------------------------------
// Get id of last vertex
vtkIdType mafPolylineGraph::Branch::GetLastVertexId() const
//-------------------------------------------------------------------------
{
  int n = GetNumberOfVertices() ;
  return GetVertexId(n-1) ;
}



//-------------------------------------------------------------------------
// Delete last vertex and edge from branch
void mafPolylineGraph::Branch::DeleteLastVertex()
//-------------------------------------------------------------------------
{
  if (GetNumberOfVertices() > 0)
    vertexId.pop_back() ;

  if (GetNumberOfEdges() > 0)
    edgeId.pop_back() ;
}



//-------------------------------------------------------------------------
// return vertices as a vtkIdlist
void mafPolylineGraph::Branch::GetVerticesIdList(vtkIdList *idlist) const
//-------------------------------------------------------------------------
{
  idlist->Initialize() ;

  for (int i = 0 ;  i < GetNumberOfVertices() ;  i++)
    idlist->InsertNextId(GetVertexId(i)) ;
}



//-------------------------------------------------------------------------
// Reverse direction of branch
void mafPolylineGraph::Branch::ReverseDirection()
//-------------------------------------------------------------------------
{
  vtkIdType tempId ;
  int i, j ;

  for (i = 0, j = edgeId.size()-1 ;  i < j ;  i++, j--){
    tempId = edgeId.at(i) ;
    edgeId.at(i) = edgeId.at(j) ;
    edgeId.at(j) = tempId ;
  }

  for (i = 0, j = vertexId.size()-1 ;  i < j ;  i++, j--){
    tempId = vertexId.at(i) ;
    vertexId.at(i) = vertexId.at(j) ;
    vertexId.at(j) = tempId ;
  }
}




//-------------------------------------------------------------------------
// get mapping from branch to output cell
vtkIdType mafPolylineGraph::Branch::GetMappingToOutputPolydata() const 
//-------------------------------------------------------------------------
{
  return m_OutputPolydataCell ;
}

//-------------------------------------------------------------------------
// set mapping from branch to output cell
void mafPolylineGraph::Branch::SetMappingToOutputPolydata(vtkIdType cellid)
//-------------------------------------------------------------------------
{
  m_OutputPolydataCell = cellid ;
}


//-------------------------------------------------------------------------
// Clear data from branch
void mafPolylineGraph::Branch::Clear()
//-------------------------------------------------------------------------
{
  edgeId.clear() ;
  vertexId.clear() ;
}




//-------------------------------------------------------------------------
// Test internal consistency of Branch
bool mafPolylineGraph::Branch::SelfCheck() const
//-------------------------------------------------------------------------
{
  // no. of vertices and edges should be (0,0) or (nv,nv-1)
  // n.b this assumes that an empty branch is ok.
  if (GetNumberOfVertices() == 0){
    if (GetNumberOfEdges() != 0){
      mafLogMessage("empty branch contains %d edges", GetNumberOfEdges()) ;
      return false ;
    }
  }
  else{
    if (GetNumberOfEdges() != GetNumberOfVertices()-1){
      mafLogMessage("mismatched list lengths in branch: %d vertices %d edges", GetNumberOfVertices(), GetNumberOfEdges()) ;
      return false ;
    }
  }

  // branch must not contain any duplicate edges
  for (int i = 0 ;  i < GetNumberOfEdges()-1 ;  i++){
    for (int j = i+1 ;  j < GetNumberOfEdges() ;  j++){
      if (GetEdgeId(i) == GetEdgeId(j)){
        mafLogMessage("duplicate edge %d found in branch at %d and %d", GetEdgeId(i), i, j) ;
        return false ;
      }
    }
  }


  return true ;
}



//-------------------------------------------------------------------------
// Print branch
void mafPolylineGraph::Branch::PrintSelf(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  int i ;

  mafIndent indent(tabs);

  os << indent << "edges " ;
  for (i = 0 ;  i < GetNumberOfEdges() ;  i++){
    os << GetEdgeId(i) << " " ;
  }

  os << "  vertices " ;
  for (i = 0 ;  i < GetNumberOfVertices() ;  i++){
    os << GetVertexId(i) << " " ;
  }
  
  os << std::endl ;
}










//-------------------------------------------------------------------------
// Constructor
mafPolylineGraph::mafPolylineGraph()
//-------------------------------------------------------------------------
{}



//-------------------------------------------------------------------------
// Destructor
mafPolylineGraph::~mafPolylineGraph()
//-------------------------------------------------------------------------
{
  m_Vertex.clear() ;
  m_Edge.clear() ;
  m_Branch.clear() ;
}


//-------------------------------------------------------------------------
// get number of vertices
int mafPolylineGraph::GetNumberOfVertices() const
//-------------------------------------------------------------------------
{
  return m_Vertex.size() ;
}

//-------------------------------------------------------------------------
// get number of edges
int mafPolylineGraph::GetNumberOfEdges() const
//-------------------------------------------------------------------------
{
  return m_Edge.size() ;
}

//-------------------------------------------------------------------------
// get number of branches
int mafPolylineGraph::GetNumberOfBranches() const
//-------------------------------------------------------------------------
{
  return m_Branch.size() ;
}


//-------------------------------------------------------------------------
// get last vertex id in list
vtkIdType mafPolylineGraph::GetMaxVertexId() const
//-------------------------------------------------------------------------
{
  return (vtkIdType)(m_Vertex.size() - 1) ;
}

//-------------------------------------------------------------------------
// get last edge id in list
vtkIdType mafPolylineGraph::GetMaxEdgeId() const
//-------------------------------------------------------------------------
{
  return (vtkIdType)(m_Edge.size() - 1) ;
}

//-------------------------------------------------------------------------
// get last branch id in list
vtkIdType mafPolylineGraph::GetMaxBranchId() const
//-------------------------------------------------------------------------
{
  return (vtkIdType)(m_Branch.size() - 1) ;
}





//-------------------------------------------------------------------------
// Get the vertex coords
// This allows public access to change the coords of a vertex
void mafPolylineGraph::GetVertexCoords(vtkIdType v, double *coords) const
//-------------------------------------------------------------------------
{
  GetConstVertexPtr(v)->GetCoords(coords) ;
}


//-------------------------------------------------------------------------
// Set the vertex coords
// This allows public access to change the coords of a vertex
void mafPolylineGraph::SetVertexCoords(vtkIdType v, const double *coords)
//-------------------------------------------------------------------------
{
  GetVertexPtr(v)->SetCoords(coords) ;
}


//-------------------------------------------------------------------------
// get name of branch
void mafPolylineGraph::GetBranchName(vtkIdType b, wxString *name) const
//-------------------------------------------------------------------------
{
  GetConstBranchPtr(b)->GetName(name) ;
}

//-------------------------------------------------------------------------
// set name of branch
void mafPolylineGraph::SetBranchName(vtkIdType b, const wxString *name)
//-------------------------------------------------------------------------
{
  GetBranchPtr(b)->SetName(name) ;
}

//-------------------------------------------------------------------------
// unset name of branch to undefined
void mafPolylineGraph::UnsetBranchName(vtkIdType b)
//-------------------------------------------------------------------------
{
  GetBranchPtr(b)->UnsetName() ;
}


//-------------------------------------------------------------------------
// find id of branch with name
// return undefinedId if not found
vtkIdType mafPolylineGraph::FindBranchName(const wxString *name) const
//-------------------------------------------------------------------------
{
  wxString namei ;

  for (int i = 0 ;  i < GetNumberOfBranches() ;  i++){
    GetConstBranchPtr(i)->GetName(&namei) ;
    if (namei == *name)
      return i ;
  }

  return UndefinedId ;
}


//-------------------------------------------------------------------------
// get directed property of edge
bool mafPolylineGraph::IsEdgeDirected(vtkIdType e) const
//-------------------------------------------------------------------------
{
  return GetConstEdgePtr(e)->IsDirected() ;
}

//-------------------------------------------------------------------------
// set directed property of edge
void mafPolylineGraph::SetEdgeDirected(vtkIdType e, bool directed)
//-------------------------------------------------------------------------
{
  GetEdgePtr(e)->SetDirected(directed) ;
}

//-------------------------------------------------------------------------
// reverse direction of edge, ie swap end vertices
void mafPolylineGraph::ReverseEdge(vtkIdType e)
//-------------------------------------------------------------------------
{
  GetEdgePtr(e)->ReverseDirection() ;
}


//-------------------------------------------------------------------------
// get weight of edge
double mafPolylineGraph::GetEdgeWeight(vtkIdType e) const
//-------------------------------------------------------------------------
{
  return GetConstEdgePtr(e)->GetWeight() ;
}

//-------------------------------------------------------------------------
// set weight of edge
void mafPolylineGraph::SetEdgeWeight(vtkIdType e, double wt)
//-------------------------------------------------------------------------
{
  GetEdgePtr(e)->SetWeight(wt) ;
}


//-------------------------------------------------------------------------
// return pointer to vertex
// returns NULL if index is out of range
mafPolylineGraph::Vertex* mafPolylineGraph::GetVertexPtr(vtkIdType i)
//-------------------------------------------------------------------------
{
  if ((i >= 0) && (i < GetNumberOfVertices()))
    return &(m_Vertex.at(i)) ;
  else
    return NULL ;
}

//-------------------------------------------------------------------------
// return pointer to edge
mafPolylineGraph::Edge* mafPolylineGraph::GetEdgePtr(vtkIdType i)
//-------------------------------------------------------------------------
{
  if ((i >= 0) && (i < GetNumberOfEdges()))
    return &(m_Edge.at(i)) ;
  else
    return NULL ;
}


//-------------------------------------------------------------------------
// return pointer to branch
mafPolylineGraph::Branch* mafPolylineGraph::GetBranchPtr(vtkIdType i)
//-------------------------------------------------------------------------
{
  if ((i >= 0) && (i < GetNumberOfBranches()))
    return &(m_Branch.at(i)) ;
  else
    return NULL ;
}



//-------------------------------------------------------------------------
// return pointer to vertex
// returns NULL if index is out of range
const mafPolylineGraph::Vertex* mafPolylineGraph::GetConstVertexPtr(vtkIdType i) const
//-------------------------------------------------------------------------
{
  if ((i >= 0) && (i < GetNumberOfVertices()))
    return &(m_Vertex.at(i)) ;
  else
    return NULL ;
}

//-------------------------------------------------------------------------
// return pointer to edge
const mafPolylineGraph::Edge* mafPolylineGraph::GetConstEdgePtr(vtkIdType i) const
//-------------------------------------------------------------------------
{
  if ((i >= 0) && (i < GetNumberOfEdges()))
    return &(m_Edge.at(i)) ;
  else
    return NULL ;
}

//-------------------------------------------------------------------------
// return pointer to branch
const mafPolylineGraph::Branch* mafPolylineGraph::GetConstBranchPtr(vtkIdType i) const
//-------------------------------------------------------------------------
{
  if ((i >= 0) && (i < GetNumberOfBranches()))
    return &(m_Branch.at(i)) ;
  else
    return NULL ;
}


//-------------------------------------------------------------------------
// initialize or resize list of vertices
void mafPolylineGraph::AllocateVertices(int nv)
//-------------------------------------------------------------------------
{
  Vertex v ;
  m_Vertex.resize(nv, v) ;
}

//-------------------------------------------------------------------------
// initialize or resize list of edges
void mafPolylineGraph::AllocateEdges(int ne)
//-------------------------------------------------------------------------
{
  Edge e ;
  m_Edge.resize(ne, e) ;
}

//-------------------------------------------------------------------------
// initialize or resize list of branches
void mafPolylineGraph::AllocateBranches(int nb)
//-------------------------------------------------------------------------
{
  Branch b ;
  m_Branch.resize(nb, b) ;
}




//-------------------------------------------------------------------------
// Read graph from vtkPolyData (polyline only).
// Each polydata point becomes a graph vertex.
// Each polydata cell (line or polyline) becomes a graph branch.
// Each polydata line or line segment becomes a graph edge.
// The points stay in the same order: point i in the graph is the same as point i in the polydata.
bool mafPolylineGraph::CopyFromPolydata(vtkPolyData *polydata)
//-------------------------------------------------------------------------
{
  vtkIdType i ;
  int j ;

  // check that data is polyline only
  int ncells = polydata->GetNumberOfCells() ;
  int nlines = polydata->GetNumberOfLines() ;
  if (nlines != ncells){
    mafLogMessage("Input data is not polyline") ;
    return false ;
  }

  // Allocate the vertices in the graph.
  // There is a one-to-one correspondence between the vertices in the graph and the points in the polydata.
  vtkPoints *points = polydata->GetPoints() ;
  int npts = points->GetNumberOfPoints() ;
  AllocateVertices(npts) ;

  // Allocate the branches in the graph
  // Every simple line or polyline cell in the polydata creates a branch in the graph,
  // so there is intially a one-to-one correspondence between the branches and the polydata cells.
  AllocateBranches(ncells) ;

  // Allocate the edges in the graph.
  // There is not a one-to-one correspondence between the edges in the graph and the "lines" in the polydata
  // because the cells in the polydata can be simple lines or polylines.
  int nedges ;
  for (i = 0, nedges = 0 ;  i < ncells ;  i++){
    vtkCell* cell = polydata->GetCell(i) ;
    nedges += cell->GetNumberOfPoints() - 1 ;   // no. of edges in the cell must be no. of points - 1
  }
  AllocateEdges(nedges) ;


  // Write the coords of the points to the vertices
  for (j = 0 ;  j < npts ;  j++)
    GetVertexPtr(j)->SetCoords(points->GetPoint(j)) ;


  // Trace each polydata cell.
  // The branches are completed by adding the vertices and edges.
  // The edges are completed by adding vertices and branches.
  vtkIdType edgeIndex ;
  for (i = 0, edgeIndex = 0 ;  i < ncells ;  i++){
    vtkCell* cell = polydata->GetCell(i) ;
    vtkIdList* idlist = cell->GetPointIds() ;

    // copy vertex ids into branch
    for (j = 0 ;  j < idlist->GetNumberOfIds() ;  j++){
      vtkIdType v0 = idlist->GetId(j) ;
      GetBranchPtr(i)->AddVertexId(v0) ;
    }

    for (j = 0 ;  j < idlist->GetNumberOfIds() - 1 ;  j++){
      // get each line segment as a pair of vertices
      vtkIdType v0 = idlist->GetId(j) ;
      vtkIdType v1 = idlist->GetId(j+1) ;

      // copy current edge index into branch
      GetBranchPtr(i)->AddEdgeId(edgeIndex) ;

      // set the vertices and the branch of the current edge
      GetEdgePtr(edgeIndex)->SetVertexIds(v0,v1) ;
      GetEdgePtr(edgeIndex)->SetBranchId(i) ;

      // increment the current edge index
      edgeIndex++ ;
    }
  }


  // Complete vertices by adding the edge and vertex data
  for (i = 0 ;  i < GetNumberOfEdges() ;  i++){
    vtkIdType v0 = GetEdgePtr(i)->GetVertexId(0) ;
    vtkIdType v1 = GetEdgePtr(i)->GetVertexId(1) ;

    // vertex v0 is joined to v1 by edge i
    GetVertexPtr(v0)->AddEdgeId(i) ;
    GetVertexPtr(v0)->AddVertexId(v1) ;

    // vertex v1 is joined to v0 by edge i
    GetVertexPtr(v1)->AddEdgeId(i) ;
    GetVertexPtr(v1)->AddVertexId(v0) ;
  }

  return true ;
}



//-------------------------------------------------------------------------
// Write polyline graph to vtkPolyData
// Each graph vertex becomes a polydata point.
// Each graph branch becomes a line or polyline.
// Edges which are not members of branches become lines.
// The points stay in the same order: point i in the polydata is the same as point i in the graph.
// This also sets the mapping between the graph edges and the cells in the output polydata.
bool mafPolylineGraph::CopyToPolydata(vtkPolyData *polydata)
//-------------------------------------------------------------------------
{
  int i, j ;

  // clear the polydata
  polydata->Initialize() ;

  vtkPoints *points = vtkPoints::New() ;
  vtkCellArray *lines = vtkCellArray::New() ;
  vtkIdList *idlist = vtkIdList::New() ;

  // copy vertices to polydata points
  double coords[3] ;
  for (i = 0 ;  i < GetNumberOfVertices() ;  i++){
    GetConstVertexPtr(i)->GetCoords(coords) ;
    points->InsertNextPoint(coords) ;
  }

  // copy branches to polyline cells
  for (i = 0 ;  i < GetNumberOfBranches() ;  i++){
    if (GetConstBranchPtr(i)->GetNumberOfVertices() > 0){
      GetConstBranchPtr(i)->GetVerticesIdList(idlist) ;
      lines->InsertNextCell(idlist) ;

      // set mapping between branch and output cell
      GetBranchPtr(i)->SetMappingToOutputPolydata(i) ;

      // set mapping between edges and output cells
      for (j = 0 ;  j < GetConstBranchPtr(i)->GetNumberOfEdges() ;  j++){
        vtkIdType edgeid = GetConstBranchPtr(i)->GetEdgeId(j) ;
        GetEdgePtr(edgeid)->SetMappingToOutputPolydata(i, j) ; // this edge is edge j in cell i
      }
    }
  }

  // copy any left-over edges which did not belong to branches
  for (i = 0 ;  i < GetNumberOfEdges() ;  i++){
    vtkIdType b = GetConstEdgePtr(i)->GetBranchId() ;
    if (b == UndefinedId){
      GetConstEdgePtr(i)->GetVerticesIdList(idlist) ;
      vtkIdType cellid = lines->InsertNextCell(idlist) ;

      // map edge to output cell
      GetEdgePtr(i)->SetMappingToOutputPolydata(cellid, 0) ; // this edge is edge 0 in cellid
    }
  }

  // put points and lines into polydata
  polydata->SetPoints(points) ;
  polydata->SetLines(lines) ;

  idlist->Delete() ;
  lines->Delete() ;
  points->Delete() ;

  return true ;
}


//-------------------------------------------------------------------------
// Get location of edge in output polydata
// The mapping is set by CopyToPolydata()
void mafPolylineGraph::GetOutputCellCorrespondingToEdge(vtkIdType edgeid, vtkIdType *cellid, vtkIdType *index) const
//-------------------------------------------------------------------------
{
  GetConstEdgePtr(edgeid)->GetMappingToOutputPolydata(cellid, index) ;

  // undefined values - fatal error
  if ((*cellid == UndefinedId) || (*index == UndefinedId)){
    mafLogMessage("GetOutputCellCorrespondingToEdge() can't find output cell corresponding to edge %d", edgeid) ;
    mafLogMessage("Perhaps CopyToPolydata() has not been called ?") ;
    assert(false) ;
  }
}


//-------------------------------------------------------------------------
// Get edge id in graph which is (cellid,index) in the output polydata
// The mapping is set by CopyToPolydata()
vtkIdType mafPolylineGraph::GetEdgeCorrespondingToOutputCell(vtkIdType cellid, vtkIdType index) const
//-------------------------------------------------------------------------
{
  vtkIdType edgeid, j, k ;

  for (edgeid = 0 ;  edgeid < GetNumberOfEdges() ;  edgeid++){
    GetConstEdgePtr(edgeid)->GetMappingToOutputPolydata(&j, &k) ;
    if ((j == cellid) && (k == index))
      return edgeid ;
  }

  // output cell and index not found in edge mapping - fatal error
  mafLogMessage("GetEdgeCorrespondingToOutputCell() can't find output cell corresponding to edge %d", edgeid) ;
  mafLogMessage("Perhaps CopyToPolydata() has not been called ?") ;
  assert(false) ;
  return -1 ;
}


//-------------------------------------------------------------------------
// Get location of branch in output polydata
// The mapping is set by CopyToPolydata()
void mafPolylineGraph::GetOutputCellCorrespondingToBranch(vtkIdType branchid, vtkIdType *cellid) const
//-------------------------------------------------------------------------
{
  *cellid = GetConstBranchPtr(branchid)->GetMappingToOutputPolydata() ;

  // undefined values - fatal error
  if (*cellid == UndefinedId){
    mafLogMessage("GetOutputCellCorrespondingToBranch() can't find output cell corresponding to branch %d", branchid) ;
    mafLogMessage("Perhaps CopyToPolydata() has not been called ?") ;
    assert(false) ;
  }
}


//-------------------------------------------------------------------------
// Get branch id in graph which corresponds to cellid in the output polydata
// The mapping is set by CopyToPolydata()
// Returns -1 if branch not found.
vtkIdType mafPolylineGraph::GetBranchCorrespondingToOutputCell(vtkIdType cellid) const
//-------------------------------------------------------------------------
{
  vtkIdType branchid, j ;

  for (branchid = 0 ;  branchid < GetNumberOfBranches() ;  branchid++){
    j = GetConstBranchPtr(branchid)->GetMappingToOutputPolydata() ;
    if (j == cellid)
      return branchid ;
  }

  // output cell does not correspond to a branch - return -1 
  return -1 ;
}


//-------------------------------------------------------------------------
// Clear data from graph
void mafPolylineGraph::Clear()
//-------------------------------------------------------------------------
{
  m_Vertex.clear() ;
  m_Edge.clear() ;
  m_Branch.clear() ;
}


//-------------------------------------------------------------------------
// Test self-consistency of graph
bool mafPolylineGraph::SelfCheck() const
//-------------------------------------------------------------------------
{
  vtkIdType v, b, e ;
  int j ;

  // self-check the vertices
  for (v = 0 ;  v < GetNumberOfVertices() ;  v++){
    if (!GetConstVertexPtr(v)->SelfCheck()){
      mafLogMessage("graph self-check error: vertex %d failed self check", v) ;
      return false ;
    }
  }

  // self-check the edges
  for (e = 0 ;  e < GetNumberOfEdges() ;  e++){
    if (!GetConstEdgePtr(e)->SelfCheck()){
      mafLogMessage("graph self-check error: edge %d failed self check", e) ;
      return false ;
    }
  }

  // self-check the branches
  for (b = 0 ;  b < GetNumberOfBranches() ;  b++){
    if (!GetConstBranchPtr(b)->SelfCheck()){
      mafLogMessage("graph self-check error: branch %d failed self check", b) ;
      return false ;
    }
  }


  // check that the sum of the degrees of the vertices is 2 * the number of edges
  int sumdeg ;
  for (v = 0, sumdeg = 0 ;  v < GetNumberOfVertices() ;  v++)
    sumdeg += GetConstVertexPtr(v)->GetDegree() ;
  if (sumdeg != 2*GetNumberOfEdges()){
    mafLogMessage("graph self check error: sum of degrees %d != 2 * no. of edges %d", sumdeg, GetNumberOfEdges()) ;
    return false ;
  }

  // check that the sum of the edges in the branches is equal to the sum of the branches in edges
  int sum1, sum2 ;
  for (b = 0, sum1 = 0 ;  b < GetNumberOfBranches() ;  b++)
    sum1 += GetConstBranchPtr(b)->GetNumberOfEdges() ;
  for (e = 0, sum2 = 0 ;  e < GetNumberOfEdges() ;  e++){
    if (GetConstEdgePtr(e)->GetBranchId() != UndefinedId)
      sum2++ ;
  }
  if (sum1 != sum2){
    mafLogMessage("graph self check error: no. of edges in branches %d != no. of branches in edges %d", sum1, sum2) ;
    return false ;
  }


  // check that the vertices point at the edges consistently
  for (v = 0 ;  v < GetNumberOfVertices() ;  v++){
    const Vertex *vert = GetConstVertexPtr(v) ;

    for (j = 0 ;  j < vert->GetDegree() ;  j++){
      vtkIdType ke = vert->GetEdgeId(j) ;             // ke = index of edge j on vertex i
      vtkIdType kv = vert->GetVertexId(j) ;           // kv = index of vertex j on vertex i

      // indices read from vertex must point to valid objects
      if ((GetConstEdgePtr(ke) == NULL) || (GetConstVertexPtr(kv) == NULL)){
        mafLogMessage("graph self check error: edge index %d or vertex index %d on vertex %d out of range", ke, kv, v) ;
        return false ;
      }

      // edge ke must be the vertex pair (v, kv)
      if (!GetConstEdgePtr(ke)->IsVertexPair(v, kv)){
        mafLogMessage("graph self check error: edge %d is not vertex pair %d %d from vertex %d", ke, v, kv, v) ;
        return false ;
      }
    }
  }


  // check that the edges point at the vertices and branches consistently
  for (e = 0 ;  e < GetNumberOfEdges() ;  e++){
    const Edge *ed = GetConstEdgePtr(e) ;

    for (j = 0 ;  j < 2 ;  j++){
      vtkIdType kv = ed->GetVertexId(j) ;          // kv = index of vertex j on edge i
      vtkIdType kb = ed->GetBranchId() ;           // kb = index of branch on edge i

      // vertices read from edge must point at valid objects
      if (GetConstVertexPtr(kv) == NULL){
        mafLogMessage("graph self check error: vertex index %d out of range in edge %d", kv, e) ;
        return false ;
      }

      // vertex kv must contain this edge
      if (GetConstVertexPtr(kv)->FindEdgeId(e) == UndefinedInt){
        mafLogMessage("graph self check error: vertex %d does not contain edge %d", kv, e) ;
        return false ;
      }

      if (kb != UndefinedId){
        // branch index must be valid
        if (GetConstBranchPtr(kb) == NULL){
          mafLogMessage("graph self check error: branch index %d out of range in edge %d", kb, e) ;
          return false ;
        }

        // branch kb must contain this edge
        if (GetConstBranchPtr(kb)->FindEdgeId(e) == UndefinedInt){
          mafLogMessage("graph self check error: branch %d does not contain edge %d", kb, e) ;
          return false ;
        }
      }
    }
  }


  // check that the branches point at the vertices and edges consistently
  for (b = 0 ;  b < GetNumberOfBranches() ;  b++){
    const Branch *br = GetConstBranchPtr(b) ;

    for (j = 0 ;  j < br->GetNumberOfEdges() ;  j++){
      vtkIdType ke = br->GetEdgeId(j) ;             // ke = index of edge j on branch i

      // indices read from branch must point to valid objects
      if (GetConstEdgePtr(ke) == NULL){
        mafLogMessage("graph self check error: edge index %d out of range in branch %d", ke, b) ;
        return false ;
      }

      // edge ke must point back at this branch
      if (GetConstEdgePtr(ke)->GetBranchId() != b){
        mafLogMessage("graph self check error: edge %d does not point back at branch %d", ke, b) ;
        return false ;
      }
    }

    for (j = 0 ;  j < br->GetNumberOfVertices() ;  j++){
      vtkIdType kv = br->GetVertexId(j) ;             // kv = index of vertex j on branch i

      // indices read from branch must point to valid objects
      if (GetConstVertexPtr(kv) == NULL){
        mafLogMessage("graph self check error: vertex index %d out of range in branch %d", kv, b) ;
        return false ;
      }
    }
  }


  // check that the branches contain vertices and edges which agree with each other
  for (b = 0 ;  b < GetNumberOfBranches() ;  b++){
    const Branch *br = GetConstBranchPtr(b) ;

    for (j = 0 ;  j < br->GetNumberOfVertices()-1 ;  j++){
      vtkIdType kv0 = br->GetVertexId(j) ;          // kv0 = index of vertex j
      vtkIdType kv1 = br->GetVertexId(j+1) ;        // kv1 = index of vertex j+1
      vtkIdType ke = br->GetEdgeId(j) ;             // ke = index of edge j

      // edge ke must join kv0 and kv1
      if (!GetConstEdgePtr(ke)->IsVertexPair(kv0,kv1)){
        mafLogMessage("graph self check error: edge %d in branch %d does not join vertices %d and %d", ke, b, kv0, kv1) ;
        return false ;
      }
    }
  }

  return true ;
}



//-------------------------------------------------------------------------
// Print graph
void mafPolylineGraph::PrintSelf(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  int i ;

  mafIndent indent(tabs);

  os << indent << "Polyline graph" << std::endl ;
  os << indent << "no. of vertices " << GetNumberOfVertices() << std::endl ;
  os << indent << "no. of edges    " << GetNumberOfEdges() << std::endl ;
  os << indent << "no. of branches " << GetNumberOfBranches() << std::endl ;

  os << indent << "Vertices" << std::endl ;
  for (i = 0 ;  i < GetNumberOfVertices() ;  i++){
    os << indent << "  " << i << " " ;
    GetConstVertexPtr(i)->PrintSelf(os, 0) ;
  }

  os << indent << "Edges" << std::endl ;
  for (i = 0 ;  i < GetNumberOfEdges() ;  i++){
    os << indent << "  " << i << " " ;
    GetConstEdgePtr(i)->PrintSelf(os, 0) ;
  }

  os << indent << "Branches" << std::endl ;
  for (i = 0 ;  i < GetNumberOfBranches() ;  i++){
    os << indent << "  " << i << " " ;
    GetConstBranchPtr(i)->PrintSelf(os, 0) ;
  }

  os << std::endl << std::endl ;
}




//-------------------------------------------------------------------------
// Is graph connected
bool mafPolylineGraph::IsConnected() const
//-------------------------------------------------------------------------
{
  int i, j ;
  unsigned int ii, nfound ;
  std::vector<vtkIdType> vertexlist0, vertexlist1 ;      // 2 lists of vertices connected to start vertex
  bool *flag ;                                           // table of flags - true if vertex has been seen


  // initialize table of flags
  int npts = GetNumberOfVertices() ;
  flag = new bool[npts] ;
  for (i = 0 ;  i < npts ; i++)
    flag[i] = false ;

  // initialize old list with start point 0
  vertexlist0.push_back(0) ;
  flag[0] = true ;
  nfound = 1 ;


  do{
    // list the (unvisited) neighbours of all the vertices currently in vertexlist0
    vertexlist1.clear() ;
    for (ii = 0 ;  ii < vertexlist0.size() ;  ii++){
      vtkIdType vi = vertexlist0[ii] ;   // index of current vertex

      for (j = 0 ;  j < GetConstVertexPtr(vi)->GetDegree() ;  j++){
        vtkIdType vj = GetConstVertexPtr(vi)->GetVertexId(j) ;  // index of jth neighbour

        // if vertex not visited, add to list and set flag
        if (!flag[vj]){
          vertexlist1.push_back(vj) ;
          flag[vj] = true ;
        }
      }
    }

    // add to total found so far
    nfound += vertexlist1.size() ;

    // copy new list back to old list
    vertexlist0 = vertexlist1 ;

  } while(vertexlist1.size() > 0) ;  // stop when no more vertices are found


  // clear memory
  delete [] flag ;
  vertexlist0.clear() ;
  vertexlist1.clear() ;

  // graph is connected if no. of connected vertices is equal to total
  return (nfound == npts) ;
}


//-------------------------------------------------------------------------
// Is graph a connected tree
bool mafPolylineGraph::IsConnectedTree() const
//-------------------------------------------------------------------------
{
  if (!IsConnected())
    return false ;

  return (GetNumberOfEdges() == GetNumberOfVertices()-1) ;
}





//-------------------------------------------------------------------------
// Add a new edge between existing vertices
bool mafPolylineGraph::AddNewEdge(vtkIdType v0, vtkIdType v1)
//-------------------------------------------------------------------------
{
  // check that input indices are in range
  Vertex *vert0 = GetVertexPtr(v0) ;
  Vertex *vert1 = GetVertexPtr(v1) ;
  if ((vert0 == NULL) || (vert1 == NULL)){
    mafLogMessage("AddNewEdge() failed: vertex indices %d %d out of range", v0, v1) ;
    return false ;
  }

  // construct new edge with given vertices and add to graph
  Edge ed(v0,v1) ;
  m_Edge.push_back(ed) ;

  // get edge index and add to vertices
  vtkIdType e = GetMaxEdgeId() ;
  vert0->AddEdgeId(e) ;
  vert0->AddVertexId(v1) ;
  vert1->AddEdgeId(e) ;
  vert1->AddVertexId(v0) ;

  return true ;
}



//-------------------------------------------------------------------------
// Add a new empty vertex to the graph
void mafPolylineGraph::AddNewVertex(double *coords)
//-------------------------------------------------------------------------
{
  Vertex vert ;

  if (coords != NULL)
    vert.SetCoords(coords) ;

  m_Vertex.push_back(vert) ;
}



//-------------------------------------------------------------------------
// Add a new vertex to the graph, joining to existing vertex v0 with a new edge
bool mafPolylineGraph::AddNewVertex(vtkIdType v0, double *coords)
//-------------------------------------------------------------------------
{
  // check that input indices are in range
  Vertex *vert0 = GetVertexPtr(v0) ;
  if (vert0 == NULL){
    mafLogMessage("AddNewVertex(v0) failed: vertex index %d out of range", v0) ;
    return false ;
  }

  // create new empty vertex and add to graph
  Vertex vert ;
  if (coords != NULL)
    vert.SetCoords(coords) ;

  m_Vertex.push_back(vert) ;

  // get index of new vertex and connect it to the input vertex
  vtkIdType v1 = GetMaxVertexId() ;
  AddNewEdge(v0,v1) ;

  return true ;
}


//-------------------------------------------------------------------------
// Add a new empty branch to the graph
void mafPolylineGraph::AddNewBranch(const wxString *name)
//-------------------------------------------------------------------------
{
  Branch br(name) ;
  m_Branch.push_back(br) ;
}

//-------------------------------------------------------------------------
// Add a new empty branch to the graph
bool mafPolylineGraph::AddNewBranch(vtkIdType v0, const wxString *name)
//-------------------------------------------------------------------------
{
  // check that input indices are in range
  Vertex *vert0 = GetVertexPtr(v0) ;
  if (vert0 == NULL){
    mafLogMessage("AddNewBranch(v0) failed: vertex index %d out of range", v0) ;
    return false ;
  }

  // construct new branch with start vertex and add to graph
  Branch br(v0, name) ;
  m_Branch.push_back(br) ;

  return true ;
}




//-------------------------------------------------------------------------
// Split branch by removing ref to edge
// The front part of the split gets the original name, the end part is unnamed
bool mafPolylineGraph::SplitBranchAtEdge(vtkIdType b, vtkIdType e)
//-------------------------------------------------------------------------
{
  int i, j ;

  // create a new branch at the end of the list
  Branch Br ;
  m_Branch.push_back(Br) ;
  Branch *bold = GetBranchPtr(b) ;
  Branch *bnew = GetBranchPtr(GetMaxBranchId()) ;
  Branch *btemp = &Br ;

  // find the position of the edge
  i = bold->FindEdgeId(e) ;
  if (i == UndefinedInt){
    mafLogMessage("SplitBranchAtEdge() can't find edge %d in branch %d", e, b) ;
    return false ;
  }

  // copy edges 0..i-1 to temp branch
  for (j = 0 ;  j <= i-1 ;  j++)
    btemp->AddEdgeId(bold->GetEdgeId(j)) ;

  // copy edges i+1..n-1 to new branch
  for (j = i+1 ;  j < bold->GetNumberOfEdges() ;  j++)
    bnew->AddEdgeId(bold->GetEdgeId(j)) ;

  // copy vertices 0..i to temp branch
  for (j = 0 ;  j <= i ;  j++)
    btemp->AddVertexId(bold->GetVertexId(j)) ;

  // copy vertices i+i..n-1 to new branch
  for (j = i+1 ;  j < bold->GetNumberOfVertices() ;  j++)
    bnew->AddVertexId(bold->GetVertexId(j)) ;

  // copy name to temp branch
  btemp->SetName(bold->GetName()) ;

  // copy temp branch back to old branch
  *bold = *btemp ;

  // set the branch reference of the split edge to undefined
  GetEdgePtr(e)->SetBranchId(UndefinedId) ;

  // update all the edges which got moved to the new branch
  for (i = 0 ;  i < bnew->GetNumberOfEdges() ;  i++){
    vtkIdType ei = bnew->GetEdgeId(i) ;
    GetEdgePtr(ei)->SetBranchId(GetMaxBranchId()) ;
  }



  return true ;
}



//-------------------------------------------------------------------------
// Split branch by duplicating vertex at position i
// The front part of the split gets the original name, the end part is unnamed
bool mafPolylineGraph::SplitBranchAtVertex(vtkIdType b, vtkIdType v)
//-------------------------------------------------------------------------
{
  int i, j ;

  // create a new branch at the end of the list
  Branch Br ;
  m_Branch.push_back(Br) ;
  Branch *bold = GetBranchPtr(b) ;
  Branch *bnew = GetBranchPtr(GetMaxBranchId()) ;
  Branch *btemp = &Br ;

  // find the position of the vertex
  i = bold->FindVertexId(v) ;
  if (i == UndefinedInt){
    mafLogMessage("SplitBranchAtVertex() can't find vertex %d in branch %d", v, b) ;
    return false ;
  }

  // copy edges 0..i-1 to temp branch
  for (j = 0 ;  j <= i-1 ;  j++)
    btemp->AddEdgeId(bold->GetEdgeId(j)) ;

  // copy edges i..n-1 to new branch
  for (j = i ;  j < bold->GetNumberOfEdges() ;  j++)
    bnew->AddEdgeId(bold->GetEdgeId(j)) ;

  // copy vertices 0..i to temp branch
  for (j = 0 ;  j <= i ;  j++)
    btemp->AddVertexId(bold->GetVertexId(j)) ;

  // copy vertices i..n-1 to new branch
  for (j = i ;  j < bold->GetNumberOfVertices() ;  j++)
    bnew->AddVertexId(bold->GetVertexId(j)) ;

  // copy name to temp branch
  btemp->SetName(bold->GetName()) ;

  // copy temp brach back to old branch
  *bold = *btemp ;


  // update all the edges which got moved to the new branch
  for (i = 0 ;  i < bnew->GetNumberOfEdges() ;  i++){
    vtkIdType ei = bnew->GetEdgeId(i) ;
    GetEdgePtr(ei)->SetBranchId(GetMaxBranchId()) ;
  }

  return true ; 
}





//-------------------------------------------------------------------------
// Add a new vertex to the end of the branch
// If the branch is empty, just add a new vertex.
// If the branch is not empty, add a new vertex and a new edge.
void mafPolylineGraph::AddNewVertexToBranch(vtkIdType b, double *coords)
//-------------------------------------------------------------------------
{
  Branch *Br = GetBranchPtr(b) ;

  if (Br->GetNumberOfVertices() == 0){
    // empty branch - just add vertex
    AddNewVertex(coords) ;
    vtkIdType vnew = GetMaxVertexId() ;
    Br->AddVertexId(vnew) ;
  }
  else{
    // not empty branch - add vertex and edge
    AddNewVertex(coords) ;
    vtkIdType vnew = GetMaxVertexId() ;

    // create new edge joining last vertex in branch to new vertex
    vtkIdType vlast = Br->GetLastVertexId() ;
    AddNewEdge(vlast, vnew) ;
    vtkIdType enew = GetMaxEdgeId() ;

    // add vertex and edge to branch
    Br->AddVertexId(vnew) ;
    Br->AddEdgeId(enew) ;
    GetEdgePtr(enew)->SetBranchId(b) ;
  }
}




//-------------------------------------------------------------------------
// Add an existing vertex to the end of the branch
// If the branch is empty, just add the vertex.
// If the branch is not empty, add vertex and edge.
// You can optionally specify the edge.
// The edge must not already belong to a branch.
bool mafPolylineGraph::AddExistingVertexToBranch(vtkIdType b, vtkIdType v, vtkIdType e)
//-------------------------------------------------------------------------
{
  Branch *Br = GetBranchPtr(b) ;

  if (Br == NULL){
    mafLogMessage("AddExistingVertexToBranch() found branch %d out of range", b) ;
    return false ;
  }

  if (GetVertexPtr(v) == NULL){
    mafLogMessage("AddExistingVertexToBranch() found vertex %d out of range", v) ;
    return false ;
  }

  if (Br->GetNumberOfVertices() == 0){
    // empty branch - just add vertex
    Br->AddVertexId(v) ;
  }
  else{
    // not empty branch - find edge to make join
    vtkIdType vlast = Br->GetLastVertexId() ;

    if (e == UndefinedId){
      // find the edge which joins vlast to v
      int j = GetVertexPtr(vlast)->FindVertexId(v) ;
      if (j == UndefinedInt){
        mafLogMessage("AddExistingVertexToBranch() can't find edge joining %d to %d", vlast, v) ;
        return false ;
      }
      e = GetVertexPtr(vlast)->GetEdgeId(j) ;
    }
    else{
      // check that the given edge really joins vlast to v
      if (!GetEdgePtr(e)->IsVertexPair(vlast, v)){
        mafLogMessage("AddExistingVertexToBranch(): edge %d does not join vertices %d and %d", e, vlast, v) ;
        return false ;
      }
    }

    // check that e does not already belong to a branch
    if (GetEdgePtr(e)->GetBranchId() != UndefinedId){
      mafLogMessage("AddExistingVertexToBranch(): edge %d already belongs to a branch", e) ;
      return false ;
    }

    // add vertex and edge
    Br->AddVertexId(v) ;
    Br->AddEdgeId(e) ;
    GetEdgePtr(e)->SetBranchId(b) ;
  }

  return true ;
}



//-------------------------------------------------------------------------
// Add an existing edge to the end of the branch.
// The edge must share a vertex with the end of the branch, and must not already belong to a branch. 
// The branch must not be empty.
bool mafPolylineGraph::AddExistingEdgeToBranch(vtkIdType b, vtkIdType e)
//-------------------------------------------------------------------------
{
  Branch *Br = GetBranchPtr(b) ;
  Edge *Ed = GetEdgePtr(e) ;

  if (Br == NULL){
    mafLogMessage("AddExistingEdgeToBranch() found branch %d out of range", b) ;
    return false ;
  }

  if (Ed == NULL){
    mafLogMessage("AddExistingEdgeToBranch() found edge %d out of range", e) ;
    return false ;
  }

  // check that edge does not already belong to a branch
  if (GetEdgePtr(e)->GetBranchId() != UndefinedId){
    mafLogMessage("AddExistingEdgeToBranch(): edge %d already belongs to a branch", e) ;
    return false ;
  }

  if (Br->GetNumberOfVertices() == 0){
    // empty branch - don't know which vertex is the start of the branch
    mafLogMessage("AddExistingEdgeToBranch(): branch %d is empty, need at least one start vertex", b) ;
    return false ;
  }


  //find last vertex in branch
  vtkIdType vlast = Br->GetLastVertexId() ;

  // find the vertex at the other end of the edge
  vtkIdType vend = Ed->GetOtherEndVertexId(vlast) ;

  if (vend == UndefinedId){
    mafLogMessage("AddExistingEdgeToBranch() can't join edge %d to branch %d", e, b) ;
    return false ;
  }

  // add the edge and the end vertex
  Br->AddEdgeId(e) ;
  Br->AddVertexId(vend) ;
  Ed->SetBranchId(b) ;


  return true ;
}



//-------------------------------------------------------------------------
// Reverse direction of branch
void mafPolylineGraph::ReverseBranch(vtkIdType b)
//-------------------------------------------------------------------------
{
  GetBranchPtr(b)->ReverseDirection() ;
}


//-------------------------------------------------------------------------
// Delete edge from graph
// If the edge was a member of a branch, the branch will be split.
// Be careful: this also moves the edge at the end of the list to index i */
bool mafPolylineGraph::DeleteEdge(vtkIdType i)
//-------------------------------------------------------------------------
{
  vtkIdType elast = GetMaxEdgeId() ;

  if (!SwapEdgeIndices(i,elast)) 
    return false ;

  if (!DeleteLastEdge()){
    mafLogMessage("DeleteEdge() cant delete edge %d", i) ;
    return false ;
  }

  return true ;
}



//-------------------------------------------------------------------------
// Delete last edge from the graph. 
// If the edge was a member of a branch, the branch will be split.
// To delete any edge, first move the edge to the end with GetMaxEdgeId() and SwapEdgeIndices().
//-------------------------------------------------------------------------
bool mafPolylineGraph::DeleteLastEdge()
{
  int i ;

  // get the id of the last edge
  vtkIdType elast = GetMaxEdgeId() ;

  // remove instances of the edge from the vertices
  for (i = 0 ;  i < 2 ;  i++){
    vtkIdType v = GetEdgePtr(elast)->GetVertexId(i) ;
    GetVertexPtr(v)->DeleteEdgeId(elast) ;
  }

  // split the branch which the edge belongs to
  vtkIdType br1 = GetEdgePtr(elast)->GetBranchId() ;
  if (br1 != UndefinedId){
    SplitBranchAtEdge(br1, elast) ;
  }

  // finally delete the edge from the end of the list
  m_Edge.pop_back() ;

  return true ;
}



//-------------------------------------------------------------------------
// Delete vertex from graph
// The vertex must be degree zero, so you have to remove any edges first.
// Any branches containing this vertex will be left empty.
// Be careful: this also moves the vertex at the end of the list to index i */
bool mafPolylineGraph::DeleteVertex(vtkIdType i)
//-------------------------------------------------------------------------
{
  vtkIdType elast = GetMaxVertexId() ;

  if (!SwapVertexIndices(i,elast)) 
    return false ;

  if (!DeleteLastVertex()){
    mafLogMessage("DeleteVertex() cant delete vertex %d", i) ;
    return false ;
  }

  return true ;
}



//-------------------------------------------------------------------------
// Delete last vertex from the graph. 
// To delete any vertex, first move the vertex to the end with GetMaxVertexId() and SwapVertexIndices().
// The vertex must be degree zero, so you have to remove any edges first.
// Any branches containing this vertex will be left empty.
//-------------------------------------------------------------------------
bool mafPolylineGraph::DeleteLastVertex()
{
  // get the id of the last vertex
  vtkIdType vlast = GetMaxVertexId() ;

  Vertex *vert = GetVertexPtr(vlast) ;
  if (vert->GetDegree() != 0){
    mafLogMessage("DeleteLastVertex() attempt to delete vertex %d of degree %d", vlast, vert->GetDegree()) ;
    return false ;
  }

  // remove branches which refer to vertex
  for (vtkIdType b = 0 ;  b < GetNumberOfBranches() ;  b++){
    Branch *br = GetBranchPtr(b) ;
    int j = br->FindVertexId(vlast) ;
    if (j != UndefinedInt){
      if (br->GetNumberOfVertices() != 1){
        // found a branch with the vertex, but it should be the only only one in the branch
        mafLogMessage("DeleteLastVertex() found branch %d with %d vertices", b, br->GetNumberOfVertices()) ;
        return false ;
      }

      // remove the vertex from the branch (leaving an empty branch)
      br->DeleteLastVertex() ;
    }
  }

  // finally delete the vertex from the end of the list
  m_Vertex.pop_back() ;

  return true ;
}



//-------------------------------------------------------------------------
// Delete branch from graph
// Be careful: this also moves the branch at the end of the list to index i */
bool mafPolylineGraph::DeleteBranch(vtkIdType i)
//-------------------------------------------------------------------------
{
  vtkIdType elast = GetMaxBranchId() ;

  if (!SwapBranchIndices(i,elast)) 
    return false ;

  if (!DeleteLastBranch()){
    mafLogMessage("DeleteBranch() cant delete branch %d", i) ;
    return false ;
  }

  return true ;
}


//-------------------------------------------------------------------------
// Delete last branch from the graph. 
// To delete any edge, first move to the end with GetMaxBranchId() and SwapBranchIndices().
//-------------------------------------------------------------------------
bool mafPolylineGraph::DeleteLastBranch()
{
  int i ;

  // get the id of the last branch
  vtkIdType blast = GetMaxBranchId() ;
  Branch *Br = GetBranchPtr(blast) ;

  // remove references to the branch from the edges
  for (i = 0 ;  i < Br->GetNumberOfEdges() ;  i++){
    vtkIdType e = Br->GetEdgeId(i) ;
    GetEdgePtr(e)->SetBranchId(UndefinedId) ;
  }

  // finally delete the branch from the end of the list
  m_Branch.pop_back() ;

  return true ;
}



//-------------------------------------------------------------------------
// Swap vertex indices throughout graph
// NB Changes in indices should be explicit to the user, so try not to
// hide this method inside other methods.
bool mafPolylineGraph::SwapVertexIndices(vtkIdType v0, vtkIdType v1)
//-------------------------------------------------------------------------
{
  vtkIdType v, e, b ;
  int j ;

  if ((GetVertexPtr(v0) == NULL) || (GetVertexPtr(v1) == NULL)){
    mafLogMessage("invalid input indices %d %d in SwapVertexIndices()", v0, v1) ;
    return false ;
  }

  if (v0 == v1)
    return true ;

  // exchange all references to v0 and v1 in the list of vertices
  for (v = 0 ;  v < GetNumberOfVertices() ;  v++){
    Vertex *vert = GetVertexPtr(v) ;
    for (j = 0 ;  j < vert->GetDegree() ;  j++){
      if (vert->GetVertexId(j) == v0)
        vert->SetVertexId(j, v1) ;
      else if (vert->GetVertexId(j) == v1)
        vert->SetVertexId(j, v0) ;
    }
  }

  // exchange all references to v0 and v1 in the list of edges
  for (e = 0 ;  e < GetNumberOfEdges() ;  e++){
    Edge *ed = GetEdgePtr(e) ;
    for (j = 0 ;  j < 2 ;  j++){
      if (ed->GetVertexId(j) == v0)
        ed->SetVertexId(j, v1) ;
      else if (ed->GetVertexId(j) == v1)
        ed->SetVertexId(j, v0) ;
    }
  }

  // exchange all references to v0 and v1 in the list of branches
  for (b = 0 ;  b < GetNumberOfBranches() ;  b++){
    Branch *br = GetBranchPtr(b) ;
    for (j = 0 ;  j < br->GetNumberOfVertices() ;  j++){
      if (br->GetVertexId(j) == v0)
        br->SetVertexId(j, v1) ;
      else if (br->GetVertexId(j) == v1)
        br->SetVertexId(j, v0) ;
    }
  }

  // now swap the actual vertex objects in the list
  Vertex vtemp ;
  vtemp = *GetVertexPtr(v0) ;
  *GetVertexPtr(v0) = *GetVertexPtr(v1) ;
  *GetVertexPtr(v1) = vtemp ;

  return true ;
}


//-------------------------------------------------------------------------
// Swap edge indices throughout graph
// NB Changes in indices should be explicit to the user, so try not to
// hide this method inside other methods.
bool mafPolylineGraph::SwapEdgeIndices(vtkIdType e0, vtkIdType e1)
//-------------------------------------------------------------------------
{
  vtkIdType v, b ;
  int j ;

  if ((GetEdgePtr(e0) == NULL) || (GetEdgePtr(e1) == NULL)){
    mafLogMessage("invalid input indices %d %d in SwapEdgeIndices()", e0, e1) ;
    return false ;
  }

  if (e0 == e1)
    return true ;

  // exchange all references to e0 and e1 in the list of vertices
  for (v = 0 ;  v < GetNumberOfVertices() ;  v++){
    Vertex *vert = GetVertexPtr(v) ;
    for (j = 0 ;  j < vert->GetDegree() ;  j++){
      if (vert->GetEdgeId(j) == e0)
        vert->SetEdgeId(j, e1) ;
      else if (vert->GetEdgeId(j) == e1)
        vert->SetEdgeId(j, e0) ;
    }
  }

  // exchange all references to e0 and e1 in the list of branches
  for (b = 0 ;  b < GetNumberOfBranches() ;  b++){
    Branch *br = GetBranchPtr(b) ;
    for (j = 0 ;  j < br->GetNumberOfEdges() ;  j++){
      if (br->GetEdgeId(j) == e0)
        br->SetEdgeId(j, e1) ;
      else if (br->GetEdgeId(j) == e1)
        br->SetEdgeId(j, e0) ;
    }
  }

  // now swap the actual edge objects in the list
  Edge etemp ;
  etemp = *GetEdgePtr(e0) ;
  *GetEdgePtr(e0) = *GetEdgePtr(e1) ;
  *GetEdgePtr(e1) = etemp ;

  return true ;
}


//-------------------------------------------------------------------------
// Swap branch indices throughout graph
// NB Changes in indices should be explicit to the user, so try not to
// hide this method inside other methods.
bool mafPolylineGraph::SwapBranchIndices(vtkIdType b0, vtkIdType b1)
//-------------------------------------------------------------------------
{
  vtkIdType e ;

  if ((GetBranchPtr(b0) == NULL) || (GetBranchPtr(b1) == NULL)){
    mafLogMessage("invalid input indices %d %d in SwapBranchIndices()", b0, b1) ;
    return false ;
  }

  if (b0 == b1)
    return true ;

  // exchange all references to b0 and b1 in the list of edges
  for (e = 0 ;  e < GetNumberOfEdges() ;  e++){
    Edge *ed = GetEdgePtr(e) ;
    if (ed->GetBranchId() == b0)
      ed->SetBranchId(b1) ;
    else if (ed->GetBranchId() == b1)
      ed->SetBranchId(b0) ;
  }

  // now swap the actual branch objects in the list
  Branch btemp ;
  btemp = *GetBranchPtr(b0) ;
  *GetBranchPtr(b0) = *GetBranchPtr(b1) ;
  *GetBranchPtr(b1) = btemp ;

  return true ;
}


double mafPolylineGraph::GetBranchLength(vtkIdType b) const
{
  double sum = 0;

  const mafPolylineGraph::Branch *br = GetConstBranchPtr(b);
  assert(br != NULL);

  vtkIdList *verticesIdList = vtkIdList::New();
  br->GetVerticesIdList(verticesIdList);

  for (int i = 0 ;  i < br->GetNumberOfVertices() ;  i++)
  { 
    if (i > 0)
    {
      double pos1[3], pos2[3];

      const mafPolylineGraph::Vertex *vertex1 = GetConstVertexPtr(verticesIdList->GetId(i));
      vertex1->GetCoords(pos1);

      const mafPolylineGraph::Vertex *vertex2 = GetConstVertexPtr(verticesIdList->GetId(i-1));
      vertex2->GetCoords(pos2);

      sum += sqrt(vtkMath::Distance2BetweenPoints(pos1, pos2));

    }
  }

  // clean up
  vtkDEL(verticesIdList);

  return sum;
}

double mafPolylineGraph::GetBranchIntervalLength(vtkIdType b, vtkIdType startVertexId, vtkIdType endVertexId) const
{
  double sum = 0;

  assert(startVertexId  <=  endVertexId );

  const mafPolylineGraph::Branch *br = GetConstBranchPtr(b);
  assert(br != NULL);

  vtkIdList *verticesIdList = vtkIdList::New();
  br->GetVerticesIdList(verticesIdList);

  int numberOfIds = verticesIdList->GetNumberOfIds();
  
  int startId = -1;

  // find start vertex id
  for (int i = 0; i < numberOfIds; i++) 
  { 
    vtkIdType currentId = verticesIdList->GetId(i);
    if (currentId == startVertexId)
    {
      startId = i;
    }
  }
  
  int endId = -1;

  // find start vertex id
  for (int j = 0; j < numberOfIds; j++) 
  { 
    vtkIdType currentId = verticesIdList->GetId(j);
    if (currentId == endVertexId)
    {
      endId = j;
    }
  }

  assert(endId >= startId );

  for (int k = startId ;  k <= endId ;  k++)
  { 
    if (k > 0)
    {
      double pos1[3], pos2[3];

      const mafPolylineGraph::Vertex *vertex1 = GetConstVertexPtr(verticesIdList->GetId(k));
      vertex1->GetCoords(pos1);

      const mafPolylineGraph::Vertex *vertex2 = GetConstVertexPtr(verticesIdList->GetId(k-1));
      vertex2->GetCoords(pos2);

      sum += sqrt(vtkMath::Distance2BetweenPoints(pos1, pos2));

    }
  }

  // clean up
  vtkDEL(verticesIdList);

  return sum;
}

#pragma region BES April 2008
//------------------------------------------------------------------------
/** Merges two branches.
Both branches (identified by b1 and b2) must share common endpoint.
Be careful: this also moves the branch at the end of the list to index b2.*/
bool mafPolylineGraph::MergeBranches(vtkIdType b1, vtkIdType b2)
{  
  if (b1 == b2)
    return true;

 Branch* br1 = GetBranchPtr(b1);
 Branch* br2 = GetBranchPtr(b2);

  if (br1 == NULL || br2 == NULL){
    mafLogMessage("invalid input branches %d %d in MergeBranches()", b1, b2) ;
    return false ;
  }

  //get the last vertex id
  vtkIdType v = br1->GetLastVertexId();
  int iPos = br2->FindVertexId(v);
  if (iPos == br2->GetNumberOfVertices() - 1)
    this->ReverseBranch(b2);  //we need to connect to the end of the second branch
  else if (iPos != 0)
  {
    //OK, we cannot connect b2 after b1, let us check, if we can do it vice versa
    v = br2->GetLastVertexId();
    iPos = br1->FindVertexId(v);
    if (iPos == br1->GetNumberOfVertices() - 1)
      this->ReverseBranch(b1);  //we need to connect to the end of the second branch
    else if (iPos != 0)
    {
      mafLogMessage("branches %d %d does not share common vertex in MergeBranches()", b1, b2) ;
      return false ;
    }

    //swap b1 and b2
    vtkIdType tmp = b1;
    b1 = b2; b2 = tmp;

    Branch* btmp = br1;
    br1 = br2; br2 = btmp;
  }

  //now merge b2 after b1
  for (int i = 0; i < br2->GetNumberOfEdges(); i++)
  {
    vtkIdType e = br2->GetEdgeId(i);    

    br1->AddEdgeId(e);
    br1->AddVertexId(br2->GetVertexId(i + 1));
    GetEdgePtr(e)->SetBranchId(b1); 
  }  

  br2->Clear();
  return this->DeleteBranch(b2);
}

/** Merges two branches connected at the given vertex.
The vertex may not be at a bifurcation, i.e., it must be of order 2
Be careful: this also moves the branch at the end of the list to index b1.*/
bool mafPolylineGraph::MergeSimpleJoinedBranchesAtVertex(vtkIdType v)
{
  const Vertex* vert = this->GetConstVertexPtr(v);
  if (vert->GetDegree() != 2)
    return false; //either terminal node of the graph or some bifurcation

  const Edge* e1 = this->GetConstEdgePtr(vert->GetEdgeId(0));
  const Edge* e2 = this->GetConstEdgePtr(vert->GetEdgeId(1));


  return MergeBranches(e1->GetBranchId(), e2->GetBranchId());
}

/** Merges all branches in the graph connected in vertices of order 2  
Be careful: this leads to reindexing of branches */
void mafPolylineGraph::MergeSimpleJoinedBranches()
{
  for (vtkIdType v = 0; v < this->GetMaxVertexId(); v++) {
    MergeSimpleJoinedBranchesAtVertex(v);
  }
}
#pragma endregion