/*=========================================================================

 Program: MAF2
 Module: mafVMEItemTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafVMEItemTest.h"
#include "mafVMEItem.h"
#include "mafSmartPointer.h"
#include "mafVMEItemVTK.h"
#include "mafTagArray.h"

#include "vtkPolyData.h"
#include "vtkDataSet.h"
#include "vtkMAFSmartPointer.h"
#include "vtkSphereSource.h"

enum VME_ITEM_WRITE_MODALITIY {DEFAULT,TMP_FILE,MEMORY};

/** mafVMEItemDummy 
Real class implemented to test mafVMEItem (abstract class).
*/
//------------------------------------------------------------------------
class mafVMEItemDummy : public mafVMEItem
//------------------------------------------------------------------------
{
public:

  mafTypeMacro(mafVMEItemDummy, mafVMEItem);

  // Redefine pure virtual functions.
  void ReleaseData(){return;};
  const char * GetDataFileExtension(){return NULL;};
  void ReleaseOutputMemory(){return;};
  void GetOutputMemory(const char *&out_str, int &size){return;};
  bool IsDataPresent(){return false;};
  void UpdateBounds(){return;};
  void UpdateData(){return;};
  int ReadData(mafString &filename, int resolvedURL = MAF_OK){return MAF_ERROR;};
  bool StoreToArchive(wxZipOutputStream &zip){return false;};

  int GetIOMode(){return m_IOMode;}; // Redefined in order to test SetIOMode()

protected:

  int CheckFile(const char *input_string, int input_len){return 0;};
  int CheckFile(const char *filename){return 0;};
  int InternalRestoreData(){return 0;};
  int InternalStoreData(const char *url){return 0;}; 
};

mafCxxTypeMacro(mafVMEItemDummy);

//------------------------------------------------------------------------
void mafVMEItemTest::setUp()
//------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------
void mafVMEItemTest::tearDown()
//------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------
void mafVMEItemTest::TestFixture()
//------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------
void mafVMEItemTest::SetGetTimeStampTest()
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item;
  mafTimeStamp t = 1.1;
  item->SetTimeStamp(t);
  CPPUNIT_ASSERT(item->GetTimeStamp() == t);
}
//------------------------------------------------------------------------
void mafVMEItemTest::DeepCopyTest()
//------------------------------------------------------------------------
{
   mafSmartPointer<mafVMEItemDummy> item;
   item->SetDataType("vtkPolydata");
   item->SetTimeStamp(mafTimeStamp(.1));

  mafSmartPointer<mafVMEItemDummy> itemCpy;
  itemCpy->DeepCopy(item);

  CPPUNIT_ASSERT(itemCpy->Equals(item));
}
//------------------------------------------------------------------------
void mafVMEItemTest::ShallowCopyTest()
//------------------------------------------------------------------------
{
  // the same of deep copy
  mafSmartPointer<mafVMEItemDummy> item;
  item->SetDataType("vtkPolydata");
  item->SetTimeStamp(mafTimeStamp(.1));

  mafSmartPointer<mafVMEItemDummy> itemCpy;
  itemCpy->DeepCopy(item);

  CPPUNIT_ASSERT(itemCpy->Equals(item));
}
//------------------------------------------------------------------------
void mafVMEItemTest::IsDataModifiedTest()
//------------------------------------------------------------------------
{
//   mafSmartPointer<mafVMEItemDummy> item;
//   item->SetDataModified(true); Protected method!
//   CPPUNIT_ASSERT(item->IsDataModified() == true);

  // workaround can test with DeepCopy!
  mafSmartPointer<mafVMEItemDummy> item;
  mafSmartPointer<mafVMEItemDummy> itemCpy;
  itemCpy->DeepCopy(item); // This will set data modified on true!

  CPPUNIT_ASSERT(itemCpy->IsDataModified() == true);
}
//------------------------------------------------------------------------
void mafVMEItemTest::SetGetURLTest()
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item;
  mafString url = "c:\\fakeURL";
  item->SetURL(url);
  CPPUNIT_ASSERT(url.Equals(item->GetURL()));
}
//------------------------------------------------------------------------
void mafVMEItemTest::SetGetIOModeTest() // Can test this?
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item;

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
void mafVMEItemTest::ExtractFileFromArchiveTest()
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item;

  mafString zipFileName = MAF_DATA_ROOT;
  zipFileName << "/Test_Item/item.zvtk";

  mafString fileName = "item.vtk"; // = MAF_DATA_ROOT;
  //fileName <<  "";

  CPPUNIT_ASSERT(item->ExtractFileFromArchive(zipFileName,fileName) == MAF_OK);

  CPPUNIT_ASSERT(item->GetInputMemorySize() > 0);
  CPPUNIT_ASSERT(NULL != item->GetInputMemory());

  delete item->GetInputMemory(); // Need to be done manually
}
//------------------------------------------------------------------------
void mafVMEItemTest::SetGetInputMemoryTest()
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item;
  char memory[10];
  item->SetInputMemory(memory,10);
  CPPUNIT_ASSERT(item->GetInputMemory() == memory);
  CPPUNIT_ASSERT(item->GetInputMemorySize() == 10);
}
//------------------------------------------------------------------------
void mafVMEItemTest::GetTagArrayTest()
//------------------------------------------------------------------------
{
  // workaround: can test with a DeepCopy!
  // Create an object that is a real implementation of mafVMEItem (mafVMEItemVTK)
  mafSmartPointer<mafVMEItemVTK> itemVTK;

  vtkMAFSmartPointer<vtkSphereSource> sphere;

  itemVTK->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  mafSmartPointer<mafVMEItemDummy> item;
  item->DeepCopy(itemVTK); // this will copy the tag array!

  // Assume that GetTagArray() method of mafVMEItemVTK works properly
  CPPUNIT_ASSERT(itemVTK->GetTagArray()->Equals(item->GetTagArray()));

}
//------------------------------------------------------------------------
void mafVMEItemTest::SetGetDataTypeTest()
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item;
  item->SetDataType("vtkPolyData");
  CPPUNIT_ASSERT(mafString::Equals(item->GetDataType(),"vtkPolyData"));
}
//------------------------------------------------------------------------
void mafVMEItemTest::SetGetIdTest()
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item;
  mafID id = 2;
  item->SetId(id);
  CPPUNIT_ASSERT(item->GetId() == id);
}
//------------------------------------------------------------------------
void mafVMEItemTest::EqualsTest()
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item1;
  mafSmartPointer<mafVMEItemDummy> item2;

  item1->SetDataType("vtkPolydata");
  item1->SetTimeStamp(mafTimeStamp(.1));

  // Both data type and time stamp different
  item2->SetDataType("vtkRectilinearGrid");
  item2->SetTimeStamp(mafTimeStamp(.2));
  CPPUNIT_ASSERT(!item2->Equals(item1));

  // Different data type
  item2->SetDataType("vtkRectilinearGrid");
  item2->SetTimeStamp(mafTimeStamp(.1));
  CPPUNIT_ASSERT(!item2->Equals(item1));

  // Different time stamp
  item2->SetDataType("vtkPolydata");
  item2->SetTimeStamp(mafTimeStamp(.2));
  CPPUNIT_ASSERT(!item2->Equals(item1));

  // Both equals
  item2->SetDataType("vtkPolydata");
  item2->SetTimeStamp(mafTimeStamp(.1));
  CPPUNIT_ASSERT(item2->Equals(item1));
}
//------------------------------------------------------------------------
void mafVMEItemTest::SetGetGlobalCompareDataFlagTest()
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item;
  item->SetGlobalCompareDataFlag(true);
  CPPUNIT_ASSERT((*item->GetGlobalCompareDataFlag()) == true);

  item->GlobalCompareDataFlagOff();
  CPPUNIT_ASSERT((*item->GetGlobalCompareDataFlag()) == false);

  item->GlobalCompareDataFlagOn();
  CPPUNIT_ASSERT((*item->GetGlobalCompareDataFlag()) == true);
}
// //------------------------------------------------------------------------
// void mafVMEItemTest::GetBoundsTets() // Cannot test
// //------------------------------------------------------------------------
// {
// }
// //------------------------------------------------------------------------
// void mafVMEItemTest::IsWaitingDataTest() // Cannot test
// //------------------------------------------------------------------------
// {
// }
// //------------------------------------------------------------------------
// void mafVMEItemTest::GetUpdateTimeTest() // Cannot test
// //------------------------------------------------------------------------
// {
// }
//------------------------------------------------------------------------
void mafVMEItemTest::SetGetCryptingTest()
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item;
  item->SetCrypting(true);
  CPPUNIT_ASSERT(item->GetCrypting() == true);
}
//------------------------------------------------------------------------
void mafVMEItemTest::SetGetArchiveFileNameTest()
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item;
  mafString fileName = "fakeArchive.zip";
  item->SetArchiveFileName(fileName);
  CPPUNIT_ASSERT(fileName.Equals(item->GetArchiveFileName()));
}
//------------------------------------------------------------------------
void mafVMEItemTest::SetGetTempFileNameTest()
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item;
  mafString fileName = "fakeTmpFile.zip";
  item->SetTempFileName(fileName);
  CPPUNIT_ASSERT(fileName.Equals(item->GetTempFileName()));
}
// //------------------------------------------------------------------------
// void mafVMEItemTest::UpdateItemIdTest() // Cannot test
// //------------------------------------------------------------------------
// {
// }
//------------------------------------------------------------------------
void mafVMEItemTest::PrintTest()
//------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemDummy> item;
  item->Print(std::cout, 3);
}