/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: EdgeTest.cpp,v $
Language:  C++
Date:      $Date: 2010-11-18 14:40:29 $
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
#include "EdgeTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"

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
