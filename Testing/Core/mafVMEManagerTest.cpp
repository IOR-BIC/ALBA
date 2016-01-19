/*=========================================================================

 Program: MAF2
 Module: mafVMEManagerTest
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
#include "mafCoreTests.h"
#include "mafVMEManager.h"
#include "mafVMEManagerTest.h"
#include "mafSmartPointer.h"
#include "mafVME.h"
#include "mafVMEStorage.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEFactory.h"
#include "mafDataVector.h"
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
void mafVMEManagerTest::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case CREATE_STORAGE: // the manager ask for storage creation (MSFNew method)
        {
          // create a vme storage for mafVMEManager
          mafVMEStorage *storage = (mafVMEStorage*)e->GetMafObject();
          if(storage!=NULL)
          {
            storage->Delete();
          }
          storage = mafVMEStorage::New();
          storage->GetRoot()->SetName("root");
          storage->SetListener((mafVMEManager*)e->GetSender());
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
void mafVMEManagerTest::TestFixture()
//----------------------------------------------------------------------------
{
  cppDEL(m_Config); // remove leaks (here the manager is not instantiated and m_Config was not destroyed)
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::BeforeTest()
//----------------------------------------------------------------------------
{
  // create a wxConfig
  m_Config = new wxFileConfig("mafVMEManagerTest");
  wxConfigBase::Set(m_Config);

  int result = mafVMEFactory::Initialize();
}

//----------------------------------------------------------------------------
void mafVMEManagerTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEManager* manager = new mafVMEManager();
  cppDEL(manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafVMEManager manager;
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::MSFNewTest()
//----------------------------------------------------------------------------
{
  m_TestId = MSFNEW_TEST; // MSFNewTest

  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew(true);
  
  // OnEvent check events raised by the manager

  CPPUNIT_ASSERT(NULL != m_Manager->GetRoot()); // root must not be null

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::MSFOpenTest()
//----------------------------------------------------------------------------
{
  // test compatibility with other app stamps
  std::vector<mafString> stamps_v;
  stamps_v.push_back("mafVMEManagerTest");
   
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(stamps_v);
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew(true);

  mafString filename = MAF_DATA_ROOT;
  filename << "\\Test_VMEManager\\test.msf";

  // test open with file name
  // Beware: Error message are not handled so this method must be completed successfully
  m_Manager->MSFOpen(filename);

  // check added vme
  mafVMERoot *root = m_Manager->GetRoot();

  CPPUNIT_ASSERT(NULL != root);
  CPPUNIT_ASSERT(mafString("root").Equals(root->GetName()));
  
  CPPUNIT_ASSERT(NULL != root->GetChild(0)->IsA("mafVMESurface"));
  CPPUNIT_ASSERT(mafString("Surface").Equals(root->GetChild(0)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(1)->IsA("mafVMEMesh"));
  CPPUNIT_ASSERT(mafString("Mesh").Equals(root->GetChild(1)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(2)->IsA("mafVMESurface"));
  CPPUNIT_ASSERT(mafString("Surface_2").Equals(root->GetChild(2)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(3)->IsA("mafVMEPolyline"));
  CPPUNIT_ASSERT(mafString("Polyline").Equals(root->GetChild(3)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(4)->IsA("mafVMEPolyline"));
  CPPUNIT_ASSERT(mafString("Polyline_2").Equals(root->GetChild(4)->GetName()));

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::ZIPOpenTest()
//----------------------------------------------------------------------------
{
  // test compatibility with this app stamp
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew(true);

  mafString filename = MAF_DATA_ROOT;
  filename << "\\Test_VMEManager\\test.zmsf";

  if(m_Manager->m_FileSystem == NULL)
  {
    m_Manager->m_FileSystem  = new wxFileSystem();
  }
  m_Manager->m_FileSystem->AddHandler(new wxLocalFSHandler());

  // test open with file name
  // Beware: Error message are not handled so this method must be completed successfully
  mafString msfFile = mafString(m_Manager->ZIPOpen(filename));

  wxString path,name,ext;

  wxSplitPath(msfFile.GetCStr(),&path,&name,&ext);
  
  m_Manager->m_TmpDir = ""; // workaround

  m_Manager->MSFOpen(msfFile);

  m_Manager->m_TmpDir = mafString(path.c_str());

  // check added vme
  mafVMERoot *root = m_Manager->GetRoot();

  CPPUNIT_ASSERT(NULL != root);
  CPPUNIT_ASSERT(mafString("root").Equals(root->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(0)->IsA("mafVMESurface"));
  CPPUNIT_ASSERT(mafString("Surface").Equals(root->GetChild(0)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(1)->IsA("mafVMEMesh"));
  CPPUNIT_ASSERT(mafString("Mesh").Equals(root->GetChild(1)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(2)->IsA("mafVMESurface"));
  CPPUNIT_ASSERT(mafString("Surface_2").Equals(root->GetChild(2)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(3)->IsA("mafVMEPolyline"));
  CPPUNIT_ASSERT(mafString("Polyline").Equals(root->GetChild(3)->GetName()));

  CPPUNIT_ASSERT(NULL != root->GetChild(4)->IsA("mafVMEPolyline"));
  CPPUNIT_ASSERT(mafString("Polyline_2").Equals(root->GetChild(4)->GetName()));

  m_Manager->RemoveTempDirectory();

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::MSFSaveTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  mafVMESurfaceParametric *sphere;
  mafVMESurfaceParametric *cube;

  mafNEW(sphere);
  mafNEW(cube);

  sphere->SetGeometryType(0);
  cube->SetGeometryType(3);

  // create a new msf

  m_Manager->MSFNew(false); // do not notify root creation

  // add some vme
  m_Manager->VmeAdd(sphere);
  m_Manager->VmeAdd(cube);

  // set file name
  mafString filename = MAF_DATA_ROOT;
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

  mafDEL(sphere);
  mafDEL(cube);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::ZIPSaveTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  // set file name
  mafString filename = MAF_DATA_ROOT;
  filename << "\\Test_VMEManager\\test_save.zmsf";

  mafString dirname = MAF_DATA_ROOT;
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
void mafVMEManagerTest::MSFIsModifiedTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  CPPUNIT_ASSERT(m_Manager->MSFIsModified() == false);

  m_Manager->MSFModified(true);

  CPPUNIT_ASSERT(m_Manager->MSFIsModified() == true);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::VmeAddRemoveTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew(false);

  // test vme add
  mafVMESurfaceParametric *sphere;
  mafNEW(sphere);

  m_Manager->VmeAdd(sphere);

  mafVMERoot* root = m_Manager->GetRoot();
  // the surface must be the first root's child
  CPPUNIT_ASSERT(root->GetFirstChild()==sphere);

  // test vme remove
  m_Manager->VmeRemove(sphere);
  CPPUNIT_ASSERT(root->GetFirstChild()==NULL);

  mafDEL(sphere);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::TimeGetBoundsTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew(false);
  
  double min,max;

  m_Manager->TimeGetBounds(&min,&max);

  CPPUNIT_ASSERT(min == 0 && max == 0);

  //load a time-varying vme

  // set file name
  mafString filename = MAF_DATA_ROOT;
  filename << "\\Test_VMEManager\\test_tv.msf";
  m_Manager->MSFOpen(filename);

  m_Manager->TimeGetBounds(&min,&max);

  CPPUNIT_ASSERT(min == 0 && max == 1);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::TimeSetTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew();
  m_Manager->TimeSet(1);

  CPPUNIT_ASSERT(m_Manager->GetRoot()->GetTimeStamp() == 1);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::NotifyAddRemoveTest()
//----------------------------------------------------------------------------
{
  m_TestId = NOTIFYADDREMOVE_TEST;

  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew();
  
  // test vme add
  mafVMESurfaceParametric *sphere;
  mafNEW(sphere);

  m_Result = false;

  m_Manager->VmeAdd(sphere);

  // OnEvent test on raised event and set m_Result (if the raised event is VME_ADDED)
  TEST_RESULT;

  mafVMERoot* root = m_Manager->GetRoot();
  // the surface must be the first root's child
  CPPUNIT_ASSERT(root->GetFirstChild()==sphere);

  m_Result = false;

  // test vme remove
  m_Manager->VmeRemove(sphere);
  
  // OnEvent test on raised event and set m_Result (if the raised event is VME_REMOVING)
  TEST_RESULT;
  
  CPPUNIT_ASSERT(root->GetFirstChild()==NULL);

  mafDEL(sphere);
  
  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::SetGetFileNameTest()
//----------------------------------------------------------------------------
{

  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->SetFileName(mafString("filename.msf"));

  CPPUNIT_ASSERT(m_Manager->GetFileName().Equals("filename.msf"));

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::GetRootTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  CPPUNIT_ASSERT(NULL == m_Manager->GetRoot());

  m_Manager->MSFNew(true);

  CPPUNIT_ASSERT(m_Manager->GetStorage()->GetRoot() == m_Manager->GetRoot());

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::GetStorageTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  CPPUNIT_ASSERT(NULL == m_Manager->GetStorage());

  m_Manager->MSFNew(true);

  // On event check on storage
  CPPUNIT_ASSERT(NULL != m_Manager->GetStorage());

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::MakeBakFileOnSaveTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MakeBakFileOnSave(true);
  mafVMESurfaceParametric *sphere;
  mafVMESurfaceParametric *cube;

  mafNEW(sphere);
  mafNEW(cube);

  sphere->SetGeometryType(0);
  cube->SetGeometryType(3);

  // create a new msf

  m_Manager->MSFNew(false); // do not notify root creation

  // add some vme
  m_Manager->VmeAdd(sphere);
  m_Manager->VmeAdd(cube);

  // set file name
  mafString filename = MAF_DATA_ROOT;
  filename << "\\Test_VMEManager\\test_save.msf";

  m_Manager->SetFileName(filename);
  m_Manager->MSFSave(); // save the msf in test data directory

  m_Manager->MSFSave(); // save twice to create the bak file!

  mafString bakfilename = MAF_DATA_ROOT;

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

  mafDEL(sphere);
  mafDEL(cube);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::SetDirNameTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->SetDirName(MAF_DATA_ROOT);

  CPPUNIT_ASSERT(m_Manager->m_MSFDir == MAF_DATA_ROOT);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::SetLocalCacheFolderTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->MSFNew(false);

  mafString tmpfolder;
  tmpfolder << MAF_DATA_ROOT;
  tmpfolder << "/";
  m_Manager->SetLocalCacheFolder(tmpfolder);

  tmpfolder << "#tmp.0";

  mafString stotmpfolder;
  m_Manager->GetStorage()->GetTmpFile(stotmpfolder);
  CPPUNIT_ASSERT(tmpfolder.Equals(stotmpfolder));

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::SetHostTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->SetHost("127.0.0.1");
  
  CPPUNIT_ASSERT(m_Manager->m_Host.Equals("127.0.0.1"));

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::SetUserTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->SetUser("user");

  CPPUNIT_ASSERT(m_Manager->m_User.Equals("user"));

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::SetPasswordTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->SetPassword("password");

  CPPUNIT_ASSERT(m_Manager->m_Pwd.Equals("password"));

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::SetRemotePortTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);

  m_Manager->SetRemotePort(80);

  CPPUNIT_ASSERT(m_Manager->m_Port == 80);

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::RemoveTempDirectoryTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);
  m_Manager->MSFNew(true);

  // The only way to force the manager to create the temp directory is open a zmsf
  mafString filename = MAF_DATA_ROOT;
  filename << "\\Test_VMEManager\\test.zmsf";

  if(m_Manager->m_FileSystem == NULL)
  {
    m_Manager->m_FileSystem  = new wxFileSystem();
  }
  m_Manager->m_FileSystem->AddHandler(new wxLocalFSHandler());

  // test open with file name
  // Beware: Error message are not handled so this method must be completed successfully
  mafString msfFile = mafString(m_Manager->ZIPOpen(filename));

  wxString path,name,ext;

  // path contains the temp directory!
  wxSplitPath(msfFile.GetCStr(),&path,&name,&ext);

  m_Manager->RemoveTempDirectory();

  CPPUNIT_ASSERT(!wxDirExists(path));

  cppDEL(m_Manager);
}
//----------------------------------------------------------------------------
void mafVMEManagerTest::SetSingleBinaryFileTest()
//----------------------------------------------------------------------------
{
  m_Manager = new mafVMEManager();
  m_Manager->SetListener(this);
  m_Manager->SetApplicationStamp(mafString("mafVMEManagerTest"));
  m_Manager->SetTestMode(true);
  m_Manager->MSFNew(true);

  m_Manager->SetSingleBinaryFile(true);
  
  mafEvent *e = new mafEvent(this,mafDataVector::GetSingleFileDataId(),false);
  e->SetChannel(MCH_UP);
  m_Manager->OnEvent(e);

  CPPUNIT_ASSERT(e->GetBool() == true);

  cppDEL(e);

  cppDEL(m_Manager);
}