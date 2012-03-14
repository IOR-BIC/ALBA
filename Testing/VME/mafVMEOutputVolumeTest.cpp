/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEOutputVolumeTest.cpp,v $
Language:  C++
Date:      $Date: 2009-04-27 10:40:24 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
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
#include "mafVMEOutputVolumeTest.h"

#include "mafVMEOutputVolume.h"
#include "mafVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"

#include "vtkMAFSmartPointer.h"

#include "mafMatrix.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkUnstructuredGrid.h"
#include "vtkIntArray.h"
#include "vtkFloatArray.h"
#include "vtkVoxel.h"

#include "vtkPointData.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void mafVMEOutputVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputVolumeTest::setUp()
//----------------------------------------------------------------------------
{
	m_Result = false;
}
//----------------------------------------------------------------------------
void mafVMEOutputVolumeTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputVolumeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputVolume outputVolume;
}
//----------------------------------------------------------------------------
void mafVMEOutputVolumeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputVolume *outputVolume = new mafVMEOutputVolume();
  cppDEL(outputVolume);
}
//----------------------------------------------------------------------------
void mafVMEOutputVolumeTest::TestGetStructuredData()
//----------------------------------------------------------------------------
{
  const int dimension = 5;
  //create a vme volume gray with imagedata inside

  vtkMAFSmartPointer<vtkIntArray> array;
  int i = 0;
  for(;i<dimension*dimension*dimension - 1; i++)
  {
    array->InsertNextTuple1(i);
  }
  array->SetName("scalars");

  vtkMAFSmartPointer<vtkImageData> imageData;
  imageData->SetDimensions(dimension,dimension,dimension);
  imageData->SetSpacing(1.,1.,1.);
  imageData->GetPointData()->AddArray(array);
  imageData->GetPointData()->SetActiveScalars("scalars");
  imageData->UpdateData();

  mafVMEVolumeGray *vmeVolume = NULL;
  mafNEW(vmeVolume);
  vmeVolume->SetData(imageData, 0.);
  vmeVolume->GetOutput()->GetVTKData()->Update();
  vmeVolume->Update();

  mafVMEOutputVolume *outputVolume = NULL;
  outputVolume = vmeVolume->GetVolumeOutput();
  m_Result = outputVolume->GetStructuredData() == vmeVolume->GetOutput()->GetVTKData();

  TEST_RESULT;

  mafDEL(vmeVolume);
  
}
//----------------------------------------------------------------------------
void mafVMEOutputVolumeTest::TestGetRectilinearData()
//----------------------------------------------------------------------------
{
  const int dimension = 5;
  //create a vme volume gray with rectilinear grid inside

  vtkMAFSmartPointer<vtkIntArray> array;
  int i = 0;
  for(;i<dimension*dimension*dimension - 1; i++)
  {
    array->InsertNextTuple1(i);
  }
  array->SetName("scalars");

  // create rectilinear grid data
  vtkMAFSmartPointer<vtkFloatArray> xCoordinates;
  xCoordinates->SetNumberOfValues(dimension);
  xCoordinates->SetValue(0, 0.f);
  xCoordinates->SetValue(1, 1.f);
  xCoordinates->SetValue(2, 2.f);
  xCoordinates->SetValue(3, 1.f);
  xCoordinates->SetValue(4, 0.f); 

  vtkMAFSmartPointer<vtkFloatArray> yCoordinates;
  yCoordinates->SetNumberOfValues(dimension);
  yCoordinates->SetValue(0, 0.f);
  yCoordinates->SetValue(1, 1.f);
  yCoordinates->SetValue(2, 2.f);
  yCoordinates->SetValue(3, 1.f);
  yCoordinates->SetValue(4, 0.f); 

  vtkMAFSmartPointer<vtkFloatArray> zCoordinates;
  zCoordinates->SetNumberOfValues(dimension);
  zCoordinates->SetValue(0, 0.f);
  zCoordinates->SetValue(1, 1.f);
  zCoordinates->SetValue(2, 2.f);
  zCoordinates->SetValue(3, 1.f);
  zCoordinates->SetValue(4, 0.f); 

  vtkMAFSmartPointer<vtkRectilinearGrid> rectilinearGrid;
  rectilinearGrid->SetDimensions(dimension,dimension,dimension);
  rectilinearGrid->SetXCoordinates(xCoordinates);
  rectilinearGrid->SetYCoordinates(yCoordinates);
  rectilinearGrid->SetZCoordinates(zCoordinates);

  rectilinearGrid->GetPointData()->AddArray(array);
  rectilinearGrid->Modified();
  rectilinearGrid->Update();

  mafVMEVolumeGray *vmeVolume = NULL;
  mafNEW(vmeVolume);
  vmeVolume->SetData(rectilinearGrid, 0.);
  vmeVolume->GetOutput()->GetVTKData()->Update();
  vmeVolume->Update();

  mafVMEOutputVolume *outputVolume = NULL;
  outputVolume = vmeVolume->GetVolumeOutput();
  m_Result = outputVolume->GetRectilinearData() == vmeVolume->GetOutput()->GetVTKData();

  TEST_RESULT;

  mafDEL(vmeVolume);
}
//----------------------------------------------------------------------------
void mafVMEOutputVolumeTest::TestGetUnstructuredData()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPoints> voxelPoints;
  voxelPoints->SetNumberOfPoints(8);
  voxelPoints->InsertPoint(0, 0, 0, 0);
  voxelPoints->InsertPoint(1, 1, 0, 0);
  voxelPoints->InsertPoint(2, 0, 1, 0);
  voxelPoints->InsertPoint(3, 1, 1, 0);
  voxelPoints->InsertPoint(4, 0, 0, 1);
  voxelPoints->InsertPoint(5, 1, 0, 1);
  voxelPoints->InsertPoint(6, 0, 1, 1);
  voxelPoints->InsertPoint(7, 1, 1, 1);
  vtkMAFSmartPointer<vtkVoxel> aVoxel;
  aVoxel->GetPointIds()->SetId(0, 0);
  aVoxel->GetPointIds()->SetId(1, 1);
  aVoxel->GetPointIds()->SetId(2, 2);
  aVoxel->GetPointIds()->SetId(3, 3);
  aVoxel->GetPointIds()->SetId(4, 4);
  aVoxel->GetPointIds()->SetId(5, 5);
  aVoxel->GetPointIds()->SetId(6, 6);
  aVoxel->GetPointIds()->SetId(7, 7);
  vtkMAFSmartPointer<vtkUnstructuredGrid> aVoxelGrid;
  aVoxelGrid->Allocate(1, 1);
  aVoxelGrid->InsertNextCell(aVoxel->GetCellType(), aVoxel->GetPointIds());
  aVoxelGrid->SetPoints(voxelPoints);

  mafVMEVolumeGray *vmeVolume = NULL;
  mafNEW(vmeVolume);
  vmeVolume->mafVMEVolume::SetData(aVoxelGrid, 0.);
  vmeVolume->GetOutput()->GetVTKData()->Update();
  vmeVolume->Update();

  mafVMEOutputVolume *outputVolume = NULL;
  outputVolume = vmeVolume->GetVolumeOutput();
  m_Result = outputVolume->GetUnstructuredData() == vmeVolume->GetOutput()->GetVTKData();

  TEST_RESULT;

  mafDEL(vmeVolume);
}
//----------------------------------------------------------------------------
void mafVMEOutputVolumeTest::TestSetGetVolumeMaterial()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  const int dimension = 5;
  //create a vme volume gray with imagedata inside

  vtkMAFSmartPointer<vtkIntArray> array;
  int i = 0;
  for(;i<dimension*dimension*dimension - 1; i++)
  {
    array->InsertNextTuple1(i);
  }
  array->SetName("scalars");

  vtkMAFSmartPointer<vtkImageData> imageData;
  imageData->SetDimensions(dimension,dimension,dimension);
  imageData->SetSpacing(1.,1.,1.);
  imageData->GetPointData()->AddArray(array);
  imageData->GetPointData()->SetActiveScalars("scalars");
  imageData->UpdateData();

  mafVMEVolumeGray *vmeVolume = NULL;
  mafNEW(vmeVolume);
  vmeVolume->SetData(imageData, 0.);
  vmeVolume->GetOutput()->GetVTKData()->Update();
  vmeVolume->Update();

  mafVMEOutputVolume *outputVolume = vmeVolume->GetVolumeOutput();
  mmaVolumeMaterial *material;

  m_Result = vmeVolume->GetMaterial() == outputVolume->GetMaterial();
  TEST_RESULT;
  material = outputVolume->GetMaterial();


  m_Result = outputVolume->GetMaterial() == material && vmeVolume->GetMaterial() == material;
  TEST_RESULT;
  

  mafDEL(vmeVolume);
}
//----------------------------------------------------------------------------
void mafVMEOutputVolumeTest::TestGetVTKDataTypeAsString_Update()
//----------------------------------------------------------------------------
{
  const int dimension = 5;
  //create a vme volume gray with imagedata inside

  vtkMAFSmartPointer<vtkIntArray> array;
  int i = 0;
  for(;i<dimension*dimension*dimension - 1; i++)
  {
    array->InsertNextTuple1(i);
  }
  array->SetName("scalars");

  vtkMAFSmartPointer<vtkImageData> imageData;
  imageData->SetDimensions(dimension,dimension,dimension);
  imageData->SetSpacing(1.,1.,1.);
  imageData->GetPointData()->AddArray(array);
  imageData->GetPointData()->SetActiveScalars("scalars");
  imageData->UpdateData();

  mafVMEVolumeGray *vmeVolume = NULL;
  mafNEW(vmeVolume);
  vmeVolume->SetData(imageData, 0.);
  vmeVolume->GetOutput()->GetVTKData()->Update();
  vmeVolume->Update();

  mafString className = vmeVolume->GetOutput()->GetVTKData()->GetClassName();

  mafVMEOutputVolume *outputVolume = NULL;
  outputVolume = vmeVolume->GetVolumeOutput();
  
  m_Result = !className.Equals(outputVolume->GetVTKDataTypeAsString());
  TEST_RESULT;

  outputVolume->Update();
  m_Result = className.Equals(outputVolume->GetVTKDataTypeAsString());
  TEST_RESULT;
	
	mafDEL(vmeVolume);
}