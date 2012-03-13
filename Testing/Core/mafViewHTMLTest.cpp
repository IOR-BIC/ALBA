/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewHTMLTest.cpp,v $
Language:  C++
Date:      $Date: 2006-09-20 12:19:59 $
Version:   $Revision: 1.1 $
Authors:   Marco Petrone, Stefano Perticoni
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
#include "mafViewHTML.h"
#include "mafViewHTMLTest.h"

#include "mafVMESurface.h"
#include "mafSceneGraph.h"

#include <iostream>

//-------------------------------------------------------------------------
void mafViewHTMLTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  mafViewHTML *htmlView = new mafViewHTML();
  htmlView->Delete();
   
  CPPUNIT_ASSERT(true);
 
}
//-------------------------------------------------------------------------
void mafViewHTMLTest::VmeShowTest()
//-------------------------------------------------------------------------
{
  mafViewHTML *htmlView = new mafViewHTML();
  htmlView->Create(); 
 
  htmlView->Delete();
   

  CPPUNIT_ASSERT(true);
 
}

  
