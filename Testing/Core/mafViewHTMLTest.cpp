/*=========================================================================

 Program: MAF2
 Module: mafViewHTMLTest
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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

  
