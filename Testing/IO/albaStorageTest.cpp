/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaStorageTest
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

#include "albaStorage.h"
#include "albaStorageTest.h"
#include "albaStorable.h"

/** albaStorageDummy: Dummy class for testing */
//----------------------------------------------------------------------------
class albaStorageDummy : public albaStorage
  //----------------------------------------------------------------------------
{
public:
  albaStorageDummy(){Superclass;};
  ~albaStorageDummy(){m_FilesDictionary.clear();};
  albaTypeMacro(albaStorageDummy,albaStorage);

  /*virtual*/ int ResolveInputURL(const char * url, albaString &filename, albaObserver *observer = NULL){return ALBA_OK;};
  /*virtual*/ int StoreToURL(const char *filename, const char *url=NULL){return ALBA_OK;};
  /*virtual*/ int ReleaseURL(const char *url){return ALBA_OK;};
  /** New method for test IsFileInDirectory() member */
  void AddFileToDirectory(albaString filename){m_FilesDictionary.insert(filename);};
  /** New method for test NeedsUpgrade() member */
  void ForceNeedsUpgrade(){m_NeedsUpgrade = true;};

protected:
  /*virtual*/ int InternalStore(){return ALBA_OK;};
  /*virtual*/ int InternalRestore(){return ALBA_OK;};
  /*virtual*/ int OpenDirectory(const char *dir_name){if(wxDirExists(dir_name)) return ALBA_OK; else return ALBA_ERROR;};
};
albaCxxTypeMacro(albaStorageDummy);

/** albaStorableDummy: Dummy class for testing */
//----------------------------------------------------------------------------
class albaStorableDummy : public albaStorable
//----------------------------------------------------------------------------
{
public:

  albaStorableDummy(){};
  ~albaStorableDummy(){};

protected:
  int InternalStore(albaStorageElement *node){return ALBA_OK;};
  int InternalRestore(albaStorageElement *node){return ALBA_OK;};
};


//----------------------------------------------------------------------------
void albaStorageTest::SetGetURLTest()
//----------------------------------------------------------------------------
{
  albaStorageDummy storage;
  albaString url = ALBA_DATA_ROOT;
  storage.SetURL(url);

  CPPUNIT_ASSERT(url.Equals(storage.GetURL()));
}
//----------------------------------------------------------------------------
void albaStorageTest::ForceGetParserURLTest()
//----------------------------------------------------------------------------
{
  albaStorageDummy storage;
  albaString url = ALBA_DATA_ROOT;
  storage.SetURL(url);
  storage.ForceParserURL(); // Copy storage url to parser url

  CPPUNIT_ASSERT(url.Equals(storage.GetPareserURL()));
}
//----------------------------------------------------------------------------
void albaStorageTest::StoreTest()
//----------------------------------------------------------------------------
{
  albaStorageDummy storage;

  // if url do not exists Store() must return ALBA_ERROR // cannot test because of albaErrorMessage()
  // albaString url = "//ThisPathDoNotExists//";
  // storage.SetURL(url);
  // CPPUNIT_ASSERT(ALBA_ERROR == storage.Store());

  // Now set the url
  albaString url = ALBA_DATA_ROOT;
  storage.SetURL(url);
  CPPUNIT_ASSERT(ALBA_OK == storage.Store()); // Now will call the redefined InternalRestore() method that always returns ALBA_OK
}
//----------------------------------------------------------------------------
void albaStorageTest::RestoreTest()
//----------------------------------------------------------------------------
{
  albaStorageDummy storage;
  CPPUNIT_ASSERT(ALBA_OK == storage.Restore());
}
//----------------------------------------------------------------------------
void albaStorageTest::SetGetDocumentTest()
//----------------------------------------------------------------------------
{
  albaStorageDummy storage;
  albaStorableDummy *doc = new albaStorableDummy; // Uses an arbitrary storable object (that inherit from albaStorable)
  
  storage.SetDocument(doc);

  CPPUNIT_ASSERT(storage.GetDocument() == doc);

  cppDEL(doc);
}
//----------------------------------------------------------------------------
void albaStorageTest::GetTmpFileTest()
//----------------------------------------------------------------------------
{
  albaStorageDummy storage;
  albaString tmpFolder = ALBA_DATA_ROOT;
  tmpFolder << "/";
  albaString tmpFileDummy = tmpFolder;
  tmpFileDummy << "#tmp.0";
  storage.SetTmpFolder(tmpFolder);

  albaString tmpFile;
  storage.GetTmpFile(tmpFile);
  CPPUNIT_ASSERT(tmpFile.Equals(tmpFileDummy));

}
//----------------------------------------------------------------------------
void albaStorageTest::ReleaseTmpFileTest()
//----------------------------------------------------------------------------
{
  // first create some tmp files
  albaStorageDummy storage;
  albaString tmpFolder = ALBA_DATA_ROOT;
  tmpFolder << "/";
  storage.SetTmpFolder(tmpFolder);

  std::vector<albaString> tmpFiles;
  for(int i = 0; i < 3; i++)
  {
    albaString tmpFile;
    storage.GetTmpFile(tmpFile);
    tmpFiles.push_back(tmpFile);
  }

  // ReleaseTmpFile() must delete temporary file on disk
  for(int i = 0; i < tmpFiles.size(); i++)
  {
    storage.ReleaseTmpFile(tmpFiles.at(i));
    CPPUNIT_ASSERT(!wxFileExists(tmpFiles.at(i).GetCStr()));
  }
}
//----------------------------------------------------------------------------
void albaStorageTest::IsFileInDirectoryTest()
//----------------------------------------------------------------------------
{
  albaStorageDummy storage;
  // IsFileInDirectory method check on m_FilesDictionary member variable that cannot be modified
  // so this method always return false
  CPPUNIT_ASSERT(false == storage.IsFileInDirectory("filename"));

  // use the method AddFileToDirectory implemented in the dummy class
  storage.AddFileToDirectory("filename");
  CPPUNIT_ASSERT(true == storage.IsFileInDirectory("filename"));
}
//----------------------------------------------------------------------------
void albaStorageTest::SetGetTmpFolderTest()
//----------------------------------------------------------------------------
{
  albaStorageDummy storage;
  albaString tmpFolder = ALBA_DATA_ROOT;
  tmpFolder << "/";
  storage.SetTmpFolder(tmpFolder);

  CPPUNIT_ASSERT(tmpFolder.Equals(storage.GetTmpFolder()));
}
//----------------------------------------------------------------------------
void albaStorageTest::SetGetErrorCodeTest()
//----------------------------------------------------------------------------
{
  albaStorageDummy storage;
  storage.SetErrorCode(0);
  CPPUNIT_ASSERT(0 == storage.GetErrorCode());
  storage.SetErrorCode(1);
  CPPUNIT_ASSERT(1 == storage.GetErrorCode());
}
//----------------------------------------------------------------------------
void albaStorageTest::NeedsUpgradeTest()
//----------------------------------------------------------------------------
{
  albaStorageDummy storage;
  // NeedsUpgrade method check on m_NeedsUpgrade member variable that cannot be modified
  // so this method always return false
  CPPUNIT_ASSERT(false == storage.NeedsUpgrade());

  // use the method ForceNeedsUpgrade implemented in the dummy class
  storage.ForceNeedsUpgrade();
  CPPUNIT_ASSERT(true == storage.NeedsUpgrade());
}