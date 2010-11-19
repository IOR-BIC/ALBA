/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: CoredPointIndexTest.cpp,v $
Language:  C++
Date:      $Date: 2010-11-19 09:36:27 $
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
#include "CoredPointIndexTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"

#define TOLLERANCE 0.0001

//-------------------------------------------------------------------------
void CoredPointIndexTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  CoredPointIndex *coredPointIndex = new CoredPointIndex();

  delete coredPointIndex;
}
//-------------------------------------------------------------------------
void CoredPointIndexTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
  CoredPointIndex coredPointIndex;
}
//-------------------------------------------------------------------------
void CoredPointIndexTest::TestEquals()
//-------------------------------------------------------------------------
{
  CoredPointIndex coredPointIndex1;
  coredPointIndex1.inCore = 'a';
  coredPointIndex1.index = 0;
  CoredPointIndex coredPointIndex2;
  coredPointIndex2.inCore = 'a';
  coredPointIndex2.index = 0;

  CPPUNIT_ASSERT( coredPointIndex1 == coredPointIndex2 );
}
//-------------------------------------------------------------------------
void CoredPointIndexTest::TestDifferent()
//-------------------------------------------------------------------------
{
  CoredPointIndex coredPointIndex1;
  coredPointIndex1.inCore = 'a';
  coredPointIndex1.index = 0;
  CoredPointIndex coredPointIndex2;
  coredPointIndex2.inCore = 'b';
  coredPointIndex2.index = 0;

  CPPUNIT_ASSERT( coredPointIndex1 != coredPointIndex2 );
}
