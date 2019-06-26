/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPolylineGraphEdgeTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
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
#include "albaPolylineGraph.h"
#include "albaPolylineGraphEdgeTest.h"
#include "wx/wx.h"
#include <fstream>



void albaPolylineGraphEdgeTest::TestDynamicAllocation()
{
  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

  vtkIdType v1, v2;
  v1 = 1;
  v2 = 5;
  albaPolylineGraph::Edge *Edge2 = new albaPolylineGraph::Edge(v1, v2);

  cppDEL(Edge);
  cppDEL(Edge2);
}

void albaPolylineGraphEdgeTest::TestGetOtherEndVertexId()
{
  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 1;
  Edge->SetVertexIds(v0, v1);

  CPPUNIT_ASSERT(Edge->GetOtherEndVertexId(v0) == v1);
  CPPUNIT_ASSERT(Edge->GetOtherEndVertexId(v1) == v0);

  cppDEL(Edge);

}

void albaPolylineGraphEdgeTest::TestGetSetBranchId()
{
  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

  vtkIdType b_id = 0;

  Edge->SetBranchId(b_id);

  CPPUNIT_ASSERT(Edge->GetBranchId() == b_id);

  cppDEL(Edge);

}

void albaPolylineGraphEdgeTest::TestGetSetVertexId()
{
  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 1;

  Edge->SetVertexId(0, v0);
  Edge->SetVertexId(1, v1);

  CPPUNIT_ASSERT(Edge->GetVertexId(0) == v0 && Edge->GetVertexId(1) == v1);

  cppDEL(Edge);

}

void albaPolylineGraphEdgeTest::TestGetSetVertexIds()
{
  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 1;

  Edge->SetVertexIds(v0, v1);

  CPPUNIT_ASSERT(Edge->GetVertexId(0) == v0 && Edge->GetVertexId(1) == v1);

  cppDEL(Edge);

}

void albaPolylineGraphEdgeTest::TestGetSetWeight()
{
  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

  double w = 0.7;
  Edge->SetWeight(w);

  CPPUNIT_ASSERT(Edge->GetWeight() == w);

  cppDEL(Edge);

}

void albaPolylineGraphEdgeTest::TestGetVerticesIdList()
{
  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

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

void albaPolylineGraphEdgeTest::TestIsSetVertexPairOrdered()
{
  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 1;

  Edge->SetVertexIds(v0, v1);

  CPPUNIT_ASSERT(Edge->IsVertexPairOrdered(v0, v1));
  CPPUNIT_ASSERT(!Edge->IsVertexPairOrdered(v1, v0));

  cppDEL(Edge);

}

void albaPolylineGraphEdgeTest::TestIsVertexPair()
{
  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

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

void albaPolylineGraphEdgeTest::TestReverseDirection()
{
  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

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

void albaPolylineGraphEdgeTest::TestSelfCheck()
{
  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

  CPPUNIT_ASSERT(!Edge->SelfCheck());

  vtkIdType v1, v2;
  v1 =0;
  v2 = 1;
  Edge->SetVertexIds(v1, v2);
  CPPUNIT_ASSERT(Edge->SelfCheck());

  cppDEL(Edge);

}

void albaPolylineGraphEdgeTest::TestSetIsDirected()
{
  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

  bool directed = true;
  Edge->SetDirected(directed);
  CPPUNIT_ASSERT(Edge->IsDirected() == directed);

  directed = false;
  Edge->SetDirected(directed);
  CPPUNIT_ASSERT(Edge->IsDirected() == directed);

  cppDEL(Edge);

}

void albaPolylineGraphEdgeTest::TestGetSetMappingToOutputPolydata()
{

  albaPolylineGraph::Edge *Edge = new albaPolylineGraph::Edge();

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