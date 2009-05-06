/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPolylineGraph.h,v $
Language:  C++
Date:      $Date: 2009-05-06 14:22:15 $
Version:   $Revision: 1.9.2.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================
Modified by Josef Kohout to strip off MAF stuff from it making it possible
to use in VTK filters (=> wxString was replaced by char*)*/
#ifndef __mafPolylineGraph_h
#define __mafPolylineGraph_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "vtkPolydata.h"
#include <ostream>
#include <vector>


//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/** mafPolylineGraph class 

mafPolylineGraph is a class for navigating and editing polyline data.

It consists of a list of vertices, edges and branches.
Vertices are points.
Edges connect pairs of vertices.
Branches are named sequences of connected vertices and edges.

The graph can read from and write to vtkPolydata.
Navigation and editing is much simpler with this class than vtkPolydata.

mafPolylineGraph provides top-level methods which always leave a self-consistent graph, ie SelfCheck() = true.
There is also public read-only access to the vertices, edges and branches using:
mafPolylineGraph->GetConstVertexPtr()
mafPolylineGraph->GetConstEdgePtr()
mafPolylineGraph->GetConstBranchPtr()

Eg to get the degree of vertex i:
Graph->GetConstVertexPtr(i)->GetDegree() ;

Keeping track of object ids:
This scheme causes the least inconvenience to the user and the programmer:
Methods which create new vertices, edges and branches do so at the end of the id list,
so you can easily get the id of the new object with Graph->GetMaxVertexId() etc.
Methods which delete objects overwrite them from the end of the list, and delete the end of the list.
Eg Graph->DeleteBranch(i) moves branch(idmax) to branch(i), and deletes branch(idmax).

Branches: adding and removing edges
You can only add and remove edges and vertices from a branch by editing the branch.
To remove an edge from a branch you have to split the branch.
Edges and vertices can only be added to the end of the branch.
Adding and deleting items from branches has no effect on the graph connectivity.
*/

//----------------------------------------------------------------------------
class mafPolylineGraph
{
public:  
  /** consts for undefined index values */
  const static vtkIdType UndefinedId;
  const static int UndefinedInt;
  const static char* UndefinedName;

public:
  //-----------------------------------------------------------------------------
  // public methods
  //-----------------------------------------------------------------------------
  mafPolylineGraph() ;                       ///< constructor: empty graph
  ~mafPolylineGraph() ;                      ///< destructor
  int GetNumberOfVertices() const ;          ///< get no. of vertices
  int GetNumberOfEdges() const ;             ///< get no. of edges
  int GetNumberOfBranches() const ;          ///< get no. of branches
  vtkIdType GetMaxVertexId() const ;         ///< get index of last vertex in graph
  vtkIdType GetMaxEdgeId() const ;           ///< get index of last edge in graph
  vtkIdType GetMaxBranchId() const ;         ///< get index of last branch in graph

  void GetVertexCoords(vtkIdType v, double *coords) const ;      ///< get coords of vertex
  void SetVertexCoords(vtkIdType v, const double *coords) ;      ///< set coords of vertex

  const char* GetBranchName(vtkIdType b) const ;      ///< get name of branch
  void SetBranchName(vtkIdType b, const char* name) ; ///< set name of branch
  void UnsetBranchName(vtkIdType b) ;                 ///< unset name of branch to undefined
  vtkIdType FindBranchName(const char* name) const ;  ///< find branch with name

  double GetBranchLength(vtkIdType b) const;              ///< return the length of the branch
  double GetBranchIntervalLength(vtkIdType b, vtkIdType startVertexId, vtkIdType endVertexId) const;

  bool IsEdgeDirected(vtkIdType e) const ;                ///< get directed property of edge
  void SetEdgeDirected(vtkIdType e, bool directed) ;      ///< set directed property of edge
  void ReverseEdge(vtkIdType e) ;                         ///< reverse direction of edge (swap end vertices)

  double GetEdgeWeight(vtkIdType e) const ;               ///< get weight of edge
  void SetEdgeWeight(vtkIdType e, double wt) ;            ///< set weight of edge

  bool AddNewEdge(vtkIdType v0, vtkIdType v1) ;           ///< Add new edge between existing vertices

  /** Add new vertex to graph.
  The vertex can be empty or it can connect to an existing vertex with a new edge 
  You can optionally specify the coords of the vertex */
  void AddNewVertex(double *coords = NULL) ;                ///< add new empty vertex
  bool AddNewVertex(vtkIdType v0, double *coords = NULL) ;  ///< add new vertex, connecting to existing vertex

  /** Add new branch to graph */
  void AddNewBranch(const char* name = NULL) ;               ///< add new empty branch with optional name
  bool AddNewBranch(vtkIdType v0, const char* name = NULL) ; ///< add new branch with start vertex and optional name

  /** Read graph from vtkPolyData (polyline only).
  Each polydata point becomes a graph vertex.
  Each polydata cell (line or polyline) becomes a graph branch.
  Each polydata line or line segment becomes a graph edge. 
  The points stay in the same order: point i in the graph is the same as point i in the polydata.*/
  bool CopyFromPolydata(vtkPolyData *polydata) ;

  /** Write polyline graph to vtkPolyData.
  Each graph vertex becomes a polydata point.
  Each graph branch becomes a line or polyline.
  Edges which are not members of branches become lines.
  The points stay in the same order: point i in the polydata is the same as point i in the graph.
  This also sets the mapping between the graph edges and the cells in the output polydata.*/
  bool CopyToPolydata(vtkPolyData *polydata) ;

  /* Get location of edge in output polydata.
  Returns the cellid and the index on the cell.
  The mapping is set by CopyToPolydata(). */
  void GetOutputCellCorrespondingToEdge(vtkIdType edgeid, vtkIdType *cellid, vtkIdType *index) const ;

  /* Get edge id in graph which corresponds to (cellid,index) in the output data
  The mapping is set by CopyToPolydata(). */
  vtkIdType GetEdgeCorrespondingToOutputCell(vtkIdType cellid, vtkIdType index) const ;

  /* Get location of branch in output polydata.
  The mapping is set by CopyToPolydata(). */
  void GetOutputCellCorrespondingToBranch(vtkIdType branchid, vtkIdType *cellid) const ;

  /* Get branch id in graph corresponding to cell in output polydata.
  The mapping is set by CopyToPolydata().
  NB this returns -1 if the cell does not map to a branch. */
  vtkIdType GetBranchCorrespondingToOutputCell(vtkIdType cellid) const ;

  /** Split branch by finding and removing edge.
  A new branch is created at the end of the list. 
  The front section of the split gets the name. */
  bool SplitBranchAtEdge(vtkIdType b, vtkIdType e) ;

  /** Split branch at vertex v.
  A new branch is created at the end of the list.
  The front section of the split gets the name. */
  bool SplitBranchAtVertex(vtkIdType b, vtkIdType v) ;

  /** Add new vertex to end of branch. 
  If the branch is empty, this adds creates a new vertex and adds it to the branch. 
  If the branch is not empty, it creates a new vertex and edge and adds them. */
  void AddNewVertexToBranch(vtkIdType b, double *coords = NULL) ;

  /** Add existing vertex to end of branch. 
  You can optionally specify the edge required to make the join.
  If the edge is omitted, it will be found automatically. */
  bool AddExistingVertexToBranch(vtkIdType b, vtkIdType v, vtkIdType e = UndefinedId) ;

  /** Add existing edge to end of branch.
  The edge must share a vertex with the end of the branch, and must not already belong to a branch. 
  The branch must not be empty. */
  bool AddExistingEdgeToBranch(vtkIdType b, vtkIdType e) ;

  /** Reverse direction of branch 
  This is useful if you want to add items at the front end of the branch. */
  void ReverseBranch(vtkIdType b) ;

  /** Delete edge from graph
  If the edge was a member of a branch, the branch will be split.
  Be careful: this also moves the edge at the end of the list to index i. */
  bool DeleteEdge(vtkIdType i) ;

  /** Delete vertex from the graph.    
  The vertex must be degree zero, so you have to remove any edges first.
  Any branches containing this vertex will be left empty.
  Be careful: this also moves the vertex at the end of the list to index i. */
  bool DeleteVertex(vtkIdType i) ;

  /** Delete branch from the graph.    
  Be careful: this also moves the branch at the end of the list to index i. */
  bool DeleteBranch(vtkIdType i) ;

#pragma region BES April 2008
  /** Merges two branches.
  Both branches (identified by b1 and b2) must share common endpoint.
  Be careful: this also moves the branch at the end of the list to index b2.*/
  bool MergeBranches(vtkIdType b1, vtkIdType b2);

  /** Merges two branches connected at the given vertex.
  The vertex may not be at a bifurcation, i.e., it must be of order 2
  Be careful: this also moves the branch at the end of the list to index b1.*/
  bool MergeSimpleJoinedBranchesAtVertex(vtkIdType v);

  /** Merges all branches in the graph connected in vertices of order 2  
  Be careful: this leads to reindexing of branches */
  void MergeSimpleJoinedBranches();
#pragma endregion

  bool IsConnected() const ;                                ///< is graph connected
  bool IsConnectedTree() const ;                            ///< is graph a connected tree
  void Clear() ;                                            ///< clear graph
  bool SelfCheck() const ;                                  ///< check internal consistency
  void PrintSelf(std::ostream& os, const int tabs) const ;  ///< print self

protected:
  ///** Logs messages. Formatted */
  //void LogMessageF(const char* format, ...);

  ///** Logs message.  */
  //virtual void LogMessage(const char* szMessage) {
  //  //default implementation is to do nothing
  //}
public:
  //----------------------------------------------------------------------------
  /** Nested Vertex class 
  This is a list of the edges and vertices which are joined to this vertex.
  The ith edge joins this vertex to the ith vertex neighbour. 
  Coordinates are optional */
  //----------------------------------------------------------------------------
  class Vertex{
  public:
    Vertex() ;                   ///< constructor
    Vertex(double *coords) ;     ///< constructor
    ~Vertex() ;                  ///< destructor
    int GetDegree() const ;                 ///< return degree of vertex
    vtkIdType GetEdgeId(int i) const ;      ///< get index of ith edge
    void SetEdgeId(int i, vtkIdType e) ;    ///< set ith edge
    void AddEdgeId(vtkIdType e) ;           ///< add new edge id to end of list
    int FindEdgeId(vtkIdType e) const ;     ///< find position (0..deg-1) of this edge id in the list, return -1 if invalid
    bool DeleteEdgeId(vtkIdType e) ;        ///< delete this edge and assoc. vertex from the vertex
    vtkIdType GetVertexId(int i) const ;    ///< get index of ith vertex neighbour
    void SetVertexId(int i, vtkIdType v) ;  ///< set ith vertex neighbour
    void AddVertexId(vtkIdType v) ;         ///< add new vertex id to end of list
    int FindVertexId(vtkIdType v) const ;   ///< find position (0..deg-1) of this vertex id in the list, return undefinedInt if invalid
    bool DeleteVertexId(vtkIdType v) ;      ///< delete this vertex and assoc. edge from the vertex
    void GetVerticesIdList(vtkIdList *idlist) const ;         ///< return vertices as a vtkIdlist
    void GetCoords(double *coords) const ;                    ///< get coords of vertex
    void SetCoords(const double *coords) ;                    ///< set coords of vertex  
    void Clear() ;                                            ///< clear data from vertex
    bool SelfCheck() const ;                                  ///< check self consistency
    void PrintSelf(std::ostream& os, const int tabs) const ;  ///< print self
  private:
    double m_Coords[3] ;                    ///< coordinates of point
    std::vector<vtkIdType> m_EdgeId ;         ///< edges 0..deg-1 attached to vertex
    std::vector<vtkIdType> m_VertexId ;       ///< vertices 0..deg-1 attached to vertex, in same order
  } ;


  //----------------------------------------------------------------------------
  /** Nested Edge class 
  This is a vertex pair.
  The edge can have a weight, and can be directed.
  The edge can optionally be a member of a branch. */
  //----------------------------------------------------------------------------
  class Edge{
  public:
    Edge() ;                                     ///< constructor
    Edge(vtkIdType v0, vtkIdType v1) ;           ///< constructor with end vertices
    Edge(const Edge& e) ;                        ///< copy constructor (because array member doesn't deep copy)
    ~Edge() ;                                    ///< destructor
    Edge& operator=(const Edge& e) ;             ///< assignment operator
    vtkIdType GetVertexId(int i) const ;                    ///< get index of vertex i, where i = 0 or 1
    void SetVertexId(int i, vtkIdType v) ;                  ///< set index of vertex, where i = 0 or 1
    void SetVertexIds(vtkIdType v0, vtkIdType v1) ;         ///< set indices of vertices
    void GetVerticesIdList(vtkIdList *idlist) const ;       ///< return vertices as a vtkIdlist
    vtkIdType GetOtherEndVertexId(vtkIdType v) const ;      ///< get the vertex id at the other end of the edge
    vtkIdType GetBranchId() const ;                         ///< get branch which edge belongs to
    void SetBranchId(vtkIdType b) ;                         ///< set branch which edge belongs to
    bool IsVertexPair(vtkIdType v0, vtkIdType v1) const  ;        ///< is the edge equal to these vertices (order doesn't matter)
    bool IsVertexPairOrdered(vtkIdType v0, vtkIdType v1) const ;  ///< is the edge equal to these vertices in this order
    void SetDirected(bool directed) ;                             ///< set directed property
    bool IsDirected() const ;                                     ///< is edge directed
    void ReverseDirection() ;                               ///< exchange vertices 0 and 1
    double GetWeight() const ;                                                    ///< get weight
    void SetWeight(double w) ;                                                    ///< set weight
    void GetMappingToOutputPolydata(vtkIdType *cellid, vtkIdType *index) const ;  ///< get mapping from edge to location in output
    void SetMappingToOutputPolydata(vtkIdType cellid, vtkIdType index) ;          ///< set mapping from edge to location in output
    bool SelfCheck() const ;                                                      ///< check self consistency
    void PrintSelf(std::ostream& os, const int tabs) const ;                      ///< print self
  private:
    vtkIdType m_VertexId[2] ;                                 ///< vertices at ends of edge
    bool m_Directed ;                                         ///< directed edge - if true, points from v0 to v1 (default = false)
    double m_Weight ;                                         ///< weight (default = 1.0)
    vtkIdType m_BranchId ;                                      ///< index of branch which edge belongs to (value = UndefinedId if no branch)

    vtkIdType m_OutputPolydataCell ;      ///< cell id of edge in output polydata
    vtkIdType m_OutputPolydataCellIndex ; ///< index of edge on cell in output polydata

  } ;


  //----------------------------------------------------------------------------
  /** Nested Branch class
  A branch is a connected sequence of vertex id's.
  The sequence is stored as a list of vertices and a list of edges. 
  The branch can optionally have a name */
  //----------------------------------------------------------------------------
  class Branch{
  public:
    Branch(const char* name = NULL) ;                                     ///< constructor: empty branch with optional name
    explicit Branch(vtkIdType startvertex, const char* name = NULL) ; ///< constructor: branch with initial vertex and optional name
    Branch(const Branch& src);
    
    ~Branch() ;                                                           ///< destructor
    int GetNumberOfVertices() const ;                   ///< get number of vertices
    int GetNumberOfEdges() const ;                      ///< get number of edges (should be vertices - 1)
    void GetName(char* name) const ;                    ///< get name of branch
    const char* GetName() const ;                       ///< get name of branch
    void SetName(const char *name) ;                   ///< set name of branch
    void UnsetName() ;                                  ///< unset name of branch to undefined
    vtkIdType GetEdgeId(int i) const ;                  ///< get index of ith edge
    void SetEdgeId(int i, vtkIdType e) ;                ///< set ith edge
    void AddEdgeId(vtkIdType e) ;                       ///< add edge to end of list
    int FindEdgeId(vtkIdType e) const ;                 ///< find position (0..ne-1) of this edge id in the list, return -1 if invalid
    void InsertEdgeId(int i, vtkIdType e) ;             ///< insert edge
    vtkIdType GetLastEdgeId() const ;                   ///< get id of last edge
    vtkIdType GetVertexId(int i) const ;                ///< get index of ith vertex
    void SetVertexId(int i, vtkIdType v) ;              ///< set ith vertex
    void AddVertexId(vtkIdType v) ;                     ///< add new vertex to end of list
    int FindVertexId(vtkIdType v) const ;               ///< find position (0..nv-1) of this vertex id in the list, return -1 if invalid
    void InsertVertexId(int i, vtkIdType v) ;           ///< insert vertex at position i in list
    vtkIdType GetLastVertexId() const ;                 ///< get id of last vertex
    void DeleteLastVertex() ;                           ///< delete last vertex and edge from end of list
    void GetVerticesIdList(vtkIdList *idlist) const ;   ///< return vertices as a vtkIdlist
    void ReverseDirection() ;                           ///< reverse branch direction
    vtkIdType GetMappingToOutputPolydata() const ;          ///< get mapping from branch to output cell
    void SetMappingToOutputPolydata(vtkIdType cellid) ;     ///< set mapping from branch to output cell
    void Clear() ;                                          ///< clear data from branch
    bool SelfCheck() const ;                                  ///< check self consistency (empty branches are allowed)
    void PrintSelf(std::ostream& os, const int tabs) const ;  ///< print self

    Branch& operator=(const Branch& src);
  private:
    char* m_Name ;                                        ///< name of branch
    std::vector<vtkIdType> m_VertexId ;                   ///< list by vertices
    std::vector<vtkIdType> m_EdgeId ;                     ///< list by edges

    vtkIdType m_OutputPolydataCell ;                    ///< cell id of branch in output polydata

  } ;

  //-----------------------------------------------------------------------------
  // Public functions which return const pointers to vertices, edges and branches.
  // These allow read-only access to const methods of the nested classes
  //-----------------------------------------------------------------------------
  const Vertex* GetConstVertexPtr(vtkIdType i) const ;   ///< return read-only pointer to vertex, return NULL if index out of range
  const Edge* GetConstEdgePtr(vtkIdType i) const ;       ///< return read-only pointer to edge, return NULL if index out of range
  const Branch* GetConstBranchPtr(vtkIdType i) const ;   ///< return read-only pointer to branch, return NULL if index out of range

private:
  //-----------------------------------------------------------------------------
  // private methods
  //-----------------------------------------------------------------------------
  void AllocateVertices(int nv) ;             ///< initialize or resize list of vertices
  void AllocateEdges(int ne) ;                ///< initialize or resize list of edges
  void AllocateBranches(int ne) ;             ///< initialize or resize list of branches

  bool SwapVertexIndices(vtkIdType i, vtkIdType j) ;  ///< swap two vertex indices throughout graph
  bool SwapEdgeIndices(vtkIdType i, vtkIdType j) ;    ///< swap two edge indices throughout graph
  bool SwapBranchIndices(vtkIdType i, vtkIdType j) ;  ///< swap two branch indices throughout graph

  /** Delete last edge from the graph.    
  If the edge was a member of a branch, the branch will be split. */
  bool DeleteLastEdge() ;

  /** Delete last vertex from the graph.    
  The vertex must be degree zero, so you have to remove any edges first
  Any branches containing this vertex will be left empty. */
  bool DeleteLastVertex() ;

  /** Delete last branch from the graph. */   
  bool DeleteLastBranch() ;


  //-----------------------------------------------------------------------------
  // Private functions which return non-const pointers to vertices, edges and branches
  // These give full read and write access to the nested classes,
  // and can only be used by methods of mafPolylineGraph.
  //-----------------------------------------------------------------------------
  Vertex* GetVertexPtr(vtkIdType i) ;         ///< private: return pointer to vertex, return NULL if index out of range
  Edge* GetEdgePtr(vtkIdType i)  ;            ///< private: return pointer to edge, return NULL if index out of range
  Branch* GetBranchPtr(vtkIdType i)  ;        ///< private: return pointer to branch, return NULL if index out of range
 
  //-----------------------------------------------------------------------------
  // member variables
  //-----------------------------------------------------------------------------
  std::vector<Vertex> m_Vertex ;                ///< list of vertices
  std::vector<Edge> m_Edge ;                    ///< list of edges
  std::vector<Branch> m_Branch ;                ///< list of branches
} ;

#endif

