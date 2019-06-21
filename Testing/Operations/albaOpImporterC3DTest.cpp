/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterC3DTest
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpImporterC3DTest.h"
#include "albaOpImporterC3D.h"

#include <assert.h>

//-----------------------------------------------------------
void albaOpImporterC3DTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpImporterC3D *importer = new albaOpImporterC3D("C3D importer");
  cppDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterC3DTest::TestCopy() 
//-----------------------------------------------------------
{
  albaOpImporterC3D *importer = new albaOpImporterC3D("C3D importer");
  albaOpImporterC3D *importer_copy = NULL;

  importer->TestModeOn();
  importer_copy = ((albaOpImporterC3D*)importer->Copy());

  CPPUNIT_ASSERT(importer_copy);

  cppDEL(importer_copy);
  cppDEL(importer);
}
