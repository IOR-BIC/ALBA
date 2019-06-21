/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: CoredPointIndexTest
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
#include "CoredPointIndexTest.h"

#include "vtkALBAPoissonSurfaceReconstruction.h"

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
