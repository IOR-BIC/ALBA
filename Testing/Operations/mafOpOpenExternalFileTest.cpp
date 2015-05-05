/*=========================================================================

 Program: MAF2
 Module: mafOpOpenExternalFileTest
 Authors: Roberto Mucci
 
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

#include "mafOpOpenExternalFileTest.h"
#include "mafOpOpenExternalFile.h"
#include "mafOpImporterExternalFile.h"
#include "mafVMEFactory.h"
#include "mafString.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include <iostream>

//----------------------------------------------------------------------------
void mafOpOpenExternalFileTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafOpOpenExternalFileTest::setUp()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafOpOpenExternalFileTest::tearDown()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafOpOpenExternalFileTest::TestConstructor()
//----------------------------------------------------------------------------
{
  //check memory leaks mad by constructor
  mafOpOpenExternalFile *openExternalFile  = new mafOpOpenExternalFile("openExternalFile");
  mafDEL(openExternalFile);
}

//----------------------------------------------------------------------------
void mafOpOpenExternalFileTest::TestImporter()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");

  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();

  // IMPORT the external file
	mafOpImporterExternalFile *importer=new mafOpImporterExternalFile("importer");
	importer->SetInput(storage->GetRoot());
	mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_OpenExternalFile/ExternalFile.txt";
  importer->SetFileName(filename.GetCStr());
  importer->OpRun();

  //OPEN the external file
  mafOpOpenExternalFile *externalFile = new mafOpOpenExternalFile("openExternalFile");
  externalFile->TestModeOn();
  externalFile->SetInput(importer->GetOutput());
  externalFile->OpRun();
  long pid = externalFile->GetPid();

  //wait for application to open the file and then kill the application
  Sleep(500);
  wxKill(pid);
  
  mafDEL(externalFile);
  mafDEL(importer);
  mafDEL(storage);
}
