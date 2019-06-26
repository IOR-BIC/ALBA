/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPolylineGraphBranchTest
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
#include "albaPolylineGraphBranchTest.h"
#include "wx/wx.h"
#include <fstream>

#include "albaString.h"


void albaPolylineGraphBranchTest::TestDynamicAllocation()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  vtkIdType v1;
  v1 = 1;
  albaPolylineGraph::Branch *branch2 = new albaPolylineGraph::Branch(v1, "branch2");

  cppDEL(branch);
  cppDEL(branch2);
}

void albaPolylineGraphBranchTest::TestGetSetMappingToOutputPolydata()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  vtkIdType cellID = 1;

  branch->SetMappingToOutputPolydata(cellID);
  CPPUNIT_ASSERT(branch->GetMappingToOutputPolydata() == cellID);

  cppDEL(branch);

}

void albaPolylineGraphBranchTest::TestAddEdgeId()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  cppDEL(branch);

}

void albaPolylineGraphBranchTest::TestAddVertexId()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  cppDEL(branch);

}

void albaPolylineGraphBranchTest::TestDeleteLastVertex()
{
  vtkIdType v1, v2;
  v1 = 0;
  v2 = 1;

  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch(v1);

  branch->AddVertexId(v2);
  branch->DeleteLastVertex();
  CPPUNIT_ASSERT(branch->GetLastVertexId() == v1);

  cppDEL(branch);

}

void albaPolylineGraphBranchTest::TestFindEdgeId()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  vtkIdType e0, e1;
  e0 = 0;
  e1 = 2;

  branch->AddEdgeId(e0);
  branch->AddEdgeId(e1);

  CPPUNIT_ASSERT(branch->FindEdgeId(e0) == 0);
  CPPUNIT_ASSERT(branch->FindEdgeId(e1) == 1);

  cppDEL(branch);

}

void albaPolylineGraphBranchTest::TestFindVertexId()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 2;

  branch->AddVertexId(v0);
  branch->AddVertexId(v1);

  CPPUNIT_ASSERT(branch->FindVertexId(v0) == 0);
  CPPUNIT_ASSERT(branch->FindVertexId(v1) == 1);

  cppDEL(branch);

}

void albaPolylineGraphBranchTest::TestGetLastEdgeId()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  vtkIdType e0, e1;
  e0 = 0;
  e1 = 2;

  branch->AddEdgeId(e0);
  branch->AddEdgeId(e1);

  CPPUNIT_ASSERT(branch->GetLastEdgeId() == e1);

  cppDEL(branch);

}

void albaPolylineGraphBranchTest::TestGetLastVertexId()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 2;

  branch->AddVertexId(v0);
  branch->AddVertexId(v1);

  CPPUNIT_ASSERT(branch->GetLastVertexId() == v1);

  cppDEL(branch);


}

void albaPolylineGraphBranchTest::TestGetNumberOfEdges()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  CPPUNIT_ASSERT(branch->GetNumberOfEdges() == 0);

  vtkIdType e0, e1;
  e0 = 0;
  e1 = 1;

  branch->AddEdgeId(e0);
  branch->AddEdgeId(e1);

  CPPUNIT_ASSERT(branch->GetNumberOfEdges() == 2);

  cppDEL(branch);


}

void albaPolylineGraphBranchTest::TestGetNumberOfVertices()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 1;

  branch->AddVertexId(v0);
  branch->AddVertexId(v1);

  CPPUNIT_ASSERT(branch->GetNumberOfVertices() == 2);

  cppDEL(branch);


}

void albaPolylineGraphBranchTest::TestGetSetEdgeId()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  vtkIdType e0, e1, e2;
  e0 = 0;
  e1 = 1;
  e2 = 2;

  branch->AddEdgeId(e0);
  branch->AddEdgeId(e1);

  CPPUNIT_ASSERT(branch->GetEdgeId(0) == e0);
  CPPUNIT_ASSERT(branch->GetEdgeId(1) == e1);

  branch->SetEdgeId(1, e2);

  CPPUNIT_ASSERT(branch->GetEdgeId(1) == e2);

  cppDEL(branch);


}

void albaPolylineGraphBranchTest::TestGetSetName()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch("myBranch");

  albaString outName = albaString(branch->GetName());
  CPPUNIT_ASSERT(outName.Compare("myBranch") == 0);

  branch->SetName("myNewBranch");
  outName = albaString(branch->GetName());
  CPPUNIT_ASSERT(outName.Compare("myNewBranch") == 0);

  cppDEL(branch);

}

void albaPolylineGraphBranchTest::TestGetSetVertexId()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  vtkIdType v0, v1, v2;
  v0 = 0;
  v1 = 1;
  v2 = 2;

  branch->AddVertexId(v0);
  branch->AddVertexId(v1);

  CPPUNIT_ASSERT(branch->GetVertexId(0) == v0);
  CPPUNIT_ASSERT(branch->GetVertexId(1) == v1);

  branch->SetVertexId(1, v2);

  CPPUNIT_ASSERT(branch->GetVertexId(1) == v2);

  cppDEL(branch);

}

void albaPolylineGraphBranchTest::TestGetVerticesIdList()
{
  
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  vtkIdType v0, v1;
  v0 = 0;
  v1 = 1;

  branch->AddVertexId(v0);
  branch->AddVertexId(v1);

  vtkIdList* const idList = vtkIdList::New();
  branch->GetVerticesIdList(idList);

  CPPUNIT_ASSERT(idList->GetId(0) == v0);
  CPPUNIT_ASSERT(idList->GetId(1) == v1);

  idList->Delete();

  cppDEL(branch);


}

void albaPolylineGraphBranchTest::TestReverseDirection()
{ 
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  vtkIdType e0, e1, e2;
  e0 = 0;
  e1 = 1;
  e2 = 2;

  branch->AddEdgeId(e0);
  branch->AddEdgeId(e1);
  branch->AddEdgeId(e2);

  vtkIdType v0, v1, v2;
  v0 = 0;
  v1 = 1;
  v2 = 2;

  branch->AddVertexId(v0);
  branch->AddVertexId(v1);
  branch->AddVertexId(v2);

  branch->ReverseDirection();

  CPPUNIT_ASSERT(branch->GetLastEdgeId() == e0);
  CPPUNIT_ASSERT(branch->GetLastVertexId() == v0);
  
  cppDEL(branch);

}

void albaPolylineGraphBranchTest::TestSelfCheck()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch();

  CPPUNIT_ASSERT(branch->SelfCheck());

  vtkIdType e =0;
  vtkIdType v0, v1;
  v0 = 0;
  v1 = 1;

  branch->AddEdgeId(e);
  CPPUNIT_ASSERT(!branch->SelfCheck());

  branch->AddVertexId(v0);
  branch->AddVertexId(v1);
  CPPUNIT_ASSERT(branch->SelfCheck());

  cppDEL(branch);


}

void albaPolylineGraphBranchTest::TestUnsetName()
{
  albaPolylineGraph::Branch *branch = new albaPolylineGraph::Branch("myBranch");

  branch->UnsetName();
  albaString outName = albaString(branch->GetName());
  CPPUNIT_ASSERT(outName.Compare(albaPolylineGraph::m_UndefinedName) == 0);

  cppDEL(branch);


}