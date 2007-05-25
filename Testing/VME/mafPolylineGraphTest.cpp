/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPolylineGraphTest.cpp,v $
Language:  C++
Date:      $Date: 2007-05-25 13:22:52 $
Version:   $Revision: 1.1 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <cppunit/config/SourcePrefix.h>
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkCell.h"
#include "vtkCell.h"
#include "mafPolylineGraph.h"
#include "mafPolylineGraphTest.h"
#include "wx/wx.h"
#include <fstream>

static bool ExtractModel   = true;
static bool CleanModel     = false;

void mafPolylineGraphTest::setUp()
{
  m_Polydata = vtkPolyData::New() ;
  CreateExamplePolydata();
}

void mafPolylineGraphTest::tearDown()
{
  m_Polydata->Delete() ;
  m_Polydata = NULL ;

}


void mafPolylineGraphTest::TestFixture()
{
}


//------------------------------------------------------------------------------
// Test copy from polydata
void mafPolylineGraphTest::TestCopyFromPolydata() 
//------------------------------------------------------------------------------
{
  int i, nedges, nbranches ;

  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // count how many vertices, edges and branches we should have got from the polydata
  int npts = m_Polydata->GetPoints()->GetNumberOfPoints() ;
  int nlines = m_Polydata->GetNumberOfLines() ;
  for (i = 0, nedges = 0, nbranches = 0 ;  i < nlines ;  i++){
    vtkCell *cell = m_Polydata->GetCell(i) ;
    if (cell->GetCellType() == VTK_LINE){
      // each simple line in the polydata is an edge and a branch in the graph
      nbranches++ ;
      nedges++ ;
    }
    else if(cell->GetCellType() == VTK_POLY_LINE){
      // each polyline in the polydata is a branch and a set of edges in the graph
      nbranches++ ;
      nedges += cell->GetNumberOfPoints() - 1 ;
    }
    else
      CPPUNIT_ASSERT(false) ;
  }

  // compare the polydata results with the graph
  CPPUNIT_ASSERT(Graph->GetNumberOfVertices() == npts) ;
  CPPUNIT_ASSERT(Graph->GetNumberOfEdges() == nedges) ;
  CPPUNIT_ASSERT(Graph->GetNumberOfBranches() == nbranches) ;

  // this data should be a connected tree
  CPPUNIT_ASSERT(Graph->IsConnected()) ;
  CPPUNIT_ASSERT(Graph->IsConnectedTree()) ;

  delete Graph ;
}



//------------------------------------------------------------------------------
// Test copy to polydata
void mafPolylineGraphTest::TestCopyToPolydata() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph1 = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph1->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph1->SelfCheck()) ;

  // copy the graph back to polydata
  vtkPolyData *polydata = vtkPolyData::New() ;
  CPPUNIT_ASSERT(Graph1->CopyToPolydata(polydata)) ;

  // copy the polydata into another graph
  mafPolylineGraph *Graph2 = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph2->CopyFromPolydata(polydata)) ;
  CPPUNIT_ASSERT(Graph2->SelfCheck()) ;

  // compare the new graph with the original
  CPPUNIT_ASSERT(Graph2->GetNumberOfVertices() == Graph1->GetNumberOfVertices()) ;
  CPPUNIT_ASSERT(Graph2->GetNumberOfEdges() == Graph1->GetNumberOfEdges()) ;
  CPPUNIT_ASSERT(Graph2->GetNumberOfBranches() == Graph1->GetNumberOfBranches()) ;

  delete Graph1 ;
  delete Graph2 ;
  polydata->Delete() ;
}




//------------------------------------------------------------------------------
// Test adding an empty vertex to the graph
void mafPolylineGraphTest::TestAddNewVertex1() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType idv1 = Graph->GetMaxVertexId() ;

  // add empty vertex and check graph
  double coordsin[3] = {7, 0, 0} ;
  Graph->AddNewVertex(coordsin) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // check that max index has increased by one
  vtkIdType idv2 = Graph->GetMaxVertexId() ;
  CPPUNIT_ASSERT(idv2 = idv1+1) ;

  // check that new vertex is empty and has given coords
  double coordsout[3] ;
  Graph->GetVertexCoords(idv2, coordsout) ;
  CPPUNIT_ASSERT(Graph->GetConstVertexPtr(idv2)->GetDegree() == 0) ;
  CPPUNIT_ASSERT(coordsout[0] == coordsin[0] && coordsout[1] == coordsin[1] && coordsout[2] == coordsin[2]) ;

  // the graph should no longer be connected
  CPPUNIT_ASSERT(!Graph->IsConnected()) ;

  delete Graph ;

}


//------------------------------------------------------------------------------
// Test adding a new vertex by connecting to existing vertex with new edge
void mafPolylineGraphTest::TestAddNewVertex2() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType idv1 = Graph->GetMaxVertexId() ;
  vtkIdType ide1 = Graph->GetMaxEdgeId() ;

  // add new vertex to vertex 9 and check graph
  double coordsin[3] = {7, 0, 0} ;
  Graph->AddNewVertex(9, coordsin) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // check that max indices have increased by one
  vtkIdType idv2 = Graph->GetMaxVertexId() ;
  vtkIdType ide2 = Graph->GetMaxEdgeId() ;
  CPPUNIT_ASSERT(idv2 = idv1+1) ;
  CPPUNIT_ASSERT(ide2 = ide1+1) ;

  // check that new vertex is degree 1 and has given coords
  double coordsout[3] ;
  Graph->GetVertexCoords(idv2, coordsout) ;
  CPPUNIT_ASSERT(Graph->GetConstVertexPtr(idv2)->GetDegree() == 1) ;
  CPPUNIT_ASSERT(coordsout[0] == coordsin[0] && coordsout[1] == coordsin[1] && coordsout[2] == coordsin[2]) ;

  // check that new edge joins vertex 9 to vertex idv2
  CPPUNIT_ASSERT(Graph->GetConstEdgePtr(ide2)->IsVertexPair(9, idv2)) ;

  // the graph should still be a connected tree
  CPPUNIT_ASSERT(Graph->IsConnectedTree()) ;

  Graph->GetNumberOfVertices() ;

  delete Graph ;


}



//------------------------------------------------------------------------------
// Test adding a new edge between existing vertices
void mafPolylineGraphTest::TestAddNewEdge() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType ide1 = Graph->GetMaxEdgeId() ;

  // add new edge between vertices 12 and 21 and check graph
  Graph->AddNewEdge(12, 21) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // check that max index has increased by one
  vtkIdType ide2 = Graph->GetMaxEdgeId() ;
  CPPUNIT_ASSERT(ide2 = ide1+1) ;

  // check that new edge joins vertex 9 to vertex idv2
  CPPUNIT_ASSERT(Graph->GetConstEdgePtr(ide2)->IsVertexPair(12, 21)) ;

  // the graph should be connected but no longer a tree
  CPPUNIT_ASSERT(Graph->IsConnected()) ;
  CPPUNIT_ASSERT(!Graph->IsConnectedTree()) ;

  delete Graph ;

}


//------------------------------------------------------------------------------
// Test adding a new branch
void mafPolylineGraphTest::TestAddNewBranch() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType idb1 = Graph->GetMaxBranchId() ;

  // add new empty branch
  Graph->AddNewBranch() ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // check that max index increased by 1 and that branch is empty
  vtkIdType idb2 = Graph->GetMaxBranchId() ;
  CPPUNIT_ASSERT(idb2 == idb1+1) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetNumberOfVertices() == 0) ;

  // add new branch starting at vertex 8
  Graph->AddNewBranch(8) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // check that max index increased by 1 and that branch contains the correct vertex
  vtkIdType idb3 = Graph->GetMaxBranchId() ;
  CPPUNIT_ASSERT(idb3 == idb2+1) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb3)->GetNumberOfVertices() == 1) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb3)->GetVertexId(0) == idb3) ;

  // the graph should still be a connected tree
  CPPUNIT_ASSERT(Graph->IsConnectedTree()) ;

  delete Graph ;
}




//------------------------------------------------------------------------------
// Test splitting branch at edge
void mafPolylineGraphTest::TestSplitBranchAtEdge() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType ide1 = Graph->GetMaxEdgeId() ;
  vtkIdType idb1 = Graph->GetMaxBranchId() ;

  // split branch 4 at edge 7, creating new branch at maxid+1
  CPPUNIT_ASSERT(Graph->SplitBranchAtEdge(4, 7)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // check that max branch index increased by 1 and that branches contain the correct number of vertices
  vtkIdType idb2 = Graph->GetMaxBranchId() ;
  CPPUNIT_ASSERT(idb2 == idb1+1) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(4)->GetNumberOfVertices() == 4) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetNumberOfVertices() == 5) ;

  // check first and last vertices of split branch
  int nv = Graph->GetConstBranchPtr(4)->GetNumberOfVertices() ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(4)->GetVertexId(0) == 1) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(4)->GetVertexId(nv-1) == 7) ;
  nv = Graph->GetConstBranchPtr(idb2)->GetNumberOfVertices() ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(0) == 8) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(nv-1) == 12) ;

  // check that no. of edges has not changed and that edge is no longer a member of a branch
  CPPUNIT_ASSERT(Graph->GetMaxEdgeId() == ide1) ;
  CPPUNIT_ASSERT(Graph->GetConstEdgePtr(7)->GetBranchId() == UndefinedId) ;

  // the graph should still be a connected tree
  CPPUNIT_ASSERT(Graph->IsConnectedTree()) ;

  delete Graph ;
}


//------------------------------------------------------------------------------
// Test splitting branch at vertex
void mafPolylineGraphTest::TestSplitBranchAtVertex() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType idv1 = Graph->GetMaxVertexId() ;
  vtkIdType idb1 = Graph->GetMaxBranchId() ;

  // split branch 5 at vertex 15, creating new branch at maxid+1
  CPPUNIT_ASSERT(Graph->SplitBranchAtVertex(5, 15)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // check that max branch index increased by 1 and that branches contain the correct number of vertices
  vtkIdType idb2 = Graph->GetMaxBranchId() ;
  CPPUNIT_ASSERT(idb2 == idb1+1) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetNumberOfVertices() == 4) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetNumberOfVertices() == 3) ;

  // check first and last vertices of split branch
  int nv = Graph->GetConstBranchPtr(5)->GetNumberOfVertices() ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetVertexId(0) == 10) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetVertexId(nv-1) == 15) ;
  nv = Graph->GetConstBranchPtr(idb2)->GetNumberOfVertices() ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(0) == 15) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(nv-1) == 17) ;

  // check that no. of vertices has not changed and that vertex still has degree 3
  CPPUNIT_ASSERT(Graph->GetMaxVertexId() == idv1) ;
  CPPUNIT_ASSERT(Graph->GetConstVertexPtr(15)->GetDegree() == 3) ;

  // the graph should still be a connected tree
  CPPUNIT_ASSERT(Graph->IsConnectedTree()) ;

  delete Graph ;
}




//------------------------------------------------------------------------------
// Test delete an edge
void mafPolylineGraphTest::TestDeleteEdge() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType ide1 = Graph->GetMaxEdgeId() ;
  vtkIdType idb1 = Graph->GetMaxBranchId() ;

  // delete edge no. 2
  CPPUNIT_ASSERT(Graph->DeleteEdge(2)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // check that no. of edges has decreased by 1
  vtkIdType ide2 = Graph->GetMaxEdgeId() ;
  CPPUNIT_ASSERT(ide2 == ide1-1) ;

  // check that edge 2 was overwritten by the last edge, which connected vertices 20 and 21 and belonged to branch 6
  CPPUNIT_ASSERT(Graph->GetConstEdgePtr(2)->GetVertexId(0) == 20) ;
  CPPUNIT_ASSERT(Graph->GetConstEdgePtr(2)->GetVertexId(1) == 21) ;
  CPPUNIT_ASSERT(Graph->GetConstEdgePtr(2)->GetBranchId() == 6) ;

  // check that degrees of vertices are correct
  CPPUNIT_ASSERT(Graph->GetConstVertexPtr(3)->GetDegree() == 3) ;
  CPPUNIT_ASSERT(Graph->GetConstVertexPtr(4)->GetDegree() == 0) ;

  // this should have split branch 2 - check that the no. of branches has increased
  vtkIdType idb2 = Graph->GetMaxBranchId() ;
  CPPUNIT_ASSERT(idb2 == idb1+1) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(2)->GetNumberOfVertices() == 1) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetNumberOfVertices() == 1) ;

  // the graph should no longer be connected
  CPPUNIT_ASSERT(!Graph->IsConnected()) ;

  delete Graph ;
}




//------------------------------------------------------------------------------
// Test delete a vertex
void mafPolylineGraphTest::TestDeleteVertex() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType idv1 = Graph->GetMaxVertexId() ;
  vtkIdType ide1 = Graph->GetMaxEdgeId() ;
  vtkIdType idb1 = Graph->GetMaxBranchId() ;

  // first remove edge 11 from vertex 12
  Graph->DeleteEdge(11) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;
  CPPUNIT_ASSERT(Graph->GetConstVertexPtr(12)->GetDegree() == 0) ;

  vtkIdType idv2 = Graph->GetMaxVertexId() ;
  vtkIdType ide2 = Graph->GetMaxEdgeId() ;
  vtkIdType idb2 = Graph->GetMaxBranchId() ;
  CPPUNIT_ASSERT(idv2 == idv1) ;
  CPPUNIT_ASSERT(ide2 == ide1-1) ;
  CPPUNIT_ASSERT(idb2 == idb1+1) ;

  // the new branch should consist only of vertex 12
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetNumberOfVertices() == 1) ;

  // now delete vertex
  CPPUNIT_ASSERT(Graph->DeleteVertex(12)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType idv3 = Graph->GetMaxVertexId() ;
  vtkIdType ide3 = Graph->GetMaxEdgeId() ;
  vtkIdType idb3 = Graph->GetMaxBranchId() ;
  CPPUNIT_ASSERT(idv3 == idv2-1) ;
  CPPUNIT_ASSERT(ide3 == ide2) ;
  CPPUNIT_ASSERT(idb3 == idb2) ;

  // the last branch should now be empty
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb3)->GetNumberOfVertices() == 0) ;

  // the graph should still be a connected tree
  CPPUNIT_ASSERT(Graph->IsConnectedTree()) ;

  delete Graph ;
}




//------------------------------------------------------------------------------
// Test delete a branch
void mafPolylineGraphTest::TestDeleteBranch() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType idv1 = Graph->GetMaxVertexId() ;
  vtkIdType ide1 = Graph->GetMaxEdgeId() ;
  vtkIdType idb1 = Graph->GetMaxBranchId() ;

  // delete branch 5
  Graph->DeleteBranch(5) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // check that no. of branches has decreased
  vtkIdType idv2 = Graph->GetMaxVertexId() ;
  vtkIdType ide2 = Graph->GetMaxEdgeId() ;
  vtkIdType idb2 = Graph->GetMaxBranchId() ;
  CPPUNIT_ASSERT(idv2 == idv1) ;
  CPPUNIT_ASSERT(ide2 == ide1) ;
  CPPUNIT_ASSERT(idb2 == idb1-1) ;

  // check that edges 12-16 are no longer members of a branch
  CPPUNIT_ASSERT(Graph->GetConstEdgePtr(12)->GetBranchId() == UndefinedId) ;
  CPPUNIT_ASSERT(Graph->GetConstEdgePtr(13)->GetBranchId() == UndefinedId) ;
  CPPUNIT_ASSERT(Graph->GetConstEdgePtr(14)->GetBranchId() == UndefinedId) ;
  CPPUNIT_ASSERT(Graph->GetConstEdgePtr(15)->GetBranchId() == UndefinedId) ;
  CPPUNIT_ASSERT(Graph->GetConstEdgePtr(16)->GetBranchId() == UndefinedId) ;

  // check that branch 5 was correctly overwritten by branch 6
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetNumberOfEdges() == 4) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetEdgeId(0) == 17) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetEdgeId(1) == 18) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetEdgeId(2) == 19) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetEdgeId(3) == 20) ;

  // the graph should still be a connected tree
  CPPUNIT_ASSERT(Graph->IsConnectedTree()) ;

  delete Graph ;
}




//------------------------------------------------------------------------------
// Test adding new vertices to a branch
void mafPolylineGraphTest::TestAddNewVertexToBranch() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType idv1 = Graph->GetMaxVertexId() ;
  vtkIdType ide1 = Graph->GetMaxEdgeId() ;
  vtkIdType idb1 = Graph->GetMaxBranchId() ;

  // start a new branch at vertex 8
  Graph->AddNewBranch(8) ;
  vtkIdType idb2 = Graph->GetMaxBranchId() ;

  // add some new vertices to the branch
  double coords[3] ;
  Graph->GetConstVertexPtr(8)->GetCoords(coords) ;
  coords[1] -= 1.0 ;
  Graph->AddNewVertexToBranch(idb2, coords) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;
  coords[0] += 1.0 ; coords[1] -= 1.0 ;
  Graph->AddNewVertexToBranch(idb2, coords) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;
  coords[0] -= 2.0 ; coords[1] -= 1.0 ;
  Graph->AddNewVertexToBranch(idb2, coords) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;
  coords[0] += 1.0 ; coords[1] -= 1.0 ;
  Graph->AddNewVertexToBranch(idb2, coords) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // check that the no. of vertices and edges have increased coorectly
  vtkIdType idv3 = Graph->GetMaxVertexId() ;
  vtkIdType ide3 = Graph->GetMaxEdgeId() ;
  vtkIdType idb3 = Graph->GetMaxBranchId() ;
  CPPUNIT_ASSERT(idv3 == idv1 + 4) ;
  CPPUNIT_ASSERT(ide3 == ide1 + 4) ;
  CPPUNIT_ASSERT(idb3 == idb1 + 1) ;

  // check that the branch contains vertices 8, idv1+1, idv1+2...
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(0) == 8) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(1) == idv1+1) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(2) == idv1+2) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(3) == idv1+3) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(4) == idv1+4) ;

  // the graph should still be a connected tree
  CPPUNIT_ASSERT(Graph->IsConnectedTree()) ;

  delete Graph ;
}




//------------------------------------------------------------------------------
// Test adding existing vertices to a branch
void mafPolylineGraphTest::TestAddExistingVertexToBranch() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType idb1 = Graph->GetMaxBranchId() ;

  // delete branch 6 so we have some edges and vertices which are not in a branch
  Graph->DeleteBranch(6) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // now reconstruct the branch by adding existing vertices
  Graph->AddNewBranch() ;
  vtkIdType idb2 = Graph->GetMaxBranchId() ;
  CPPUNIT_ASSERT(Graph->AddExistingVertexToBranch(idb2, 15)) ;      // first vertex: no need to add edge
  CPPUNIT_ASSERT(Graph->AddExistingVertexToBranch(idb2, 18)) ;      // these automatically find the joining edge
  CPPUNIT_ASSERT(Graph->AddExistingVertexToBranch(idb2, 19)) ;
  CPPUNIT_ASSERT(Graph->AddExistingVertexToBranch(idb2, 20, 19)) ;  // these specify the edge explicitly
  CPPUNIT_ASSERT(Graph->AddExistingVertexToBranch(idb2, 21, 20)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // check that the branch contains vertices 15, 18, 19, 20, 21
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(0) == 15) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(1) == 18) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(2) == 19) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(3) == 20) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(4) == 21) ;

  // the graph should still be a connected tree
  CPPUNIT_ASSERT(Graph->IsConnectedTree()) ;

  delete Graph ;
}


//------------------------------------------------------------------------------
// Test adding existing edges to a branch
void mafPolylineGraphTest::TestAddExistingEdgeToBranch() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  vtkIdType idb1 = Graph->GetMaxBranchId() ;

  // delete branch 6 so we have some edges and vertices which are not in a branch
  Graph->DeleteBranch(6) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // now reconstruct the branch by adding existing edges
  Graph->AddNewBranch(15) ; // n.b. need a start vertex
  vtkIdType idb2 = Graph->GetMaxBranchId() ;
  CPPUNIT_ASSERT(Graph->AddExistingEdgeToBranch(idb2, 17)) ;
  CPPUNIT_ASSERT(Graph->AddExistingEdgeToBranch(idb2, 18)) ;
  CPPUNIT_ASSERT(Graph->AddExistingEdgeToBranch(idb2, 19)) ;
  CPPUNIT_ASSERT(Graph->AddExistingEdgeToBranch(idb2, 20)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // check that the branch contains vertices 15, 18, 19, 20, 21
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(0) == 15) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(1) == 18) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(2) == 19) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(3) == 20) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(idb2)->GetVertexId(4) == 21) ;

  // the graph should still be a connected tree
  CPPUNIT_ASSERT(Graph->IsConnectedTree()) ;

  delete Graph ;
}


//------------------------------------------------------------------------------
// Test naming a branch
void mafPolylineGraphTest::TestBranchName() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  wxString name0("OldBranch") ;
  wxString name1("NewBranch") ;
  wxString nameout ;

  // name an old branch
  Graph->SetBranchName(4, &name0) ;
  Graph->GetBranchName(4, &nameout) ;
  CPPUNIT_ASSERT(nameout == name0) ;

  // create a new branch with a name
  Graph->AddNewBranch(7, &name1) ;
  Graph->GetBranchName(7, &nameout) ;
  CPPUNIT_ASSERT(nameout == name1) ;

  // find names
  vtkIdType i0 = Graph->FindBranchName(&name0) ;
  vtkIdType i1 = Graph->FindBranchName(&name1) ;
  CPPUNIT_ASSERT(i0 == 4) ;
  CPPUNIT_ASSERT(i1 == Graph->GetMaxBranchId()) ;

  // remove name from branch 4
  Graph->UnsetBranchName(4) ;
  i0 = Graph->FindBranchName(&name0) ;
  CPPUNIT_ASSERT(i0 == UndefinedId) ;

  delete Graph ;
}



//------------------------------------------------------------------------------
// Test reverse branch direction
void mafPolylineGraphTest::TestReverseBranch() 
//------------------------------------------------------------------------------
{
  mafPolylineGraph *Graph = new mafPolylineGraph ;
  CPPUNIT_ASSERT(Graph->CopyFromPolydata(m_Polydata)) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;

  // reverse branch 5
  Graph->ReverseBranch(5) ;
  CPPUNIT_ASSERT(Graph->SelfCheck()) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetVertexId(0) == 17) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetVertexId(1) == 16) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetVertexId(2) == 15) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetVertexId(3) == 14) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetVertexId(4) == 13) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetVertexId(5) == 10) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetEdgeId(0) == 16) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetEdgeId(1) == 15) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetEdgeId(2) == 14) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetEdgeId(3) == 13) ;
  CPPUNIT_ASSERT(Graph->GetConstBranchPtr(5)->GetEdgeId(4) == 12) ;

  delete Graph ;
}



//------------------------------------------------------------------------------
/* Create example polydata.  This is a connected tree of lines and polylines.

                          17
                          |
                          16
                          |
                          15 - 18 - 19 - 20 - 21
                          |
                          14
     2                    |
    /                     13
   /                      |                        
  1 - 3 - 6 - 7 - 8 - 9 - 10 - 11 - 12
  |   |\
  |   | \
  0   5  4

*/  
void mafPolylineGraphTest::CreateExamplePolydata()
//------------------------------------------------------------------------------
{
  vtkPoints *points = vtkPoints::New() ;
  vtkCellArray *lines = vtkCellArray::New() ;

  int i ;

  // coordinates of vertices
  static double vertices[22][3] ={
    {0,0,0},
    {1,2,0},
    {2,4,0},
    {2,1,0},
    {3,1,0},
    {3,0,0},

    {3,2,0},
    {4,1,0},
    {5,2,0},
    {6,1,0},
    {7,2,0},
    {8,1,0},
    {9,2,0},

    {7,4,0},
    {9,6,0},
    {10,8,0},
    {13,10,0},
    {14,12,0},

    {11,9,0},
    {12,8,0},
    {13,8,0},
    {16,10,0}
  } ;

  // indices of simple lines and polylines
  static vtkIdType lineids[7][10] = {
    {0,1},
    {1,2},
    {3,4},
    {3,5},
    {1, 3, 6, 7, 8, 9, 10, 11, 12},
    {10, 13, 14, 15, 16, 17},
    {15, 18, 19, 20, 21}
  };

  // insert points
  for (i = 0 ;  i < 22 ;  i++)
    points->InsertNextPoint(vertices[i]) ;

  // insert lines and polylines
  lines->InsertNextCell(2, lineids[0]) ;
  lines->InsertNextCell(2, lineids[1]) ;
  lines->InsertNextCell(2, lineids[2]) ;
  lines->InsertNextCell(2, lineids[3]) ;
  lines->InsertNextCell(9, lineids[4]) ;
  lines->InsertNextCell(6, lineids[5]) ;
  lines->InsertNextCell(5, lineids[6]) ;

  // put points and cells in polydata
  m_Polydata->SetPoints(points) ;
  m_Polydata->SetLines(lines) ;
  points->Delete() ;
  lines->Delete() ;

}
