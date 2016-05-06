/*=========================================================================

 Program: MAF2
 Module: mafOpExporterMSFTest
 Authors: Matteo Giacomoni
 
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
#include "mafOperationsTests.h"

#include "mafOpExporterMSFTest.h"

#include "mafOpExporterMSF.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMEFactory.h"
#include "mafVMESlicer.h"
#include "mafVMEGroup.h"



//----------------------------------------------------------------------------
void mafOpExporterMSFTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafOpExporterMSFTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafOpExporterMSF op;
}
//----------------------------------------------------------------------------
void mafOpExporterMSFTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpExporterMSF *op = new mafOpExporterMSF();
  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpExporterMSFTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafSmartPointer<DummyVme> vme;
  mafSmartPointer<mafVMERoot> root;
  
  mafOpExporterMSF *op = new mafOpExporterMSF();
  
  //All vme are accepted
  CPPUNIT_ASSERT( op->Accept(vme) );

  //VmeRoot is no accepted
  CPPUNIT_ASSERT( !op->Accept(root) );

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpExporterMSFTest::TestSetFileName()
//----------------------------------------------------------------------------
{
  mafOpExporterMSF *op = new mafOpExporterMSF();

  op->SetFileName("Test.msf");
  
  CPPUNIT_ASSERT( op->GetFileName().Equals("Test.msf") );

  mafDEL(op);

}
//----------------------------------------------------------------------------
void mafOpExporterMSFTest::TestExportMSF1()
//----------------------------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageExport;
  storageExport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageExport->GetRoot()->SetName("root");
  storageExport->GetRoot()->Initialize();

  //To restore it is necessary use "\\" instead "/"
  wxString msfFileNameIn = MAF_DATA_ROOT;
  msfFileNameIn<<"\\Test_ExporterMSF\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameIn.Replace("/","\\");

  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ExporterMSF\\MSF_TEST_OUT.msf";

  msfFileNameOut.Replace("/","\\");

  //Restore the input MSF
  storageExport->SetURL(msfFileNameIn.c_str());
  CPPUNIT_ASSERT ( storageExport->Restore() == MAF_OK );

  mafOpExporterMSF *op = new mafOpExporterMSF();

  //Input the group
  CPPUNIT_ASSERT ( op->Accept(storageExport->GetRoot()->GetFirstChild()) );
  op->TestModeOn();
  op->SetInput(storageExport->GetRoot()->GetFirstChild());
  op->SetFileName(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( op->ExportMSF() == MAF_OK );

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ExporterMSF\\MSF_TEST_OUT\\MSF_TEST_OUT.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );

  //Check of the vme type inside the tree
  mafVME *group = storageImport->GetRoot()->GetFirstChild();
  group->Update();
  CPPUNIT_ASSERT( group->IsA("mafVMEGroup") );
  mafVME *surfaceParametric =group->GetChild(0);
  surfaceParametric->Update();
  CPPUNIT_ASSERT( surfaceParametric->IsA("mafVMESurfaceParametric") );
  mafVME *volume = group->GetChild(1);
  volume->Update();
  CPPUNIT_ASSERT( volume->IsA("mafVMEVolumeGray") );
  mafVME *slicer = volume->GetChild(0);
  slicer->Update();
  CPPUNIT_ASSERT( slicer->IsA("mafVMESlicer") );

  //Check if the link is correct
  CPPUNIT_ASSERT( mafVMESlicer::SafeDownCast(slicer)->GetSlicedVMELink() == volume );

  //Check if the ABS matrix are equals
  mafVME *groupExported = storageExport->GetRoot()->GetFirstChild();
  groupExported->Update();
  CPPUNIT_ASSERT( groupExported->GetOutput()->GetAbsMatrix()->Equals(group->GetOutput()->GetAbsMatrix()) );
  mafVME *surfaceParametricExported =groupExported->GetChild(0);
  surfaceParametricExported->Update();
  CPPUNIT_ASSERT( surfaceParametricExported->GetOutput()->GetAbsMatrix()->Equals(surfaceParametric->GetOutput()->GetAbsMatrix()) );
  mafVME *volumeExported = groupExported->GetChild(1);
  volumeExported->Update();
  CPPUNIT_ASSERT( volumeExported->GetOutput()->GetAbsMatrix()->Equals(volume->GetOutput()->GetAbsMatrix()) );
  mafVME *slicerExported = volume->GetChild(0);
  slicerExported->Update();
  CPPUNIT_ASSERT( slicerExported->GetOutput()->GetAbsMatrix()->Equals(slicer->GetOutput()->GetAbsMatrix()) );


  mafDEL(op);
  mafDEL(storageExport);
  mafDEL(storageImport);
}
//----------------------------------------------------------------------------
void mafOpExporterMSFTest::TestExportMSF2()
//----------------------------------------------------------------------------
{
  //To Eliminate VTK warning window
  vtkObject::GlobalWarningDisplayOff();

  mafVMEStorage *storageExport;
  storageExport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageExport->GetRoot()->SetName("root");
  storageExport->GetRoot()->Initialize();

  //To restore it is necessary use "\\" instead "/"
  wxString msfFileNameIn = MAF_DATA_ROOT;
  msfFileNameIn<<"\\Test_ExporterMSF\\MSF_TEST\\MSF_TEST.msf";

  msfFileNameIn.Replace("/","\\");

  wxString msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ExporterMSF\\MSF_TEST_OUT.msf";

  msfFileNameOut.Replace("/","\\");

  //Restore the input MSF
  storageExport->SetURL(msfFileNameIn.c_str());
  CPPUNIT_ASSERT ( storageExport->Restore() == MAF_OK );

  mafOpExporterMSF *op = new mafOpExporterMSF();

  //Input the volume
  CPPUNIT_ASSERT ( op->Accept(storageExport->GetRoot()->GetFirstChild()->GetChild(1)) );
  op->TestModeOn();
  op->SetInput(storageExport->GetRoot()->GetFirstChild()->GetChild(1));
  op->SetFileName(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( op->ExportMSF() == MAF_OK );

  mafVMEStorage *storageImport;
  storageImport = mafVMEStorage::New();
  // in order to create VME from storage I need the factory to be initialized
  mafVMEFactory::Initialize();
  storageImport->GetRoot()->SetName("root");
  storageImport->GetRoot()->Initialize();

  //Restore the MSF exported
  msfFileNameOut = MAF_DATA_ROOT;
  msfFileNameOut<<"\\Test_ExporterMSF\\MSF_TEST_OUT\\MSF_TEST_OUT.msf";

  msfFileNameOut.Replace("/","\\");
  storageImport->SetURL(msfFileNameOut.c_str());
  CPPUNIT_ASSERT ( storageImport->Restore() == MAF_OK );

  //Check of the vme type inside the tree
  mafVME *volume = storageImport->GetRoot()->GetFirstChild();
  volume->Update();
  CPPUNIT_ASSERT( volume->IsA("mafVMEVolumeGray") );
  mafVME *slicer = volume->GetChild(0);
  slicer->Update();
  CPPUNIT_ASSERT( slicer->IsA("mafVMESlicer") );

  //Check if the link is correct
  CPPUNIT_ASSERT( mafVMESlicer::SafeDownCast(slicer)->GetSlicedVMELink() == volume );

  //Check if the ABS matrix are equals
  mafVME *volumeExported = storageExport->GetRoot()->GetFirstChild()->GetChild(1);
  volumeExported->Update();
  CPPUNIT_ASSERT( volumeExported->GetOutput()->GetAbsMatrix()->Equals(volume->GetOutput()->GetAbsMatrix()) );
  mafVME *slicerExported = volume->GetChild(0);
  slicerExported->Update();
  CPPUNIT_ASSERT( slicerExported->GetOutput()->GetAbsMatrix()->Equals(slicer->GetOutput()->GetAbsMatrix()) );


  mafDEL(op);
  mafDEL(storageExport);
  mafDEL(storageImport);
}