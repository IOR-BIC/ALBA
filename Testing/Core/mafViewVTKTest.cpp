/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewVTKTest.cpp,v $
Language:  C++
Date:      $Date: 2006-07-13 08:17:18 $
Version:   $Revision: 1.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafViewVTKTest.h"
#include "mafViewVTK.h"
#include <cppunit/config/SourcePrefix.h>

//#include "mafVMESurface.h"
//#include "mafPipeSurface.h"



//-------------------------------------------------------------------------
void mafViewVTKTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  mafViewVTK *View=new mafViewVTK();
  View->Delete();
   
  CPPUNIT_ASSERT(true);
 
}
//-------------------------------------------------------------------------
void mafViewVTKTest::Surface()
//-------------------------------------------------------------------------
{
  mafViewVTK *View=new mafViewVTK();
	View->PlugVisualPipe("mafVMESurface","mafPipeSurface");
  View->Delete();
   
  CPPUNIT_ASSERT(true);
 
}
