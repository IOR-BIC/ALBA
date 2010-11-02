/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewSliceOnCurveTest.cpp,v $
Language:  C++
Date:      $Date: 2010-11-02 12:54:57 $
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
#include "medViewSliceOnCurve.h"
#include "medViewSliceOnCurveTest.h"

#include <iostream>

//-------------------------------------------------------------------------
void medViewSliceOnCurveTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  medViewSliceOnCurve *socView = new medViewSliceOnCurve("");

  socView->PackageView();
  socView->Delete();;

  CPPUNIT_ASSERT(true);

}