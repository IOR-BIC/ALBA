/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVolumeRGBTest
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
#include "albaVMEVolumeRGBTest.h"
#include <iostream>

#include "albaVMEGeneric.h"

#include "vtkPointData.h"
#include "vtkALBASmartPointer.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkFloatArray.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
void albaVMEVolumeRGBTest::BeforeTest()
//----------------------------------------------------------------------------
{
  // create vme volume
   albaNEW(m_VmeVolumeRGB);
}
//----------------------------------------------------------------------------
void albaVMEVolumeRGBTest::AfterTest()
//----------------------------------------------------------------------------
{
  // destroy vme
  albaDEL(m_VmeVolumeRGB);
}

//----------------------------------------------------------
int albaVMEVolumeRGBTest::CreateVolumeWithImageData()
//----------------------------------------------------------
{
  // create some VTK data
  
  vtkALBASmartPointer<vtkFloatArray> scalarsR;
  vtkALBASmartPointer<vtkFloatArray> scalarsG;
  vtkALBASmartPointer<vtkFloatArray> scalarsB;
  scalarsR->SetNumberOfComponents(1);
  scalarsG->SetNumberOfComponents(1);
  scalarsB->SetNumberOfComponents(1);
  for (int s = 0; s < 25; s++)
  {
    scalarsR->InsertNextValue(s * 1.0);
    scalarsG->InsertNextValue(s * 2.0);
    scalarsB->InsertNextValue(s * 3.0);
  }

  // create structured data
  vtkImageData *imageData = vtkImageData::New();
  imageData->SetDimensions(5, 5, 5);
  imageData->SetOrigin(-1, -1, -1);
  imageData->SetSpacing(1,1,1);

  imageData->GetPointData()->AddArray(scalarsR);
  imageData->GetPointData()->AddArray(scalarsG);
  imageData->GetPointData()->AddArray(scalarsB);

  // try to set this data to the volume
  int returnValue = -1;
  returnValue = m_VmeVolumeRGB->SetData(imageData, 0);


  // destroy VTK data
  imageData->Delete();

  return returnValue;
}
//----------------------------------------------------------
int albaVMEVolumeRGBTest::CreateVolumeWithRectilinearGrid()
//----------------------------------------------------------
{
  // create some VTK data
  vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::New() ;

  vtkALBASmartPointer<vtkFloatArray> scalarsR;
  vtkALBASmartPointer<vtkFloatArray> scalarsG;
  vtkALBASmartPointer<vtkFloatArray> scalarsB;
  scalarsR->SetNumberOfComponents(1);
  scalarsG->SetNumberOfComponents(1);
  scalarsB->SetNumberOfComponents(1);
  for (int s = 0; s < 25; s++)
  {
    scalarsR->InsertNextValue(s * 1.0);
    scalarsG->InsertNextValue(s * 2.0);
    scalarsB->InsertNextValue(s * 3.0);
  }

  // create rectilinear grid data
  vtkFloatArray *xCoordinates = vtkFloatArray::New();
  xCoordinates->SetNumberOfValues(5);
  xCoordinates->SetValue(0, 0.f);
  xCoordinates->SetValue(1, 1.f);
  xCoordinates->SetValue(2, 2.f);
  xCoordinates->SetValue(3, 1.f);
  xCoordinates->SetValue(4, 0.f); 

  vtkFloatArray *yCoordinates = vtkFloatArray::New();
  yCoordinates->SetNumberOfValues(5);
  yCoordinates->SetValue(0, 0.f);
  yCoordinates->SetValue(1, 1.f);
  yCoordinates->SetValue(2, 2.f);
  yCoordinates->SetValue(3, 1.f);
  yCoordinates->SetValue(4, 0.f); 

  vtkFloatArray *zCoordinates = vtkFloatArray::New();
  zCoordinates->SetNumberOfValues(5);
  zCoordinates->SetValue(0, 0.f);
  zCoordinates->SetValue(1, 1.f);
  zCoordinates->SetValue(2, 2.f);
  zCoordinates->SetValue(3, 1.f);
  zCoordinates->SetValue(4, 0.f); 

  rectilinearGrid->SetDimensions(5,5,5);
  rectilinearGrid->SetXCoordinates(xCoordinates);
  rectilinearGrid->SetYCoordinates(yCoordinates);
  rectilinearGrid->SetZCoordinates(zCoordinates);

  rectilinearGrid->GetPointData()->AddArray(scalarsR);
  rectilinearGrid->GetPointData()->AddArray(scalarsG);
  rectilinearGrid->GetPointData()->AddArray(scalarsB);
  rectilinearGrid->Modified();

  // try to set this data to the volume
  int returnValue = -1;
  returnValue = m_VmeVolumeRGB->SetData(rectilinearGrid, 0);


  // destroy VTK data
  rectilinearGrid->Delete();
  xCoordinates->Delete();
  yCoordinates->Delete();
  zCoordinates->Delete();

  return returnValue;
}

//----------------------------------------------------------
void albaVMEVolumeRGBTest::TestSetDataWithImageData()
//----------------------------------------------------------
{
  int returnValue = CreateVolumeWithImageData();

  //imageData control
  CPPUNIT_ASSERT(returnValue == ALBA_OK);
  CPPUNIT_ASSERT(m_VmeVolumeRGB->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfComponents() == 3);
  CPPUNIT_ASSERT(m_VmeVolumeRGB->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfArrays() == 3);
}

//----------------------------------------------------------
void albaVMEVolumeRGBTest::TestSetDataWithRectilinearGrid()
//----------------------------------------------------------
{
  int returnValue = CreateVolumeWithRectilinearGrid();

  //rectilinearGrid control
  CPPUNIT_ASSERT(returnValue == ALBA_OK);
  CPPUNIT_ASSERT(m_VmeVolumeRGB->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfComponents() == 3);
  CPPUNIT_ASSERT(m_VmeVolumeRGB->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfArrays() == 3);

}

//----------------------------------------------------------
void albaVMEVolumeRGBTest::TestGetVolumeOutput()
//----------------------------------------------------------
{
  CreateVolumeWithImageData();
  CPPUNIT_ASSERT(m_VmeVolumeRGB->GetVolumeOutput() != NULL);
}
//----------------------------------------------------------
void albaVMEVolumeRGBTest::TestGetOutput()
//----------------------------------------------------------
{
  CreateVolumeWithImageData();
  CPPUNIT_ASSERT(m_VmeVolumeRGB->GetOutput() != NULL);
}
