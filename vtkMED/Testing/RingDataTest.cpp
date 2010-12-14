/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: RingDataTest.cpp,v $
Language:  C++
Date:      $Date: 2010-12-14 09:43:25 $
Version:   $Revision: 1.1.2.1 $
Authors:   Roberto Mucci
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
#include "RingDataTest.h"

#include "vtkMEDExtrudeToCircle.h"



//-------------------------------------------------------------------------
void RingDataTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkMEDExtrudeToCircle::RingData *ring = new vtkMEDExtrudeToCircle::RingData();
}

//-------------------------------------------------------------------------
void RingDataTest::TestAllocateVertices()
//-------------------------------------------------------------------------
{
  vtkMEDExtrudeToCircle::RingData *ring = new vtkMEDExtrudeToCircle::RingData();
  ring->AllocateVertices(5);
  delete ring;
}
