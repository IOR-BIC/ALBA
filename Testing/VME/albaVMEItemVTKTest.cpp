/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEItemVTKTest
 Authors: mgiacom
 
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
#include "albaVMEItemVTKTest.h"

#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "albaVMEItemVTK.h"
#include "albaString.h"
#include "albaVMEStorage.h"
#include "albaEventIO.h"
#include "albaObserver.h"

#include "vtkSphereSource.h"
#include "vtkPolyData.h"
#include "vtkDataSet.h"
#include "vtkCubeSource.h"


#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
class DummyObserver : public albaObserver
//----------------------------------------------------------------------------
{
public:

  DummyObserver() 
  {
    m_Storage = new albaVMEStorage();
  };
  ~DummyObserver()
  {
    delete m_Storage;
  };

  albaStorage* GetStorage()
  {
    return m_Storage;
  };

  virtual void OnEvent(albaEventBase *alba_event)
  {
    if(alba_event->GetId() == NODE_GET_STORAGE)
    {
      albaEventIO *io_event=albaEventIO::SafeDownCast(alba_event);
      io_event->SetStorage(m_Storage);
    }
  }

protected:
  albaVMEStorage *m_Storage;
};
//----------------------------------------------------------------------------
void albaVMEItemVTKTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEItemVTKTest::TestSetData()
//----------------------------------------------------------------------------
{
  albaVMEItemVTK *item;
  albaNEW(item);

  vtkSphereSource *sphere;
  vtkNEW(sphere);
	sphere->Update();

  item->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  result = item->GetData()==vtkDataSet::SafeDownCast(sphere->GetOutput()) ? true : false;

  TEST_RESULT;

  double bItem[6],bSphere[6];
  item->GetBounds(bItem);

  sphere->GetOutput()->GetBounds(bSphere);

  result=true;
  for (int i=0;i<6;i++)
  {
    result = (bItem[i] == bSphere[i]) && result;
  }

  TEST_RESULT

  vtkDEL(sphere);
  albaDEL(item);
}
//----------------------------------------------------------------------------
void albaVMEItemVTKTest::TestEquals()
//----------------------------------------------------------------------------
{
  albaVMEItemVTK *itemSphere;
  albaNEW(itemSphere);

  vtkSphereSource *sphere;
  vtkNEW(sphere);
	sphere->Update();

  itemSphere->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  albaVMEItemVTK *itemCube;
  albaNEW(itemCube);

  vtkCubeSource *cube;
  vtkNEW(cube);
	cube->Update();

  itemCube->SetData(vtkDataSet::SafeDownCast(cube->GetOutput()));

  // Two items VTK with a different vtk data should be equals
  result = !itemSphere->Equals(itemCube);

  TEST_RESULT;

  albaVMEItemVTK *itemSphereToCompare;
  albaNEW(itemSphereToCompare);

  vtkSphereSource *sphereToCompare;
  vtkNEW(sphereToCompare);
	sphereToCompare->Update();

  itemSphereToCompare->SetData(vtkDataSet::SafeDownCast(sphereToCompare->GetOutput()));

  // Two items VTK with an equal vtk data should be equals
  result = itemSphere->Equals(itemSphereToCompare);

  TEST_RESULT;

  vtkDEL(sphereToCompare);
  albaDEL(itemSphereToCompare);

  vtkDEL(cube);
  albaDEL(itemCube);

  vtkDEL(sphere);
  albaDEL(itemSphere);
}
//----------------------------------------------------------------------------
void albaVMEItemVTKTest::TestIsDataPresent()
//----------------------------------------------------------------------------
{
  albaVMEItemVTK *itemSphere;
  albaNEW(itemSphere);

  vtkSphereSource *sphere;
  vtkNEW(sphere);
	sphere->Update();

  result = !itemSphere->IsDataPresent();
  TEST_RESULT;

  itemSphere->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  result = itemSphere->IsDataPresent();
  TEST_RESULT;

  itemSphere->ReleaseData();

  result = !itemSphere->IsDataPresent();
  TEST_RESULT;

  vtkDEL(sphere);
  albaDEL(itemSphere);
}
//----------------------------------------------------------------------------
void albaVMEItemVTKTest::TestGetDataFileExtension()
//----------------------------------------------------------------------------
{
  albaVMEItemVTK *item;
  albaNEW(item);

  result = (bool)strcmp(item->GetDataFileExtension(),".vtk");

  TEST_RESULT;

  albaDEL(item);
}
//----------------------------------------------------------------------------
void albaVMEItemVTKTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  albaVMEItemVTK *item1;
  albaNEW(item1);

  vtkSphereSource *sphere;
  vtkNEW(sphere);
	sphere->Update();

  item1->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  albaVMEItemVTK *item2;
  albaNEW(item2);

  item2->DeepCopy(item1);

  vtkPolyData *item1Data = vtkPolyData::SafeDownCast(item1->GetData());
  vtkPolyData *item2Data = vtkPolyData::SafeDownCast(item2->GetData());

  result = item1Data->GetNumberOfPoints() == item2Data->GetNumberOfPoints();
  TEST_RESULT;

  result = item1Data->GetNumberOfCells() == item2Data->GetNumberOfCells();
  TEST_RESULT;
  
  double b1[6],b2[6];
  item1Data->GetBounds(b1);
  item2Data->GetBounds(b2);

  result=true;
  for (int i=0;i<6;i++)
  {
    result = (b1[i] == b2[i]) && result;
  }
  TEST_RESULT;

  albaDEL(item2);
  vtkDEL(sphere);
  albaDEL(item1);
}
//----------------------------------------------------------------------------
void albaVMEItemVTKTest::TestShallowCopy()
//----------------------------------------------------------------------------
{
  albaVMEItemVTK *item1;
  albaNEW(item1);

  vtkSphereSource *sphere;
  vtkNEW(sphere);
	sphere->Update();

  item1->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  albaVMEItemVTK *item2;
  albaNEW(item2);

  item2->ShallowCopy(item1);

  vtkPolyData *item1Data = vtkPolyData::SafeDownCast(item1->GetData());
  vtkPolyData *item2Data = vtkPolyData::SafeDownCast(item2->GetData());

  result = item1Data == item2Data;
  TEST_RESULT;

  albaDEL(item2);
  vtkDEL(sphere);
  albaDEL(item1);
}
//----------------------------------------------------------------------------
void albaVMEItemVTKTest::TestReadData()
//----------------------------------------------------------------------------
{
  albaVMEItemVTK *item;
  albaNEW(item);

  albaString filename = ALBA_DATA_ROOT;
  filename<<"/ItemVTK/Polyline.vtk";

  item->SetDataType("vtkPolyData");
  item->ReadData(filename);
  result = item->IsDataPresent();

  TEST_RESULT;

  vtkPolyData *itemData = vtkPolyData::SafeDownCast(item->GetData());
  result = itemData->GetNumberOfPoints() == 5;
  TEST_RESULT;

  result = itemData->GetNumberOfCells() == 4;
  TEST_RESULT;

  result = itemData->GetNumberOfLines() == 4;
  TEST_RESULT;

  albaDEL(item);
}
//----------------------------------------------------------------------------
void albaVMEItemVTKTest::TestInternalStoreData()
//----------------------------------------------------------------------------
{
  albaVMEItemVTK *item;
  albaNEW(item);

  DummyObserver *observer = new DummyObserver();

  item->SetListener(observer);

  vtkSphereSource *sphere;
  vtkNEW(sphere);
	sphere->Update();

  item->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  albaString filename = GET_TEST_DATA_DIR();
  filename<<"/item.vtk";
  result = item->StoreData(filename.GetCStr()) == ALBA_OK;

  TEST_RESULT;

  albaVMEItemVTK *itemRead;
  albaNEW(itemRead);

  itemRead->SetDataType("vtkPolyData");
  itemRead->ReadData(filename);

  vtkPolyData *itemReadData = vtkPolyData::SafeDownCast(itemRead->GetData());
  vtkPolyData *itemData = vtkPolyData::SafeDownCast(item->GetData());

  result = itemReadData->GetNumberOfPoints() == itemData->GetNumberOfPoints();
  TEST_RESULT;

  result = itemReadData->GetNumberOfCells() == itemData->GetNumberOfCells();
  TEST_RESULT;

  double b1[6],b2[6];
  itemReadData->GetBounds(b1);
  item->GetBounds(b2);

  result=true;
  for (int i=0;i<6;i++)
  {
    result = (b1[i] == b2[i]) && result;
  }
  TEST_RESULT;

  albaDEL(itemRead);
  vtkDEL(sphere);
  albaDEL(item);
  delete observer;
}
//----------------------------------------------------------------------------
void albaVMEItemVTKTest::TestStoreToArchive()
//----------------------------------------------------------------------------
{
  albaVMEItemVTK *item;
  albaNEW(item);

  DummyObserver *observer = new DummyObserver();

  item->SetListener(observer);

  vtkSphereSource *sphere;
  vtkNEW(sphere);
	sphere->Update();

  item->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  albaString filename = ALBA_DATA_ROOT;
  filename<<"/ItemVTK/item.vtk";

  albaString filenameZip = GET_TEST_DATA_DIR();
  filenameZip<<"/item.zvtk";
  
  albaString workDir = ALBA_DATA_ROOT;
  workDir<<"/ItemVTK";
  wxSetWorkingDirectory(workDir.GetCStr());

  albaString tmp_archive;
  observer->GetStorage()->GetTmpFile(tmp_archive);

  wxFileOutputStream out(tmp_archive.GetCStr());
  wxZipOutputStream zip(out);

  item->SetIOModeToMemory();
  item->StoreData(NULL);
  item->SetURL("item.vtk");
  item->ReleaseOldFileOn();
  item->SetArchiveFileName(filenameZip);

  result = item->StoreToArchive(zip);

  if (!zip.Close() || !out.Close())
  {
    result = false;
  }

  TEST_RESULT;

  result = observer->GetStorage()->StoreToURL(tmp_archive, filenameZip) == ALBA_OK;

  TEST_RESULT;

  albaVMEItemVTK *itemRead;
  albaNEW(itemRead);

  itemRead->SetURL("item.vtk");
  itemRead->SetDataType("vtkPolyData");
  itemRead->SetIOModeToMemory();
  itemRead->ReadData(filenameZip);

  vtkPolyData *itemReadData = vtkPolyData::SafeDownCast(itemRead->GetData());
  vtkPolyData *itemData = vtkPolyData::SafeDownCast(item->GetData());

  result = itemReadData->GetNumberOfPoints() == itemData->GetNumberOfPoints();
  TEST_RESULT;

  result = itemReadData->GetNumberOfCells() == itemData->GetNumberOfCells();
  TEST_RESULT;

  double b1[6],b2[6];
  itemReadData->GetBounds(b1);
  item->GetBounds(b2);

  result=true;
  for (int i=0;i<6;i++)
  {
    result = (b1[i] == b2[i]) && result;
  }
  TEST_RESULT;

  albaDEL(itemRead);

  vtkDEL(sphere);
  albaDEL(item);
  delete observer;

}
