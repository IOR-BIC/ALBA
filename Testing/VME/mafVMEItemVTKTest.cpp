/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEItemVTKTest.cpp,v $
Language:  C++
Date:      $Date: 2008-06-05 13:58:47 $
Version:   $Revision: 1.1 $
Authors:   mgiacom
==========================================================================
  Copyright (c) 2002/2008
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
#include "mafVMEItemVTKTest.h"

#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "mafVMEItemVTK.h"
#include "mafString.h"
#include "mafVMEStorage.h"
#include "mafEventIO.h"
#include "mafObserver.h"

#include "vtkSphereSource.h"
#include "vtkPolyData.h"
#include "vtkDataSet.h"
#include "vtkCubeSource.h"


#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
class DummyObserver : public mafObserver
//----------------------------------------------------------------------------
{
public:

  DummyObserver() 
  {
    m_Storage = new mafVMEStorage();
  };
  ~DummyObserver()
  {
    delete m_Storage;
  };

  mafStorage* GetStorage()
  {
    return m_Storage;
  };

  virtual void OnEvent(mafEventBase *maf_event)
  {
    if(maf_event->GetId() == NODE_GET_STORAGE)
    {
      mafEventIO *io_event=mafEventIO::SafeDownCast(maf_event);
      io_event->SetStorage(m_Storage);
    }
  }

protected:
  mafVMEStorage *m_Storage;
};
//----------------------------------------------------------------------------
void mafVMEItemVTKTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEItemVTKTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafVMEItemVTKTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEItemVTKTest::TestSetData()
//----------------------------------------------------------------------------
{
  mafVMEItemVTK *item;
  mafNEW(item);

  vtkSphereSource *sphere;
  vtkNEW(sphere);

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
  mafDEL(item);
}
//----------------------------------------------------------------------------
void mafVMEItemVTKTest::TestEquals()
//----------------------------------------------------------------------------
{
  mafVMEItemVTK *itemSphere;
  mafNEW(itemSphere);

  vtkSphereSource *sphere;
  vtkNEW(sphere);

  itemSphere->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  mafVMEItemVTK *itemCube;
  mafNEW(itemCube);

  vtkCubeSource *cube;
  vtkNEW(cube);

  itemCube->SetData(vtkDataSet::SafeDownCast(cube->GetOutput()));

  // Two items VTK with a different vtk data should be equals
  result = !itemSphere->Equals(itemCube);

  TEST_RESULT;

  mafVMEItemVTK *itemSphereToCompare;
  mafNEW(itemSphereToCompare);

  vtkSphereSource *sphereToCompare;
  vtkNEW(sphereToCompare);

  itemSphereToCompare->SetData(vtkDataSet::SafeDownCast(sphereToCompare->GetOutput()));

  // Two items VTK with an equal vtk data should be equals
  result = itemSphere->Equals(itemSphereToCompare);

  TEST_RESULT;

  vtkDEL(sphereToCompare);
  mafDEL(itemSphereToCompare);

  vtkDEL(cube);
  mafDEL(itemCube);

  vtkDEL(sphere);
  mafDEL(itemSphere);
}
//----------------------------------------------------------------------------
void mafVMEItemVTKTest::TestIsDataPresent()
//----------------------------------------------------------------------------
{
  mafVMEItemVTK *itemSphere;
  mafNEW(itemSphere);

  vtkSphereSource *sphere;
  vtkNEW(sphere);

  result = !itemSphere->IsDataPresent();
  TEST_RESULT;

  itemSphere->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  result = itemSphere->IsDataPresent();
  TEST_RESULT;

  itemSphere->ReleaseData();

  result = !itemSphere->IsDataPresent();
  TEST_RESULT;

  vtkDEL(sphere);
  mafDEL(itemSphere);
}
//----------------------------------------------------------------------------
void mafVMEItemVTKTest::TestGetDataFileExtension()
//----------------------------------------------------------------------------
{
  mafVMEItemVTK *item;
  mafNEW(item);

  result = (bool)strcmp(item->GetDataFileExtension(),".vtk");

  TEST_RESULT;

  mafDEL(item);
}
//----------------------------------------------------------------------------
void mafVMEItemVTKTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  mafVMEItemVTK *item1;
  mafNEW(item1);

  vtkSphereSource *sphere;
  vtkNEW(sphere);

  item1->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  mafVMEItemVTK *item2;
  mafNEW(item2);

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

  mafDEL(item2);
  vtkDEL(sphere);
  mafDEL(item1);
}
//----------------------------------------------------------------------------
void mafVMEItemVTKTest::TestShallowCopy()
//----------------------------------------------------------------------------
{
  mafVMEItemVTK *item1;
  mafNEW(item1);

  vtkSphereSource *sphere;
  vtkNEW(sphere);

  item1->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  mafVMEItemVTK *item2;
  mafNEW(item2);

  item2->ShallowCopy(item1);

  vtkPolyData *item1Data = vtkPolyData::SafeDownCast(item1->GetData());
  vtkPolyData *item2Data = vtkPolyData::SafeDownCast(item2->GetData());

  result = item1Data == item2Data;
  TEST_RESULT;

  mafDEL(item2);
  vtkDEL(sphere);
  mafDEL(item1);
}
//----------------------------------------------------------------------------
void mafVMEItemVTKTest::TestReadData()
//----------------------------------------------------------------------------
{
  mafVMEItemVTK *item;
  mafNEW(item);

  mafString filename = MAF_DATA_ROOT;
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

  delete wxLog::SetActiveTarget(NULL);

  mafDEL(item);
}
//----------------------------------------------------------------------------
void mafVMEItemVTKTest::TestInternalStoreData()
//----------------------------------------------------------------------------
{
  mafVMEItemVTK *item;
  mafNEW(item);

  DummyObserver *observer = new DummyObserver();

  item->SetListener(observer);

  vtkSphereSource *sphere;
  vtkNEW(sphere);

  item->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  mafString filename = MAF_DATA_ROOT;
  filename<<"/ItemVTK/item.vtk";
  result = item->StoreData(filename.GetCStr()) == MAF_OK;

  TEST_RESULT;

  mafVMEItemVTK *itemRead;
  mafNEW(itemRead);

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

  mafDEL(itemRead);
  vtkDEL(sphere);
  mafDEL(item);
  delete observer;
}
//----------------------------------------------------------------------------
void mafVMEItemVTKTest::TestStoreToArchive()
//----------------------------------------------------------------------------
{
  mafVMEItemVTK *item;
  mafNEW(item);

  DummyObserver *observer = new DummyObserver();

  item->SetListener(observer);

  vtkSphereSource *sphere;
  vtkNEW(sphere);

  item->SetData(vtkDataSet::SafeDownCast(sphere->GetOutput()));

  mafString filename = MAF_DATA_ROOT;
  filename<<"/ItemVTK/item.vtk";
  mafString filenameZip = MAF_DATA_ROOT;
  filenameZip<<"/ItemVTK/item.zvtk";
  
  mafString workDir = MAF_DATA_ROOT;
  workDir<<"/ItemVTK";
  wxSetWorkingDirectory(workDir.GetCStr());

  mafString tmp_archive;
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

  result = observer->GetStorage()->StoreToURL(tmp_archive, filenameZip) == MAF_OK;

  TEST_RESULT;

  mafVMEItemVTK *itemRead;
  mafNEW(itemRead);

  itemRead->SetURL("item.vtk");
  itemRead->SetDataType("vtkPolyData");
  itemRead->SetIOModeToMemory();
  itemRead->ReadData(filenameZip);

  vtkPolyData *itemReadData = vtkPolyData::SafeDownCast(itemRead->GetData());
  itemReadData->Update();
  vtkPolyData *itemData = vtkPolyData::SafeDownCast(item->GetData());
  itemData->Update();

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

  mafDEL(itemRead);

  vtkDEL(sphere);
  mafDEL(item);
  delete observer;

}
