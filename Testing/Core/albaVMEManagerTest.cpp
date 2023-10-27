/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEManagerTest
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
#include "albaCoreTests.h"
#include "albaVMEManager.h"
#include "albaVMEManagerTest.h"
#include "albaSmartPointer.h"
#include "albaVME.h"
#include "albaVMEStorage.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMEFactory.h"
#include "albaDataVector.h"
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/ffile.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>
#include <wx/dir.h>
#include <wx/filename.h>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result);


// enum test that need events check
enum TEST_ID
{
  MSFNEW_TEST = MINID,
  NOTIFYADDREMOVE_TEST,
};


//----------------------------------------------------------------------------
void albaVMEManagerTest::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if(albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case CREATE_STORAGE: // the manager ask for storage creation (MSFNew method)
        {
          // create a vme storage for albaVMEManager
          albaVMEStorage *storage = (albaVMEStorage*)e->GetMafObject();
          if(storage!=NULL)
          {
            storage->Delete();
          }
          storage = albaVMEStorage::New();
          storage->GetRoot()->SetName("root");
          storage->SetListener((albaVMEManager*)e->GetSender());
          storage->GetRoot()->Initialize();
          e->SetMafObject(storage);
        }
        break;
      case VME_ADDED:
        {
          if(m_TestId == MSFNEW_TEST) // MSFNew method
          {
            CPPUNIT_ASSERT(m_Manager->GetRoot() == e->GetVme());
          }
          if(m_TestId == NOTIFYADDREMOVE_TEST)
          {
            m_Result = true;
          }
        }
        break;
      case VME_REMOVING:
        {
          if(m_TestId == NOTIFYADDREMOVE_TEST)
          {
            m_Result = true;
          }
        }
        break;
      case VME_SELECTED:
        {
          if(m_TestId == MSFNEW_TEST) // MSFNEw method
          {
            CPPUNIT_ASSERT(m_Manager->GetRoot() == e->GetVme());
          }
        }
        break;
    default:
      {
      }
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::TestFixture()
//----------------------------------------------------------------------------
{
  cppDEL(m_Config); // remove leaks (here the manager is not instantiated and m_Config was not destroyed)
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::BeforeTest()
//----------------------------------------------------------------------------
{
  // create a wxConfig
  m_Config = new wxFileConfig("albaVMEManagerTest");
  wxConfigBase::Set(m_Config);

  int result = albaVMEFactory::Initialize();
}

//----------------------------------------------------------------------------
void albaVMEManagerTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEManager* manager = new albaVMEManager();
  cppDEL(manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVMEManager manager;
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::MSFNewTest()
//----------------------------------------------------------------------------
{
  m_TestId = MSFNEW_TEST; // MSFNewTest

  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew();
  
  // OnEvent check events raised by the manager

  CPPUNIT_ASSERT(NULL != m_Manager->GetRoot()); // root must not be null

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::MSFOpenTest()
//----------------------------------------------------------------------------
{
  // test compatibility with other app stamps
  std::vector<albaString> stamps_v;
  stamps_v.push_back("albaVMEManagerTest");
   
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(stamps_v);
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew();

  albaString filename = ALBA_DATA_ROOT;
  filename << "\\Test_VMEManager\\test.msf";

  // test open with file name
  // Beware: Error message are not handled so this method must be completed successfully
  m_Manager->MSFOpen(filename);

  // check added vme
  albaVMERoot *root = m_Manager->GetRoot();

  CPPUNIT_ASSERT(NULL != root);
  CPPUNIT_ASSERT(albaString("root").Equals(root->GetName()));
  
  CPPUNIT_ASSERT(NULL != root->GetChild(0)->IsA("albaVMESurface"));
  CPPUNIT_ASSERT(albaString("Surface").Equals(root->GetChild(0)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(1)->IsA("albaVMEMesh"));
  CPPUNIT_ASSERT(albaString("Mesh").Equals(root->GetChild(1)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(2)->IsA("albaVMESurface"));
  CPPUNIT_ASSERT(albaString("Surface_2").Equals(root->GetChild(2)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(3)->IsA("albaVMEPolyline"));
  CPPUNIT_ASSERT(albaString("Polyline").Equals(root->GetChild(3)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(4)->IsA("albaVMEPolyline"));
  CPPUNIT_ASSERT(albaString("Polyline_2").Equals(root->GetChild(4)->GetName()));

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::ZIPOpenTest()
//----------------------------------------------------------------------------
{
  // test compatibility with this app stamp
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew();

  albaString filename = ALBA_DATA_ROOT;
  filename << "\\Test_VMEManager\\test.zmsf";

  if(m_Manager->m_FileSystem == NULL)
  {
    m_Manager->m_FileSystem  = new wxFileSystem();
  }
  m_Manager->m_FileSystem->AddHandler(new wxLocalFSHandler());

  // test open with file name
  // Beware: Error message are not handled so this method must be completed successfully
  albaString msfFile = albaString(m_Manager->ZIPOpen(filename));

  wxString path,name,ext;

  wxSplitPath(msfFile.GetCStr(),&path,&name,&ext);
  
  m_Manager->m_TmpDir = ""; // workaround

  m_Manager->MSFOpen(msfFile);

  m_Manager->m_TmpDir = albaString(path.char_str());

  // check added vme
  albaVMERoot *root = m_Manager->GetRoot();

  CPPUNIT_ASSERT(NULL != root);
  CPPUNIT_ASSERT(albaString("root").Equals(root->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(0)->IsA("albaVMESurface"));
  CPPUNIT_ASSERT(albaString("Surface").Equals(root->GetChild(0)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(1)->IsA("albaVMEMesh"));
  CPPUNIT_ASSERT(albaString("Mesh").Equals(root->GetChild(1)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(2)->IsA("albaVMESurface"));
  CPPUNIT_ASSERT(albaString("Surface_2").Equals(root->GetChild(2)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(3)->IsA("albaVMEPolyline"));
  CPPUNIT_ASSERT(albaString("Polyline").Equals(root->GetChild(3)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(4)->IsA("albaVMEPolyline"));
  CPPUNIT_ASSERT(albaString("Polyline_2").Equals(root->GetChild(4)->GetName()));

  m_Manager->RemoveTempDirectory();

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::MSFSaveTest()
//----------------------------------------------------------------------------
{
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  albaVMESurfaceParametric *sphere;
  albaVMESurfaceParametric *cube;

  albaNEW(sphere);
  albaNEW(cube);

  sphere->SetGeometryType(0);
  cube->SetGeometryType(3);

  // create a new msf

  m_Manager->MSFNew(); // do not notify root creation

  // add some vme
  m_Manager->VmeAdd(sphere);
  m_Manager->VmeAdd(cube);

  // set file name
  albaString filename = ALBA_DATA_ROOT;
  filename << "\\Test_VMEManager\\test_save.msf";

  m_Manager->SetFileName(filename);
  m_Manager->MSFSave(); // save the msf in test data directory

  CPPUNIT_ASSERT(wxFileExists(_T(filename.GetCStr()))); // msf exists?

  // delete it for future test
  bool result = false;
  while(result == false)
  {
    result = wxRemoveFile(_T(filename.GetCStr()));
  }
  // do not raise particular events

  albaDEL(sphere);
  albaDEL(cube);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::ZIPSaveTest()
//----------------------------------------------------------------------------
{
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  // set file name
  albaString filename = ALBA_DATA_ROOT;
  filename << "\\Test_VMEManager\\test_save.zmsf";

  albaString dirname = ALBA_DATA_ROOT;
  dirname << "\\Test_VMEManager\\";

  m_Manager->m_TmpDir = dirname;

  m_Manager->ZIPSave(filename);  

  CPPUNIT_ASSERT(wxFileExists(_T(filename.GetCStr()))); // zmsf exists?

  // delete it for future test
 bool result = false;
  while(result == false)
  {
    result = wxRemoveFile(_T(filename.GetCStr()));
  }
  // do not raise particular events

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::MSFIsModifiedTest()
//----------------------------------------------------------------------------
{
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  CPPUNIT_ASSERT(m_Manager->MSFIsModified() == false);

  m_Manager->MSFModified(true);

  CPPUNIT_ASSERT(m_Manager->MSFIsModified() == true);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::VmeAddRemoveTest()
//----------------------------------------------------------------------------
{
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew();

  // test vme add
  albaVMESurfaceParametric *sphere;
  albaNEW(sphere);

  m_Manager->VmeAdd(sphere);

  albaVMERoot* root = m_Manager->GetRoot();
  // the surface must be the first root's child
  CPPUNIT_ASSERT(root->GetFirstChild()==sphere);

  // test vme remove
  m_Manager->VmeRemove(sphere);
  CPPUNIT_ASSERT(root->GetFirstChild()==NULL);

  albaDEL(sphere);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::TimeGetBoundsTest()
//----------------------------------------------------------------------------
{
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew();
  
  double min,max;

  m_Manager->TimeGetBounds(&min,&max);

  CPPUNIT_ASSERT(min == 0 && max == 0);

  //load a time-varying vme

  // set file name
  albaString filename = ALBA_DATA_ROOT;
  filename << "\\Test_VMEManager\\test_tv.msf";
  m_Manager->MSFOpen(filename);

  m_Manager->TimeGetBounds(&min,&max);

  CPPUNIT_ASSERT(min == 0 && max == 1);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::TimeSetTest()
//----------------------------------------------------------------------------
{
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew();
  m_Manager->TimeSet(1);

  CPPUNIT_ASSERT(m_Manager->GetRoot()->GetTimeStamp() == 1);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::NotifyAddRemoveTest()
//----------------------------------------------------------------------------
{
  m_TestId = NOTIFYADDREMOVE_TEST;

  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew();
  
  // test vme add
  albaVMESurfaceParametric *sphere;
  albaNEW(sphere);

  m_Result = false;

  m_Manager->VmeAdd(sphere);

  // OnEvent test on raised event and set m_Result (if the raised event is VME_ADDED)
  TEST_RESULT;

  albaVMERoot* root = m_Manager->GetRoot();
  // the surface must be the first root's child
  CPPUNIT_ASSERT(root->GetFirstChild()==sphere);

  m_Result = false;

  // test vme remove
  m_Manager->VmeRemove(sphere);
  
  // OnEvent test on raised event and set m_Result (if the raised event is VME_REMOVING)
  TEST_RESULT;
  
  CPPUNIT_ASSERT(root->GetFirstChild()==NULL);

  albaDEL(sphere);
  
  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::SetGetFileNameTest()
//----------------------------------------------------------------------------
{

  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->SetFileName(albaString("filename.msf"));

  CPPUNIT_ASSERT(m_Manager->GetFileName().Equals("filename.msf"));

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::GetRootTest()
//----------------------------------------------------------------------------
{
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  CPPUNIT_ASSERT(NULL == m_Manager->GetRoot());

  m_Manager->MSFNew();

  CPPUNIT_ASSERT(m_Manager->GetStorage()->GetRoot() == m_Manager->GetRoot());

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::GetStorageTest()
//----------------------------------------------------------------------------
{
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  CPPUNIT_ASSERT(NULL == m_Manager->GetStorage());

  m_Manager->MSFNew();

  // On event check on storage
  CPPUNIT_ASSERT(NULL != m_Manager->GetStorage());

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::MakeBakFileOnSaveTest()
//----------------------------------------------------------------------------
{
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MakeBakFileOnSave(true);
  albaVMESurfaceParametric *sphere;
  albaVMESurfaceParametric *cube;

  albaNEW(sphere);
  albaNEW(cube);

  sphere->SetGeometryType(0);
  cube->SetGeometryType(3);

  // create a new msf

 m_Manager->MSFNew(); // do not notify root creation

  // add some vme
  m_Manager->VmeAdd(sphere);
  m_Manager->VmeAdd(cube);

  // set file name
  albaString filename = ALBA_DATA_ROOT;
  filename << "\\Test_VMEManager\\test_save.msf";

  m_Manager->SetFileName(filename);
  m_Manager->MSFSave(); // save the msf in test data directory

  m_Manager->MSFSave(); // save twice to create the bak file!

  albaString bakfilename = ALBA_DATA_ROOT;

  bakfilename << "\\Test_VMEManager\\test_save.msf.bak";
  CPPUNIT_ASSERT(wxFileExists(_T(bakfilename.GetCStr()))); // bak exists?

  // delete it for future test
  bool result = false;
  while(result == false)
  {
    result = wxRemoveFile(_T(filename.GetCStr()));
  }
  result = false;
  while(result == false)
  {
    result = wxRemoveFile(_T(bakfilename.GetCStr()));
  }

  // do not raise particular events

  albaDEL(sphere);
  albaDEL(cube);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void albaVMEManagerTest::SetDirNameTest()
//----------------------------------------------------------------------------
{
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->SetDirName(ALBA_DATA_ROOT);

  CPPUNIT_ASSERT(m_Manager->m_MSFDir == ALBA_DATA_ROOT);

  cppDEL(m_Manager);
}

//----------------------------------------------------------------------------
void albaVMEManagerTest::SetSingleBinaryFileTest()
//----------------------------------------------------------------------------
{
  m_Manager = new albaVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(albaString("albaVMEManagerTest"));
  m_Manager->SetTestMode(true);
  m_Manager->MSFNew();

  m_Manager->SetSingleBinaryFile(true);
  
  albaEvent *e = new albaEvent(this,albaDataVector::GetSingleFileDataId(),false);
  e->SetChannel(MCH_UP);
  m_Manager->OnEvent(e);

  CPPUNIT_ASSERT(e->GetBool() == true);

  cppDEL(e);

  cppDEL(m_Manager);
}