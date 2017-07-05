/*=========================================================================

 Program: MAF2
 Module: mafVMEVolumeRGBTest
 Authors: Daniele Giunchi
 
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
#include "mafVMEVolumeRGBTest.h"
#include <iostream>

#include "mafVMEGeneric.h"

#include "vtkPointData.h"
#include "vtkMAFSmartPointer.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkFloatArray.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
void mafVMEVolumeRGBTest::BeforeTest()
//----------------------------------------------------------------------------
{
  // create vme volume
   mafNEW(m_VmeVolumeRGB);
}
//----------------------------------------------------------------------------
void mafVMEVolumeRGBTest::AfterTest()
//----------------------------------------------------------------------------
{
  // destroy vme
  mafDEL(m_VmeVolumeRGB);
}

//----------------------------------------------------------
int mafVMEVolumeRGBTest::CreateVolumeWithImageData()
//----------------------------------------------------------
{
  // create some VTK data
  
  vtkMAFSmartPointer<vtkFloatArray> scalarsR;
  vtkMAFSmartPointer<vtkFloatArray> scalarsG;
  vtkMAFSmartPointer<vtkFloatArray> scalarsB;
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
int mafVMEVolumeRGBTest::CreateVolumeWithRectilinearGrid()
//----------------------------------------------------------
{
  // create some VTK data
  vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::New() ;

  vtkMAFSmartPointer<vtkFloatArray> scalarsR;
  vtkMAFSmartPointer<vtkFloatArray> scalarsG;
  vtkMAFSmartPointer<vtkFloatArray> scalarsB;
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
void mafVMEVolumeRGBTest::TestSetDataWithImageData()
//----------------------------------------------------------
{
  int returnValue = CreateVolumeWithImageData();

  //imageData control
  CPPUNIT_ASSERT(returnValue == MAF_OK);
  CPPUNIT_ASSERT(m_VmeVolumeRGB->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfComponents() == 3);
  CPPUNIT_ASSERT(m_VmeVolumeRGB->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfArrays() == 3);
}

//----------------------------------------------------------
void mafVMEVolumeRGBTest::TestSetDataWithRectilinearGrid()
//----------------------------------------------------------
{
  int returnValue = CreateVolumeWithRectilinearGrid();

  //rectilinearGrid control
  CPPUNIT_ASSERT(returnValue == MAF_OK);
  CPPUNIT_ASSERT(m_VmeVolumeRGB->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfComponents() == 3);
  CPPUNIT_ASSERT(m_VmeVolumeRGB->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfArrays() == 3);

}

//----------------------------------------------------------
void mafVMEVolumeRGBTest::TestGetVolumeOutput()
//----------------------------------------------------------
{
  CreateVolumeWithImageData();
  CPPUNIT_ASSERT(m_VmeVolumeRGB->GetVolumeOutput() != NULL);
}
//----------------------------------------------------------
void mafVMEVolumeRGBTest::TestGetOutput()
//----------------------------------------------------------
{
  CreateVolumeWithImageData();
  CPPUNIT_ASSERT(m_VmeVolumeRGB->GetOutput() != NULL);
}
