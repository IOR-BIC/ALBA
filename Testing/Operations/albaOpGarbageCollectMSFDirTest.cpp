/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpGarbageCollectMSFDirTest
 Authors: Stefano Perticoni
 
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
#include "albaOpGarbageCollectMSFDirTest.h"
#include "albaOpGarbageCollectMSFDir.h"

#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMEFactory.h"
#include "albaVMESurface.h"
#include "albaVMEVolume.h"
#include "albaDataVector.h"
#include "albaVMEItem.h"

//---------------------------------------------------------
void albaOpGarbageCollectMSFDirTest::TestOpRun()
//---------------------------------------------------------
{
  // Create useless files to be removed by the garbage collector
  wxString sourceFile1 = wxString(ALBA_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\TestMSF.6.vtk";
  CPPUNIT_ASSERT(wxFileExists(sourceFile1));

  wxString targetFakeFile1 = wxString(ALBA_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\Alien.vtk";
  wxCopyFile(sourceFile1, targetFakeFile1);
  CPPUNIT_ASSERT(wxFileExists(targetFakeFile1));

  wxString sourceFile2 = wxString(ALBA_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\TestMSF.54.vtk";
  CPPUNIT_ASSERT(wxFileExists(sourceFile2));

  wxString targetFakeFile2 = wxString(ALBA_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\Klingon.VTK";
  wxCopyFile(sourceFile2, targetFakeFile2);
  CPPUNIT_ASSERT(wxFileExists(targetFakeFile2));

  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = ALBA_DATA_ROOT;
  msfFileNameOut<<"\\Test_GarbageCollectMSFDir\\TestMSF\\TestMSF.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.char_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );

  albaOpGarbageCollectMSFDir *op = new albaOpGarbageCollectMSFDir();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  op->OpRun();

  albaDEL(op);
  albaDEL(storageImport);

  CPPUNIT_ASSERT(wxFileExists(sourceFile1));
  CPPUNIT_ASSERT(wxFileExists(sourceFile2));
  CPPUNIT_ASSERT(!wxFileExists(targetFakeFile1));
  CPPUNIT_ASSERT(!wxFileExists(targetFakeFile2));
}

//---------------------------------------------------------
void albaOpGarbageCollectMSFDirTest::TestGarbageCollect()
//---------------------------------------------------------
{
  // Create useless files to be removed by the garbage collector
  wxString sourceFile1 = wxString(ALBA_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\TestMSF.6.vtk";
  CPPUNIT_ASSERT(wxFileExists(sourceFile1));

  wxString targetFakeFile1 = wxString(ALBA_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\Alien.vtk";
  wxCopyFile(sourceFile1, targetFakeFile1);
  CPPUNIT_ASSERT(wxFileExists(targetFakeFile1));

  wxString sourceFile2 = wxString(ALBA_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\TestMSF.54.vtk";
  CPPUNIT_ASSERT(wxFileExists(sourceFile2));

  wxString targetFakeFile2 = wxString(ALBA_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\Klingon.VTK";
  wxCopyFile(sourceFile2, targetFakeFile2);
  CPPUNIT_ASSERT(wxFileExists(targetFakeFile2));

  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = ALBA_DATA_ROOT;
  msfFileNameOut<<"\\Test_GarbageCollectMSFDir\\TestMSF\\TestMSF.msf";

  CPPUNIT_ASSERT(wxFileExists(msfFileNameOut));

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.char_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );

  albaOpGarbageCollectMSFDir *op = new albaOpGarbageCollectMSFDir();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->GarbageCollect();
  
  CPPUNIT_ASSERT(result == ALBA_OK);

  CPPUNIT_ASSERT(wxFileExists(sourceFile1));
  CPPUNIT_ASSERT(wxFileExists(sourceFile2));
  CPPUNIT_ASSERT(!wxFileExists(targetFakeFile1));
  CPPUNIT_ASSERT(!wxFileExists(targetFakeFile2));

  albaDEL(op);
  albaDEL(storageImport);
}
