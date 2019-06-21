/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterExternalFileTest
 Authors: Roberto Mucci
 
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

#include <cppunit/config/SourcePrefix.h>

#include "albaOpImporterExternalFileTest.h"
#include "albaOpImporterExternalFile.h"
#include "albaVMEFactory.h"
#include "albaString.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include <iostream>

//----------------------------------------------------------------------------
void albaOpImporterExternalFileTest::TestFixture()
//----------------------------------------------------------------------------
{
}


//----------------------------------------------------------------------------
void albaOpImporterExternalFileTest::TestConstructor()
//----------------------------------------------------------------------------
{
  //check memory leaks mad by constructor
  albaOpImporterExternalFile *importerExternalFile  = new albaOpImporterExternalFile("importerExtFile");
  albaDEL(importerExternalFile);
}

//----------------------------------------------------------------------------
void albaOpImporterExternalFileTest::TestImporter()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");

  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();

	albaOpImporterExternalFile *importer=new albaOpImporterExternalFile("importer");
	importer->SetInput(storage->GetRoot());
	albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_ImporterExternalFile/ExternalFile.doc";
  importer->SetFileName(filename.GetCStr());
  importer->OpRun();

  albaVME *node = importer->GetOutput();
  //Check if the VME is of the right kinnd
  bool type = node->IsA("albaVMEExternalData");
  CPPUNIT_ASSERT(type);

  //check the name of the Group imported by the importerExternalFile
  albaString name = node->GetName();
  CPPUNIT_ASSERT(name.Compare("ExternalFile.doc") == 0);
 
  albaDEL(importer);
  albaDEL(storage);
}
