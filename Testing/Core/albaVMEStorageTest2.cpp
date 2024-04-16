/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEStorageTest2
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
#include "albaCoreTests.h"
#include "albaVMEStorageTest2.h"

#include "albaVMEFactory.h"
#include "albaCoreFactory.h"
#include "albaVMEIterator.h"
#include "albaVMESurface.h"
#include "albaVMEItem.h"
#include "albaVMEOutput.h"
#include "albaAbsMatrixPipe.h"
#include "albaTagArray.h"

#include "vtkCubeSource.h"
#include "vtkConeSource.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkALBASmartPointer.h"



#include <iostream>

// files and directories names
const char *storage1Dir = "storage1Dir";

// file must have an extension otherwise store will fail!
const char *storage1MSF = "storage1.msf";



//----------------------------------------------------------------------------
void albaVMEStorageTest2::TestFixture()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void albaVMEStorageTest2::BeforeTest()
//----------------------------------------------------------------------------
{
  // member variables initialization
  m_SphereSource = 0;
  m_ConeSource = 0;
  m_Storage1Root = 0;

  // create folder for test file
  wxMkdir(storage1Dir);

  albaString storageURL;
  storageURL.Append(storage1Dir);
  storageURL.Append("/");
  storageURL.Append(storage1MSF);

	m_Storage1=new albaVMEStorage();
  m_Storage1->SetURL(storageURL.GetCStr());

  m_Storage1Root = m_Storage1->GetRoot();
  CPPUNIT_ASSERT(m_Storage1Root);

  // root register the storage so its reference count must be one 
  CPPUNIT_ASSERT(m_Storage1Root->GetReferenceCount() == 1);

  // create a VME surface with a sphere
  albaSmartPointer<albaVMESurface> vmeSphere;
  vmeSphere->SetName("sphere");

  // create VTK surface data for the VME
  CPPUNIT_ASSERT(0 == m_SphereSource);
  m_SphereSource = vtkSphereSource::New();

  m_SphereSource->Update();
  CPPUNIT_ASSERT(m_SphereSource->GetOutput());

  // okay, we got some cells
  CPPUNIT_ASSERT(m_SphereSource->GetOutput()->GetNumberOfCells());
  // ... and some points
  CPPUNIT_ASSERT(m_SphereSource->GetOutput()->GetNumberOfPoints());
  // and polys
  CPPUNIT_ASSERT(m_SphereSource->GetOutput()->GetNumberOfPolys());

  // testing vtkDataSet interface
  vtkDataSet *data = m_SphereSource->GetOutput();
  CPPUNIT_ASSERT(data->GetNumberOfCells());
  CPPUNIT_ASSERT(data->GetNumberOfPoints());

  vtkPolyData *polyDataSphereSource = vtkPolyData::SafeDownCast(m_SphereSource->GetOutput());
  CPPUNIT_ASSERT(polyDataSphereSource->GetNumberOfPolys());

  // current time for test
  m_CurrentTime = 0.0;

  CPPUNIT_ASSERT(ALBA_OK == vmeSphere->SetData(m_SphereSource->GetOutput(),m_CurrentTime,albaVMEGeneric::ALBA_VME_COPY_DATA));
  vmeSphere->Update();

  vmeSphere->SetTreeTime(m_CurrentTime);
  vmeSphere->SetTimeStamp(m_CurrentTime);

  CPPUNIT_ASSERT(vmeSphere->GetDataPipe());

  albaVMEOutput* vmeSphereOutput = vmeSphere->GetOutput();
  CPPUNIT_ASSERT(vmeSphereOutput);

  vtkDataSet* vtkSphereOutput = vmeSphereOutput->GetVTKData();
  CPPUNIT_ASSERT(vtkSphereOutput);

  // if you remove comments from next line the next assert will fail
  // vmeSphereOutput->Update();

  CPPUNIT_ASSERT(vtkSphereOutput->GetNumberOfPoints()  == m_SphereSource->GetOutput()->GetNumberOfPoints());

  CPPUNIT_ASSERT(vtkSphereOutput->GetNumberOfPoints() == polyDataSphereSource->GetNumberOfPoints());

  CPPUNIT_ASSERT(vtkSphereOutput->GetNumberOfCells() == polyDataSphereSource->GetNumberOfCells());

  // getting VTK data from albaVMEOutput
  vtkPolyData* polyDataFromVMEOutput = vtkPolyData::SafeDownCast(vmeSphereOutput->GetVTKData());

  CPPUNIT_ASSERT(polyDataFromVMEOutput->GetNumberOfPolys());

  // getting VTK data from albaVMEOutputSurface
  albaVMEOutputSurface* outputSurface = albaVMEOutputSurface::SafeDownCast(vmeSphereOutput);
  CPPUNIT_ASSERT(outputSurface);

  vtkPolyData* polyDataFromVMEOutputSurface = vtkPolyData::SafeDownCast(outputSurface->GetSurfaceData());

  CPPUNIT_ASSERT(polyDataFromVMEOutputSurface);
  CPPUNIT_ASSERT(polyDataFromVMEOutputSurface->GetNumberOfPolys() == m_SphereSource->GetOutput()->GetNumberOfPolys());

  m_Storage1Root->AddChild(vmeSphere);

  // create a VME surface with a cone
  albaSmartPointer<albaVMESurface> vmeCone;
  vmeCone->SetName("cone");

  // create VTK surface data for the VME
  CPPUNIT_ASSERT(0 == m_ConeSource);
  m_ConeSource = vtkConeSource::New();

  m_ConeSource->Update();
  CPPUNIT_ASSERT(m_ConeSource->GetOutput());

  CPPUNIT_ASSERT(ALBA_OK == vmeCone->SetData(m_SphereSource->GetOutput(),m_CurrentTime,albaVMEGeneric::ALBA_VME_COPY_DATA));

  m_Storage1Root->AddChild(vmeCone);
  
  // set the tree time
  m_Storage1Root->SetTreeTime(m_CurrentTime);

  // perform some checks on the storage tree
  CPPUNIT_ASSERT(m_Storage1Root->GetNumberOfChildren() == 2);
  CPPUNIT_ASSERT(0 == strcmp(m_Storage1Root->GetChild(0)->GetName(), "sphere"));
  CPPUNIT_ASSERT(0 == strcmp(m_Storage1Root->GetChild(1)->GetName(), "cone"));

  albaVMESurface* shouldBeTheVmeSphere = albaVMESurface::SafeDownCast(m_Storage1Root->GetChild(0));
  CPPUNIT_ASSERT(shouldBeTheVmeSphere);

  // reference count
  // this returns 2
  int shouldBeTheVmeSphereRC = shouldBeTheVmeSphere->GetReferenceCount();

  CPPUNIT_ASSERT(shouldBeTheVmeSphere->GetAbsMatrixPipe());

  vtkDataSet* shouldBeTheVTKSphere = shouldBeTheVmeSphere->GetOutput()->GetVTKData();
  CPPUNIT_ASSERT(shouldBeTheVTKSphere);
  CPPUNIT_ASSERT(shouldBeTheVTKSphere->GetNumberOfPoints());

  CPPUNIT_ASSERT(shouldBeTheVTKSphere->GetNumberOfCells() == m_SphereSource->GetOutput()->GetNumberOfCells());

  CPPUNIT_ASSERT(m_Storage1->Store() == ALBA_OK);	
}

//----------------------------------------------------------------------------
void albaVMEStorageTest2::AfterTest()
//----------------------------------------------------------------------------
{
  // clean up
  vtkDEL(m_SphereSource);
  vtkDEL(m_ConeSource);

  m_Storage1Root->CleanTree();

  // cleanup created directories   
  bool removeSuccessful = true;

  // remove folder for test file
  removeSuccessful = RemoveDir(storage1Dir);
  CPPUNIT_ASSERT(removeSuccessful);

	albaDEL(m_Storage1);
}


void albaVMEStorageTest2::TestRestoreOnNewStorage()
{

  albaString storageURL;
  storageURL.Append(storage1Dir);
  storageURL.Append("/");
  storageURL.Append(storage1MSF);

  // testing storage open

  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  //create a new storage: this also creates a root
  albaVMEStorage storage2;
  storage2.SetURL(storageURL.GetCStr());

  albaVMERoot* storage2Root;
  storage2Root = NULL;

  storage2Root = storage2.GetRoot();
  CPPUNIT_ASSERT(storage2Root);

  // root register the storage so its reference count must be one 
  CPPUNIT_ASSERT(storage2Root->GetReferenceCount() == 1);

  CPPUNIT_ASSERT(ALBA_OK == storage2.Restore());

  CPPUNIT_ASSERT(storage2Root->GetReferenceCount() == 1);
  
  // set the Tree time
  storage2Root->SetTreeTime(m_CurrentTime);

  // perform some checks on the storage tree
  CPPUNIT_ASSERT(storage2Root->GetNumberOfChildren() == 2);
  CPPUNIT_ASSERT(0 == strcmp(storage2Root->GetChild(0)->GetName(), "sphere"));
  CPPUNIT_ASSERT(0 == strcmp(storage2Root->GetChild(1)->GetName(), "cone"));

  albaVMESurface* shouldBeTheVmeSphere = albaVMESurface::SafeDownCast(storage2Root->GetChild(0));
  CPPUNIT_ASSERT(shouldBeTheVmeSphere);

  // reference count
  int shouldBeTheVmeSphereRC = shouldBeTheVmeSphere->GetReferenceCount();

  CPPUNIT_ASSERT(shouldBeTheVmeSphereRC == 1);
  CPPUNIT_ASSERT(shouldBeTheVmeSphere->GetAbsMatrixPipe());
  
  int shouldBeTheVmeSphereTARC = shouldBeTheVmeSphere->GetTagArray()->GetReferenceCount();
  CPPUNIT_ASSERT(shouldBeTheVmeSphereTARC == 1);

  vtkDataSet* shouldBeTheVTKSphere = shouldBeTheVmeSphere->GetOutput()->GetVTKData();
  CPPUNIT_ASSERT(shouldBeTheVTKSphere);

  CPPUNIT_ASSERT(shouldBeTheVTKSphere->GetNumberOfPoints());

  CPPUNIT_ASSERT(shouldBeTheVTKSphere->GetNumberOfCells() == m_SphereSource->GetOutput()->GetNumberOfCells());


  albaSleep(1000);
}

void albaVMEStorageTest2::TestRestoreOnSameStorage()
{
  // initialize the vme factory: this is needed to restore vme from disk
  albaVMEFactory::Initialize();

  albaString storageURL;
  storageURL.Append(storage1Dir);
  storageURL.Append("/");
  storageURL.Append(storage1MSF);

  // root register the storage so its reference count must be one 
  CPPUNIT_ASSERT(m_Storage1Root->GetReferenceCount() == 1);
  
  CPPUNIT_ASSERT(ALBA_OK == m_Storage1->Restore());

  CPPUNIT_ASSERT(m_Storage1Root->GetReferenceCount() == 1);

  // set the Tree time
  m_Storage1Root->SetTreeTime(m_CurrentTime);

  // perform some checks on the storage tree
  CPPUNIT_ASSERT(m_Storage1Root->GetNumberOfChildren() == 2);
  CPPUNIT_ASSERT(0 == strcmp(m_Storage1Root->GetChild(0)->GetName(), "sphere"));
  CPPUNIT_ASSERT(0 == strcmp(m_Storage1Root->GetChild(1)->GetName(), "cone"));

  albaVMESurface* shouldBeTheVmeSphere = albaVMESurface::SafeDownCast(m_Storage1Root->GetChild(0));
  CPPUNIT_ASSERT(shouldBeTheVmeSphere);

  // reference count
  // this returns 1
  int shouldBeTheVmeSphereRC = shouldBeTheVmeSphere->GetReferenceCount();
  
  CPPUNIT_ASSERT(shouldBeTheVmeSphere->GetAbsMatrixPipe());

  vtkDataSet* shouldBeTheVTKSphere = shouldBeTheVmeSphere->GetOutput()->GetVTKData();
  CPPUNIT_ASSERT(shouldBeTheVTKSphere);
  CPPUNIT_ASSERT(shouldBeTheVTKSphere->GetNumberOfPoints());

  CPPUNIT_ASSERT(shouldBeTheVTKSphere->GetNumberOfCells() == m_SphereSource->GetOutput()->GetNumberOfCells());
}

void albaVMEStorageTest2::TestStoreRestoreTagArray()
{
  albaVMEStorage emptyStorage;
  albaVMERoot *emptyStorageRoot = 0;

  // files and directories names
  const char *emptyVMEStorageDir = "emptyVMEStorageDir";

  // file must have an extension otherwise store will fail!
  const char *emptyVMEStorageMSF = "emptyVMEStorage.msf";

  // create folder for test file
  wxMkdir(emptyVMEStorageDir);

  albaString storageURL;
  storageURL.Append(emptyVMEStorageDir);
  storageURL.Append("/");
  storageURL.Append(emptyVMEStorageMSF);

  emptyStorage.SetURL(storageURL.GetCStr());

  emptyStorageRoot = emptyStorage.GetRoot();
  CPPUNIT_ASSERT(emptyStorageRoot);

  // root register the storage so its reference count must be one 
  CPPUNIT_ASSERT(emptyStorageRoot->GetReferenceCount() == 1);

  // create an empty VME surface 
  albaSmartPointer<albaVMESurface> vmeEmptySurface;
  vmeEmptySurface->SetName("empty surface vme");

  vmeEmptySurface->GetTagArray()->SetTag("pippo","pluto");

  albaTagArray *ta = vmeEmptySurface->GetTagArray();
  CPPUNIT_ASSERT(ta);
  int tagsNumber = ta->GetNumberOfTags();
  CPPUNIT_ASSERT(tagsNumber == 1);

  std::string tagValue;
  ta->GetTag("pippo")->GetValueAsSingleString(tagValue);

  CPPUNIT_ASSERT(strcmp(tagValue.c_str(),"(\"pluto\")") == 0);
  
  // current time for test
  m_CurrentTime = 0.0;
  emptyStorageRoot->AddChild(vmeEmptySurface);

  // set the tree time
  emptyStorageRoot->SetTreeTime(m_CurrentTime);

  // perform some checks on the storage tree
  CPPUNIT_ASSERT(emptyStorageRoot->GetNumberOfChildren() == 1);
  CPPUNIT_ASSERT(0 == strcmp(emptyStorageRoot->GetChild(0)->GetName(), "empty surface vme"));
  
  albaVMESurface* shouldBeTheEmptyVme = albaVMESurface::SafeDownCast(emptyStorageRoot->GetChild(0));
  CPPUNIT_ASSERT(shouldBeTheEmptyVme);

  // reference count
  // this returns 2
  int shouldBeTheEmptyVmeRC = shouldBeTheEmptyVme->GetReferenceCount();

  CPPUNIT_ASSERT(shouldBeTheEmptyVme->GetAbsMatrixPipe());

  CPPUNIT_ASSERT(emptyStorage.Store() == ALBA_OK);	

  emptyStorageRoot->CleanTree();

  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  CPPUNIT_ASSERT(ALBA_OK == emptyStorage.Restore());

  CPPUNIT_ASSERT(emptyStorageRoot->GetReferenceCount() == 1);

  // set the Tree time
  emptyStorageRoot->SetTreeTime(m_CurrentTime);

  // perform some checks on the storage tree
  CPPUNIT_ASSERT(emptyStorageRoot->GetNumberOfChildren() == 1);
  CPPUNIT_ASSERT(0 == strcmp(emptyStorageRoot->GetChild(0)->GetName(), "empty surface vme"));

  albaTagArray *reloadTA = emptyStorageRoot->GetChild(0)->GetTagArray();
  CPPUNIT_ASSERT(reloadTA);
  tagsNumber = reloadTA->GetNumberOfTags();
  CPPUNIT_ASSERT(tagsNumber == 1);
  
  std::string reloadTagValue;
  reloadTA->GetTag("pippo")->GetValueAsSingleString(reloadTagValue);
  
  CPPUNIT_ASSERT(strcmp(reloadTagValue.c_str(),"(\"pluto\")") == 0);

  // cleanup created directories   
  bool removeSuccessful = true;

  // remove folder for test file
  removeSuccessful = RemoveDir(emptyVMEStorageDir);
  CPPUNIT_ASSERT(removeSuccessful);
}