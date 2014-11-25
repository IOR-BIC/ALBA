/*=========================================================================

 Program: MAF2Medical
 Module: TriangulationTriangleTest
 Authors: Matteo Giacomoni
 
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
#include "TriangulationTriangleTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"

//-------------------------------------------------------------------------
void TriangulationTriangleTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  TriangulationTriangle *var = new TriangulationTriangle();

  CPPUNIT_ASSERT( var->EIndex[0] == -1 && var->EIndex[1] == -1 && var->EIndex[2] );

  delete var;
}
//-------------------------------------------------------------------------
void TriangulationTriangleTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
  TriangulationTriangle var;
}
