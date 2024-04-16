/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEExternalDataTest
 Authors: Stefano Perticoni
 
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

#include "albaVMEExternalDataTest.h"
#include "albaVMEExternalData.h"

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

  bool result = true;

  result = wxRmdir(dirName);
  if (result == false)
  {
    return false;
  }

  return true;

}


void albaVMEExternalDataTest::BeforeTest()
{
  m_Storage1Root = 0;

  // create folder for test file
  wxMkdir(externalDataTestDir);

  albaString storageURL;
  storageURL.Append(externalDataTestDir);
  storageURL.Append("/");
  storageURL.Append(externalDataTestMSF);

	m_Storage1=new albaVMEStorage();
  m_Storage1->SetURL(storageURL.GetCStr());

  m_Storage1Root = m_Storage1->GetRoot();
  CPPUNIT_ASSERT(m_Storage1Root);

  // root register the storage so its reference count must be one 
  CPPUNIT_ASSERT(m_Storage1Root->GetReferenceCount() == 1);

  CPPUNIT_ASSERT(m_Storage1->Store() == ALBA_OK);	
}

void albaVMEExternalDataTest::AfterTest()
{
  m_Storage1Root->CleanTree();

  // cleanup created directories   
  bool removeSuccessful = true;

  // remove folder for test file
  removeSuccessful = RemoveDir(externalDataTestDir);
  CPPUNIT_ASSERT(removeSuccessful);

	albaDEL(m_Storage1);
}


void albaVMEExternalDataTest::TestFixture()
{

}

void albaVMEExternalDataTest::TestConstructorDestructor()
{
  albaVMEExternalData *vmeED = albaVMEExternalData::New();
  vmeED->Delete();
}

void albaVMEExternalDataTest::TestSetGetExtension()
{
  albaVMEExternalData *vmeED = albaVMEExternalData::New();  
  vmeED->SetExtension("pippo");
  albaString getMT = vmeED->GetExtension();
  CPPUNIT_ASSERT(getMT.Equals("pippo"));
  vmeED->Delete();
}

void albaVMEExternalDataTest::TestSetGetFileName()
{
  albaVMEExternalData *vmeED = albaVMEExternalData::New();
  vmeED->SetFileName("pippo");
  albaString getFN = vmeED->GetFileName();
  CPPUNIT_ASSERT(getFN.Equals("pippo"));
  vmeED->Delete();
}

void albaVMEExternalDataTest::TestSetGetMimeType()
{
  albaVMEExternalData *vmeED = albaVMEExternalData::New();
  vmeED->SetMimeType("pippo");
  albaString getMT = vmeED->GetMimeType();
  CPPUNIT_ASSERT( getMT.Equals("pippo"));
  vmeED->Delete();
}

void albaVMEExternalDataTest::TestGetAbsoluteFileName()
{
  
  albaSmartPointer<albaVMEExternalData> vmeED;
  vmeED->SetName("externalData");
  
  albaString inLocalFileNameWithoutExtension=ALBA_DATA_ROOT;
  inLocalFileNameWithoutExtension<<"/Test_ExporterBMP/Bn_2295_bc_0012";

  vmeED->SetFileName(inLocalFileNameWithoutExtension);
  vmeED->SetExtension("bmp");

  m_Storage1Root->AddChild(vmeED);
  albaString outputABSFileNameWithExtension = vmeED->GetAbsoluteFileName();
  
  albaString slash = "\\"; /** ??? */
  std::string inABSFileName = slash.Append(inLocalFileNameWithoutExtension.Append(".bmp"));

  int result = outputABSFileNameWithExtension.Equals(inABSFileName.c_str());
  CPPUNIT_ASSERT(result == true);
}

void albaVMEExternalDataTest::TestSetCurrentPath()
{
  albaVMEExternalData *vmeED = albaVMEExternalData::New();
  // TODO: REFACTOR THIS
  // API not clear
  vmeED->SetCurrentPath("pippo");
  CPPUNIT_ASSERT(albaString("pippo").Equals(vmeED->m_TmpPath.GetCStr()));
  vmeED->Delete();
}

void albaVMEExternalDataTest::TestDeepCopy()
{
  albaVMEExternalData *source = albaVMEExternalData::New();
  source->SetName("pippo");

  albaVMEExternalData *target = albaVMEExternalData::New();
  
  target->DeepCopy(source);
  
  CPPUNIT_ASSERT(albaString(source->GetName()).Equals(target->GetName()));

  source->Delete();
  target->Delete();
}

void albaVMEExternalDataTest::TestEquals()
{
  albaVMEExternalData *source = albaVMEExternalData::New();
  albaVMEExternalData *target = albaVMEExternalData::New();
  
  CPPUNIT_ASSERT(source->Equals(target));

  source->Delete();
  target->Delete();
}
