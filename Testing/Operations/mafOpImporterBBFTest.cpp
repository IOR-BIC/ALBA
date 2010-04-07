/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpImporterBBFTest.cpp,v $
Language:  C++
Date:      $Date: 2010-04-07 10:15:53 $
Version:   $Revision: 1.1.2.1 $
Authors:   Roberto Mucci
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

#include "mafOpImporterBBFTest.h"
#include "mafOpImporterBBF.h"
//#include <vnl/vnl_vector.h>

#include "mafString.h"
#include "mafVMEVolumeLarge.h"
//#include "medVMEAnalog.h"
//#include "mafVMEOutputScalarMatrix.h"

#include <string>
#include <assert.h>

#include <iostream>

//-----------------------------------------------------------
void mafOpImporterBBFTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpImporterBBF *importer = new mafOpImporterBBF("importer");
  cppDEL(importer);
}
//-----------------------------------------------------------
void mafOpImporterBBFTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  mafOpImporterBBF importer; 
}

//-----------------------------------------------------------
void mafOpImporterBBFTest::TestImport() 
//-----------------------------------------------------------
{
	mafOpImporterBBF *importer=new mafOpImporterBBF("importer");
	importer->TestModeOn();
	mafString filename=MED_DATA_ROOT;
  filename<<"/Test_ImporterBBF/prova_upload_vme_1_01.bbf";
	importer->SetFileName(filename.GetCStr());
	CPPUNIT_ASSERT(importer->ImportBBF() == MAF_OK);

  mafVMEVolumeLarge *vmeLarge = (mafVMEVolumeLarge *)importer->GetOutput();
  vmeLarge->Update();

  CPPUNIT_ASSERT(vmeLarge != NULL);
  cppDEL(importer);
}