/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEItemTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include "albaVMEItemTest.h"
#include "albaVMEItem.h"
#include "albaSmartPointer.h"
#include "albaVMEItemVTK.h"
#include "albaTagArray.h"

#include "vtkPolyData.h"
#include "vtkDataSet.h"
#include "vtkALBASmartPointer.h"
#include "vtkSphereSource.h"

enum VME_ITEM_WRITE_MODALITIY {DEFAULT,TMP_FILE,MEMORY};

/** albaVMEItemDummy 
Real class implemented to test albaVMEItem (abstract class).
*/
//------------------------------------------------------------------------
class albaVMEItemDummy : public albaVMEItem
//------------------------------------------------------------------------
{
public:

  albaTypeMacro(albaVMEItemDummy, albaVMEItem);

  // Redefine pure virtual functions.
  void ReleaseData(){return;};
  const char * GetDataFileExtension(){return NULL;};
  void ReleaseOutputMemory(){return;};
  void GetOutputMemory(const char *&out_str, int &size){return;};
  bool IsDataPresent(){return false;};
  void UpdateBounds(){return;};
  void UpdateData(){return;};
  int ReadData(albaString &filename, int resolvedURL = ALBA_OK){return ALBA_ERROR;};
  bool StoreToArchive(wxZipOutputStream &zip){return false;};

  int GetIOMode(){return m_IOMode;}; // Redefined in order to test SetIOMode()

protected:

  int CheckFile(const char *input_string, int input_len){return 0;};
  int CheckFile(const char *filename){return 0;};
  int InternalRestoreData(){return 0;};
  int InternalStoreData(const char *url){return 0;}; 
};

albaCxxTypeMacro(albaVMEItemDummy);

//------------------------------------------------------------------------
void albaVMEItemTest::TestFixture()
//------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------
void albaVMEItemTest::SetGetTimeStampTest()
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item;
  albaTimeStamp t = 1.1;
  item->SetTimeStamp(t);
  CPPUNIT_ASSERT(item->GetTimeStamp() == t);
}
//------------------------------------------------------------------------
void albaVMEItemTest::DeepCopyTest()
//------------------------------------------------------------------------
{
   albaSmartPointer<albaVMEItemDummy> item;
   item->SetDataType("vtkPolydata");
   item->SetTimeStamp(albaTimeStamp(.1));

  albaSmartPointer<albaVMEItemDummy> itemCpy;
  itemCpy->DeepCopy(item);

  CPPUNIT_ASSERT(itemCpy->Equals(item));
}
//------------------------------------------------------------------------
void albaVMEItemTest::ShallowCopyTest()
//------------------------------------------------------------------------
{
  // the same of deep copy
  albaSmartPointer<albaVMEItemDummy> item;
  item->SetDataType("vtkPolydata");
  item->SetTimeStamp(albaTimeStamp(.1));

  albaSmartPointer<albaVMEItemDummy> itemCpy;
  itemCpy->DeepCopy(item);

  CPPUNIT_ASSERT(itemCpy->Equals(item));
}
//------------------------------------------------------------------------
void albaVMEItemTest::IsDataModifiedTest()
//------------------------------------------------------------------------
{
//   albaSmartPointer<albaVMEItemDummy> item;
//   item->SetDataModified(true); Protected method!
//   CPPUNIT_ASSERT(item->IsDataModified() == true);

  // workaround can test with DeepCopy!
  albaSmartPointer<albaVMEItemDummy> item;
  albaSmartPointer<albaVMEItemDummy> itemCpy;
  itemCpy->DeepCopy(item); // This will set data modified on true!

  CPPUNIT_ASSERT(itemCpy->IsDataModified() == true);
}
//------------------------------------------------------------------------
void albaVMEItemTest::SetGetURLTest()
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item;
  albaString url = "c:\\fakeURL";
  item->SetURL(url);
  CPPUNIT_ASSERT(url.Equals(item->GetURL()));
}
//------------------------------------------------------------------------
void albaVMEItemTest::SetGetIOModeTest() // Can test this?
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item;

  item->SetIOMode(MEMORY);
  CPPUNIT_ASSERT(item->GetIOMode() == MEMORY);

  item->SetIOModeToDefault();
  CPPUNIT_ASSERT(item->GetIOMode() == DEFAULT);

  item->SetIOModeToMemory();
  CPPUNIT_ASSERT(item->GetIOMode() == MEMORY);

  item->SetIOModeToTmpFile();
  CPPUNIT_ASSERT(item->GetIOMode() == TMP_FILE);
}
//------------------------------------------------------------------------
void albaVMEItemTest::ExtractFileFromArchiveTest()
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item;

  albaString zipFileName = ALBA_DATA_ROOT;
  zipFileName << "/Test_Item/item.zvtk";

  albaString fileName = "item.vtk"; // = ALBA_DATA_ROOT;
  //fileName <<  "";

  CPPUNIT_ASSERT(item->ExtractFileFromArchive(zipFileName,fileName) == ALBA_OK);

  CPPUNIT_ASSERT(item->GetInputMemorySize() > 0);
  CPPUNIT_ASSERT(NULL != item->GetInputMemory());

  delete item->GetInputMemory(); // Need to be done manually
}
//------------------------------------------------------------------------
void albaVMEItemTest::SetGetInputMemoryTest()
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item;
  char memory[10];
  item->SetInputMemory(memory,10);
  CPPUNIT_ASSERT(item->GetInputMemory() == memory);
  CPPUNIT_ASSERT(item->GetInputMemorySize() == 10);
}
//------------------------------------------------------------------------
void albaVMEItemTest::GetTagArrayTest()
//------------------------------------------------------------------------
{
  // workaround: can test with a DeepCopy!
  // Create an object that is a real implementation of albaVMEItem (albaVMEItemVTK)
  albaSmartPointer<albaVMEItemVTK> itemVTK;

  vtkALBASmartPointer<vtkSphereSource> sphere;

  itemVTK->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  albaSmartPointer<albaVMEItemDummy> item;
  item->DeepCopy(itemVTK); // this will copy the tag array!

  // Assume that GetTagArray() method of albaVMEItemVTK works properly
  CPPUNIT_ASSERT(itemVTK->GetTagArray()->Equals(item->GetTagArray()));

}
//------------------------------------------------------------------------
void albaVMEItemTest::SetGetDataTypeTest()
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item;
  item->SetDataType("vtkPolyData");
  CPPUNIT_ASSERT(albaString::Equals(item->GetDataType(),"vtkPolyData"));
}
//------------------------------------------------------------------------
void albaVMEItemTest::SetGetIdTest()
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item;
  albaID id = 2;
  item->SetId(id);
  CPPUNIT_ASSERT(item->GetId() == id);
}
//------------------------------------------------------------------------
void albaVMEItemTest::EqualsTest()
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item1;
  albaSmartPointer<albaVMEItemDummy> item2;

  item1->SetDataType("vtkPolydata");
  item1->SetTimeStamp(albaTimeStamp(.1));

  // Both data type and time stamp different
  item2->SetDataType("vtkRectilinearGrid");
  item2->SetTimeStamp(albaTimeStamp(.2));
  CPPUNIT_ASSERT(!item2->Equals(item1));

  // Different data type
  item2->SetDataType("vtkRectilinearGrid");
  item2->SetTimeStamp(albaTimeStamp(.1));
  CPPUNIT_ASSERT(!item2->Equals(item1));

  // Different time stamp
  item2->SetDataType("vtkPolydata");
  item2->SetTimeStamp(albaTimeStamp(.2));
  CPPUNIT_ASSERT(!item2->Equals(item1));

  // Both equals
  item2->SetDataType("vtkPolydata");
  item2->SetTimeStamp(albaTimeStamp(.1));
  CPPUNIT_ASSERT(item2->Equals(item1));
}
//------------------------------------------------------------------------
void albaVMEItemTest::SetGetGlobalCompareDataFlagTest()
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item;
  item->SetGlobalCompareDataFlag(true);
  CPPUNIT_ASSERT((*item->GetGlobalCompareDataFlag()) == true);

  item->GlobalCompareDataFlagOff();
  CPPUNIT_ASSERT((*item->GetGlobalCompareDataFlag()) == false);

  item->GlobalCompareDataFlagOn();
  CPPUNIT_ASSERT((*item->GetGlobalCompareDataFlag()) == true);
}
// //------------------------------------------------------------------------
// void albaVMEItemTest::GetBoundsTets() // Cannot test
// //------------------------------------------------------------------------
// {
// }
// //------------------------------------------------------------------------
// void albaVMEItemTest::IsWaitingDataTest() // Cannot test
// //------------------------------------------------------------------------
// {
// }
// //------------------------------------------------------------------------
// void albaVMEItemTest::GetUpdateTimeTest() // Cannot test
// //------------------------------------------------------------------------
// {
// }
//------------------------------------------------------------------------
void albaVMEItemTest::SetGetCryptingTest()
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item;
  item->SetCrypting(true);
  CPPUNIT_ASSERT(item->GetCrypting() == true);
}
//------------------------------------------------------------------------
void albaVMEItemTest::SetGetArchiveFileNameTest()
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item;
  albaString fileName = "fakeArchive.zip";
  item->SetArchiveFileName(fileName);
  CPPUNIT_ASSERT(fileName.Equals(item->GetArchiveFileName()));
}
//------------------------------------------------------------------------
void albaVMEItemTest::SetGetTempFileNameTest()
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item;
  albaString fileName = "fakeTmpFile.zip";
  item->SetTempFileName(fileName);
  CPPUNIT_ASSERT(fileName.Equals(item->GetTempFileName()));
}
// //------------------------------------------------------------------------
// void albaVMEItemTest::UpdateItemIdTest() // Cannot test
// //------------------------------------------------------------------------
// {
// }
//------------------------------------------------------------------------
void albaVMEItemTest::PrintTest()
//------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEItemDummy> item;
  item->Print(std::cout, 3);
}