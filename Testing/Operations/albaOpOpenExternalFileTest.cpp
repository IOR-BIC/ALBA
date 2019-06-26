/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpOpenExternalFileTest
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

#include "albaOpOpenExternalFileTest.h"
#include "albaOpOpenExternalFile.h"
#include "albaOpImporterExternalFile.h"
#include "albaVMEFactory.h"
#include "albaString.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include <iostream>

//----------------------------------------------------------------------------
void albaOpOpenExternalFileTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaOpOpenExternalFileTest::TestConstructor()
//----------------------------------------------------------------------------
{
  //check memory leaks mad by constructor
  albaOpOpenExternalFile *openExternalFile  = new albaOpOpenExternalFile("openExternalFile");
  albaDEL(openExternalFile);
}

//----------------------------------------------------------------------------
void albaOpOpenExternalFileTest::TestImporter()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");

  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();

  // IMPORT the external file
	albaOpImporterExternalFile *importer=new albaOpImporterExternalFile("importer");
	importer->SetInput(storage->GetRoot());
	albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_OpenExternalFile/ExternalFile.txt";
  importer->SetFileName(filename.GetCStr());
  importer->OpRun();

  //OPEN the external file
  albaOpOpenExternalFile *externalFile = new albaOpOpenExternalFile("openExternalFile");
  externalFile->TestModeOn();
  externalFile->SetInput(importer->GetOutput());
  externalFile->OpRun();
  long pid = externalFile->GetPid();

  //wait for application to open the file and then kill the application
  Sleep(500);
  wxKill(pid);
  
  albaDEL(externalFile);
  albaDEL(importer);
  albaDEL(storage);
}
