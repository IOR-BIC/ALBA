/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttributeSegmentationVolumeTest
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
#include "albaAttributeSegmentationVolumeTest.h"
#include "albaAttributeSegmentationVolume.h"
#include "albaVMESegmentationVolume.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEFactory.h"

//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestDynamicAllocation()
{
  albaAttributeSegmentationVolume *attribute1 = NULL;
  albaNEW(attribute1);
  albaDEL(attribute1);

  albaSmartPointer<albaAttributeSegmentationVolume> attribute2;
}
//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestSetAutomaticSegmentationThresholdModality()
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
	storage->SetURL(fileName);
	//////////////////////////////////////////////////////////////////////////
	//Default value
	CPPUNIT_ASSERT(attribute->GetAutomaticSegmentationThresholdModality() == albaVMESegmentationVolume::GLOBAL);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Test SET
	attribute->SetAutomaticSegmentationThresholdModality(albaVMESegmentationVolume::RANGE);
	CPPUNIT_ASSERT(attribute->GetAutomaticSegmentationThresholdModality() == albaVMESegmentationVolume::RANGE);
	attribute->SetAutomaticSegmentationThresholdModality(albaVMESegmentationVolume::GLOBAL);
	CPPUNIT_ASSERT(attribute->GetAutomaticSegmentationThresholdModality() == albaVMESegmentationVolume::GLOBAL);
	attribute->SetAutomaticSegmentationThresholdModality(albaVMESegmentationVolume::RANGE);
	CPPUNIT_ASSERT(attribute->GetAutomaticSegmentationThresholdModality() == albaVMESegmentationVolume::RANGE);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	CPPUNIT_ASSERT(restoredAttribute->GetAutomaticSegmentationThresholdModality() == albaVMESegmentationVolume::RANGE);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}
//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestSetAutomaticSegmentationGlobalThreshold()
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
	storage->SetURL(fileName);

	//////////////////////////////////////////////////////////////////////////
	//Default value
	CPPUNIT_ASSERT(attribute->GetAutomaticSegmentationGlobalThreshold() == 0.0);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Test SET
	attribute->SetAutomaticSegmentationGlobalThreshold(-100.0);
	CPPUNIT_ASSERT(attribute->GetAutomaticSegmentationGlobalThreshold() == -100.0);
	attribute->SetAutomaticSegmentationGlobalThreshold(100.0);
	CPPUNIT_ASSERT(attribute->GetAutomaticSegmentationGlobalThreshold() == 100.0);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	CPPUNIT_ASSERT(restoredAttribute->GetAutomaticSegmentationGlobalThreshold() == 100.0);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}
//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestAddRange()
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
	storage->SetURL(fileName);

	//////////////////////////////////////////////////////////////////////////
	//Test SET
	int resultAddGet;
	int startSlice, endSlice;
	double threshold;
	resultAddGet = attribute->AddRange(0, 10, 5.0);
	CPPUNIT_ASSERT(resultAddGet == ALBA_OK);
	resultAddGet = attribute->AddRange(12, 22, 10.0);
	CPPUNIT_ASSERT(resultAddGet == ALBA_OK);
	resultAddGet = attribute->GetRange(0, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(resultAddGet == ALBA_OK);
	CPPUNIT_ASSERT(startSlice == 0 && endSlice == 10 && threshold == 5.0);
	resultAddGet = attribute->GetRange(1, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(resultAddGet == ALBA_OK);
	CPPUNIT_ASSERT(startSlice == 12 && endSlice == 22 && threshold == 10.0);
	resultAddGet = attribute->GetRange(2, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(resultAddGet == ALBA_ERROR);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	restoredAttribute->GetRange(0, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(startSlice == 0 && endSlice == 10 && threshold == 5.0);
	restoredAttribute->GetRange(1, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(startSlice == 12 && endSlice == 22 && threshold == 10.0);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}
//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestUpdateRange()
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
	storage->SetURL(fileName);

	//////////////////////////////////////////////////////////////////////////
	//Test SET
	int resultUpdate;
	int startSlice, endSlice;
	double threshold;
	attribute->AddRange(0, 10, 5.0);
	attribute->AddRange(12, 22, 10.0);
	attribute->GetRange(0, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(startSlice == 0 && endSlice == 10 && threshold == 5.0);
	attribute->GetRange(1, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(startSlice == 12 && endSlice == 22 && threshold == 10.0);
	resultUpdate = attribute->UpdateRange(0, 6, 7, 2.0);
	CPPUNIT_ASSERT(resultUpdate == ALBA_OK);
	attribute->GetRange(0, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(startSlice == 6 && endSlice == 7 && threshold == 2.0);
	resultUpdate = attribute->UpdateRange(2, 6, 7, 2.0);
	CPPUNIT_ASSERT(resultUpdate == ALBA_ERROR);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	restoredAttribute->GetRange(0, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(startSlice == 6 && endSlice == 7 && threshold == 2.0);
	restoredAttribute->GetRange(1, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(startSlice == 12 && endSlice == 22 && threshold == 10.0);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}
//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestDeleteRange()
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
	storage->SetURL(fileName);

	//////////////////////////////////////////////////////////////////////////
	//Test SET
	int resultDelete;
	int startSlice, endSlice;
	double threshold;
	attribute->AddRange(0, 10, 5.0);
	attribute->AddRange(12, 22, 10.0);
	resultDelete = attribute->DeleteRange(0);
	CPPUNIT_ASSERT(resultDelete == ALBA_OK);
	attribute->GetRange(0, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(startSlice == 12 && endSlice == 22 && threshold == 10.0);
	resultDelete = attribute->GetRange(1, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(resultDelete == ALBA_ERROR);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	restoredAttribute->GetRange(0, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(startSlice == 12 && endSlice == 22 && threshold == 10.0);
	resultDelete = restoredAttribute->GetRange(1, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(resultDelete == ALBA_ERROR);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}
//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestRemoveAllRanges()
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
	storage->SetURL(fileName);

	//////////////////////////////////////////////////////////////////////////
	//Test SET
	int resultDelete;
	int startSlice, endSlice;
	double threshold;
	attribute->AddRange(0, 10, 5.0);
	attribute->AddRange(12, 22, 10.0);
	attribute->RemoveAllRanges();
	resultDelete = attribute->GetRange(0, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(resultDelete == ALBA_ERROR);
	resultDelete = attribute->GetRange(1, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(resultDelete == ALBA_ERROR);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	resultDelete = restoredAttribute->GetRange(0, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(resultDelete == ALBA_ERROR);
	resultDelete = restoredAttribute->GetRange(1, startSlice, endSlice, threshold);
	CPPUNIT_ASSERT(resultDelete == ALBA_ERROR);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}
//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestGetNumberOfRanges()
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
	storage->SetURL(fileName);

	//////////////////////////////////////////////////////////////////////////
	//Test SET
	int startSlice, endSlice;
	double threshold;
	attribute->AddRange(0, 10, 5.0);
	attribute->AddRange(12, 22, 10.0);
	CPPUNIT_ASSERT(attribute->GetNumberOfRanges() == 2);
	attribute->RemoveAllRanges();
	CPPUNIT_ASSERT(attribute->GetNumberOfRanges() == 0);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	CPPUNIT_ASSERT(restoredAttribute->GetNumberOfRanges() == 0);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}
//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestSetRegionGrowingUpperThreshold()
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
	storage->SetURL(fileName);
	//////////////////////////////////////////////////////////////////////////
	//Default value
	CPPUNIT_ASSERT(attribute->GetRegionGrowingUpperThreshold() == 0.0);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Test SET
	attribute->SetRegionGrowingUpperThreshold(10.0);
	CPPUNIT_ASSERT(attribute->GetRegionGrowingUpperThreshold() == 10.0);
	attribute->SetRegionGrowingUpperThreshold(-10.0);
	CPPUNIT_ASSERT(attribute->GetRegionGrowingUpperThreshold() == -10.0);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	CPPUNIT_ASSERT(attribute->GetRegionGrowingUpperThreshold() == -10.0);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}
//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestSetRegionGrowingLowerThreshold()
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
	storage->SetURL(fileName);
	//////////////////////////////////////////////////////////////////////////
	//Default value
	CPPUNIT_ASSERT(attribute->GetRegionGrowingLowerThreshold() == 0.0);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Test SET
	attribute->SetRegionGrowingLowerThreshold(10.0);
	CPPUNIT_ASSERT(attribute->GetRegionGrowingLowerThreshold() == 10.0);
	attribute->SetRegionGrowingLowerThreshold(-10.0);
	CPPUNIT_ASSERT(attribute->GetRegionGrowingLowerThreshold() == -10.0);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	CPPUNIT_ASSERT(attribute->GetRegionGrowingLowerThreshold() == -10.0);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}
//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestAddSeed()
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
	storage->SetURL(fileName);

	//////////////////////////////////////////////////////////////////////////
	//Test SET
	int resultAddGet;
	int seed[3];
	seed[0] = 0;
	seed[1] = 1;
	seed[2] = 2;
	resultAddGet = attribute->AddSeed(seed);
	CPPUNIT_ASSERT(resultAddGet == ALBA_OK);
	seed[0] = 3;
	seed[1] = 4;
	seed[2] = 5;
	resultAddGet = attribute->AddSeed(seed);
	CPPUNIT_ASSERT(resultAddGet == ALBA_OK);
	resultAddGet = attribute->GetSeed(0, seed);
	CPPUNIT_ASSERT(resultAddGet == ALBA_OK);
	CPPUNIT_ASSERT(seed[0] == 0 && seed[1] == 1 && seed[2] == 2);
	resultAddGet = attribute->GetSeed(1, seed);
	CPPUNIT_ASSERT(resultAddGet == ALBA_OK);
	CPPUNIT_ASSERT(seed[0] == 3 && seed[1] == 4 && seed[2] == 5);
	resultAddGet = attribute->GetSeed(2, seed);
	CPPUNIT_ASSERT(resultAddGet == ALBA_ERROR);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	resultAddGet = restoredAttribute->GetSeed(0, seed);
	CPPUNIT_ASSERT(resultAddGet == ALBA_OK);
	CPPUNIT_ASSERT(seed[0] == 0 && seed[1] == 1 && seed[2] == 2);
	resultAddGet = restoredAttribute->GetSeed(1, seed);
	CPPUNIT_ASSERT(resultAddGet == ALBA_OK);
	CPPUNIT_ASSERT(seed[0] == 3 && seed[1] == 4 && seed[2] == 5);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}

//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestDeleteSeed()
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
	storage->SetURL(fileName);

	//////////////////////////////////////////////////////////////////////////
	//Test SET
	int resultDelete;
	int seed[3];
	seed[0] = 0;
	seed[1] = 1;
	seed[2] = 2;
	resultDelete = attribute->AddSeed(seed);
	CPPUNIT_ASSERT(resultDelete == ALBA_OK);
	seed[0] = 3;
	seed[1] = 4;
	seed[2] = 5;
	resultDelete = attribute->AddSeed(seed);
	resultDelete = attribute->DeleteSeed(0);
	CPPUNIT_ASSERT(resultDelete == ALBA_OK);
	resultDelete = attribute->GetSeed(0, seed);
	CPPUNIT_ASSERT(resultDelete == ALBA_OK);
	CPPUNIT_ASSERT(seed[0] == 3 && seed[1] == 4 && seed[2] == 5);
	resultDelete = attribute->GetSeed(1, seed);
	CPPUNIT_ASSERT(resultDelete == ALBA_ERROR);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	resultDelete = attribute->GetSeed(0, seed);
	CPPUNIT_ASSERT(resultDelete == ALBA_OK);
	CPPUNIT_ASSERT(seed[0] == 3 && seed[1] == 4 && seed[2] == 5);
	resultDelete = attribute->GetSeed(1, seed);
	CPPUNIT_ASSERT(resultDelete == ALBA_ERROR);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}

//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestGetNumberOfSeeds()
//---------------------------------------------------------
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
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
	CPPUNIT_ASSERT(attribute->GetNumberOfSeeds() == 2);
	attribute->RemoveAllSeeds();
	CPPUNIT_ASSERT(attribute->GetNumberOfSeeds() == 0);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	CPPUNIT_ASSERT(restoredAttribute->GetNumberOfSeeds() == 0);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}

//---------------------------------------------------------
void albaAttributeSegmentationVolumeTest::TestRemoveAllSeeds()
{
	albaVMEFactory::Initialize();
	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();
	albaSmartPointer<albaVMESegmentationVolume> volume;
	storage->GetRoot()->AddChild(volume);
	volume->ReparentTo(storage->GetRoot());
	albaSmartPointer<albaAttributeSegmentationVolume> attribute;
	volume->SetAttribute("SegmentationVolumeData", attribute);
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "/SavedMSF.msf";
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
	resultDelete = attribute->GetSeed(0, seed);
	CPPUNIT_ASSERT(resultDelete == ALBA_ERROR);
	resultDelete = attribute->GetSeed(1, seed);
	CPPUNIT_ASSERT(resultDelete == ALBA_ERROR);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Check store in the msf
	int resultStoreRestore;
	resultStoreRestore = storage->Store();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	resultStoreRestore = storage->Restore();
	CPPUNIT_ASSERT(resultStoreRestore == ALBA_OK);
	albaAttributeSegmentationVolume *restoredAttribute = albaAttributeSegmentationVolume::SafeDownCast(storage->GetRoot()->GetFirstChild()->GetAttribute("SegmentationVolumeData"));
	CPPUNIT_ASSERT(restoredAttribute);
	resultDelete = restoredAttribute->GetSeed(0, seed);
	CPPUNIT_ASSERT(resultDelete == ALBA_ERROR);
	resultDelete = restoredAttribute->GetSeed(1, seed);
	CPPUNIT_ASSERT(resultDelete == ALBA_ERROR);
	//////////////////////////////////////////////////////////////////////////
	albaDEL(storage);
}
