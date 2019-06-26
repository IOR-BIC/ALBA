/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: TriangulationEdgeTest
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
#include "TriangulationEdgeTest.h"

#include "vtkALBAPoissonSurfaceReconstruction.h"

//-------------------------------------------------------------------------
void TriangulationEdgeTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  TriangulationEdge *var = new TriangulationEdge();

  CPPUNIT_ASSERT( var->pIndex[0] == -1 && var->pIndex[1] == -1 && var->tIndex[0] == -1 && var->tIndex[1] == -1 );

  delete var;
}
//-------------------------------------------------------------------------
void TriangulationEdgeTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
  TriangulationEdge var;
}
