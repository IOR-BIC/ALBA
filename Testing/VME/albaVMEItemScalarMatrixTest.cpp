/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEItemScalarMatrixTest
 Authors: Alberto Losi
 
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
#include "albaVMEItemScalarMatrixTest.h"

#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "albaVMEItemScalarMatrix.h"
#include "albaString.h"
#include "albaVMEStorage.h"
#include "albaEventIO.h"
#include "albaObserver.h"
#include <string.h>

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
class DummyObserver : public albaObserver
//----------------------------------------------------------------------------
{
public:

  DummyObserver() 
  {
    m_Storage = new albaVMEStorage();
  };
  ~DummyObserver()
  {
    delete m_Storage;
  };

  albaStorage* GetStorage()
  {
    return m_Storage;
  };

  virtual void OnEvent(albaEventBase *alba_event)
  {
    if(alba_event->GetId() == NODE_GET_STORAGE)
    {
      albaEventIO *io_event=albaEventIO::SafeDownCast(alba_event);
      io_event->SetStorage(m_Storage);
    }
  }

protected:
  albaVMEStorage *m_Storage;
};
//----------------------------------------------------------------------------
void albaVMEItemScalarMatrixTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEItemScalarMatrixTest::TestSetData()
//----------------------------------------------------------------------------
{
  albaVMEItemScalarMatrix *item;
  albaNEW(item);
  vnl_matrix<double> data;

  data.set_size(3,3);
  data.fill((double)1.64);

  item->SetData(data);
  CPPUNIT_ASSERT(data == item->GetData());

  albaDEL(item);
}
//----------------------------------------------------------------------------
void albaVMEItemScalarMatrixTest::TestEquals()
//----------------------------------------------------------------------------
{
  albaVMEItemScalarMatrix *item1;
  albaVMEItemScalarMatrix *item2;
  albaNEW(item1);
  albaNEW(item2);
  vnl_matrix<double> data1;
  vnl_matrix<double> data2;

  //Different data but the same data type: Equals should return true
  data1.set_size(3,3);
  data1.fill((double)4.94);
  data2.set_size(3,3);
  data2.fill((double)1.32);

  item1->SetData(data1);
  item2->SetData(data2);

  CPPUNIT_ASSERT(item1->Equals(item2) && item2->Equals(item1));
  
  //The same data and the same data type: Equals should return true
  item2->SetData(data1);

  CPPUNIT_ASSERT(item1->Equals(item2) && item2->Equals(item1));

  item1->SetGlobalCompareDataFlag(true); //To force compare internal data
  item2->SetGlobalCompareDataFlag(true); //To force compare internal data

  //Different data but the same data type: Equals should return false
  item2->SetData(data2);

  CPPUNIT_ASSERT(!(item1->Equals(item2) && item2->Equals(item1)));

  //The same data and the same data type: Equals should return true
  item2->SetData(data1);

  CPPUNIT_ASSERT(item1->Equals(item2) && item2->Equals(item1));

  albaDEL(item1);
  albaDEL(item2);
}
//----------------------------------------------------------------------------
void albaVMEItemScalarMatrixTest::TestIsDataPresent()
//----------------------------------------------------------------------------
{
  albaVMEItemScalarMatrix *item;
  albaNEW(item);
  vnl_matrix<double> data;

  data.set_size(3,3);
  data.fill((double)3.28);

  CPPUNIT_ASSERT(!item->IsDataPresent());

  item->SetData(data);

  CPPUNIT_ASSERT(item->IsDataPresent());

  item->ReleaseData();

  CPPUNIT_ASSERT(!item->IsDataPresent());

  albaDEL(item);
}
//----------------------------------------------------------------------------
void albaVMEItemScalarMatrixTest::TestGetDataFileExtension()
//----------------------------------------------------------------------------
{
  albaVMEItemScalarMatrix *item;
  albaString ext = albaString("sca");
  albaNEW(item);

  CPPUNIT_ASSERT(ext.Equals(albaString(item->GetDataFileExtension())));

  albaDEL(item);
}
//----------------------------------------------------------------------------
void albaVMEItemScalarMatrixTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  albaVMEItemScalarMatrix *item1;
  albaVMEItemScalarMatrix *item2;
  albaNEW(item1);
  albaNEW(item2);
  vnl_matrix<double> data;

  data.set_size(3,3);
  data.fill((double)3.28);

  item1->SetData(data);

  item2->DeepCopy(item1);

  CPPUNIT_ASSERT(item1->Equals(item2) && item2->Equals(item1));

  albaDEL(item1);
  albaDEL(item2);
}
//----------------------------------------------------------------------------
void albaVMEItemScalarMatrixTest::TestShallowCopy()
//----------------------------------------------------------------------------
{
  albaVMEItemScalarMatrix *item1;
  albaVMEItemScalarMatrix *item2;
  albaNEW(item1);
  albaNEW(item2);
  vnl_matrix<double> data;

  data.fill((double)3.28);

  item1->SetData(data);

  item2->DeepCopy(item1);

  CPPUNIT_ASSERT(item1->GetData() == item2->GetData());

  albaDEL(item1);
  albaDEL(item2);
}
//----------------------------------------------------------------------------
void albaVMEItemScalarMatrixTest::TestReadData()
//----------------------------------------------------------------------------
{
  albaVMEItemScalarMatrix *item;
  albaNEW(item);
  vnl_matrix<double> data;

  albaString filename = ALBA_DATA_ROOT;
  filename << "/ItemScalarMatrix/matrix.sca";
  
  /*
  "matrix.sca":
  3.28 16.4 4.94
  3.39 7.41 4.84
  0.52 3.75 3.33
  */

  item->SetDataType("vnl_matrix");
  item->ReadData(filename);

  CPPUNIT_ASSERT(item->IsDataPresent());

  data.set_size(3,3);
  //row 1
  data.put(0,0,3.28);//col 1
  data.put(0,1,16.4);//col 2
  data.put(0,2,4.94);//col 3

  //row 2
  data.put(1,0,3.39);//col 1
  data.put(1,1,7.41);//col 2
  data.put(1,2,4.84);//col 3

  //row 3
  data.put(2,0,0.52);//col 1
  data.put(2,1,3.75);//col 2
  data.put(2,2,3.33);//col 3

  CPPUNIT_ASSERT(item->GetData() == data);  

  albaDEL(item);
}
//----------------------------------------------------------------------------
void albaVMEItemScalarMatrixTest::TestInternalStoreData()
//----------------------------------------------------------------------------
{
  albaVMEItemScalarMatrix *itemw;
  albaNEW(itemw);
  vnl_matrix<double> data;

  DummyObserver *observer = new DummyObserver();

  itemw->SetListener(observer);

  data.set_size(3,3);
  data.fill((double)5.21);

  itemw->SetData(data);

  albaString filename = GET_TEST_DATA_DIR();
  filename << "/item.sca";

  CPPUNIT_ASSERT(itemw->StoreData(filename.GetCStr()) == ALBA_OK);

  albaVMEItemScalarMatrix *itemr;
  albaNEW(itemr);

  itemr->SetDataType("vnl_matrix");
  itemr->ReadData(filename);

  CPPUNIT_ASSERT(itemr->GetData() == itemw->GetData());

  albaDEL(itemw);
  albaDEL(itemr);
  delete observer;
}
//----------------------------------------------------------------------------
void albaVMEItemScalarMatrixTest::TestStoreToArchive()
//----------------------------------------------------------------------------
{
  albaVMEItemScalarMatrix *itemw;
  albaNEW(itemw);
  vnl_matrix<double> data;

  DummyObserver *observer = new DummyObserver();

  itemw->SetListener(observer);

  data.set_size(3,3);
  data.fill((double)5.21);

  itemw->SetData(data);

  albaString filename = ALBA_DATA_ROOT;
  filename << "/ItemScalarMatrix/item.sca";
  albaString filenamez = GET_TEST_DATA_DIR();
  filenamez << "/item.zsca";
  
  albaString workdir = ALBA_DATA_ROOT;
  workdir << "/ItemScalarMatrix";
  wxSetWorkingDirectory(workdir.GetCStr());

  albaString tmparchive;
  observer->GetStorage()->GetTmpFile(tmparchive);

  wxFileOutputStream out(tmparchive.GetCStr());
  wxZipOutputStream zip(out);

  itemw->SetIOModeToMemory();
  itemw->StoreData(NULL);
  itemw->SetURL("item.sca");
  itemw->ReleaseOldFileOn();
  itemw->SetArchiveFileName(filenamez);

  result = itemw->StoreToArchive(zip);

  if (!zip.Close() || !out.Close())
  {
    result = false;
  }

  TEST_RESULT;

  CPPUNIT_ASSERT(observer->GetStorage()->StoreToURL(tmparchive, filenamez) == ALBA_OK);

  albaVMEItemScalarMatrix *itemr;
  albaNEW(itemr);

  itemr->SetURL("item.sca");
  itemr->SetDataType("vnl_matrix");
  itemr->SetIOModeToMemory();
  itemr->ReadData(filenamez);

  CPPUNIT_ASSERT(itemr->GetData() == itemw->GetData());

  albaDEL(itemw);
  albaDEL(itemr);
  delete observer;
}
