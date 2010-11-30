/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterC3DTest.cpp,v $
Language:  C++
Date:      $Date: 2010-11-30 09:44:51 $
Version:   $Revision: 1.1.2.1 $
Authors:   Simone Brazzale
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpImporterC3DTest.h"
#include "medOpImporterC3D.h"

#include <assert.h>

//-----------------------------------------------------------
void medOpImporterC3DTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpImporterC3D *importer = new medOpImporterC3D("C3D importer");
  cppDEL(importer);
}
//-----------------------------------------------------------
void medOpImporterC3DTest::TestCopy() 
//-----------------------------------------------------------
{
  medOpImporterC3D *importer = new medOpImporterC3D("C3D importer");
  medOpImporterC3D *importer_copy = NULL;

  importer->TestModeOn();
  importer_copy = ((medOpImporterC3D*)importer->Copy());

  CPPUNIT_ASSERT(importer_copy);

  cppDEL(importer_copy);
  cppDEL(importer);
}
