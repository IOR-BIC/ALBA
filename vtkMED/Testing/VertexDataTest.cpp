/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: VertexDataTest.cpp,v $
Language:  C++
Date:      $Date: 2011-02-28 11:20:45 $
Version:   $Revision: 1.1.2.1 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "VertexDataTest.h"

#include "vtkMEDExtrudeToCircle.h"



//-------------------------------------------------------------------------
void VertexDataTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkMEDExtrudeToCircle::VertexData *vertex = new vtkMEDExtrudeToCircle::VertexData();
  delete vertex;
}

//-------------------------------------------------------------------------
void VertexDataTest::TestCylCoord()
//-------------------------------------------------------------------------
{
  double rIn = 2.5;
  double phiIn = 5;
  double zIn = 10.2;
  vtkMEDExtrudeToCircle::VertexData *vertex = new vtkMEDExtrudeToCircle::VertexData();
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
  vtkMEDExtrudeToCircle::VertexData *vertex = new vtkMEDExtrudeToCircle::VertexData();
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
  vtkMEDExtrudeToCircle::VertexData *vertex = new vtkMEDExtrudeToCircle::VertexData();
  vertex->SetId(idIn);
  int id = vertex->GetId();
  CPPUNIT_ASSERT(idIn == id);
  delete vertex;
}

