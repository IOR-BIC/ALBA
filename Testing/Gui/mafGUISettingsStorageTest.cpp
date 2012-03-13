/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUISettingsStorageTest.cpp,v $
Language:  C++
Date:      $Date: 2008-03-06 11:59:03 $
Version:   $Revision: 1.2 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafGUISettingsStorageTest.h"

#include "mafDecl.h"
#include "mafGUISettingsStorage.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

// Helper class used to build a fake application needed by the wxConfig
// to store settings into the registry with the same name of the application
// ===============================================================================
class TestApp : public wxApp
// ===============================================================================
{
public:
  bool OnInit();
  int  OnExit();
};
DECLARE_APP(TestApp)

IMPLEMENT_APP(TestApp)
//--------------------------------------------------------------------------------
bool TestApp::OnInit()
//--------------------------------------------------------------------------------
{
  return TRUE;
}
//--------------------------------------------------------------------------------
int TestApp::OnExit()
//--------------------------------------------------------------------------------
{
  wxApp::CleanUp();
  return 0;
}
// ===============================================================================

//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::setUp()
//----------------------------------------------------------------------------
{
  m_App = new TestApp();  // Instantiate the application class
  m_App->argc = 0;        // set the number of input argument to 0
  m_App->argv = NULL;     // set to NULL the input argument's parameters
  wxTheApp->SetAppName("mafGUISettingsStorageTest"); // Set the name for the application

  result = false;

  RemoveRegistryKeys();
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::tearDown()
//----------------------------------------------------------------------------
{
  cppDEL(m_App);  // Destroy the application
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage *storageSettings = new mafGUISettingsStorage(NULL);
  cppDEL(storageSettings);
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
}
//----------------------------------------------------------------------------
/*void mafGUISettingsStorageTest::TestGetSingleFileStatus()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  result = storageSettings.GetSingleFileStatus() == 1;
  CPPUNIT_ASSERT(result);
}*/
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestUseRemoteStorage()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  result = storageSettings.UseRemoteStorage() == 0;
  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestGetCacheFolder()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  result = storageSettings.GetCacheFolder().Equals(wxGetCwd().c_str());
  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestGetRemoteHostName()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  result = storageSettings.GetRemoteHostName().IsEmpty();
  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestGetRemotePort()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  result = storageSettings.GetRemotePort() == 21;
  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestGetUserName()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  result = storageSettings.GetUserName().IsEmpty();
  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestGetPassword()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  result = storageSettings.GetPassword().IsEmpty();
  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
/*void mafGUISettingsStorageTest::TestSetSingleFileStatus()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  storageSettings.SetSingleFileStatus(0);

  // Check the variable
  result = storageSettings.GetSingleFileStatus() == 0;
  CPPUNIT_ASSERT(result);

  // Check the registry
  result = CheckRegistryValue("SingleFileMode",0);
  CPPUNIT_ASSERT(result);
}*/
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestSetUseRemoteStorage()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  storageSettings.SetUseRemoteStorage(1);

  // Check the variable
  result = storageSettings.UseRemoteStorage();
  CPPUNIT_ASSERT(result);

  // Check the registry
  result = CheckRegistryValue("UseRemoteStorage", 1);
  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestSetCacheFolder()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  mafString homeFolder;
  homeFolder = wxGetHomeDir().c_str();
  storageSettings.SetCacheFolder(homeFolder);

  // Check the variable
  result = storageSettings.GetCacheFolder() == homeFolder;
  CPPUNIT_ASSERT(result);

  // Check the registry
  result = CheckRegistryValue("CacheFolder", homeFolder.GetCStr());
  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestSetRemoteHostName()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  mafString remoteHost = "www.myhost.com";
  storageSettings.SetRemoteHostName(remoteHost);

  // Check the variable
  result = storageSettings.GetRemoteHostName() == remoteHost;
  CPPUNIT_ASSERT(result);

  // Check the registry
  result = CheckRegistryValue("RemoteHost", remoteHost.GetCStr());
  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestSetRemotePort()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  storageSettings.SetRemotePort(80);

  // Check the variable
  result = storageSettings.GetRemotePort() == 80;
  CPPUNIT_ASSERT(result);

  // Check the registry
  result = CheckRegistryValue("PortConnection", 80);
  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestSetUserName()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  mafString userName = "myUser";
  storageSettings.SetUserName(userName);

  // Check the variable
  result = storageSettings.GetUserName() == userName;
  CPPUNIT_ASSERT(result);

  // Check the registry
  result = CheckRegistryValue("User", userName.GetCStr());
  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestSetPassword()
//----------------------------------------------------------------------------
{
  mafGUISettingsStorage storageSettings(NULL);
  mafString pwd = "myPwd";
  storageSettings.SetPassword(pwd);

  // Check the variable
  result = storageSettings.GetPassword() == pwd;
  CPPUNIT_ASSERT(result);

  // Password is not stored into the registry, so don't check in it!!
}

//============================================================================
// Helper functions
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::RemoveRegistryKeys()
//----------------------------------------------------------------------------
{
  wxConfig cfg;
  result = cfg.DeleteAll();
  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
bool mafGUISettingsStorageTest::CheckRegistryValue(char *key, int refVal)
//----------------------------------------------------------------------------
{
  long longVar;
  wxConfig cfg;
  cfg.Read(key, &longVar);
  return longVar == refVal;
}
//----------------------------------------------------------------------------
bool mafGUISettingsStorageTest::CheckRegistryValue(char *key, const char *refVal)
//----------------------------------------------------------------------------
{
  wxString stringVar;
  wxConfig cfg;
  cfg.Read(key, &stringVar);
  return mafString::Equals(stringVar.c_str(), refVal);
}
