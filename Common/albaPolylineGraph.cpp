/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: albaPolylineGraph.cpp,v $
Language:  C++
Date:      $Date: 2012-04-06 07:54:31 $
Version:   $Revision: 1.1.2.1 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================
Modified by Josef Kohout to strip off ALBA stuff from it making it possible
to use in VTK filters, etc. (=> wxString was replaced by char*) and
hack for albaLogMessage was used.
*/


//IMPORTANT NOTE: This class is supposed to be pure C++ class
//do not include not use wxWidgets or ALBA stuff, its use
//would lead into application crash!
//#include "albaDefines.h"

#include "albaDefines.h"
#include "albaPolylineGraph.h"
#include "vtkPolydata.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkCell.h"
#include "vtkIdType.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "vtkIndent.h"
#include <ostream>
#include <vector>
#include <assert.h>

#ifndef _NO_ALBA //ALBA platform is used  
/** write a message in the log area */
// void albaLogMessage(const char *format, ...);
#define LogMessage albaLogMessage
#else
#if defined(_MSC_VER) && _MSC_VER >= 1400
#define LogMessage  __noop
#else
#define LogMessage  (void)0
#endif
#endif

const /*static*/ vtkIdType albaPolylineGraph::m_UndefinedId = VTK_LARGE_ID ;
const /*static*/ int albaPolylineGraph::m_UndefinedInt = -1 ;
const /*static*/ char* albaPolylineGraph::m_UndefinedName = "****" ;

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
// albaPolylineGraph::albaPolylineGraph()
// albaPolylineGraph::~albaPolylineGraph()
// albaPolylineGraph::GetNumberOfVertices()
// albaPolylineGraph::GetNumberOfEdges()
// albaPolylineGraph::GetNumberOfBranches()
// albaPolylineGraph::GetMaxVertexId()
// albaPolylineGraph::GetMaxEdgeId()
// albaPolylineGraph::GetMaxBranchId()
// albaPolylineGraph::GetVertexCoords()
// albaPolylineGraph::SetVertexCoords()
// albaPolylineGraph::GetBranchName()
// albaPolylineGraph::SetBranchName()
// albaPolylineGraph::UnsetBranchName()
// albaPolylineGraph::FindBranchName()
// albaPolylineGraph::IsEdgeDirected()
// albaPolylineGraph::SetEdgeDirected()
// albaPolylineGraph::ReverseEdge()
// albaPolylineGraph::GetEdgeWeight()
// albaPolylineGraph::SetEdgeWeight()
// albaPolylineGraph::AddNewEdge()
// albaPolylineGraph::AddNewVertex()
// albaPolylineGraph::AddNewVertex(v0)
// albaPolylineGraph::AddNewBranch()
// albaPolylineGraph::AddNewBranch(v0)
// albaPolylineGraph::CopyFromPolydata()
// albaPolylineGraph::CopyToPolydata()
// albaPolylineGraph::GetOutputCellCorrespondingToEdge()
// albaPolylineGraph::GetEdgeCorrespondingToOutputCell()
// albaPolylineGraph::SplitBranchAtEdge()
// albaPolylineGraph::SplitBranchAtVertex()
// albaPolylineGraph::AddNewVertexToBranch()
// albaPolylineGraph::AddExistingVertexToBranch()
// albaPolylineGraph::AddExistingEdgeToBranch()
// albaPolylineGraph::ReverseBranch()
// albaPolylineGraph::DeleteEdge()
// albaPolylineGraph::DeleteVertex()
// albaPolylineGraph::DeleteBranch()
// albaPolylineGraph::IsConnected()
// albaPolylineGraph::IsConnectedTree()
// albaPolylineGraph::Clear() 
// albaPolylineGraph::SelfCheck() 
// albaPolylineGraph::PrintSelf()
// albaPolylineGraph::GetConstVertexPtr()
// albaPolylineGraph::GetConstEdgePtr()
// albaPolylineGraph::GetConstBranchPtr()
// albaPolylineGraph::AllocateVertices()
// albaPolylineGraph::AllocateEdges()
// albaPolylineGraph::AllocateBranches()
// albaPolylineGraph::SwapVertexIndices()
// albaPolylineGraph::SwapEdgeIndices()
// albaPolylineGraph::SwapBranchIndices()
// albaPolylineGraph::DeleteLastEdge()
// albaPolylineGraph::DeleteLastVertex()
// albaPolylineGraph::DeleteLastBranch()
// albaPolylineGraph::GetVertexPtr()
// albaPolylineGraph::GetEdgePtr()
// albaPolylineGraph::GetBranchPtr()
// 
//-------------------------------------------------------------------------



//-------------------------------------------------------------------------
// Vertex Constructor
albaPolylineGraph::Vertex::Vertex()
//-------------------------------------------------------------------------
{
  m_Coords[0] = 0.0 ;
  m_Coords[1] = 0.0 ;
  m_Coords[2] = 0.0 ;
}


//-------------------------------------------------------------------------
// Vertex Constructor
albaPolylineGraph::Vertex::Vertex(double *coords)
//-------------------------------------------------------------------------
{
  m_Coords[0] = coords[0] ;
  m_Coords[1] = coords[1] ;
  m_Coords[2] = coords[2] ;
}

//-------------------------------------------------------------------------
// Vertex Destructor
albaPolylineGraph::Vertex::~Vertex()
//-------------------------------------------------------------------------
{
  m_EdgeId.clear() ;
  m_VertexId.clear() ;
}


//-------------------------------------------------------------------------
// Get degree of vertex
int albaPolylineGraph::Vertex::GetDegree() const
//-------------------------------------------------------------------------
{
  return (int)m_EdgeId.size() ;
}


//-------------------------------------------------------------------------
// Get ith edge
vtkIdType albaPolylineGraph::Vertex::GetEdgeId(int i) const
//-------------------------------------------------------------------------
{
  return m_EdgeId.at(i) ;
}


//-------------------------------------------------------------------------
// Set ith edge
void albaPolylineGraph::Vertex::SetEdgeId(int i, vtkIdType e)
//-------------------------------------------------------------------------
{
  m_EdgeId.at(i) = e ;
}

//-------------------------------------------------------------------------
// Add new vertex id to list
void albaPolylineGraph::Vertex::AddEdgeId(vtkIdType e)
//-------------------------------------------------------------------------
{
  m_EdgeId.push_back(e) ;
}


//-------------------------------------------------------------------------
// Find position of edge in list
int albaPolylineGraph::Vertex::FindEdgeId(vtkIdType e) const
//-------------------------------------------------------------------------
{
  for (int i = 0 ;  i < GetDegree() ;  i++){
    if (GetEdgeId(i) == e)
      return i ;  // found - return position
  }
  return m_UndefinedInt ;     // not found - return UndefinedInt
}



//-------------------------------------------------------------------------
// Delete this edge index and the associated vertex from the vertex
bool albaPolylineGraph::Vertex::DeleteEdgeId(vtkIdType e)
//-------------------------------------------------------------------------
{
  // find the position of the edge on the vertex
  int j = FindEdgeId(e) ;
  if (j == m_UndefinedInt){
    LogMessage("Vertex::DeleteEdgeId() can't find edge index %d", e) ;
    return false ;
  }

  // copy the end of the list to position j and delete the end of the list
  m_EdgeId.at(j) = m_EdgeId.back() ;
  m_EdgeId.pop_back() ;

  m_VertexId.at(j) = m_VertexId.back() ;
  m_VertexId.pop_back() ;

  return true ;
}


//-------------------------------------------------------------------------
// Get ith vertex
vtkIdType albaPolylineGraph::Vertex::GetVertexId(int i) const
//-------------------------------------------------------------------------
{
  return m_VertexId.at(i) ;
}


//-------------------------------------------------------------------------
// Set ith vertex
void albaPolylineGraph::Vertex::SetVertexId(int i, vtkIdType v)
//-------------------------------------------------------------------------
{
  m_VertexId.at(i) = v ;
}


//-------------------------------------------------------------------------
// Add new vertex id to list
void albaPolylineGraph::Vertex::AddVertexId(vtkIdType v)
//-------------------------------------------------------------------------
{
  m_VertexId.push_back(v) ;
}



//-------------------------------------------------------------------------
// Find position of vertex in list
int albaPolylineGraph::Vertex::FindVertexId(vtkIdType v) const
//-------------------------------------------------------------------------
{
  for (int i = 0 ;  i < GetDegree() ;  i++){
    if (GetVertexId(i) == v)
      return i ;  // found - return position
  }

  return m_UndefinedInt ;     // not found - return UndefinedInt
}




//-------------------------------------------------------------------------
// Delete this vertex index and the associated edge from the vertex
bool albaPolylineGraph::Vertex::DeleteVertexId(vtkIdType v)
//-------------------------------------------------------------------------
{
  // find the position of the edge on the vertex
  int j = FindVertexId(v) ;
  if (j == m_UndefinedInt){
    LogMessage("Vertex::DeleteVertexId() can't find vertex index %d", v) ;
    return false ;
  }

  // copy the end of the list to position j and delete the end of the list
  m_EdgeId.at(j) = m_EdgeId.back() ;
  m_EdgeId.pop_back() ;

  m_VertexId.at(j) = m_VertexId.back() ;
  m_VertexId.pop_back() ;

  return true ;
}


//-------------------------------------------------------------------------
// return vertices as a vtkIdlist
void albaPolylineGraph::Vertex::GetVerticesIdList(vtkIdList *idlist) const
//-------------------------------------------------------------------------
{
  idlist->Initialize() ;

  for (int i = 0 ;  i < GetDegree() ;  i++)
    idlist->InsertNextId(GetVertexId(i)) ;
}



//-------------------------------------------------------------------------
// Get vertex coords
void albaPolylineGraph::Vertex::GetCoords(double *coords) const
//-------------------------------------------------------------------------
{
  coords[0] = m_Coords[0] ;
  coords[1] = m_Coords[1] ;
  coords[2] = m_Coords[2] ;
}


//-------------------------------------------------------------------------
// Set vertex coords
void albaPolylineGraph::Vertex::SetCoords(const double* coords)
//-------------------------------------------------------------------------
{
  m_Coords[0] = coords[0] ;
  m_Coords[1] = coords[1] ;
  m_Coords[2] = coords[2] ;
}


//-------------------------------------------------------------------------
// Clear data from vertex
void albaPolylineGraph::Vertex::Clear()
//-------------------------------------------------------------------------
{
  m_EdgeId.clear() ;
  m_VertexId.clear() ;
}


//-------------------------------------------------------------------------
// Test internal consistency of Vertex
// This currently does not allow repeated vertices on a vertex
bool albaPolylineGraph::Vertex::SelfCheck() const
//-------------------------------------------------------------------------
{
  // arrays should be of the same length
  if (m_EdgeId.size() != m_VertexId.size()){
#ifndef _DEBUG
      LogMessage("mismatched list sizes in vertex %d %d", m_EdgeId.size(), m_VertexId.size()) ;
#endif
    return false ;
  }

  // edges on vertex must not contain repeats
  for (int i = 0 ;  i < GetDegree()-1 ;  i++){
    for (int j = i+1 ;  j < GetDegree() ;  j++){
      if (GetEdgeId(i) == GetEdgeId(j)){
#ifndef _DEBUG
        LogMessage("repeated edge %d at %d and %d on vertex", GetEdgeId(i), i, j) ;
#endif
        return false ;
      }
    }
  }

  // vertices on vertex must not contain repeats
  for (int i = 0 ;  i < GetDegree()-1 ;  i++){
    for (int j = i+1 ;  j < GetDegree() ;  j++){
      if (GetVertexId(i) == GetVertexId(j)){
#ifndef _DEBUG
        LogMessage("repeated vertex %d at %d and %d on vertex", GetVertexId(i), i, j) ;
#endif
        return false ;
      }
    }
  }

  return true ;
}


//-------------------------------------------------------------------------
// Print vertex
void albaPolylineGraph::Vertex::PrintSelf(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  int i ;

  vtkIndent indent(tabs);
  
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
albaPolylineGraph::Edge::Edge() : m_Directed(false), m_Weight(1.0), m_BranchId(m_UndefinedId), m_OutputPolydataCell(m_UndefinedId), m_OutputPolydataCellIndex(m_UndefinedId)
//-------------------------------------------------------------------------
{
  m_VertexId[0] = m_UndefinedId ;
  m_VertexId[1] = m_UndefinedId ;
}


//-------------------------------------------------------------------------
// constructor with end vertices
albaPolylineGraph::Edge::Edge(vtkIdType v0, vtkIdType v1) : m_Directed(false), m_Weight(1.0), m_BranchId(m_UndefinedId), m_OutputPolydataCell(m_UndefinedId), m_OutputPolydataCellIndex(m_UndefinedId)
//-------------------------------------------------------------------------
{
  m_VertexId[0] = v0 ;
  m_VertexId[1] = v1 ;
}


//-------------------------------------------------------------------------
// copy constructor for edge
albaPolylineGraph::Edge::Edge(const albaPolylineGraph::Edge& e)                      
//-------------------------------------------------------------------------
{
  m_VertexId[0] = e.m_VertexId[0] ;
  m_VertexId[1] = e.m_VertexId[1] ;
  m_Directed = e.m_Directed ;
  m_Weight = e.m_Weight ;
  m_BranchId = e.m_BranchId ;
}


//-------------------------------------------------------------------------
// destructor
albaPolylineGraph::Edge::~Edge()
//-------------------------------------------------------------------------
{
}


//-------------------------------------------------------------------------
// assignment operator for edge
albaPolylineGraph::Edge& albaPolylineGraph::Edge::operator=(const albaPolylineGraph::Edge& e)                      
//-------------------------------------------------------------------------
{
  m_VertexId[0] = e.m_VertexId[0] ;
  m_VertexId[1] = e.m_VertexId[1] ;
  m_Directed = e.m_Directed ;
  m_Weight = e.m_Weight ;
  m_BranchId = e.m_BranchId ;

  return *this ;
}

//-------------------------------------------------------------------------
// get index of vertex i, where i = 0 or 1
vtkIdType albaPolylineGraph::Edge::GetVertexId(int i) const
//-------------------------------------------------------------------------
{
  return m_VertexId[i] ;
}

//-------------------------------------------------------------------------
// set index of vertex, where i = 0 or 1
void albaPolylineGraph::Edge::SetVertexId(int i, vtkIdType v)
//-------------------------------------------------------------------------
{
  m_VertexId[i] = v ;
}

//-------------------------------------------------------------------------
// set indices of vertices
void albaPolylineGraph::Edge::SetVertexIds(vtkIdType v0, vtkIdType v1)
//-------------------------------------------------------------------------
{
  m_VertexId[0] = v0 ;
  m_VertexId[1] = v1 ;
}


//-------------------------------------------------------------------------
// return vertices as a vtkIdlist
void albaPolylineGraph::Edge::GetVerticesIdList(vtkIdList *idlist) const
//-------------------------------------------------------------------------
{
  idlist->Initialize() ;

  for (int i = 0 ;  i < 2 ;  i++)
    idlist->InsertNextId(GetVertexId(i)) ;
}



//-------------------------------------------------------------------------
// get vertex id at other end of edge
vtkIdType albaPolylineGraph::Edge::GetOtherEndVertexId(vtkIdType v) const
//-------------------------------------------------------------------------
{
  if (GetVertexId(0) == v)
    return GetVertexId(1) ;
  else if (GetVertexId(1) == v)
    return GetVertexId(0) ;
  else
    return m_UndefinedId ;
}


//-------------------------------------------------------------------------
// get branch which edge belongs to
vtkIdType albaPolylineGraph::Edge::GetBranchId() const
//-------------------------------------------------------------------------
{
  return m_BranchId ;
}

//-------------------------------------------------------------------------
// set branch which edge belongs to
void albaPolylineGraph::Edge::SetBranchId(vtkIdType b)
//-------------------------------------------------------------------------
{
  m_BranchId = b ;
}

//-------------------------------------------------------------------------
// is the edge equal to these vertices (order doesn't matter)
bool albaPolylineGraph::Edge::IsVertexPair(vtkIdType v0, vtkIdType v1) const
//-------------------------------------------------------------------------
{
  return (m_VertexId[0] == v0 && m_VertexId[1] == v1) || (m_VertexId[0] == v1 && m_VertexId[1] == v0) ;
}


//-------------------------------------------------------------------------
// is the edge equal to these vertices in this order
bool albaPolylineGraph::Edge::IsVertexPairOrdered(vtkIdType v0, vtkIdType v1) const
//-------------------------------------------------------------------------
{
  return (m_VertexId[0] == v0 && m_VertexId[1] == v1) ;
}


//-------------------------------------------------------------------------
// Set/unset directed property
void albaPolylineGraph::Edge::SetDirected(bool dir)
//-------------------------------------------------------------------------
{
  m_Directed = dir ;
}


//-------------------------------------------------------------------------
// is the edge directed
bool albaPolylineGraph::Edge::IsDirected() const
//-------------------------------------------------------------------------
{
  return m_Directed ;
}



//-------------------------------------------------------------------------
// reverse direction of edge by exchanging vertices
void albaPolylineGraph::Edge::ReverseDirection()
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
double albaPolylineGraph::Edge::GetWeight() const
//-------------------------------------------------------------------------
{
  return m_Weight ;
}

//-------------------------------------------------------------------------
// set weight
void albaPolylineGraph::Edge::SetWeight(double w)
//-------------------------------------------------------------------------
{
  m_Weight = w ;
}



//-------------------------------------------------------------------------
// get mapping from edge to location in output
void albaPolylineGraph::Edge::GetMappingToOutputPolydata(vtkIdType *cellid, vtkIdType *index) const
//-------------------------------------------------------------------------
{
  *cellid = m_OutputPolydataCell ;
  *index = m_OutputPolydataCellIndex ;
}


//-------------------------------------------------------------------------
// set mapping from edge to location in output
void albaPolylineGraph::Edge::SetMappingToOutputPolydata(vtkIdType cellid, vtkIdType index)
//-------------------------------------------------------------------------
{
  m_OutputPolydataCell = cellid ;
  m_OutputPolydataCellIndex = index ;
}



//-------------------------------------------------------------------------
// check self consistency of edge
bool albaPolylineGraph::Edge::SelfCheck() const                  
//-------------------------------------------------------------------------
{
  // edge must not be empty
  if ((m_VertexId[0] == m_UndefinedId) || (m_VertexId[1] == m_UndefinedId)){
    #ifndef _DEBUG
    LogMessage("undefined vertex or vertices %d %d on edge", m_VertexId[0], m_VertexId[1]) ;
#endif
    return false ;
  }

  return true ;
}


//-------------------------------------------------------------------------
// Print edge
void albaPolylineGraph::Edge::PrintSelf(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{  
  vtkIndent indent(tabs);

  os << indent << "vertices= " << GetVertexId(0) << " " << GetVertexId(1) ;

  if (GetBranchId() == m_UndefinedId)
    os << " branch= " << "undef" ;
  else
    os << " branch= " << GetBranchId() ;

  os << " directed = " << IsDirected() << " weight= " << GetWeight() << std::endl ;
}




//-------------------------------------------------------------------------
// Branch Constructor
// Empty branch with optional name
albaPolylineGraph::Branch::Branch(const char *name) : m_OutputPolydataCell(m_UndefinedId)
//-------------------------------------------------------------------------
{
  if (name == NULL)
    name = m_UndefinedName;
  
  m_Name=name;

  m_EdgeId.clear() ;
  m_VertexId.clear() ;
}


//-------------------------------------------------------------------------
// Branch Constructor with initial vertex and optional name
albaPolylineGraph::Branch::Branch(vtkIdType v, const char *name) : m_OutputPolydataCell(m_UndefinedId)
//-------------------------------------------------------------------------
{
  // insert first vertex id
  m_VertexId.push_back(v) ;
  
  if (name == NULL)
    name = m_UndefinedName;

  m_Name=name;
}

//------------------------------------------------------------------------
albaPolylineGraph::Branch::Branch(const Branch& src)
//------------------------------------------------------------------------
{
  *this = src;  
}

//------------------------------------------------------------------------
albaPolylineGraph::Branch& albaPolylineGraph::Branch::operator=(const Branch& src)
//------------------------------------------------------------------------
{
  m_OutputPolydataCell = src.m_OutputPolydataCell;
  m_VertexId = src.m_VertexId;
  m_EdgeId = src.m_EdgeId;

  m_Name = src.m_Name;

  return *this;
}

//-------------------------------------------------------------------------
// Branch Destructor
albaPolylineGraph::Branch::~Branch()
//-------------------------------------------------------------------------
{
  m_EdgeId.clear() ;
  m_VertexId.clear() ;
}


//-------------------------------------------------------------------------
// Get number of vertices
int albaPolylineGraph::Branch::GetNumberOfVertices() const
//-------------------------------------------------------------------------
{
  return (int)m_VertexId.size() ;
}

//-------------------------------------------------------------------------
// Get number of edges
int albaPolylineGraph::Branch::GetNumberOfEdges() const
//-------------------------------------------------------------------------
{
  return (int)m_EdgeId.size() ;
}


//-------------------------------------------------------------------------
// Get name
const char* albaPolylineGraph::Branch::GetName() const
//-------------------------------------------------------------------------
{
  return m_Name.ToAscii();
}

//-------------------------------------------------------------------------
// Set or reset name
void albaPolylineGraph::Branch::SetName(const char *name)
//-------------------------------------------------------------------------
{
  m_Name=name;
}

//-------------------------------------------------------------------------
// Set name to undefined
void albaPolylineGraph::Branch::UnsetName()
//-------------------------------------------------------------------------
{
  SetName(m_UndefinedName);
}


//-------------------------------------------------------------------------
// Get ith edge
vtkIdType albaPolylineGraph::Branch::GetEdgeId(int i) const
//-------------------------------------------------------------------------
{
  return m_EdgeId.at(i) ;
}


//-------------------------------------------------------------------------
// Set ith edge
void albaPolylineGraph::Branch::SetEdgeId(int i, vtkIdType e)
//-------------------------------------------------------------------------
{
  m_EdgeId.at(i) = e ;
}


//-------------------------------------------------------------------------
// Add new edge id to end of list
void albaPolylineGraph::Branch::AddEdgeId(vtkIdType e)
//-------------------------------------------------------------------------
{
  m_EdgeId.push_back(e) ;
}

//-------------------------------------------------------------------------
// Find position of edge in list
int albaPolylineGraph::Branch::FindEdgeId(vtkIdType e) const
//-------------------------------------------------------------------------
{
  for (int i = 0 ;  i < GetNumberOfEdges() ;  i++){
    if (GetEdgeId(i) == e)
      return i ;  // found - return position
  }
  return m_UndefinedInt ;     // not found - return UndefinedInt
}

//-------------------------------------------------------------------------
// Insert edge before position i
void albaPolylineGraph::Branch::InsertEdgeId(int i, vtkIdType e)
//-------------------------------------------------------------------------
{
  m_EdgeId.insert(m_EdgeId.begin()+i, e) ;
}


//-------------------------------------------------------------------------
// Get id of last edge
vtkIdType albaPolylineGraph::Branch::GetLastEdgeId() const
//-------------------------------------------------------------------------
{
  int n = GetNumberOfEdges() ;
  return GetEdgeId(n-1) ;
}


//-------------------------------------------------------------------------
// Get ith vertex
vtkIdType albaPolylineGraph::Branch::GetVertexId(int i) const
//-------------------------------------------------------------------------
{
  return m_VertexId.at(i) ;
}


//-------------------------------------------------------------------------
// Set ith vertex
void albaPolylineGraph::Branch::SetVertexId(int i, vtkIdType v)
//-------------------------------------------------------------------------
{
  m_VertexId.at(i) = v ;
}



//-------------------------------------------------------------------------
// Add new vertex id to end of list
void albaPolylineGraph::Branch::AddVertexId(vtkIdType v)
//-------------------------------------------------------------------------
{
  m_VertexId.push_back(v) ;
}


//-------------------------------------------------------------------------
// Find position of vertex in list
int albaPolylineGraph::Branch::FindVertexId(vtkIdType v) const
//-------------------------------------------------------------------------
{
  for (int i = 0 ;  i < GetNumberOfVertices() ;  i++){
    if (GetVertexId(i) == v)
      return i ;  // found - return position
  }
  return m_UndefinedInt ;     // not found - return UndefinedInt
}


//-------------------------------------------------------------------------
// Insert vertex before position i
void albaPolylineGraph::Branch::InsertVertexId(int i, vtkIdType v)
//-------------------------------------------------------------------------
{
  m_VertexId.insert(m_VertexId.begin()+i, v) ;
}



//-------------------------------------------------------------------------
// Get id of last vertex
vtkIdType albaPolylineGraph::Branch::GetLastVertexId() const
//-------------------------------------------------------------------------
{
  int n = GetNumberOfVertices() ;
  return GetVertexId(n-1) ;
}



//-------------------------------------------------------------------------
// Delete last vertex and edge from branch
void albaPolylineGraph::Branch::DeleteLastVertex()
//-------------------------------------------------------------------------
{
  if (GetNumberOfVertices() > 0)
    m_VertexId.pop_back() ;

  if (GetNumberOfEdges() > 0)
    m_EdgeId.pop_back() ;
}



//-------------------------------------------------------------------------
// return vertices as a vtkIdlist
void albaPolylineGraph::Branch::GetVerticesIdList(vtkIdList *idlist) const
//-------------------------------------------------------------------------
{
  idlist->Initialize() ;

  for (int i = 0 ;  i < GetNumberOfVertices() ;  i++)
    idlist->InsertNextId(GetVertexId(i)) ;
}



//-------------------------------------------------------------------------
// Reverse direction of branch
void albaPolylineGraph::Branch::ReverseDirection()
//-------------------------------------------------------------------------
{
  vtkIdType tempId ;
  int i, j ;

  for (i = 0, j = (int)m_EdgeId.size()-1 ;  i < j ;  i++, j--){
    tempId = m_EdgeId.at(i) ;
    m_EdgeId.at(i) = m_EdgeId.at(j) ;
    m_EdgeId.at(j) = tempId ;
  }

  for (i = 0, j = (int)m_VertexId.size()-1 ;  i < j ;  i++, j--){
    tempId = m_VertexId.at(i) ;
    m_VertexId.at(i) = m_VertexId.at(j) ;
    m_VertexId.at(j) = tempId ;
  }
}




//-------------------------------------------------------------------------
// get mapping from branch to output cell
vtkIdType albaPolylineGraph::Branch::GetMappingToOutputPolydata() const 
//-------------------------------------------------------------------------
{
  return m_OutputPolydataCell ;
}

//-------------------------------------------------------------------------
// set mapping from branch to output cell
void albaPolylineGraph::Branch::SetMappingToOutputPolydata(vtkIdType cellid)
//-------------------------------------------------------------------------
{
  m_OutputPolydataCell = cellid ;
}


//-------------------------------------------------------------------------
// Clear data from branch
void albaPolylineGraph::Branch::Clear()
//-------------------------------------------------------------------------
{
  m_EdgeId.clear() ;
  m_VertexId.clear() ;
}




//-------------------------------------------------------------------------
// Test internal consistency of Branch
bool albaPolylineGraph::Branch::SelfCheck() const
//-------------------------------------------------------------------------
{
  // no. of vertices and edges should be (0,0) or (nv,nv-1)
  // n.b this assumes that an empty branch is ok.
  if (GetNumberOfVertices() == 0){
    if (GetNumberOfEdges() != 0){
      #ifndef _DEBUG
      LogMessage("empty branch contains %d edges", GetNumberOfEdges()) ;
      #endif
      return false ;
    }
  }
  else{
    if (GetNumberOfEdges() != GetNumberOfVertices()-1){
      #ifndef _DEBUG
      LogMessage("mismatched list lengths in branch: %d vertices %d edges", GetNumberOfVertices(), GetNumberOfEdges()) ;
#endif
      return false ;
    }
  }

  // branch must not contain any duplicate edges
  for (int i = 0 ;  i < GetNumberOfEdges()-1 ;  i++){
    for (int j = i+1 ;  j < GetNumberOfEdges() ;  j++){
      if (GetEdgeId(i) == GetEdgeId(j)){
        #ifndef _DEBUG
        LogMessage("duplicate edge %d found in branch at %d and %d", GetEdgeId(i), i, j) ;
        #endif
        return false ;
      }
    }
  }


  return true ;
}



//-------------------------------------------------------------------------
// Print branch
void albaPolylineGraph::Branch::PrintSelf(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  int i ;
  
  vtkIndent indent(tabs);

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
albaPolylineGraph::albaPolylineGraph()
//-------------------------------------------------------------------------
{}



//-------------------------------------------------------------------------
// Destructor
albaPolylineGraph::~albaPolylineGraph()
//-------------------------------------------------------------------------
{
  m_Vertex.clear() ;
  m_Edge.clear() ;
  m_Branch.clear() ;
}


//-------------------------------------------------------------------------
// get number of vertices
int albaPolylineGraph::GetNumberOfVertices() const
//-------------------------------------------------------------------------
{
  return (int)m_Vertex.size() ;
}

//-------------------------------------------------------------------------
// get number of edges
int albaPolylineGraph::GetNumberOfEdges() const
//-------------------------------------------------------------------------
{
  return (int)m_Edge.size() ;
}

//-------------------------------------------------------------------------
// get number of branches
int albaPolylineGraph::GetNumberOfBranches() const
//-------------------------------------------------------------------------
{
  return (int)m_Branch.size() ;
}


//-------------------------------------------------------------------------
// get last vertex id in list
vtkIdType albaPolylineGraph::GetMaxVertexId() const
//-------------------------------------------------------------------------
{
  return (vtkIdType)(m_Vertex.size() - 1) ;
}

//-------------------------------------------------------------------------
// get last edge id in list
vtkIdType albaPolylineGraph::GetMaxEdgeId() const
//-------------------------------------------------------------------------
{
  return (vtkIdType)(m_Edge.size() - 1) ;
}

//-------------------------------------------------------------------------
// get last branch id in list
vtkIdType albaPolylineGraph::GetMaxBranchId() const
//-------------------------------------------------------------------------
{
  return (vtkIdType)(m_Branch.size() - 1) ;
}





//-------------------------------------------------------------------------
// Get the vertex coords
// This allows public access to change the coords of a vertex
void albaPolylineGraph::GetVertexCoords(vtkIdType v, double *coords) const
//-------------------------------------------------------------------------
{
  GetConstVertexPtr(v)->GetCoords(coords) ;
}


//-------------------------------------------------------------------------
// Set the vertex coords
// This allows public access to change the coords of a vertex
void albaPolylineGraph::SetVertexCoords(vtkIdType v, const double *coords)
//-------------------------------------------------------------------------
{
  GetVertexPtr(v)->SetCoords(coords) ;
}


//-------------------------------------------------------------------------
// get name of branch
const char* albaPolylineGraph::GetBranchName(vtkIdType b) const
//-------------------------------------------------------------------------
{
  return GetConstBranchPtr(b)->GetName() ;
}

//-------------------------------------------------------------------------
// set name of branch
void albaPolylineGraph::SetBranchName(vtkIdType b, const char* name)
//-------------------------------------------------------------------------
{
  GetBranchPtr(b)->SetName(name) ;
}

//-------------------------------------------------------------------------
// unset name of branch to undefined
void albaPolylineGraph::UnsetBranchName(vtkIdType b)
//-------------------------------------------------------------------------
{
  GetBranchPtr(b)->UnsetName() ;
}


//-------------------------------------------------------------------------
// find id of branch with name
// return undefinedId if not found
vtkIdType albaPolylineGraph::FindBranchName(const char *name) const
//-------------------------------------------------------------------------
{  
  for (int i = 0 ;  i < GetNumberOfBranches();  i++)
  {
    const char* namei = GetConstBranchPtr(i)->GetName() ;
    if (strcmp(namei, name) == 0)
      return i ;
  }

  return m_UndefinedId ;
}


//-------------------------------------------------------------------------
// get directed property of edge
bool albaPolylineGraph::IsEdgeDirected(vtkIdType e) const
//-------------------------------------------------------------------------
{
  return GetConstEdgePtr(e)->IsDirected() ;
}

//-------------------------------------------------------------------------
// set directed property of edge
void albaPolylineGraph::SetEdgeDirected(vtkIdType e, bool directed)
//-------------------------------------------------------------------------
{
  GetEdgePtr(e)->SetDirected(directed) ;
}

//-------------------------------------------------------------------------
// reverse direction of edge, ie swap end vertices
void albaPolylineGraph::ReverseEdge(vtkIdType e)
//-------------------------------------------------------------------------
{
  GetEdgePtr(e)->ReverseDirection() ;
}


//-------------------------------------------------------------------------
// get weight of edge
double albaPolylineGraph::GetEdgeWeight(vtkIdType e) const
//-------------------------------------------------------------------------
{
  return GetConstEdgePtr(e)->GetWeight() ;
}

//-------------------------------------------------------------------------
// set weight of edge
void albaPolylineGraph::SetEdgeWeight(vtkIdType e, double wt)
//-------------------------------------------------------------------------
{
  GetEdgePtr(e)->SetWeight(wt) ;
}


//-------------------------------------------------------------------------
// return pointer to vertex
// returns NULL if index is out of range
albaPolylineGraph::Vertex* albaPolylineGraph::GetVertexPtr(vtkIdType i)
//-------------------------------------------------------------------------
{
  if ((i >= 0) && (i < GetNumberOfVertices()))
    return &(m_Vertex.at(i)) ;
  else
    return NULL ;
}

//-------------------------------------------------------------------------
// return pointer to edge
albaPolylineGraph::Edge* albaPolylineGraph::GetEdgePtr(vtkIdType i)
//-------------------------------------------------------------------------
{
  if ((i >= 0) && (i < GetNumberOfEdges()))
    return &(m_Edge.at(i)) ;
  else
    return NULL ;
}


//-------------------------------------------------------------------------
// return pointer to branch
albaPolylineGraph::Branch* albaPolylineGraph::GetBranchPtr(vtkIdType i)
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
const albaPolylineGraph::Vertex* albaPolylineGraph::GetConstVertexPtr(vtkIdType i) const
//-------------------------------------------------------------------------
{
  if ((i >= 0) && (i < GetNumberOfVertices()))
    return &(m_Vertex.at(i)) ;
  else
    return NULL ;
}

//-------------------------------------------------------------------------
// return pointer to edge
const albaPolylineGraph::Edge* albaPolylineGraph::GetConstEdgePtr(vtkIdType i) const
//-------------------------------------------------------------------------
{
  if ((i >= 0) && (i < GetNumberOfEdges()))
    return &(m_Edge.at(i)) ;
  else
    return NULL ;
}

//-------------------------------------------------------------------------
// return pointer to branch
const albaPolylineGraph::Branch* albaPolylineGraph::GetConstBranchPtr(vtkIdType i) const
//-------------------------------------------------------------------------
{
  if ((i >= 0) && (i < GetNumberOfBranches()))
    return &(m_Branch.at(i)) ;
  else
    return NULL ;
}


//-------------------------------------------------------------------------
// initialize or resize list of vertices
void albaPolylineGraph::AllocateVertices(int nv)
//-------------------------------------------------------------------------
{
  Vertex v ;
  m_Vertex.resize(nv, v) ;
}

//-------------------------------------------------------------------------
// initialize or resize list of edges
void albaPolylineGraph::AllocateEdges(int ne)
//-------------------------------------------------------------------------
{
  Edge e ;
  m_Edge.resize(ne, e) ;
}

//-------------------------------------------------------------------------
// initialize or resize list of branches
void albaPolylineGraph::AllocateBranches(int nb)
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
bool albaPolylineGraph::CopyFromPolydata(vtkPolyData *polydata)
//-------------------------------------------------------------------------
{
  vtkIdType i ;
  int j ;

  // check that data is polyline only
  int ncells = polydata->GetNumberOfCells() ;
  int nlines = polydata->GetNumberOfLines() ;
  if (nlines != ncells){
    LogMessage("Input data is not polyline") ;
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
bool albaPolylineGraph::CopyToPolydata(vtkPolyData *polydata)
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
    if (b == m_UndefinedId){
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
void albaPolylineGraph::GetOutputCellCorrespondingToEdge(vtkIdType edgeid, vtkIdType *cellid, vtkIdType *index) const
//-------------------------------------------------------------------------
{
  GetConstEdgePtr(edgeid)->GetMappingToOutputPolydata(cellid, index) ;

  // undefined values - fatal error
  if ((*cellid == m_UndefinedId) || (*index == m_UndefinedId)){
    LogMessage("GetOutputCellCorrespondingToEdge() can't find output cell corresponding to edge %d", edgeid) ;
    LogMessage("Perhaps CopyToPolydata() has not been called ?") ;
    assert(false) ;
  }
}


//-------------------------------------------------------------------------
// Get edge id in graph which is (cellid,index) in the output polydata
// The mapping is set by CopyToPolydata()
vtkIdType albaPolylineGraph::GetEdgeCorrespondingToOutputCell(vtkIdType cellid, vtkIdType index) const
//-------------------------------------------------------------------------
{
  vtkIdType edgeid, j, k ;

  for (edgeid = 0 ;  edgeid < GetNumberOfEdges() ;  edgeid++){
    GetConstEdgePtr(edgeid)->GetMappingToOutputPolydata(&j, &k) ;
    if ((j == cellid) && (k == index))
      return edgeid ;
  }

  // output cell and index not found in edge mapping - fatal error
  LogMessage("GetEdgeCorrespondingToOutputCell() can't find output cell corresponding to edge %d", edgeid) ;
  LogMessage("Perhaps CopyToPolydata() has not been called ?") ;
  assert(false) ;
  return -1 ;
}


//-------------------------------------------------------------------------
// Get location of branch in output polydata
// The mapping is set by CopyToPolydata()
void albaPolylineGraph::GetOutputCellCorrespondingToBranch(vtkIdType branchid, vtkIdType *cellid) const
//-------------------------------------------------------------------------
{
  *cellid = GetConstBranchPtr(branchid)->GetMappingToOutputPolydata() ;

  // undefined values - fatal error
  if (*cellid == m_UndefinedId){
    LogMessage("GetOutputCellCorrespondingToBranch() can't find output cell corresponding to branch %d", branchid) ;
    LogMessage("Perhaps CopyToPolydata() has not been called ?") ;
    assert(false) ;
  }
}


//-------------------------------------------------------------------------
// Get branch id in graph which corresponds to cellid in the output polydata
// The mapping is set by CopyToPolydata()
// Returns -1 if branch not found.
vtkIdType albaPolylineGraph::GetBranchCorrespondingToOutputCell(vtkIdType cellid) const
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
void albaPolylineGraph::Clear()
//-------------------------------------------------------------------------
{
  m_Vertex.clear() ;
  m_Edge.clear() ;
  m_Branch.clear() ;
}


//-------------------------------------------------------------------------
// Test self-consistency of graph
bool albaPolylineGraph::SelfCheck() const
//-------------------------------------------------------------------------
{
  vtkIdType v, b, e ;
  int j ;

  // self-check the vertices
  for (v = 0 ;  v < GetNumberOfVertices() ;  v++){
    if (!GetConstVertexPtr(v)->SelfCheck()){
      LogMessage("graph self-check error: vertex %d failed self check", v) ;
      return false ;
    }
  }

  // self-check the edges
  for (e = 0 ;  e < GetNumberOfEdges() ;  e++){
    if (!GetConstEdgePtr(e)->SelfCheck()){
      LogMessage("graph self-check error: edge %d failed self check", e) ;
      return false ;
    }
  }

  // self-check the branches
  for (b = 0 ;  b < GetNumberOfBranches() ;  b++){
    if (!GetConstBranchPtr(b)->SelfCheck()){
      LogMessage("graph self-check error: branch %d failed self check", b) ;
      return false ;
    }
  }


  // check that the sum of the degrees of the vertices is 2 * the number of edges
  int sumdeg ;
  for (v = 0, sumdeg = 0 ;  v < GetNumberOfVertices() ;  v++)
    sumdeg += GetConstVertexPtr(v)->GetDegree() ;
  if (sumdeg != 2*GetNumberOfEdges()){
    LogMessage("graph self check error: sum of degrees %d != 2 * no. of edges %d", sumdeg, GetNumberOfEdges()) ;
    return false ;
  }

  // check that the sum of the edges in the branches is equal to the sum of the branches in edges
  int sum1, sum2 ;
  for (b = 0, sum1 = 0 ;  b < GetNumberOfBranches() ;  b++)
    sum1 += GetConstBranchPtr(b)->GetNumberOfEdges() ;
  for (e = 0, sum2 = 0 ;  e < GetNumberOfEdges() ;  e++){
    if (GetConstEdgePtr(e)->GetBranchId() != m_UndefinedId)
      sum2++ ;
  }
  if (sum1 != sum2){
    LogMessage("graph self check error: no. of edges in branches %d != no. of branches in edges %d", sum1, sum2) ;
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
        LogMessage("graph self check error: edge index %d or vertex index %d on vertex %d out of range", ke, kv, v) ;
        return false ;
      }

      // edge ke must be the vertex pair (v, kv)
      if (!GetConstEdgePtr(ke)->IsVertexPair(v, kv)){
        LogMessage("graph self check error: edge %d is not vertex pair %d %d from vertex %d", ke, v, kv, v) ;
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
        LogMessage("graph self check error: vertex index %d out of range in edge %d", kv, e) ;
        return false ;
      }

      // vertex kv must contain this edge
      if (GetConstVertexPtr(kv)->FindEdgeId(e) == m_UndefinedInt){
        LogMessage("graph self check error: vertex %d does not contain edge %d", kv, e) ;
        return false ;
      }

      if (kb != m_UndefinedId){
        // branch index must be valid
        if (GetConstBranchPtr(kb) == NULL){
          LogMessage("graph self check error: branch index %d out of range in edge %d", kb, e) ;
          return false ;
        }

        // branch kb must contain this edge
        if (GetConstBranchPtr(kb)->FindEdgeId(e) == m_UndefinedInt){
          LogMessage("graph self check error: branch %d does not contain edge %d", kb, e) ;
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
        LogMessage("graph self check error: edge index %d out of range in branch %d", ke, b) ;
        return false ;
      }

      // edge ke must point back at this branch
      if (GetConstEdgePtr(ke)->GetBranchId() != b){
        LogMessage("graph self check error: edge %d does not point back at branch %d", ke, b) ;
        return false ;
      }
    }

    for (j = 0 ;  j < br->GetNumberOfVertices() ;  j++){
      vtkIdType kv = br->GetVertexId(j) ;             // kv = index of vertex j on branch i

      // indices read from branch must point to valid objects
      if (GetConstVertexPtr(kv) == NULL){
        LogMessage("graph self check error: vertex index %d out of range in branch %d", kv, b) ;
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
        LogMessage("graph self check error: edge %d in branch %d does not join vertices %d and %d", ke, b, kv0, kv1) ;
        return false ;
      }
    }
  }

  return true ;
}



//-------------------------------------------------------------------------
// Print graph
void albaPolylineGraph::PrintSelf(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  int i ;
  
  vtkIndent indent(tabs);

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
bool albaPolylineGraph::IsConnected() const
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
    nfound += (unsigned int)vertexlist1.size() ;

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
bool albaPolylineGraph::IsConnectedTree() const
//-------------------------------------------------------------------------
{
  if (!IsConnected())
    return false ;

  return (GetNumberOfEdges() == GetNumberOfVertices()-1) ;
}





//-------------------------------------------------------------------------
// Add a new edge between existing vertices
bool albaPolylineGraph::AddNewEdge(vtkIdType v0, vtkIdType v1)
//-------------------------------------------------------------------------
{
  // check that input indices are in range
  Vertex *vert0 = GetVertexPtr(v0) ;
  Vertex *vert1 = GetVertexPtr(v1) ;
  if ((vert0 == NULL) || (vert1 == NULL)){
    LogMessage("AddNewEdge() failed: vertex indices %d %d out of range", v0, v1) ;
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
void albaPolylineGraph::AddNewVertex(double *coords)
//-------------------------------------------------------------------------
{
  Vertex vert ;

  if (coords != NULL)
    vert.SetCoords(coords) ;

  m_Vertex.push_back(vert) ;
}



//-------------------------------------------------------------------------
// Add a new vertex to the graph, joining to existing vertex v0 with a new edge
bool albaPolylineGraph::AddNewVertex(vtkIdType v0, double *coords)
//-------------------------------------------------------------------------
{
  // check that input indices are in range
  Vertex *vert0 = GetVertexPtr(v0) ;
  if (vert0 == NULL){
    LogMessage("AddNewVertex(v0) failed: vertex index %d out of range", v0) ;
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
void albaPolylineGraph::AddNewBranch(const char *name)
//-------------------------------------------------------------------------
{
  Branch br(name) ;
  m_Branch.push_back(br) ;
}

//-------------------------------------------------------------------------
// Add a new empty branch to the graph
bool albaPolylineGraph::AddNewBranch(vtkIdType v0, const char *name)
//-------------------------------------------------------------------------
{
  // check that input indices are in range
  Vertex *vert0 = GetVertexPtr(v0) ;
  if (vert0 == NULL){
    LogMessage("AddNewBranch(v0) failed: vertex index %d out of range", v0) ;
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
bool albaPolylineGraph::SplitBranchAtEdge(vtkIdType b, vtkIdType e)
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
  if (i == m_UndefinedInt){
    LogMessage("SplitBranchAtEdge() can't find edge %d in branch %d", e, b) ;
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
  GetEdgePtr(e)->SetBranchId(m_UndefinedId) ;

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
bool albaPolylineGraph::SplitBranchAtVertex(vtkIdType b, vtkIdType v)
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
  if (i == m_UndefinedInt){
    LogMessage("SplitBranchAtVertex() can't find vertex %d in branch %d", v, b) ;
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
void albaPolylineGraph::AddNewVertexToBranch(vtkIdType b, double *coords)
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
bool albaPolylineGraph::AddExistingVertexToBranch(vtkIdType b, vtkIdType v, vtkIdType e)
//-------------------------------------------------------------------------
{
  Branch *Br = GetBranchPtr(b) ;

  if (Br == NULL){
    LogMessage("AddExistingVertexToBranch() found branch %d out of range", b) ;
    return false ;
  }

  if (GetVertexPtr(v) == NULL){
    LogMessage("AddExistingVertexToBranch() found vertex %d out of range", v) ;
    return false ;
  }

  if (Br->GetNumberOfVertices() == 0){
    // empty branch - just add vertex
    Br->AddVertexId(v) ;
  }
  else{
    // not empty branch - find edge to make join
    vtkIdType vlast = Br->GetLastVertexId() ;

    if (e == m_UndefinedId){
      // find the edge which joins vlast to v
      int j = GetVertexPtr(vlast)->FindVertexId(v) ;
      if (j == m_UndefinedInt){
        LogMessage("AddExistingVertexToBranch() can't find edge joining %d to %d", vlast, v) ;
        return false ;
      }
      e = GetVertexPtr(vlast)->GetEdgeId(j) ;
    }
    else{
      // check that the given edge really joins vlast to v
      if (!GetEdgePtr(e)->IsVertexPair(vlast, v)){
        LogMessage("AddExistingVertexToBranch(): edge %d does not join vertices %d and %d", e, vlast, v) ;
        return false ;
      }
    }

    // check that e does not already belong to a branch
    if (GetEdgePtr(e)->GetBranchId() != m_UndefinedId){
      LogMessage("AddExistingVertexToBranch(): edge %d already belongs to a branch", e) ;
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
bool albaPolylineGraph::AddExistingEdgeToBranch(vtkIdType b, vtkIdType e)
//-------------------------------------------------------------------------
{
  Branch *Br = GetBranchPtr(b) ;
  Edge *Ed = GetEdgePtr(e) ;

  if (Br == NULL){
    LogMessage("AddExistingEdgeToBranch() found branch %d out of range", b) ;
    return false ;
  }

  if (Ed == NULL){
    LogMessage("AddExistingEdgeToBranch() found edge %d out of range", e) ;
    return false ;
  }

  // check that edge does not already belong to a branch
  if (GetEdgePtr(e)->GetBranchId() != m_UndefinedId){
    LogMessage("AddExistingEdgeToBranch(): edge %d already belongs to a branch", e) ;
    return false ;
  }

  if (Br->GetNumberOfVertices() == 0){
    // empty branch - don't know which vertex is the start of the branch
    LogMessage("AddExistingEdgeToBranch(): branch %d is empty, need at least one start vertex", b) ;
    return false ;
  }


  //find last vertex in branch
  vtkIdType vlast = Br->GetLastVertexId() ;

  // find the vertex at the other end of the edge
  vtkIdType vend = Ed->GetOtherEndVertexId(vlast) ;

  if (vend == m_UndefinedId){
    LogMessage("AddExistingEdgeToBranch() can't join edge %d to branch %d", e, b) ;
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
void albaPolylineGraph::ReverseBranch(vtkIdType b)
//-------------------------------------------------------------------------
{
  GetBranchPtr(b)->ReverseDirection() ;
}


//-------------------------------------------------------------------------
// Delete edge from graph
// If the edge was a member of a branch, the branch will be split.
// Be careful: this also moves the edge at the end of the list to index i */
bool albaPolylineGraph::DeleteEdge(vtkIdType i)
//-------------------------------------------------------------------------
{
  vtkIdType elast = GetMaxEdgeId() ;

  if (!SwapEdgeIndices(i,elast)) 
    return false ;

  if (!DeleteLastEdge()){
    LogMessage("DeleteEdge() cant delete edge %d", i) ;
    return false ;
  }

  return true ;
}



//-------------------------------------------------------------------------
// Delete last edge from the graph. 
// If the edge was a member of a branch, the branch will be split.
// To delete any edge, first move the edge to the end with GetMaxEdgeId() and SwapEdgeIndices().
//-------------------------------------------------------------------------
bool albaPolylineGraph::DeleteLastEdge()
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
  if (br1 != m_UndefinedId){
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
bool albaPolylineGraph::DeleteVertex(vtkIdType i)
//-------------------------------------------------------------------------
{
  vtkIdType elast = GetMaxVertexId() ;

  if (!SwapVertexIndices(i,elast)) 
    return false ;

  if (!DeleteLastVertex()){
    LogMessage("DeleteVertex() cant delete vertex %d", i) ;
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
bool albaPolylineGraph::DeleteLastVertex()
{
  // get the id of the last vertex
  vtkIdType vlast = GetMaxVertexId() ;

  Vertex *vert = GetVertexPtr(vlast) ;
  if (vert->GetDegree() != 0){
    LogMessage("DeleteLastVertex() attempt to delete vertex %d of degree %d", vlast, vert->GetDegree()) ;
    return false ;
  }

  // remove branches which refer to vertex
  for (vtkIdType b = 0 ;  b < GetNumberOfBranches() ;  b++){
    Branch *br = GetBranchPtr(b) ;
    int j = br->FindVertexId(vlast) ;
    if (j != m_UndefinedInt){
      if (br->GetNumberOfVertices() != 1){
        // found a branch with the vertex, but it should be the only only one in the branch
        LogMessage("DeleteLastVertex() found branch %d with %d vertices", b, br->GetNumberOfVertices()) ;
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
bool albaPolylineGraph::DeleteBranch(vtkIdType i)
//-------------------------------------------------------------------------
{
  vtkIdType elast = GetMaxBranchId() ;

  if (!SwapBranchIndices(i,elast)) 
    return false ;

  if (!DeleteLastBranch()){
    LogMessage("DeleteBranch() cant delete branch %d", i) ;
    return false ;
  }

  return true ;
}


//-------------------------------------------------------------------------
// Delete last branch from the graph. 
// To delete any edge, first move to the end with GetMaxBranchId() and SwapBranchIndices().
//-------------------------------------------------------------------------
bool albaPolylineGraph::DeleteLastBranch()
{
  int i ;

  // get the id of the last branch
  vtkIdType blast = GetMaxBranchId() ;
  Branch *Br = GetBranchPtr(blast) ;

  // remove references to the branch from the edges
  for (i = 0 ;  i < Br->GetNumberOfEdges() ;  i++){
    vtkIdType e = Br->GetEdgeId(i) ;
    GetEdgePtr(e)->SetBranchId(m_UndefinedId) ;
  }

  // finally delete the branch from the end of the list
  m_Branch.pop_back() ;

  return true ;
}



//-------------------------------------------------------------------------
// Swap vertex indices throughout graph
// NB Changes in indices should be explicit to the user, so try not to
// hide this method inside other methods.
bool albaPolylineGraph::SwapVertexIndices(vtkIdType v0, vtkIdType v1)
//-------------------------------------------------------------------------
{
  vtkIdType v, e, b ;
  int j ;

  if ((GetVertexPtr(v0) == NULL) || (GetVertexPtr(v1) == NULL)){
    LogMessage("invalid input indices %d %d in SwapVertexIndices()", v0, v1) ;
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
bool albaPolylineGraph::SwapEdgeIndices(vtkIdType e0, vtkIdType e1)
//-------------------------------------------------------------------------
{
  vtkIdType v, b ;
  int j ;

  if ((GetEdgePtr(e0) == NULL) || (GetEdgePtr(e1) == NULL)){
    LogMessage("invalid input indices %d %d in SwapEdgeIndices()", e0, e1) ;
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
bool albaPolylineGraph::SwapBranchIndices(vtkIdType b0, vtkIdType b1)
//-------------------------------------------------------------------------
{
  vtkIdType e ;

  if ((GetBranchPtr(b0) == NULL) || (GetBranchPtr(b1) == NULL)){
    LogMessage("invalid input indices %d %d in SwapBranchIndices()", b0, b1) ;
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


double albaPolylineGraph::GetBranchLength(vtkIdType b) const
{
  double sum = 0;

  const albaPolylineGraph::Branch *br = GetConstBranchPtr(b);
  assert(br != NULL);

  vtkIdList *verticesIdList = vtkIdList::New();
  br->GetVerticesIdList(verticesIdList);

  for (int i = 0 ;  i < br->GetNumberOfVertices() ;  i++)
  { 
    if (i > 0)
    {
      double pos1[3], pos2[3];

      const albaPolylineGraph::Vertex *vertex1 = GetConstVertexPtr(verticesIdList->GetId(i));
      vertex1->GetCoords(pos1);

      const albaPolylineGraph::Vertex *vertex2 = GetConstVertexPtr(verticesIdList->GetId(i-1));
      vertex2->GetCoords(pos2);

      sum += sqrt(vtkMath::Distance2BetweenPoints(pos1, pos2));

    }
  }

  // clean up
  verticesIdList->Delete();  
  return sum;
}

double albaPolylineGraph::GetBranchIntervalLength(vtkIdType b, vtkIdType startVertexId, vtkIdType endVertexId) const
{
  double sum = 0;

  // assert(startVertexId  <=  endVertexId );

  const albaPolylineGraph::Branch *br = GetConstBranchPtr(b);
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

  //assert(endId >= startId );
  if (endId <= startId)
  {
    int tmp = endId;
    endId = startId;
    startId = tmp;
  }

  for (int k = startId ;  k < endId ;  k++)
  { 
    double pos1[3], pos2[3];

    const albaPolylineGraph::Vertex *vertex1 = GetConstVertexPtr(verticesIdList->GetId(k));
    vertex1->GetCoords(pos1);

    const albaPolylineGraph::Vertex *vertex2 = GetConstVertexPtr(verticesIdList->GetId(k+1));
    vertex2->GetCoords(pos2);

    sum += sqrt(vtkMath::Distance2BetweenPoints(pos1, pos2));
  }

  // clean up
  verticesIdList->Delete();
  return sum;
}

#pragma region BES April 2008
//------------------------------------------------------------------------
/** Merges two branches.
Both branches (identified by b1 and b2) must share common endpoint.
Be careful: this also moves the branch at the end of the list to index b2.*/
bool albaPolylineGraph::MergeBranches(vtkIdType b1, vtkIdType b2)
{  
  if (b1 == b2)
    return true;

 Branch* br1 = GetBranchPtr(b1);
 Branch* br2 = GetBranchPtr(b2);

  if (br1 == NULL || br2 == NULL){
    LogMessage("invalid input branches %d %d in MergeBranches()", b1, b2) ;
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
      LogMessage("branches %d %d does not share common vertex in MergeBranches()", b1, b2) ;
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
bool albaPolylineGraph::MergeSimpleJoinedBranchesAtVertex(vtkIdType v)
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
void albaPolylineGraph::MergeSimpleJoinedBranches()
{
  for (vtkIdType v = 0; v < GetNumberOfVertices(); v++) {
    MergeSimpleJoinedBranchesAtVertex(v);
  }

  //BES 19.8.2008 - remove all empty branches
  int nCount = GetNumberOfBranches();
  for (int i = 0; i < nCount; )
  {
    if (GetConstBranchPtr(i)->GetNumberOfEdges() != 0)
      i++;
    else
    {
      DeleteBranch(i);      
      nCount--;
    }    
  }
}
#pragma endregion

////------------------------------------------------------------------------
////Logs error messages. Default implementation uses VTK stream. 
///*virtual*/ void albaPolylineGraph::LogMessageF(const char* format, ...)
////------------------------------------------------------------------------
//{
//  va_list marker;
//  va_start(marker, format);
//  
//  char szStBuffer[128]; //this should be enough to deal with majority of calls   
//  
//  if (vsnprintf(szStBuffer, 127, format, marker) >= 0)
//    LogMessage(szStBuffer);  //fast static buffer is long enough
//  else
//  {
//    int nDynSize = 256;
//    char* szDynBuffer = (char*)malloc(nDynSize);
//    while (vsnprintf(szDynBuffer, nDynSize - 1, format, marker) < 0)
//    {
//      nDynSize *= 2;
//      szDynBuffer = (char*)realloc(szDynBuffer, nDynSize);
//    }
//
//    LogMessage(szDynBuffer);
//    free(szDynBuffer);
//  }
//  
//  va_end(marker);
//}