/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputVolumeTest
 Authors: Daniele Giunchi
 
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
#include "albaVMEOutputVolumeTest.h"

#include "albaVMEOutputVolume.h"
#include "albaVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"

#include "vtkALBASmartPointer.h"

#include "albaMatrix.h"
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
void albaVMEOutputVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputVolumeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputVolume outputVolume;
}
//----------------------------------------------------------------------------
void albaVMEOutputVolumeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputVolume *outputVolume = new albaVMEOutputVolume();
  cppDEL(outputVolume);
}
//----------------------------------------------------------------------------
void albaVMEOutputVolumeTest::TestGetStructuredData()
//----------------------------------------------------------------------------
{
  const int dimension = 5;
  //create a vme volume gray with imagedata inside

  vtkALBASmartPointer<vtkIntArray> array;
  int i = 0;
  for(;i<dimension*dimension*dimension - 1; i++)
  {
    array->InsertNextTuple1(i);
  }
  array->SetName("scalars");

  vtkALBASmartPointer<vtkImageData> imageData;
  imageData->SetDimensions(dimension,dimension,dimension);
  imageData->SetSpacing(1.,1.,1.);
  imageData->GetPointData()->AddArray(array);
  imageData->GetPointData()->SetActiveScalars("scalars");

  albaVMEVolumeGray *vmeVolume = NULL;
  albaNEW(vmeVolume);
  vmeVolume->SetData(imageData, 0.);
  vmeVolume->Update();

  albaVMEOutputVolume *outputVolume = NULL;
  outputVolume = vmeVolume->GetVolumeOutput();
  m_Result = outputVolume->GetStructuredData() == vmeVolume->GetOutput()->GetVTKData();

  TEST_RESULT;

  albaDEL(vmeVolume);
  
}
//----------------------------------------------------------------------------
void albaVMEOutputVolumeTest::TestGetRectilinearData()
//----------------------------------------------------------------------------
{
  const int dimension = 5;
  //create a vme volume gray with rectilinear grid inside

  vtkALBASmartPointer<vtkIntArray> array;
  int i = 0;
  for(;i<dimension*dimension*dimension - 1; i++)
  {
    array->InsertNextTuple1(i);
  }
  array->SetName("scalars");

  // create rectilinear grid data
  vtkALBASmartPointer<vtkFloatArray> xCoordinates;
  xCoordinates->SetNumberOfValues(dimension);
  xCoordinates->SetValue(0, 0.f);
  xCoordinates->SetValue(1, 1.f);
  xCoordinates->SetValue(2, 2.f);
  xCoordinates->SetValue(3, 1.f);
  xCoordinates->SetValue(4, 0.f); 

  vtkALBASmartPointer<vtkFloatArray> yCoordinates;
  yCoordinates->SetNumberOfValues(dimension);
  yCoordinates->SetValue(0, 0.f);
  yCoordinates->SetValue(1, 1.f);
  yCoordinates->SetValue(2, 2.f);
  yCoordinates->SetValue(3, 1.f);
  yCoordinates->SetValue(4, 0.f); 

  vtkALBASmartPointer<vtkFloatArray> zCoordinates;
  zCoordinates->SetNumberOfValues(dimension);
  zCoordinates->SetValue(0, 0.f);
  zCoordinates->SetValue(1, 1.f);
  zCoordinates->SetValue(2, 2.f);
  zCoordinates->SetValue(3, 1.f);
  zCoordinates->SetValue(4, 0.f); 

  vtkALBASmartPointer<vtkRectilinearGrid> rectilinearGrid;
  rectilinearGrid->SetDimensions(dimension,dimension,dimension);
  rectilinearGrid->SetXCoordinates(xCoordinates);
  rectilinearGrid->SetYCoordinates(yCoordinates);
  rectilinearGrid->SetZCoordinates(zCoordinates);

  rectilinearGrid->GetPointData()->AddArray(array);
  rectilinearGrid->Modified();

  albaVMEVolumeGray *vmeVolume = NULL;
  albaNEW(vmeVolume);
  vmeVolume->SetData(rectilinearGrid, 0.);
  vmeVolume->Update();

  albaVMEOutputVolume *outputVolume = NULL;
  outputVolume = vmeVolume->GetVolumeOutput();
  m_Result = outputVolume->GetRectilinearData() == vmeVolume->GetOutput()->GetVTKData();

  TEST_RESULT;

  albaDEL(vmeVolume);
}
//----------------------------------------------------------------------------
void albaVMEOutputVolumeTest::TestGetUnstructuredData()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPoints> voxelPoints;
  voxelPoints->SetNumberOfPoints(8);
  voxelPoints->InsertPoint(0, 0, 0, 0);
  voxelPoints->InsertPoint(1, 1, 0, 0);
  voxelPoints->InsertPoint(2, 0, 1, 0);
  voxelPoints->InsertPoint(3, 1, 1, 0);
  voxelPoints->InsertPoint(4, 0, 0, 1);
  voxelPoints->InsertPoint(5, 1, 0, 1);
  voxelPoints->InsertPoint(6, 0, 1, 1);
  voxelPoints->InsertPoint(7, 1, 1, 1);
  vtkALBASmartPointer<vtkVoxel> aVoxel;
  aVoxel->GetPointIds()->SetId(0, 0);
  aVoxel->GetPointIds()->SetId(1, 1);
  aVoxel->GetPointIds()->SetId(2, 2);
  aVoxel->GetPointIds()->SetId(3, 3);
  aVoxel->GetPointIds()->SetId(4, 4);
  aVoxel->GetPointIds()->SetId(5, 5);
  aVoxel->GetPointIds()->SetId(6, 6);
  aVoxel->GetPointIds()->SetId(7, 7);
  vtkALBASmartPointer<vtkUnstructuredGrid> aVoxelGrid;
  aVoxelGrid->Allocate(1, 1);
  aVoxelGrid->InsertNextCell(aVoxel->GetCellType(), aVoxel->GetPointIds());
  aVoxelGrid->SetPoints(voxelPoints);

  albaVMEVolumeGray *vmeVolume = NULL;
  albaNEW(vmeVolume);
  vmeVolume->albaVMEVolume::SetData(aVoxelGrid, 0.);
  vmeVolume->Update();

  albaVMEOutputVolume *outputVolume = NULL;
  outputVolume = vmeVolume->GetVolumeOutput();
  m_Result = outputVolume->GetUnstructuredData() == vmeVolume->GetOutput()->GetVTKData();

  TEST_RESULT;

  albaDEL(vmeVolume);
}
//----------------------------------------------------------------------------
void albaVMEOutputVolumeTest::TestSetGetVolumeMaterial()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  const int dimension = 5;
  //create a vme volume gray with imagedata inside

  vtkALBASmartPointer<vtkIntArray> array;
  int i = 0;
  for(;i<dimension*dimension*dimension - 1; i++)
  {
    array->InsertNextTuple1(i);
  }
  array->SetName("scalars");

  vtkALBASmartPointer<vtkImageData> imageData;
  imageData->SetDimensions(dimension,dimension,dimension);
  imageData->SetSpacing(1.,1.,1.);
  imageData->GetPointData()->AddArray(array);
  imageData->GetPointData()->SetActiveScalars("scalars");

  albaVMEVolumeGray *vmeVolume = NULL;
  albaNEW(vmeVolume);
  vmeVolume->SetData(imageData, 0.);
  vmeVolume->Update();

  albaVMEOutputVolume *outputVolume = vmeVolume->GetVolumeOutput();
  mmaVolumeMaterial *material;

  m_Result = vmeVolume->GetMaterial() == outputVolume->GetMaterial();
  TEST_RESULT;
  material = outputVolume->GetMaterial();


  m_Result = outputVolume->GetMaterial() == material && vmeVolume->GetMaterial() == material;
  TEST_RESULT;
  

  albaDEL(vmeVolume);
}
//----------------------------------------------------------------------------
void albaVMEOutputVolumeTest::TestGetVTKDataTypeAsString_Update()
//----------------------------------------------------------------------------
{
  const int dimension = 5;
  //create a vme volume gray with imagedata inside

  vtkALBASmartPointer<vtkIntArray> array;
  int i = 0;
  for(;i<dimension*dimension*dimension - 1; i++)
  {
    array->InsertNextTuple1(i);
  }
  array->SetName("scalars");

  vtkALBASmartPointer<vtkImageData> imageData;
  imageData->SetDimensions(dimension,dimension,dimension);
  imageData->SetSpacing(1.,1.,1.);
  imageData->GetPointData()->AddArray(array);
  imageData->GetPointData()->SetActiveScalars("scalars");

  albaVMEVolumeGray *vmeVolume = NULL;
  albaNEW(vmeVolume);
  vmeVolume->SetData(imageData, 0.);
  vmeVolume->Update();

  albaString className = vmeVolume->GetOutput()->GetVTKData()->GetClassName();

  albaVMEOutputVolume *outputVolume = NULL;
  outputVolume = vmeVolume->GetVolumeOutput();
  
  m_Result = !className.Equals(outputVolume->GetVTKDataTypeAsString());
  TEST_RESULT;

  outputVolume->Update();
  m_Result = className.Equals(outputVolume->GetVTKDataTypeAsString());
  TEST_RESULT;
	
	albaDEL(vmeVolume);
}