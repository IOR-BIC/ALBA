/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewRXCTTest.cpp,v $
Language:  C++
Date:      $Date: 2006-06-26 13:18:40 $
Version:   $Revision: 1.1 $
Authors:   Marco Petrone, Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "mafViewRXCT.h"
#include "mafViewRXCTTest.h"

#include "mafVMESurface.h"
#include "mafSceneGraph.h"

#include <iostream>


//-------------------------------------------------------------------------
void mafViewRXCTTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  mafViewRXCT *rxctView = new mafViewRXCT();
  rxctView->PackageView();
  rxctView->Delete();
   
  CPPUNIT_ASSERT(true);
 
}
//-------------------------------------------------------------------------
void mafViewRXCTTest::VmeShowTest()
//-------------------------------------------------------------------------
{
  mafViewRXCT *rxctView = new mafViewRXCT();
  //rxctView->PackageView();
  
  mafVMESurface *surface;


  rxctView->GetSceneGraph()->m_List;
  
  rxctView->Delete();
   

  CPPUNIT_ASSERT(true);
 
}

  
