/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewPlotTest.cpp,v $
Language:  C++
Date:      $Date: 2010-07-13 12:41:12 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafViewPlot.h"
#include "mafViewPlotTest.h"

//-------------------------------------------------------------------------
void mafViewPlotTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  mafViewPlot *plotView = new mafViewPlot();
  plotView->Create();
  plotView->Delete();

  CPPUNIT_ASSERT(true);

}


