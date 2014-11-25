/*=========================================================================

 Program: MAF2Medical
 Module: CSkeletonEdgeTest
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
#include "vtkMath.h"
#include "vtkMEDPolyDataDeformation.h"
#include "CSkeletonEdgeTest.h"

//-------------------------------------------------------------------------
void CSkeletonEdgeTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkMEDPolyDataDeformation::CSkeletonEdge *edge1 = new vtkMEDPolyDataDeformation::CSkeletonEdge();

  vtkMEDPolyDataDeformation::CSkeletonVertex *v1, *v2;
  double coords1[3], coords2[3];
  coords1[0] = coords1[1] = coords1[2] = 0.0;
  coords2[0] = coords2[1] = coords2[2] = 1.0;

  v1 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords1);
  v2 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords2);
  vtkMEDPolyDataDeformation::CSkeletonEdge *edge2 = new vtkMEDPolyDataDeformation::CSkeletonEdge(v1,v2);
 
  delete edge1;
  delete edge2;
  delete v1;
  delete v2;
}
//-------------------------------------------------------------------------
void CSkeletonEdgeTest::TestGetLength()
//-------------------------------------------------------------------------
{
  vtkMEDPolyDataDeformation::CSkeletonEdge *edge = new vtkMEDPolyDataDeformation::CSkeletonEdge();
  
  CPPUNIT_ASSERT(edge->GetLength() == 0.0);

  double coords1[3], coords2[3];
  coords1[0] = coords1[1] = coords1[2] = 0.0;
  coords2[0] = coords2[1] = coords2[2] = 1.0;

  vtkMEDPolyDataDeformation::CSkeletonVertex *v1, *v2;
  v1 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords1);
  v2 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords2);

  edge->Verts[0] = v1;
  edge->Verts[1] = v2;

  double lenght = sqrt(vtkMath::Distance2BetweenPoints(coords1, coords2));
  CPPUNIT_ASSERT(lenght == edge->GetLength());

  delete edge;
  delete v1;
  delete v2;
}

//-------------------------------------------------------------------------
void CSkeletonEdgeTest::TestGetNumberOfConnectedEdges()
//-------------------------------------------------------------------------
{
  vtkMEDPolyDataDeformation::CSkeletonEdge *edge = new vtkMEDPolyDataDeformation::CSkeletonEdge();

  double coords1[3], coords2[3];
  coords1[0] = coords1[1] = coords1[2] = 0.0;
  coords2[0] = coords2[1] = coords2[2] = 1.0;

  vtkMEDPolyDataDeformation::CSkeletonVertex *v1, *v2;
  v1 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords1);
  v2 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords2);

  edge->Verts[0] = v1;
  edge->Verts[1] = v2;

  CPPUNIT_ASSERT(edge->GetNumberOfConnectedEdges() == -2);

  delete edge;
  delete v1;
  delete v2;
}

//-------------------------------------------------------------------------
void CSkeletonEdgeTest::TestIsInternal()
//-------------------------------------------------------------------------
{
  vtkMEDPolyDataDeformation::CSkeletonEdge *edge = new vtkMEDPolyDataDeformation::CSkeletonEdge();

  double coords1[3], coords2[3];
  coords1[0] = coords1[1] = coords1[2] = 0.0;
  coords2[0] = coords2[1] = coords2[2] = 1.0;

  vtkMEDPolyDataDeformation::CSkeletonVertex *v1, *v2;
  v1 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords1);
  v2 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords2);

  edge->Verts[0] = v1;
  edge->Verts[1] = v2;

  CPPUNIT_ASSERT(edge->IsInternal());

  delete edge;
  delete v1;
  delete v2;
}