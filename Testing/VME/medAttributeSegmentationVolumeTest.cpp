/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medAttributeSegmentationVolumeTest.cpp,v $
Language:  C++
Date:      $Date: 2010-06-07 13:30:10 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2009
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medAttributeSegmentationVolumeTest.h"
#include "medAttributeSegmentationVolume.h"
#include "medVMESegmentationVolume.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "medVMEFactory.h"

//----------------------------------------------------------------------------
void medAttributeSegmentationVolumeTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medAttributeSegmentationVolumeTest::tearDown()
//----------------------------------------------------------------------------
{
}
//---------------------------------------------------------
void medAttributeSegmentationVolumeTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  medAttributeSegmentationVolume *attribute1 = NULL;
  mafNEW(attribute1);
  mafDEL(attribute1);

  mafSmartPointer<medAttributeSegmentationVolume> attribute2;
}
//---------------------------------------------------------
void medAttributeSegmentationVolumeTest::TestSetAutomaticSegmentationThresholdModality()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<medVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<medAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MED_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Default value
  CPPUNIT_ASSERT( attribute->GetAutomaticSegmentationThresholdModality() ==  medVMESegmentationVolume::GLOBAL);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Test SET
  attribute->SetAutomaticSegmentationThresholdModality(medVMESegmentationVolume::RANGE);
  CPPUNIT_ASSERT( attribute->GetAutomaticSegmentationThresholdModality() ==  medVMESegmentationVolume::RANGE);
  attribute->SetAutomaticSegmentationThresholdModality(medVMESegmentationVolume::GLOBAL);
  CPPUNIT_ASSERT( attribute->GetAutomaticSegmentationThresholdModality() ==  medVMESegmentationVolume::GLOBAL);
  attribute->SetAutomaticSegmentationThresholdModality(medVMESegmentationVolume::RANGE);
  CPPUNIT_ASSERT( attribute->GetAutomaticSegmentationThresholdModality() ==  medVMESegmentationVolume::RANGE);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  medAttributeSegmentationVolume *restoredAttribute = medAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  CPPUNIT_ASSERT( restoredAttribute->GetAutomaticSegmentationThresholdModality() == medVMESegmentationVolume::RANGE );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void medAttributeSegmentationVolumeTest::TestSetAutomaticSegmentationGlobalThreshold()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<medVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<medAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MED_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Default value
  CPPUNIT_ASSERT( attribute->GetAutomaticSegmentationGlobalThreshold() ==  0.0);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Test SET
  attribute->SetAutomaticSegmentationGlobalThreshold(-100.0);
  CPPUNIT_ASSERT( attribute->GetAutomaticSegmentationGlobalThreshold() ==  -100.0);
  attribute->SetAutomaticSegmentationGlobalThreshold(100.0);
  CPPUNIT_ASSERT( attribute->GetAutomaticSegmentationGlobalThreshold() ==  100.0);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  medAttributeSegmentationVolume *restoredAttribute = medAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  CPPUNIT_ASSERT( restoredAttribute->GetAutomaticSegmentationGlobalThreshold() == 100.0 );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void medAttributeSegmentationVolumeTest::TestAddRange()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<medVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<medAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MED_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Test SET
  int resultAddGet;
  int startSlice,endSlice;
  double threshold;
  resultAddGet = attribute->AddRange(0,10,5.0);
  CPPUNIT_ASSERT( resultAddGet == MAF_OK );
  resultAddGet = attribute->AddRange(12,22,10.0);
  CPPUNIT_ASSERT( resultAddGet == MAF_OK );
  resultAddGet = attribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( resultAddGet == MAF_OK );
  CPPUNIT_ASSERT( startSlice == 0 && endSlice == 10 && threshold == 5.0 );
  resultAddGet = attribute->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( resultAddGet == MAF_OK );
  CPPUNIT_ASSERT( startSlice == 12 && endSlice == 22 && threshold == 10.0 );
  resultAddGet = attribute->GetRange(2,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( resultAddGet == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  medAttributeSegmentationVolume *restoredAttribute = medAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  restoredAttribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 0 && endSlice == 10 && threshold == 5.0 );
  restoredAttribute->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 12 && endSlice == 22 && threshold == 10.0 );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void medAttributeSegmentationVolumeTest::TestUpdateRange()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<medVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<medAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MED_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Test SET
  int resultUpdate;
  int startSlice,endSlice;
  double threshold;
  attribute->AddRange(0,10,5.0);
  attribute->AddRange(12,22,10.0);
  attribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 0 && endSlice == 10 && threshold == 5.0 );
  attribute->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 12 && endSlice == 22 && threshold == 10.0 );
  resultUpdate = attribute->UpdateRange(0,6,7,2.0);
  CPPUNIT_ASSERT( resultUpdate == MAF_OK );
  attribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 6 && endSlice == 7 && threshold == 2.0 );
  resultUpdate = attribute->UpdateRange(2,6,7,2.0);
  CPPUNIT_ASSERT( resultUpdate == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  medAttributeSegmentationVolume *restoredAttribute = medAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  restoredAttribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 6 && endSlice == 7 && threshold == 2.0 );
  restoredAttribute->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 12 && endSlice == 22 && threshold == 10.0 );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void medAttributeSegmentationVolumeTest::TestDeleteRange()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<medVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<medAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MED_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Test SET
  int resultDelete;
  int startSlice,endSlice;
  double threshold;
  attribute->AddRange(0,10,5.0);
  attribute->AddRange(12,22,10.0);
  resultDelete = attribute->DeleteRange(0);
  CPPUNIT_ASSERT( resultDelete == MAF_OK );
  attribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 12 && endSlice == 22 && threshold == 10.0 );
  resultDelete = attribute->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  medAttributeSegmentationVolume *restoredAttribute = medAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  restoredAttribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 12 && endSlice == 22 && threshold == 10.0 );
  resultDelete = restoredAttribute->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void medAttributeSegmentationVolumeTest::TestRemoveAllRanges()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<medVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<medAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MED_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Test SET
  int resultDelete;
  int startSlice,endSlice;
  double threshold;
  attribute->AddRange(0,10,5.0);
  attribute->AddRange(12,22,10.0);
  attribute->RemoveAllRanges();
  resultDelete = attribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  resultDelete = attribute->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  medAttributeSegmentationVolume *restoredAttribute = medAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  resultDelete = restoredAttribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  resultDelete = restoredAttribute->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void medAttributeSegmentationVolumeTest::TestGetNumberOfRanges()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<medVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<medAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MED_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Test SET
  int resultDelete;
  int startSlice,endSlice;
  double threshold;
  attribute->AddRange(0,10,5.0);
  attribute->AddRange(12,22,10.0);
  CPPUNIT_ASSERT( attribute->GetNumberOfRanges() == 2 );
  attribute->RemoveAllRanges();
  CPPUNIT_ASSERT( attribute->GetNumberOfRanges() == 0 );
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  medAttributeSegmentationVolume *restoredAttribute = medAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  CPPUNIT_ASSERT( restoredAttribute->GetNumberOfRanges() == 0 );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
