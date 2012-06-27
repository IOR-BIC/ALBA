/*=========================================================================

 Program: MAF2
 Module: mafStorageTest
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

#include "mafStorage.h"
#include "mafStorageTest.h"
#include "mafStorable.h"

/** mafStorageDummy: Dummy class for testing */
//----------------------------------------------------------------------------
class mafStorageDummy : public mafStorage
  //----------------------------------------------------------------------------
{
public:
  mafStorageDummy(){Superclass;};
  ~mafStorageDummy(){m_FilesDictionary.clear();};
  mafTypeMacro(mafStorageDummy,mafStorage);

  /*virtual*/ int ResolveInputURL(const char * url, mafString &filename, mafObserver *observer = NULL){return MAF_OK;};
  /*virtual*/ int StoreToURL(const char *filename, const char *url=NULL){return MAF_OK;};
  /*virtual*/ int ReleaseURL(const char *url){return MAF_OK;};
  /** New method for test IsFileInDirectory() member */
  void AddFileToDirectory(mafString filename){m_FilesDictionary.insert(filename);};
  /** New method for test NeedsUpgrade() member */
  void ForceNeedsUpgrade(){m_NeedsUpgrade = true;};

protected:
  /*virtual*/ int InternalStore(){return MAF_OK;};
  /*virtual*/ int InternalRestore(){return MAF_OK;};
  /*virtual*/ int OpenDirectory(const char *dir_name){if(wxDirExists(dir_name)) return MAF_OK; else return MAF_ERROR;};
};
mafCxxTypeMacro(mafStorageDummy);

/** mafStorableDummy: Dummy class for testing */
//----------------------------------------------------------------------------
class mafStorableDummy : public mafStorable
//----------------------------------------------------------------------------
{
public:

  mafStorableDummy(){};
  ~mafStorableDummy(){};

protected:
  int InternalStore(mafStorageElement *node){return MAF_OK;};
  int InternalRestore(mafStorageElement *node){return MAF_OK;};
};

//----------------------------------------------------------------------------
void mafStorageTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafStorageTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafStorageTest::SetGetURLTest()
//----------------------------------------------------------------------------
{
  mafStorageDummy storage;
  mafString url = MAF_DATA_ROOT;
  storage.SetURL(url);

  CPPUNIT_ASSERT(url.Equals(storage.GetURL()));
}
//----------------------------------------------------------------------------
void mafStorageTest::ForceGetParserURLTest()
//----------------------------------------------------------------------------
{
  mafStorageDummy storage;
  mafString url = MAF_DATA_ROOT;
  storage.SetURL(url);
  storage.ForceParserURL(); // Copy storage url to parser url

  CPPUNIT_ASSERT(url.Equals(storage.GetPareserURL()));
}
//----------------------------------------------------------------------------
void mafStorageTest::StoreTest()
//----------------------------------------------------------------------------
{
  mafStorageDummy storage;

  // if url do not exists Store() must return MAF_ERROR // cannot test because of mafErrorMessage()
  // mafString url = "//ThisPathDoNotExists//";
  // storage.SetURL(url);
  // CPPUNIT_ASSERT(MAF_ERROR == storage.Store());

  // Now set the url
  mafString url = MAF_DATA_ROOT;
  storage.SetURL(url);
  CPPUNIT_ASSERT(MAF_OK == storage.Store()); // Now will call the redefined InternalRestore() method that always returns MAF_OK
}
//----------------------------------------------------------------------------
void mafStorageTest::RestoreTest()
//----------------------------------------------------------------------------
{
  mafStorageDummy storage;
  CPPUNIT_ASSERT(MAF_OK == storage.Restore());
}
//----------------------------------------------------------------------------
void mafStorageTest::SetGetDocumentTest()
//----------------------------------------------------------------------------
{
  mafStorageDummy storage;
  mafStorableDummy *doc = new mafStorableDummy; // Uses an arbitrary storable object (that inherit from mafStorable)
  
  storage.SetDocument(doc);

  CPPUNIT_ASSERT(storage.GetDocument() == doc);

  cppDEL(doc);
}
//----------------------------------------------------------------------------
void mafStorageTest::GetTmpFileTest()
//----------------------------------------------------------------------------
{
  mafStorageDummy storage;
  mafString tmpFolder = MAF_DATA_ROOT;
  tmpFolder << "/";
  mafString tmpFileDummy = tmpFolder;
  tmpFileDummy << "#tmp.0";
  storage.SetTmpFolder(tmpFolder);

  mafString tmpFile;
  storage.GetTmpFile(tmpFile);
  CPPUNIT_ASSERT(tmpFile.Equals(tmpFileDummy));

}
//----------------------------------------------------------------------------
void mafStorageTest::ReleaseTmpFileTest()
//----------------------------------------------------------------------------
{
  // first create some tmp files
  mafStorageDummy storage;
  mafString tmpFolder = MAF_DATA_ROOT;
  tmpFolder << "/";
  storage.SetTmpFolder(tmpFolder);

  std::vector<mafString> tmpFiles;
  for(int i = 0; i < 3; i++)
  {
    mafString tmpFile;
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
void mafStorageTest::IsFileInDirectoryTest()
//----------------------------------------------------------------------------
{
  mafStorageDummy storage;
  // IsFileInDirectory method check on m_FilesDictionary member variable that cannot be modified
  // so this method always return false
  CPPUNIT_ASSERT(false == storage.IsFileInDirectory("filename"));

  // use the method AddFileToDirectory implemented in the dummy class
  storage.AddFileToDirectory("filename");
  CPPUNIT_ASSERT(true == storage.IsFileInDirectory("filename"));
}
//----------------------------------------------------------------------------
void mafStorageTest::SetGetTmpFolderTest()
//----------------------------------------------------------------------------
{
  mafStorageDummy storage;
  mafString tmpFolder = MAF_DATA_ROOT;
  tmpFolder << "/";
  storage.SetTmpFolder(tmpFolder);

  CPPUNIT_ASSERT(tmpFolder.Equals(storage.GetTmpFolder()));
}
//----------------------------------------------------------------------------
void mafStorageTest::SetGetErrorCodeTest()
//----------------------------------------------------------------------------
{
  mafStorageDummy storage;
  storage.SetErrorCode(0);
  CPPUNIT_ASSERT(0 == storage.GetErrorCode());
  storage.SetErrorCode(1);
  CPPUNIT_ASSERT(1 == storage.GetErrorCode());
}
//----------------------------------------------------------------------------
void mafStorageTest::NeedsUpgradeTest()
//----------------------------------------------------------------------------
{
  mafStorageDummy storage;
  // NeedsUpgrade method check on m_NeedsUpgrade member variable that cannot be modified
  // so this method always return false
  CPPUNIT_ASSERT(false == storage.NeedsUpgrade());

  // use the method ForceNeedsUpgrade implemented in the dummy class
  storage.ForceNeedsUpgrade();
  CPPUNIT_ASSERT(true == storage.NeedsUpgrade());
}