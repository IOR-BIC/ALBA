/*=========================================================================

 Program: MAF2
 Module: mafOpGarbageCollectMSFDirTest
 Authors: Stefano Perticoni
 
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
#include "mafOpGarbageCollectMSFDirTest.h"
#include "mafOpGarbageCollectMSFDir.h"

#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMEFactory.h"
#include "mafVMESurface.h"
#include "mafVMEVolume.h"
#include "mafDataVector.h"
#include "mafVMEItem.h"

//---------------------------------------------------------
void mafOpGarbageCollectMSFDirTest::TestOpRun()
//---------------------------------------------------------
{
  // Create useless files to be removed by the garbage collector
  wxString sourceFile1 = wxString(MAF_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\TestMSF.50.vtk";
  CPPUNIT_ASSERT(wxFileExists(sourceFile1));

  wxString targetFakeFile1 = wxString(MAF_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\Alien.vtk";
  wxCopyFile(sourceFile1, targetFakeFile1);
  CPPUNIT_ASSERT(wxFileExists(targetFakeFile1));

  wxString sourceFile2 = wxString(MAF_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\TestMSF.54.vtk";
  CPPUNIT_ASSERT(wxFileExists(sourceFile2));

  wxString targetFakeFile2 = wxString(MAF_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\Klingon.VTK";
  wxCopyFile(sourceFile2, targetFakeFile2);
  CPPUNIT_ASSERT(wxFileExists(targetFakeFile2));

  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_GarbageCollectMSFDir\\TestMSF\\TestMSF.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );

  mafOpGarbageCollectMSFDir *op = new mafOpGarbageCollectMSFDir();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  op->OpRun();

  mafDEL(op);
  mafDEL(storageImport);

  CPPUNIT_ASSERT(wxFileExists(sourceFile1));
  CPPUNIT_ASSERT(wxFileExists(sourceFile2));
  CPPUNIT_ASSERT(!wxFileExists(targetFakeFile1));
  CPPUNIT_ASSERT(!wxFileExists(targetFakeFile2));
}

//---------------------------------------------------------
void mafOpGarbageCollectMSFDirTest::TestGarbageCollect()
//---------------------------------------------------------
{
  // Create useless files to be removed by the garbage collector
  wxString sourceFile1 = wxString(MAF_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\TestMSF.50.vtk";
  CPPUNIT_ASSERT(wxFileExists(sourceFile1));

  wxString targetFakeFile1 = wxString(MAF_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\Alien.vtk";
  wxCopyFile(sourceFile1, targetFakeFile1);
  CPPUNIT_ASSERT(wxFileExists(targetFakeFile1));

  wxString sourceFile2 = wxString(MAF_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\TestMSF.54.vtk";
  CPPUNIT_ASSERT(wxFileExists(sourceFile2));

  wxString targetFakeFile2 = wxString(MAF_DATA_ROOT) + "\\Test_GarbageCollectMSFDir\\TestMSF\\Klingon.VTK";
  wxCopyFile(sourceFile2, targetFakeFile2);
  CPPUNIT_ASSERT(wxFileExists(targetFakeFile2));

  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_GarbageCollectMSFDir\\TestMSF\\TestMSF.msf";

  CPPUNIT_ASSERT(wxFileExists(msfFileNameOut));

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );

  mafOpGarbageCollectMSFDir *op = new mafOpGarbageCollectMSFDir();
  op->TestModeOn();
  op->SetInput(storageImport->GetRoot());
  int result = op->GarbageCollect();
  
  CPPUNIT_ASSERT(result == MAF_OK);

  CPPUNIT_ASSERT(wxFileExists(sourceFile1));
  CPPUNIT_ASSERT(wxFileExists(sourceFile2));
  CPPUNIT_ASSERT(!wxFileExists(targetFakeFile1));
  CPPUNIT_ASSERT(!wxFileExists(targetFakeFile2));

  mafDEL(op);
  mafDEL(storageImport);
}
