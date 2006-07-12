/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewOrthoSliceTest.cpp,v $
Language:  C++
Date:      $Date: 2006-07-12 07:51:08 $
Version:   $Revision: 1.2 $
Authors:   Stefano Perticoni
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
#include "mafViewOrthoSlice.h"
#include "mafViewOrthoSliceTest.h"

#include "mafVMESurface.h"
#include "mafSceneGraph.h"

#include <iostream>

//-------------------------------------------------------------------------
void mafViewOrthoSliceTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  mafViewOrthoSlice *rxctView = new mafViewOrthoSlice();
  rxctView->PackageView();
  rxctView->Delete();
   
  CPPUNIT_ASSERT(true);
 
}
//-------------------------------------------------------------------------
void mafViewOrthoSliceTest::VmeShowTest()
//-------------------------------------------------------------------------
{
  mafViewOrthoSlice *rxctView = new mafViewOrthoSlice();
  //rxctView->PackageView();
  
  mafVMESurface *surface;


  rxctView->GetSceneGraph()->m_List;
  
  rxctView->Delete();
   

  CPPUNIT_ASSERT(true);
 
}

  
