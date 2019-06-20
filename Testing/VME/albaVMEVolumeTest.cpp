/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVolumeTest
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
#include "albaVMEVolumeGrayTest.h"
#include <iostream>
#include "vtkSphereSource.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "albaVMEVolumeGray.h"
#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "albaVMEGeneric.h"

void albaVMEVolumeGrayTest::TestSetData()
{
  // create some VTK data
  vtkSphereSource *sphereSource = vtkSphereSource::New();
  sphereSource->Update();

  vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::New() ;

  // create structured data
  vtkImageData *imageData = vtkImageData::New();
  imageData->SetDimensions(5, 5, 5);
  imageData->SetOrigin(-1, -1, -1);
  imageData->SetSpacing(1,1,1);
 
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

  albaVMEVolumeGray *vmeVolumeGray;

  // create vme volume
  albaNEW(vmeVolumeGray);

  // try to set this data to the volume
  int returnValue = -1;

  returnValue = vmeVolumeGray->SetData(imageData, 0);
  CPPUNIT_ASSERT(returnValue == ALBA_OK);

  returnValue = vmeVolumeGray->SetData(rectilinearGrid, 0);
  CPPUNIT_ASSERT(returnValue == ALBA_OK);

  // destroy vme
  albaDEL(vmeVolumeGray);

  // destroy VTK data
  sphereSource->Delete();
  rectilinearGrid->Delete();
  imageData->Delete();
  xCoordinates->Delete();
  yCoordinates->Delete();
  zCoordinates->Delete();


}
  
