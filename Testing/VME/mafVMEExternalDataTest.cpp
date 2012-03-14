/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEExternalDataTest.cpp,v $
Language:  C++
Date:      $Date: 2009-04-21 15:38:49 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
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

#include "mafVMEExternalDataTest.h"
#include "mafVMEExternalData.h"

#include <wx/dir.h>
#include <iostream>


// files and directories names
const char *externalDataTestDir = "externalDataTestDir";

// file must have an extension otherwise store will fail!
const char *externalDataTestMSF = "externalDataTest.msf";

bool RemoveDir(const char *dirName)
{
  wxArrayString filenameArray;
  wxDir::GetAllFiles(dirName,&filenameArray);
  for (int index = 0; index < filenameArray.size(); index++)
  {
    wxRemoveFile(filenameArray.Item(index));
  }

  bool result = TRUE;

  result = wxRmdir(dirName);
  if (result == false)
  {
    return false;
  }

  return true;

}


void mafVMEExternalDataTest::setUp()
{
  m_Storage1Root = 0;

  // create folder for test file
  wxMkdir(externalDataTestDir);

  mafString storageURL;
  storageURL.Append(externalDataTestDir);
  storageURL.Append("/");
  storageURL.Append(externalDataTestMSF);

  m_Storage1.SetURL(storageURL.GetCStr());

  m_Storage1Root = m_Storage1.GetRoot();
  CPPUNIT_ASSERT(m_Storage1Root);

  // root register the storage so its reference count must be one 
  CPPUNIT_ASSERT(m_Storage1Root->GetReferenceCount() == 1);

  CPPUNIT_ASSERT(m_Storage1.Store() == MAF_OK);	
}

void mafVMEExternalDataTest::tearDown()
{
  m_Storage1Root->CleanTree();

  // cleanup created directories   
  bool removeSuccessful = true;

  // remove folder for test file
  removeSuccessful = RemoveDir(externalDataTestDir);
  CPPUNIT_ASSERT(removeSuccessful);
}


void mafVMEExternalDataTest::TestFixture()
{

}

void mafVMEExternalDataTest::TestConstructorDestructor()
{
  mafVMEExternalData *vmeED = mafVMEExternalData::New();
  vmeED->Delete();
}

void mafVMEExternalDataTest::TestSetGetExtension()
{
  mafVMEExternalData *vmeED = mafVMEExternalData::New();  
  vmeED->SetExtension("pippo");
  mafString getMT = vmeED->GetExtension();
  CPPUNIT_ASSERT(getMT.Equals("pippo"));
  vmeED->Delete();
}

void mafVMEExternalDataTest::TestSetGetFileName()
{
  mafVMEExternalData *vmeED = mafVMEExternalData::New();
  vmeED->SetFileName("pippo");
  mafString getFN = vmeED->GetFileName();
  CPPUNIT_ASSERT(getFN.Equals("pippo"));
  vmeED->Delete();
}

void mafVMEExternalDataTest::TestSetGetMimeType()
{
  mafVMEExternalData *vmeED = mafVMEExternalData::New();
  vmeED->SetMimeType("pippo");
  mafString getMT = vmeED->GetMimeType();
  CPPUNIT_ASSERT( getMT.Equals("pippo"));
  vmeED->Delete();
}

void mafVMEExternalDataTest::TestGetAbsoluteFileName()
{
  
  mafSmartPointer<mafVMEExternalData> vmeED;
  vmeED->SetName("externalData");
  
  mafString inLocalFileNameWithoutExtension=MAF_DATA_ROOT;
  inLocalFileNameWithoutExtension<<"/Test_ExporterBMP/Bn_2295_bc_0012";

  vmeED->SetFileName(inLocalFileNameWithoutExtension);
  vmeED->SetExtension("bmp");

  m_Storage1Root->AddChild(vmeED);
  mafString outputABSFileNameWithExtension = vmeED->GetAbsoluteFileName();
  
  mafString slash = "\\"; /** ??? */
  std::string inABSFileName = slash.Append(inLocalFileNameWithoutExtension.Append(".bmp"));

  int result = outputABSFileNameWithExtension.Equals(inABSFileName.c_str());
  CPPUNIT_ASSERT(result == TRUE);
}

void mafVMEExternalDataTest::TestSetCurrentPath()
{
  mafVMEExternalData *vmeED = mafVMEExternalData::New();
  // TODO: REFACTOR THIS
  // API not clear
  vmeED->SetCurrentPath("pippo");
  CPPUNIT_ASSERT(mafString("pippo").Equals(vmeED->m_TmpPath.GetCStr()));
  vmeED->Delete();
}

void mafVMEExternalDataTest::TestDeepCopy()
{
  mafVMEExternalData *source = mafVMEExternalData::New();
  source->SetName("pippo");

  mafVMEExternalData *target = mafVMEExternalData::New();
  
  target->DeepCopy(source);
  
  CPPUNIT_ASSERT(mafString(source->GetName()).Equals(target->GetName()));

  source->Delete();
  target->Delete();
}

void mafVMEExternalDataTest::TestEquals()
{
  mafVMEExternalData *source = mafVMEExternalData::New();
  mafVMEExternalData *target = mafVMEExternalData::New();
  
  CPPUNIT_ASSERT(source->Equals(target));

  source->Delete();
  target->Delete();
}
