/*=========================================================================

 Program: MAF2
 Module: TriangulationEdgeTest
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
#include "TriangulationEdgeTest.h"

#include "vtkMAFPoissonSurfaceReconstruction.h"

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
