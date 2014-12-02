/*=========================================================================

 Program: MAF2
 Module: mafAttributeSegmentationVolumeTest
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
#include "mafAttributeSegmentationVolumeTest.h"
#include "mafAttributeSegmentationVolume.h"
#include "mafVMESegmentationVolume.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "medVMEFactory.h"

//----------------------------------------------------------------------------
void mafAttributeSegmentationVolumeTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafAttributeSegmentationVolumeTest::tearDown()
//----------------------------------------------------------------------------
{
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  mafAttributeSegmentationVolume *attribute1 = NULL;
  mafNEW(attribute1);
  mafDEL(attribute1);

  mafSmartPointer<mafAttributeSegmentationVolume> attribute2;
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestSetAutomaticSegmentationThresholdModality()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Default value
  CPPUNIT_ASSERT( attribute->GetAutomaticSegmentationThresholdModality() ==  mafVMESegmentationVolume::GLOBAL);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Test SET
  attribute->SetAutomaticSegmentationThresholdModality(mafVMESegmentationVolume::RANGE);
  CPPUNIT_ASSERT( attribute->GetAutomaticSegmentationThresholdModality() ==  mafVMESegmentationVolume::RANGE);
  attribute->SetAutomaticSegmentationThresholdModality(mafVMESegmentationVolume::GLOBAL);
  CPPUNIT_ASSERT( attribute->GetAutomaticSegmentationThresholdModality() ==  mafVMESegmentationVolume::GLOBAL);
  attribute->SetAutomaticSegmentationThresholdModality(mafVMESegmentationVolume::RANGE);
  CPPUNIT_ASSERT( attribute->GetAutomaticSegmentationThresholdModality() ==  mafVMESegmentationVolume::RANGE);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  CPPUNIT_ASSERT( restoredAttribute->GetAutomaticSegmentationThresholdModality() == mafVMESegmentationVolume::RANGE );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestSetAutomaticSegmentationGlobalThreshold()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
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
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  CPPUNIT_ASSERT( restoredAttribute->GetAutomaticSegmentationGlobalThreshold() == 100.0 );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestAddRange()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
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
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  restoredAttribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 0 && endSlice == 10 && threshold == 5.0 );
  restoredAttribute->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 12 && endSlice == 22 && threshold == 10.0 );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestUpdateRange()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
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
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  restoredAttribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 6 && endSlice == 7 && threshold == 2.0 );
  restoredAttribute->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 12 && endSlice == 22 && threshold == 10.0 );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestDeleteRange()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
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
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  restoredAttribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( startSlice == 12 && endSlice == 22 && threshold == 10.0 );
  resultDelete = restoredAttribute->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestRemoveAllRanges()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
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
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  resultDelete = restoredAttribute->GetRange(0,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  resultDelete = restoredAttribute->GetRange(1,startSlice,endSlice,threshold);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestGetNumberOfRanges()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Test SET
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
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  CPPUNIT_ASSERT( restoredAttribute->GetNumberOfRanges() == 0 );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestSetRegionGrowingUpperThreshold()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Default value
  CPPUNIT_ASSERT( attribute->GetRegionGrowingUpperThreshold() ==  0.0);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Test SET
  attribute->SetRegionGrowingUpperThreshold(10.0);
  CPPUNIT_ASSERT( attribute->GetRegionGrowingUpperThreshold() == 10.0 );
  attribute->SetRegionGrowingUpperThreshold(-10.0);
  CPPUNIT_ASSERT( attribute->GetRegionGrowingUpperThreshold() == -10.0 );
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  CPPUNIT_ASSERT( attribute->GetRegionGrowingUpperThreshold() == -10.0 );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestSetRegionGrowingLowerThreshold()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Default value
  CPPUNIT_ASSERT( attribute->GetRegionGrowingLowerThreshold() ==  0.0);
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Test SET
  attribute->SetRegionGrowingLowerThreshold(10.0);
  CPPUNIT_ASSERT( attribute->GetRegionGrowingLowerThreshold() == 10.0 );
  attribute->SetRegionGrowingLowerThreshold(-10.0);
  CPPUNIT_ASSERT( attribute->GetRegionGrowingLowerThreshold() == -10.0 );
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  CPPUNIT_ASSERT( attribute->GetRegionGrowingLowerThreshold() == -10.0 );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestAddSeed()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Test SET
  int resultAddGet;
  int seed[3];
  seed[0] = 0;
  seed[1] = 1;
  seed[2] = 2;
  resultAddGet = attribute->AddSeed(seed);
  CPPUNIT_ASSERT( resultAddGet == MAF_OK );
  seed[0] = 3;
  seed[1] = 4;
  seed[2] = 5;
  resultAddGet = attribute->AddSeed(seed);
  CPPUNIT_ASSERT( resultAddGet == MAF_OK );
  resultAddGet = attribute->GetSeed(0,seed);
  CPPUNIT_ASSERT( resultAddGet == MAF_OK );
  CPPUNIT_ASSERT( seed[0] == 0 && seed[1] == 1 && seed[2] == 2 );
  resultAddGet = attribute->GetSeed(1,seed);
  CPPUNIT_ASSERT( resultAddGet == MAF_OK );
  CPPUNIT_ASSERT( seed[0] == 3 && seed[1] == 4 && seed[2] == 5 );
  resultAddGet = attribute->GetSeed(2,seed);
  CPPUNIT_ASSERT( resultAddGet == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  resultAddGet = restoredAttribute->GetSeed(0,seed);
  CPPUNIT_ASSERT( resultAddGet == MAF_OK );
  CPPUNIT_ASSERT( seed[0] == 0 && seed[1] == 1 && seed[2] == 2 );
  resultAddGet = restoredAttribute->GetSeed(1,seed);
  CPPUNIT_ASSERT( resultAddGet == MAF_OK );
  CPPUNIT_ASSERT( seed[0] == 3 && seed[1] == 4 && seed[2] == 5 );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}

//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestDeleteSeed()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Test SET
  int resultDelete;
  int seed[3];
  seed[0] = 0;
  seed[1] = 1;
  seed[2] = 2;
  resultDelete = attribute->AddSeed(seed);
  CPPUNIT_ASSERT( resultDelete == MAF_OK );
  seed[0] = 3;
  seed[1] = 4;
  seed[2] = 5;
  resultDelete = attribute->AddSeed(seed);
  resultDelete = attribute->DeleteSeed(0);
  CPPUNIT_ASSERT( resultDelete == MAF_OK );
  resultDelete = attribute->GetSeed(0,seed);
  CPPUNIT_ASSERT( resultDelete == MAF_OK );
  CPPUNIT_ASSERT( seed[0] == 3 && seed[1] == 4 && seed[2] == 5 );
  resultDelete = attribute->GetSeed(1,seed);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  resultDelete = attribute->GetSeed(0,seed);
  CPPUNIT_ASSERT( resultDelete == MAF_OK );
  CPPUNIT_ASSERT( seed[0] == 3 && seed[1] == 4 && seed[2] == 5 );
  resultDelete = attribute->GetSeed(1,seed);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestGetNumberOfSeeds()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Test SET
  int seed[3];
  seed[0] = 0;
  seed[1] = 1;
  seed[2] = 2;
  attribute->AddSeed(seed);
  seed[0] = 3;
  seed[1] = 4;
  seed[2] = 5;
  attribute->AddSeed(seed);
  CPPUNIT_ASSERT( attribute->GetNumberOfSeeds() == 2 );
  attribute->RemoveAllSeeds();
  CPPUNIT_ASSERT( attribute->GetNumberOfSeeds() == 0 );
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  CPPUNIT_ASSERT( restoredAttribute->GetNumberOfSeeds() == 0 );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
//---------------------------------------------------------
void mafAttributeSegmentationVolumeTest::TestRemoveAllSeeds()
//---------------------------------------------------------
{
  medVMEFactory::Initialize();
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  mafSmartPointer<mafVMESegmentationVolume> volume;
  storage->GetRoot()->AddChild(volume);
  volume->SetParent(storage->GetRoot());
  mafSmartPointer<mafAttributeSegmentationVolume> attribute;
  volume->SetAttribute("SegmentationVolumeData",attribute);
  mafString fileName = MAF_DATA_ROOT;
  fileName << "/Test_AttributeSegmentationVolume/SavedMSF.msf";
  storage->SetURL(fileName);
  //////////////////////////////////////////////////////////////////////////
  //Test SET
  int seed[3];
  int resultDelete;
  seed[0] = 0;
  seed[1] = 1;
  seed[2] = 2;
  attribute->AddSeed(seed);
  seed[0] = 3;
  seed[1] = 4;
  seed[2] = 5;
  attribute->AddSeed(seed);
  attribute->RemoveAllSeeds();
  resultDelete = attribute->GetSeed(0,seed);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  resultDelete = attribute->GetSeed(1,seed);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //Check store in the msf
  int resultStoreRestore;
  resultStoreRestore = storage->Store();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  resultStoreRestore = storage->Restore();
  CPPUNIT_ASSERT( resultStoreRestore == MAF_OK );
  mafAttributeSegmentationVolume *restoredAttribute = mafAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
  CPPUNIT_ASSERT( restoredAttribute );
  resultDelete = restoredAttribute->GetSeed(0,seed);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  resultDelete = restoredAttribute->GetSeed(1,seed);
  CPPUNIT_ASSERT( resultDelete == MAF_ERROR );
  //////////////////////////////////////////////////////////////////////////
  mafDEL(storage);
}
