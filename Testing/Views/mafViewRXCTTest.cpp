/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewRXCTTest.cpp,v $
Language:  C++
Date:      $Date: 2008-04-11 08:44:43 $
Version:   $Revision: 1.3 $
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


  rxctView->GetSceneGraph()->GetNodeList();
  
  rxctView->Delete();
   

  CPPUNIT_ASSERT(true);
 
}

  
