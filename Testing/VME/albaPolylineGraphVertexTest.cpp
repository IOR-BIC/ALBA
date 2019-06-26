/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPolylineGraphVertexTest
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
#include "albaPolylineGraphVertexTest.h"
#include "wx/wx.h"
#include <fstream>

void albaPolylineGraphVertexTest::TestDynamicAllocation()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  double *coords;
  coords = new double[3];
  coords[0] = 0.0;
  coords[1] = 0.0;
  coords[2] = 0.0;

  albaPolylineGraph::Vertex *vertex2 = new albaPolylineGraph::Vertex(coords);

  cppDEL(vertex);
  cppDEL(vertex2);
  delete []coords;
  coords = NULL;
}

void albaPolylineGraphVertexTest::TestAddEdgeId()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  vtkIdType id = vtkIdType(3);
  vertex->AddEdgeId(id);

  vtkIdType idOut = vertex->GetEdgeId(0);
  CPPUNIT_ASSERT(idOut == id);
  CPPUNIT_ASSERT(vertex->GetDegree() == 1);

  cppDEL(vertex);

}

void albaPolylineGraphVertexTest::TestAddVertexId()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  vtkIdType id = vtkIdType(3);
  vertex->AddVertexId(id);

  vtkIdType idOut = vertex->GetVertexId(0);
  CPPUNIT_ASSERT(idOut == id);

  cppDEL(vertex);

}

void albaPolylineGraphVertexTest::TestDeleteEdgeId()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  vtkIdType e_id = vtkIdType(0);
  vtkIdType v_id = vtkIdType(0);
  vertex->AddEdgeId(e_id);
  vertex->AddVertexId(v_id);

  vertex->DeleteEdgeId(e_id);
  CPPUNIT_ASSERT(vertex->FindEdgeId(e_id) == -1);

  cppDEL(vertex);

}


void albaPolylineGraphVertexTest::TestDeleteVertexId()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  vtkIdType e_id = vtkIdType(0);
  vtkIdType v_id = vtkIdType(0);
  vertex->AddEdgeId(e_id);
  vertex->AddVertexId(v_id);

  vertex->DeleteVertexId(v_id);
  CPPUNIT_ASSERT(vertex->FindEdgeId(v_id) == -1);

  cppDEL(vertex);

}

void albaPolylineGraphVertexTest::TestFindEdgeId()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();
  
  vtkIdType e_id1, e_id2, e_id3;
  e_id1 = vtkIdType(0);
  e_id2 = vtkIdType(1);
  e_id3 = vtkIdType(2);

  vertex->AddEdgeId(e_id1);
  vertex->AddEdgeId(e_id2);

  CPPUNIT_ASSERT(vertex->FindEdgeId(e_id1) == 0);
  CPPUNIT_ASSERT(vertex->FindEdgeId(e_id2) ==1);
  CPPUNIT_ASSERT(vertex->FindEdgeId(e_id3) == -1);

  cppDEL(vertex);

}

void albaPolylineGraphVertexTest::TestFindVertexId()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  vtkIdType e_id1, e_id2;
  e_id1 = vtkIdType(0);
  e_id2 = vtkIdType(1);

  vertex->AddEdgeId(e_id1);
  vertex->AddEdgeId(e_id2);

  vtkIdType v_id1, v_id2, v_id3;
  v_id1 = vtkIdType(0);
  v_id2 = vtkIdType(1);
  v_id3 = vtkIdType(2);

  vertex->AddVertexId(v_id1);
  vertex->AddVertexId(v_id2);

  CPPUNIT_ASSERT(vertex->FindVertexId(v_id1) == 0);
  CPPUNIT_ASSERT(vertex->FindVertexId(v_id2) ==1);
  CPPUNIT_ASSERT(vertex->FindVertexId(v_id3) == -1);

  cppDEL(vertex);

}

void albaPolylineGraphVertexTest::TestGetDegree()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  CPPUNIT_ASSERT(vertex->GetDegree() == 0);

  vtkIdType e_id1, e_id2;
  e_id1 = vtkIdType(0);
  e_id2 = vtkIdType(1);

  vertex->AddEdgeId(e_id1);
  vertex->AddEdgeId(e_id2);

  CPPUNIT_ASSERT(vertex->GetDegree() == 2);

  cppDEL(vertex);

}

void albaPolylineGraphVertexTest::TestGetVerticesIdList()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  vtkIdType e_id1, e_id2;
  e_id1 = vtkIdType(0);
  e_id2 = vtkIdType(1);

  vertex->AddEdgeId(e_id1);
  vertex->AddEdgeId(e_id2);

  vtkIdType v_id1, v_id2;
  v_id1 = vtkIdType(0);
  v_id2 = vtkIdType(1);

  vertex->AddVertexId(v_id1);
  vertex->AddVertexId(v_id2);

  vtkIdList* const idList = vtkIdList::New();
  vertex->GetVerticesIdList(idList);
  
  CPPUNIT_ASSERT(idList->GetId(0) == v_id1);
  CPPUNIT_ASSERT(idList->GetId(1) == v_id2);
  CPPUNIT_ASSERT(idList->GetNumberOfIds() == 2);

  cppDEL(vertex);
  idList->Delete();

}

void albaPolylineGraphVertexTest::TestSetGetCoords()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  double *outCoords = new double[3];
  vertex->GetCoords(outCoords);

  CPPUNIT_ASSERT(outCoords[0] == 0 && outCoords[1]==0 && outCoords[2] == 0);

  double *inCoords = new double[3];
  inCoords[0] = 1.0;
  inCoords[1] = 2.0;
  inCoords[2] = 3.0;
  vertex->SetCoords(inCoords);

  vertex->GetCoords(outCoords);
  CPPUNIT_ASSERT( outCoords[0] == inCoords[0] && outCoords[1]==inCoords[1] && outCoords[2] == inCoords[2] );

  cppDEL(vertex);
  delete []inCoords;
  delete []outCoords;

}

void albaPolylineGraphVertexTest::TestSetGetEdgeId()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  vtkIdType e_id1 = vtkIdType(3);
  vtkIdType e_id2 = vtkIdType(4);
 
  vertex->AddEdgeId(e_id1);
  CPPUNIT_ASSERT(vertex->GetEdgeId(0) == e_id1);

  vertex->SetEdgeId(0, e_id2);
  CPPUNIT_ASSERT(vertex->GetEdgeId(0) == e_id2);

  cppDEL(vertex);

}

void albaPolylineGraphVertexTest::TestSetGetVertexId()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  vtkIdType v_id1 = vtkIdType(3);
  vtkIdType v_id2 = vtkIdType(4);

  vertex->AddVertexId(v_id1);
  CPPUNIT_ASSERT(vertex->GetVertexId(0) == v_id1);

  vertex->SetVertexId(0, v_id2);
  CPPUNIT_ASSERT(vertex->GetVertexId(0) == v_id2);


  cppDEL(vertex);

}

void albaPolylineGraphVertexTest::TestSelfCheck()
{
  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  vtkIdType e_id1, e_id2, e_id3;
  e_id1 = vtkIdType(0);
  e_id2 = vtkIdType(1);
  e_id3 = vtkIdType(2);

  vtkIdType v_id1, v_id2, v_id3;
  v_id1 = vtkIdType(0);
  v_id2 = vtkIdType(1);
  v_id3 = vtkIdType(2);

  vertex->AddEdgeId(e_id1);
  vertex->AddVertexId(v_id1);

  CPPUNIT_ASSERT(vertex->SelfCheck());

  vertex->AddEdgeId(e_id2);
  CPPUNIT_ASSERT(!vertex->SelfCheck()); // arrays should be of the same length
  vertex->AddVertexId(v_id2);
  CPPUNIT_ASSERT(vertex->SelfCheck());

  vertex->AddEdgeId(e_id2);
  vertex->AddVertexId(v_id3);
  CPPUNIT_ASSERT(!vertex->SelfCheck()); // edges on vertex must not contain repeats

  vertex->SetEdgeId(2, e_id3);
  CPPUNIT_ASSERT(vertex->SelfCheck());

  vertex->SetVertexId(2, v_id2);
  CPPUNIT_ASSERT(!vertex->SelfCheck()); // vertices on vertex must not contain repeats

  cppDEL(vertex);

}

void albaPolylineGraphVertexTest::TestClear()
{

  albaPolylineGraph::Vertex *vertex = new albaPolylineGraph::Vertex();

  vtkIdType e_id1, e_id2;
  e_id1 = vtkIdType(0);
  e_id2 = vtkIdType(1);

  vtkIdType v_id1, v_id2;
  v_id1 = vtkIdType(0);
  v_id2 = vtkIdType(1);

  vertex->AddEdgeId(e_id1);
  vertex->AddEdgeId(e_id2);

  vertex->AddVertexId(v_id1);
  vertex->AddVertexId(v_id2);

  vertex->Clear();
  CPPUNIT_ASSERT( vertex->GetDegree() ==0 );

  cppDEL(vertex);


}