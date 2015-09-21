/*=========================================================================

 Program: MAF2
 Module: mafViewVTKTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
}
//-------------------------------------------------------------------------
void mafViewVTKTest::Surface()
//-------------------------------------------------------------------------
{
  mafViewVTK *View=new mafViewVTK();
	View->PlugVisualPipe("mafVMESurface","mafPipeSurface");
  View->Delete();
}
