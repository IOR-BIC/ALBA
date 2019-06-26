/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: VertexDataTest
 Authors: Roberto Mucci
 
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
#include "VertexDataTest.h"

#include "vtkALBAExtrudeToCircle.h"



//-------------------------------------------------------------------------
void VertexDataTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkALBAExtrudeToCircle::VertexData *vertex = new vtkALBAExtrudeToCircle::VertexData();
  delete vertex;
}

//-------------------------------------------------------------------------
void VertexDataTest::TestCylCoord()
//-------------------------------------------------------------------------
{
  double rIn = 2.5;
  double phiIn = 5;
  double zIn = 10.2;
  vtkALBAExtrudeToCircle::VertexData *vertex = new vtkALBAExtrudeToCircle::VertexData();
  vertex->SetCylCoords(rIn, phiIn, zIn);
  double r, phi, z;
  vertex->GetCylCoords(&r, &phi, &z);
  CPPUNIT_ASSERT(r == rIn && phi == phiIn && z == zIn );
  delete vertex;
}

//-------------------------------------------------------------------------
void VertexDataTest::TestCartCoords()
//-------------------------------------------------------------------------
{
  double xIn = 2.5;
  double yIn = 5;
  double zIn = 10.2;
  vtkALBAExtrudeToCircle::VertexData *vertex = new vtkALBAExtrudeToCircle::VertexData();
  vertex->SetCartCoords(xIn, yIn, zIn);
  double x[3];
  vertex->GetCartCoords(x);
  CPPUNIT_ASSERT(x[0] == xIn && x[1] == yIn && x[2] == zIn );
  delete vertex;
}

//-------------------------------------------------------------------------
void VertexDataTest::TestId()
//-------------------------------------------------------------------------
{
  int idIn = 5;
  vtkALBAExtrudeToCircle::VertexData *vertex = new vtkALBAExtrudeToCircle::VertexData();
  vertex->SetId(idIn);
  int id = vertex->GetId();
  CPPUNIT_ASSERT(idIn == id);
  delete vertex;
}

