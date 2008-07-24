/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafView3DTest.cpp,v $
Language:  C++
Date:      $Date: 2008-07-24 09:05:18 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
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
#include "mafView3D.h"
#include "mafView3DTest.h"

#include <iostream>

//-------------------------------------------------------------------------
void mafView3DTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  mafView3D *view3D = new mafView3D();
  view3D->Delete();
   
  CPPUNIT_ASSERT(true);
}

  
