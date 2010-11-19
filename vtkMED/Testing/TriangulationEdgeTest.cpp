/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: TriangulationEdgeTest.cpp,v $
Language:  C++
Date:      $Date: 2010-11-19 10:22:41 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
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
#include "TriangulationEdgeTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"

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
