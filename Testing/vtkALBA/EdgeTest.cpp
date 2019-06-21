/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: EdgeTest
 Authors: Matteo Giacomoni
 
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
#include "EdgeTest.h"

#include "vtkALBAPoissonSurfaceReconstruction.h"

#define TOLLERANCE 0.0001

//-------------------------------------------------------------------------
void EdgeTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  Edge *edge = new Edge();

  delete edge;
}
//-------------------------------------------------------------------------
void EdgeTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
  Edge edge;
}
//-------------------------------------------------------------------------
void EdgeTest::TestLength()
//-------------------------------------------------------------------------
{
  Edge edge;
  edge.p[0][0] = 0.0;
  edge.p[0][1] = 0.0;

  edge.p[1][0] = 10.0;
  edge.p[1][1] = 20.0;

  double length = edge.Length();
  CPPUNIT_ASSERT( length+TOLLERANCE > 22.3607 && length-TOLLERANCE < 22.3607 );
}
