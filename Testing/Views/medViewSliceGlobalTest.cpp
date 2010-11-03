/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewSliceGlobalTest.cpp,v $
Language:  C++
Date:      $Date: 2010-11-03 22:37:50 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
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
#include "medViewSliceGlobal.h"
#include "medViewSliceGlobalTest.h"

//-------------------------------------------------------------------------
void medViewSliceGlobalTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  medViewSliceGlobal *view = new medViewSliceGlobal();  
  view->Delete();
   
  //check leaks
  CPPUNIT_ASSERT(true);
 
}