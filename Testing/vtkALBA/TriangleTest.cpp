/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: TriangleTest
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
#include "TriangleTest.h"

#include "vtkALBAPoissonSurfaceReconstruction.h"

#define TOLLERANCE 0.0001

//-------------------------------------------------------------------------
void TriangleTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  Triangle *triangle = new Triangle();

  delete triangle;
}
//-------------------------------------------------------------------------
void TriangleTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
  Triangle triangle;
}
//-------------------------------------------------------------------------
void TriangleTest::TestArea()
//-------------------------------------------------------------------------
{
  Triangle triangle;
  triangle.p[0][0] = 1.0;
  triangle.p[0][1] = 0.0;
  triangle.p[0][2] = 0.0;

  triangle.p[1][0] = -1.0;
  triangle.p[1][1] = 0.0;
  triangle.p[1][2] = 0.0;

  triangle.p[2][0] = 0.0;
  triangle.p[2][1] = 2.0;
  triangle.p[2][2] = 0.0;

  CPPUNIT_ASSERT( triangle.Area() == 2.0 );
}
//-------------------------------------------------------------------------
void TriangleTest::TestAspectRatio()
//-------------------------------------------------------------------------
{
  Triangle triangle;
  triangle.p[0][0] = 1.0;
  triangle.p[0][1] = 0.0;
  triangle.p[0][2] = 0.0;

  triangle.p[1][0] = -1.0;
  triangle.p[1][1] = 0.0;
  triangle.p[1][2] = 0.0;

  triangle.p[2][0] = 0.0;
  triangle.p[2][1] = 2.0;
  triangle.p[2][2] = 0.0;

  CPPUNIT_ASSERT( triangle.AspectRatio() == 2.0/14.0 );
}
