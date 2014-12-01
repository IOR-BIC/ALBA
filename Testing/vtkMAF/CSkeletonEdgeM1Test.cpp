/*=========================================================================

 Program: MAF2
 Module: CSkeletonEdgeM1Test
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
#include "vtkMAFPolyDataDeformation_M1.h"
#include "CSkeletonEdgeM1Test.h"

//-------------------------------------------------------------------------
void CSkeletonEdgeM1Test::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkMAFPolyDataDeformation_M1::CSkeletonEdge *edge1 = new vtkMAFPolyDataDeformation_M1::CSkeletonEdge();

  vtkMAFPolyDataDeformation_M1::CSkeletonVertex *v1, *v2;
  double coords1[3], coords2[3];
  coords1[0] = coords1[1] = coords1[2] = 0.0;
  coords2[0] = coords2[1] = coords2[2] = 1.0;

  v1 = new vtkMAFPolyDataDeformation_M1::CSkeletonVertex(coords1);
  v2 = new vtkMAFPolyDataDeformation_M1::CSkeletonVertex(coords2);
  vtkMAFPolyDataDeformation_M1::CSkeletonEdge *edge2 = new vtkMAFPolyDataDeformation_M1::CSkeletonEdge(v1,v2);
 
  delete edge1;
  delete edge2;
  delete v1;
  delete v2;
}
//-------------------------------------------------------------------------
void CSkeletonEdgeM1Test::TestGetLength()
//-------------------------------------------------------------------------
{
  vtkMAFPolyDataDeformation_M1::CSkeletonEdge *edge = new vtkMAFPolyDataDeformation_M1::CSkeletonEdge();
  
  CPPUNIT_ASSERT(edge->GetLength() == 0.0);

  double coords1[3], coords2[3];
  coords1[0] = coords1[1] = coords1[2] = 0.0;
  coords2[0] = coords2[1] = coords2[2] = 1.0;

  vtkMAFPolyDataDeformation_M1::CSkeletonVertex *v1, *v2;
  v1 = new vtkMAFPolyDataDeformation_M1::CSkeletonVertex(coords1);
  v2 = new vtkMAFPolyDataDeformation_M1::CSkeletonVertex(coords2);

  edge->Verts[0] = v1;
  edge->Verts[1] = v2;

  double lenght = sqrt(vtkMath::Distance2BetweenPoints(coords1, coords2));
  CPPUNIT_ASSERT(lenght == edge->GetLength());

  delete edge;
  delete v1;
  delete v2;
}

//-------------------------------------------------------------------------
void CSkeletonEdgeM1Test::TestGetNumberOfConnectedEdges()
//-------------------------------------------------------------------------
{
  vtkMAFPolyDataDeformation_M1::CSkeletonEdge *edge = new vtkMAFPolyDataDeformation_M1::CSkeletonEdge();

  double coords1[3], coords2[3];
  coords1[0] = coords1[1] = coords1[2] = 0.0;
  coords2[0] = coords2[1] = coords2[2] = 1.0;

  vtkMAFPolyDataDeformation_M1::CSkeletonVertex *v1, *v2;
  v1 = new vtkMAFPolyDataDeformation_M1::CSkeletonVertex(coords1);
  v2 = new vtkMAFPolyDataDeformation_M1::CSkeletonVertex(coords2);

  edge->Verts[0] = v1;
  edge->Verts[1] = v2;

  CPPUNIT_ASSERT(edge->GetNumberOfConnectedEdges() == -2);

  delete edge;
  delete v1;
  delete v2;
}

//-------------------------------------------------------------------------
void CSkeletonEdgeM1Test::TestIsInternal()
//-------------------------------------------------------------------------
{
  vtkMAFPolyDataDeformation_M1::CSkeletonEdge *edge = new vtkMAFPolyDataDeformation_M1::CSkeletonEdge();

  double coords1[3], coords2[3];
  coords1[0] = coords1[1] = coords1[2] = 0.0;
  coords2[0] = coords2[1] = coords2[2] = 1.0;

  vtkMAFPolyDataDeformation_M1::CSkeletonVertex *v1, *v2;
  v1 = new vtkMAFPolyDataDeformation_M1::CSkeletonVertex(coords1);
  v2 = new vtkMAFPolyDataDeformation_M1::CSkeletonVertex(coords2);

  edge->Verts[0] = v1;
  edge->Verts[1] = v2;

  CPPUNIT_ASSERT(edge->IsInternal());

  delete edge;
  delete v1;
  delete v2;
}