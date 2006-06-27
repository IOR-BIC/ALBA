/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewOrthoSliceTest.cpp,v $
Language:  C++
Date:      $Date: 2006-06-27 15:07:18 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

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

  
