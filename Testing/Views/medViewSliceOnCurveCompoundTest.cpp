/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewSliceOnCurveCompoundTest.cpp,v $
Language:  C++
Date:      $Date: 2010-04-06 09:04:49 $
Version:   $Revision: 1.1.2.1 $
Authors:   Roberto Mucci
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
#include "medViewSliceOnCurveCompound.h"
#include "medViewSliceOnCurveCompoundTest.h"

#include <iostream>

//-------------------------------------------------------------------------
void medViewSliceOnCurveCompoundTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  medViewSliceOnCurveCompound *socView = new medViewSliceOnCurveCompound("");

  socView->PackageView();
  socView->Delete();;

  CPPUNIT_ASSERT(true);

}