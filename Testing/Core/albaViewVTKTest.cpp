/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewVTKTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include "albaViewVTKTest.h"
#include "albaViewVTK.h"
#include <cppunit/config/SourcePrefix.h>

//#include "albaVMESurface.h"
//#include "albaPipeSurface.h"



//-------------------------------------------------------------------------
void albaViewVTKTest::CreateDestroyTest()
//-------------------------------------------------------------------------
{
  albaViewVTK *View=new albaViewVTK();
  View->Delete();
}
//-------------------------------------------------------------------------
void albaViewVTKTest::Surface()
//-------------------------------------------------------------------------
{
  albaViewVTK *View=new albaViewVTK();
	View->PlugVisualPipe("albaVMESurface","albaPipeSurface");
  View->Delete();
}
