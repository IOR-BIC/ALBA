/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewImageTest.cpp,v $
Language:  C++
Date:      $Date: 2006-10-30 09:17:37 $
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
#include "mafViewImage.h"
#include "mafViewImageTest.h"

#include "mafVMESurface.h"
#include "mafSceneGraph.h"

#include <iostream>

//-------------------------------------------------------------------------
void mafViewImageTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  mafViewImage *imageView = new mafViewImage();
  imageView->Delete();
   
  CPPUNIT_ASSERT(true);
}

  
