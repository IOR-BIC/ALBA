/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: TriangulationTriangleTest.cpp,v $
Language:  C++
Date:      $Date: 2011-09-13 12:46:40 $
Version:   $Revision: 1.1.2.2 $
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
