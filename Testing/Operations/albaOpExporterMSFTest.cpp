/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMSFTest
 Authors: Matteo Giacomoni
 
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
#include "albaOperationsTests.h"

#include "albaOpExporterMSFTest.h"

#include "albaOpExporterMSF.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMEFactory.h"
#include "albaVMESlicer.h"
#include "albaVMEGroup.h"



//----------------------------------------------------------------------------
void albaOpExporterMSFTest::TestFixture()
{
}

//----------------------------------------------------------------------------
void albaOpExporterMSFTest::TestStaticAllocation()
{
  albaOpExporterMSF op;
}
//----------------------------------------------------------------------------
void albaOpExporterMSFTest::TestDynamicAllocation()
{
  albaOpExporterMSF *op = new albaOpExporterMSF();
  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpExporterMSFTest::TestAccept()
{
  albaSmartPointer<DummyVme> vme;
  albaSmartPointer<albaVMERoot> root;
  
  albaOpExporterMSF *op = new albaOpExporterMSF();
  
  //All vme are accepted
  CPPUNIT_ASSERT( op->Accept(vme) );

  //VmeRoot is no accepted
  CPPUNIT_ASSERT( !op->Accept(root) );

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpExporterMSFTest::TestSetFileName()
{
  albaOpExporterMSF *op = new albaOpExporterMSF();

  op->SetFileName("Test.msf");
  
  CPPUNIT_ASSERT( op->GetFileName().Equals("Test.msf") );

  albaDEL(op);

}
//----------------------------------------------------------------------------
void albaOpExporterMSFTest::TestExportMSF1()
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageExport;
  storageExport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageExport->GetRoot()->SetName("root");
  storageExport->GetRoot()->Initialize();

  //To restore it is necessary use "\\" instead "/"
  wxString msfFileNameIn = ALBA_DATA_ROOT;
	msfFileNameIn << "/Test_ExporterMSF/MSF_TEST/MSF_TEST.msf";

  wxString msfFileNameOut = GET_TEST_DATA_DIR();
	msfFileNameOut << "/MSF_TEST_OUT.msf";

  //Restore the input MSF
  storageExport->SetURL(msfFileNameIn.ToAscii());
  CPPUNIT_ASSERT ( storageExport->Restore() == ALBA_OK );

  albaOpExporterMSF *op = new albaOpExporterMSF();

  //Input the group
  CPPUNIT_ASSERT ( op->Accept(storageExport->GetRoot()->GetFirstChild()) );
  op->TestModeOn();
  op->SetInput(storageExport->GetRoot()->GetFirstChild());
  op->SetFileName(msfFileNameOut.ToAscii());
  CPPUNIT_ASSERT ( op->ExportMSF() == ALBA_OK );

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  msfFileNameOut = GET_TEST_DATA_DIR();
	msfFileNameOut << "/MSF_TEST_OUT/MSF_TEST_OUT.msf";

  storageImport->SetURL(msfFileNameOut.ToAscii());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );

  //Check of the vme type inside the tree
  albaVME *group = storageImport->GetRoot()->GetFirstChild();
  group->Update();
  CPPUNIT_ASSERT( group->IsA("albaVMEGroup") );
  albaVME *surfaceParametric =group->GetChild(0);
  surfaceParametric->Update();
  CPPUNIT_ASSERT( surfaceParametric->IsA("albaVMESurfaceParametric") );
  albaVME *volume = group->GetChild(1);
  volume->Update();
  CPPUNIT_ASSERT( volume->IsA("albaVMEVolumeGray") );
  albaVME *slicer = volume->GetChild(0);
  slicer->Update();
  CPPUNIT_ASSERT( slicer->IsA("albaVMESlicer") );

  //Check if the link is correct
  CPPUNIT_ASSERT( albaVMESlicer::SafeDownCast(slicer)->GetSlicedVMELink() == volume );

  //Check if the ABS matrix are equals
  albaVME *groupExported = storageExport->GetRoot()->GetFirstChild();
  groupExported->Update();
  CPPUNIT_ASSERT( groupExported->GetOutput()->GetAbsMatrix()->Equals(group->GetOutput()->GetAbsMatrix()) );
  albaVME *surfaceParametricExported =groupExported->GetChild(0);
  surfaceParametricExported->Update();
  CPPUNIT_ASSERT( surfaceParametricExported->GetOutput()->GetAbsMatrix()->Equals(surfaceParametric->GetOutput()->GetAbsMatrix()) );
  albaVME *volumeExported = groupExported->GetChild(1);
  volumeExported->Update();
  CPPUNIT_ASSERT( volumeExported->GetOutput()->GetAbsMatrix()->Equals(volume->GetOutput()->GetAbsMatrix()) );
  albaVME *slicerExported = volume->GetChild(0);
  slicerExported->Update();
  CPPUNIT_ASSERT( slicerExported->GetOutput()->GetAbsMatrix()->Equals(slicer->GetOutput()->GetAbsMatrix()) );


  albaDEL(op);
  albaDEL(storageExport);
  albaDEL(storageImport);
}
//----------------------------------------------------------------------------
void albaOpExporterMSFTest::TestExportMSF2()
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  albaVMEStorage *storageExport;
  storageExport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageExport->GetRoot()->SetName("root");
  storageExport->GetRoot()->Initialize();

  //To restore it is necessary use "\\" instead "/"
  wxString msfFileNameIn = ALBA_DATA_ROOT;
  msfFileNameIn<<"/Test_ExporterMSF/MSF_TEST/MSF_TEST.msf";

  wxString msfFileNameOut = GET_TEST_DATA_DIR();;
	msfFileNameOut << "/MSF_TEST_OUT.msf";

  //Restore the input MSF
  storageExport->SetURL(msfFileNameIn.ToAscii());
  CPPUNIT_ASSERT ( storageExport->Restore() == ALBA_OK );

  albaOpExporterMSF *op = new albaOpExporterMSF();

  //Input the volume
  CPPUNIT_ASSERT ( op->Accept(storageExport->GetRoot()->GetFirstChild()->GetChild(1)) );
  op->TestModeOn();
  op->SetInput(storageExport->GetRoot()->GetFirstChild()->GetChild(1));
  op->SetFileName(msfFileNameOut.ToAscii());
  CPPUNIT_ASSERT ( op->ExportMSF() == ALBA_OK );

  albaVMEStorage *storageImport;
  storageImport = albaVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  albaVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  msfFileNameOut = GET_TEST_DATA_DIR();
  msfFileNameOut<<"/MSF_TEST_OUT/MSF_TEST_OUT.msf";

  storageImport->SetURL(msfFileNameOut.ToAscii());
  CPPUNIT_ASSERT ( storageImport->Restore() == ALBA_OK );

  //Check of the vme type inside the tree
  albaVME *volume = storageImport->GetRoot()->GetFirstChild();
  volume->Update();
  CPPUNIT_ASSERT( volume->IsA("albaVMEVolumeGray") );
  albaVME *slicer = volume->GetChild(0);
  slicer->Update();
  CPPUNIT_ASSERT( slicer->IsA("albaVMESlicer") );

  //Check if the link is correct
  CPPUNIT_ASSERT( albaVMESlicer::SafeDownCast(slicer)->GetSlicedVMELink() == volume );

  //Check if the ABS matrix are equals
  albaVME *volumeExported = storageExport->GetRoot()->GetFirstChild()->GetChild(1);
  volumeExported->Update();
  CPPUNIT_ASSERT( volumeExported->GetOutput()->GetAbsMatrix()->Equals(volume->GetOutput()->GetAbsMatrix()) );
  albaVME *slicerExported = volumeExported->GetChild(0);
  slicerExported->Update();
  CPPUNIT_ASSERT( slicerExported->GetOutput()->GetAbsMatrix()->Equals(slicer->GetOutput()->GetAbsMatrix()) );
	
  albaDEL(op);
  albaDEL(storageExport);
  albaDEL(storageImport);
}