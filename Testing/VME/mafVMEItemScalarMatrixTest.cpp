/*=========================================================================

 Program: MAF2
 Module: mafVMEItemScalarMatrixTest
 Authors: Alberto Losi
 
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
#include "mafVMEItemScalarMatrixTest.h"

#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "mafVMEItemScalarMatrix.h"
#include "mafString.h"
#include "mafVMEStorage.h"
#include "mafEventIO.h"
#include "mafObserver.h"
#include <string.h>

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
class DummyObserver : public mafObserver
//----------------------------------------------------------------------------
{
public:

  DummyObserver() 
  {
    m_Storage = new mafVMEStorage();
  };
  ~DummyObserver()
  {
    delete m_Storage;
  };

  mafStorage* GetStorage()
  {
    return m_Storage;
  };

  virtual void OnEvent(mafEventBase *maf_event)
  {
    if(maf_event->GetId() == NODE_GET_STORAGE)
    {
      mafEventIO *io_event=mafEventIO::SafeDownCast(maf_event);
      io_event->SetStorage(m_Storage);
    }
  }

protected:
  mafVMEStorage *m_Storage;
};
//----------------------------------------------------------------------------
void mafVMEItemScalarMatrixTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEItemScalarMatrixTest::TestSetData()
//----------------------------------------------------------------------------
{
  mafVMEItemScalarMatrix *item;
  mafNEW(item);
  vnl_matrix<double> data;

  data.set_size(3,3);
  data.fill((double)1.64);

  item->SetData(data);
  CPPUNIT_ASSERT(data == item->GetData());

  mafDEL(item);
}
//----------------------------------------------------------------------------
void mafVMEItemScalarMatrixTest::TestEquals()
//----------------------------------------------------------------------------
{
  mafVMEItemScalarMatrix *item1;
  mafVMEItemScalarMatrix *item2;
  mafNEW(item1);
  mafNEW(item2);
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

  mafDEL(item1);
  mafDEL(item2);
}
//----------------------------------------------------------------------------
void mafVMEItemScalarMatrixTest::TestIsDataPresent()
//----------------------------------------------------------------------------
{
  mafVMEItemScalarMatrix *item;
  mafNEW(item);
  vnl_matrix<double> data;

  data.set_size(3,3);
  data.fill((double)3.28);

  CPPUNIT_ASSERT(!item->IsDataPresent());

  item->SetData(data);

  CPPUNIT_ASSERT(item->IsDataPresent());

  item->ReleaseData();

  CPPUNIT_ASSERT(!item->IsDataPresent());

  mafDEL(item);
}
//----------------------------------------------------------------------------
void mafVMEItemScalarMatrixTest::TestGetDataFileExtension()
//----------------------------------------------------------------------------
{
  mafVMEItemScalarMatrix *item;
  mafString ext = mafString("sca");
  mafNEW(item);

  CPPUNIT_ASSERT(ext.Equals(mafString(item->GetDataFileExtension())));

  mafDEL(item);
}
//----------------------------------------------------------------------------
void mafVMEItemScalarMatrixTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  mafVMEItemScalarMatrix *item1;
  mafVMEItemScalarMatrix *item2;
  mafNEW(item1);
  mafNEW(item2);
  vnl_matrix<double> data;

  data.set_size(3,3);
  data.fill((double)3.28);

  item1->SetData(data);

  item2->DeepCopy(item1);

  CPPUNIT_ASSERT(item1->Equals(item2) && item2->Equals(item1));

  mafDEL(item1);
  mafDEL(item2);
}
//----------------------------------------------------------------------------
void mafVMEItemScalarMatrixTest::TestShallowCopy()
//----------------------------------------------------------------------------
{
  mafVMEItemScalarMatrix *item1;
  mafVMEItemScalarMatrix *item2;
  mafNEW(item1);
  mafNEW(item2);
  vnl_matrix<double> data;

  data.fill((double)3.28);

  item1->SetData(data);

  item2->DeepCopy(item1);

  CPPUNIT_ASSERT(item1->GetData() == item2->GetData());

  mafDEL(item1);
  mafDEL(item2);
}
//----------------------------------------------------------------------------
void mafVMEItemScalarMatrixTest::TestReadData()
//----------------------------------------------------------------------------
{
  mafVMEItemScalarMatrix *item;
  mafNEW(item);
  vnl_matrix<double> data;

  mafString filename = MAF_DATA_ROOT;
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

  mafDEL(item);
}
//----------------------------------------------------------------------------
void mafVMEItemScalarMatrixTest::TestInternalStoreData()
//----------------------------------------------------------------------------
{
  mafVMEItemScalarMatrix *itemw;
  mafNEW(itemw);
  vnl_matrix<double> data;

  DummyObserver *observer = new DummyObserver();

  itemw->SetListener(observer);

  data.set_size(3,3);
  data.fill((double)5.21);

  itemw->SetData(data);

  mafString filename = MAF_DATA_ROOT;
  filename << "/ItemScalarMatrix/item.sca";

  CPPUNIT_ASSERT(itemw->StoreData(filename.GetCStr()) == MAF_OK);

  mafVMEItemScalarMatrix *itemr;
  mafNEW(itemr);

  itemr->SetDataType("vnl_matrix");
  itemr->ReadData(filename);

  CPPUNIT_ASSERT(itemr->GetData() == itemw->GetData());

  mafDEL(itemw);
  mafDEL(itemr);
  delete observer;
}
//----------------------------------------------------------------------------
void mafVMEItemScalarMatrixTest::TestStoreToArchive()
//----------------------------------------------------------------------------
{
  mafVMEItemScalarMatrix *itemw;
  mafNEW(itemw);
  vnl_matrix<double> data;

  DummyObserver *observer = new DummyObserver();

  itemw->SetListener(observer);

  data.set_size(3,3);
  data.fill((double)5.21);

  itemw->SetData(data);

  mafString filename = MAF_DATA_ROOT;
  filename << "/ItemScalarMatrix/item.sca";
  mafString filenamez = MAF_DATA_ROOT;
  filenamez << "/ItemScalarMatrix/item.zsca";
  
  mafString workdir = MAF_DATA_ROOT;
  workdir << "/ItemScalarMatrix";
  wxSetWorkingDirectory(workdir.GetCStr());

  mafString tmparchive;
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

  CPPUNIT_ASSERT(observer->GetStorage()->StoreToURL(tmparchive, filenamez) == MAF_OK);

  mafVMEItemScalarMatrix *itemr;
  mafNEW(itemr);

  itemr->SetURL("item.sca");
  itemr->SetDataType("vnl_matrix");
  itemr->SetIOModeToMemory();
  itemr->ReadData(filenamez);

  CPPUNIT_ASSERT(itemr->GetData() == itemw->GetData());

  mafDEL(itemw);
  mafDEL(itemr);
  delete observer;
}
