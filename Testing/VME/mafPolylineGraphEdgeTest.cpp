/*=========================================================================

 Program: MAF2Medical
 Module: mafPolylineGraphEdgeTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
#include "mafPolylineGraphEdgeTest.h"
#include "wx/wx.h"
#include <fstream>


void mafPolylineGraphEdgeTest::setUp()
{

}

void mafPolylineGraphEdgeTest::tearDown()
{

}

void mafPolylineGraphEdgeTest::TestDynamicAllocation()
{
  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  vtkIdType v1, v2;
  v1 = 1;
  v2 = 5;
  mafPolylineGraph::Edge *Edge2 = new mafPolylineGraph::Edge(v1, v2);

  cppDEL(Edge);
  cppDEL(Edge2);
}

void mafPolylineGraphEdgeTest::TestGetOtherEndVertexId()
{
  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 1;
  Edge->SetVertexIds(v0, v1);

  CPPUNIT_ASSERT(Edge->GetOtherEndVertexId(v0) == v1);
  CPPUNIT_ASSERT(Edge->GetOtherEndVertexId(v1) == v0);

  cppDEL(Edge);

}

void mafPolylineGraphEdgeTest::TestGetSetBranchId()
{
  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  vtkIdType b_id = 0;

  Edge->SetBranchId(b_id);

  CPPUNIT_ASSERT(Edge->GetBranchId() == b_id);

  cppDEL(Edge);

}

void mafPolylineGraphEdgeTest::TestGetSetVertexId()
{
  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 1;

  Edge->SetVertexId(0, v0);
  Edge->SetVertexId(1, v1);

  CPPUNIT_ASSERT(Edge->GetVertexId(0) == v0 && Edge->GetVertexId(1) == v1);

  cppDEL(Edge);

}

void mafPolylineGraphEdgeTest::TestGetSetVertexIds()
{
  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 1;

  Edge->SetVertexIds(v0, v1);

  CPPUNIT_ASSERT(Edge->GetVertexId(0) == v0 && Edge->GetVertexId(1) == v1);

  cppDEL(Edge);

}

void mafPolylineGraphEdgeTest::TestGetSetWeight()
{
  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  double w = 0.7;
  Edge->SetWeight(w);

  CPPUNIT_ASSERT(Edge->GetWeight() == w);

  cppDEL(Edge);

}

void mafPolylineGraphEdgeTest::TestGetVerticesIdList()
{
  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 1;

  Edge->SetVertexIds(v0, v1);


  vtkIdList* const idList = vtkIdList::New();
  Edge->GetVerticesIdList(idList);

  CPPUNIT_ASSERT(idList->GetId(0) == v0);
  CPPUNIT_ASSERT(idList->GetId(1) == v1);

  idList->Delete();

  cppDEL(Edge);

}

void mafPolylineGraphEdgeTest::TestIsSetVertexPairOrdered()
{
  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 1;

  Edge->SetVertexIds(v0, v1);

  CPPUNIT_ASSERT(Edge->IsVertexPairOrdered(v0, v1));
  CPPUNIT_ASSERT(!Edge->IsVertexPairOrdered(v1, v0));

  cppDEL(Edge);

}

void mafPolylineGraphEdgeTest::TestIsVertexPair()
{
  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  vtkIdType v0, v1, v2;
  v0 = 0;
  v1 = 1;
  v2 = 2;

  Edge->SetVertexIds(v0, v1);

  CPPUNIT_ASSERT(Edge->IsVertexPair(v0, v1));
  CPPUNIT_ASSERT(Edge->IsVertexPair(v1, v0));
  CPPUNIT_ASSERT(!Edge->IsVertexPair(v0, v2));
  CPPUNIT_ASSERT(!Edge->IsVertexPair(v2, v1));


  cppDEL(Edge);

}

void mafPolylineGraphEdgeTest::TestReverseDirection()
{
  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  vtkIdType v1, v2;
  v1 =0;
  v2 = 1;
  Edge->SetVertexIds(v1, v2);

  bool directed = 0;
  Edge->SetDirected(directed);
  Edge->ReverseDirection();

  CPPUNIT_ASSERT(Edge->IsDirected());

  cppDEL(Edge);

}

void mafPolylineGraphEdgeTest::TestSelfCheck()
{
  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  CPPUNIT_ASSERT(!Edge->SelfCheck());

  vtkIdType v1, v2;
  v1 =0;
  v2 = 1;
  Edge->SetVertexIds(v1, v2);
  CPPUNIT_ASSERT(Edge->SelfCheck());

  cppDEL(Edge);

}

void mafPolylineGraphEdgeTest::TestSetIsDirected()
{
  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  bool directed = true;
  Edge->SetDirected(directed);
  CPPUNIT_ASSERT(Edge->IsDirected() == directed);

  directed = false;
  Edge->SetDirected(directed);
  CPPUNIT_ASSERT(Edge->IsDirected() == directed);

  cppDEL(Edge);

}

void mafPolylineGraphEdgeTest::TestGetSetMappingToOutputPolydata()
{

  mafPolylineGraph::Edge *Edge = new mafPolylineGraph::Edge();

  vtkIdType v1, v2;
  v1 =0;
  v2 = 1;
  Edge->SetVertexIds(v1, v2);

  vtkIdType cellIndex = 1;
  vtkIdType cellId = 12;

  Edge->SetMappingToOutputPolydata(cellId, cellIndex);

  vtkIdType cellIndexRes, cellIdRes;
  Edge->GetMappingToOutputPolydata(&cellIdRes, &cellIndexRes);

  CPPUNIT_ASSERT(cellIndexRes == cellIndex && cellIdRes == cellId);

  cppDEL(Edge);

}