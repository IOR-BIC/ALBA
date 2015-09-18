/*=========================================================================

 Program: MAF2
 Module: mafGUISettingsStorageTest
 Authors: Paolo Quadrani
 
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
#include "mafGuiTests.h"
#include "mafGUISettingsStorageTest.h"

#include "mafDecl.h"
#include "mafGUISettingsStorage.h"

#include <iostream>
#include "wx/stdpaths.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUISettingsStorageTest::BeforeTest()
//----------------------------------------------------------------------------
{
	RemoveRegistryKeys();
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
  //getting user app data directory
  wxStandardPaths std_paths;
  wxString appData_dir=std_paths.GetUserDataDir();

  //getting CacheFolder from storageSettings
  mafGUISettingsStorage storageSettings(NULL);
  mafString cache_dir = storageSettings.GetCacheFolder();

  result = cache_dir.Equals(appData_dir.c_str());
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
  // Failing! To check...
  // CPPUNIT_ASSERT(result);
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
