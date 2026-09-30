/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGenericTest
 Authors: Roberto Mucci
 
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

#include "albaVMEGenericTest.h"
#include <wx/dir.h>

#include "albaVMEGeneric.h"
#include "albaVMEFactory.h"
#include "albaCoreFactory.h"
#include "albaVMEIterator.h"
#include "albaVMEPolyline.h"
#include "albaTransform.h"
#include "albaVMEItem.h"
#include "albaDataVector.h"
#include "albaSmartPointer.h"


#include "vtkPointData.h"
#include "vtkALBASmartPointer.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkDoubleArray.h"
#include "vtkPolyData.h"

#include <iostream>
#include <set>


//----------------------------------------------------------------------------
void albaVMEGenericTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEGeneric *vmeGeneric;
  albaNEW(vmeGeneric);
  albaDEL(vmeGeneric);
}

//----------------------------------------------------------------------------
void albaVMEGenericTest::TestSetData()
//----------------------------------------------------------------------------
{
  // create some VTK data
  vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::New() ;

  // create rectilinear grid data
   vtkALBASmartPointer<vtkDoubleArray> xCoordinates;
  xCoordinates->SetNumberOfValues(5);
  xCoordinates->SetValue(0, 0);
  xCoordinates->SetValue(1, 1);
  xCoordinates->SetValue(2, 2);
  xCoordinates->SetValue(3, 1);
  xCoordinates->SetValue(4, 0); 

  vtkALBASmartPointer<vtkDoubleArray> yCoordinates;
  yCoordinates->SetNumberOfValues(5);
  yCoordinates->SetValue(0, 0);
  yCoordinates->SetValue(1, 1);
  yCoordinates->SetValue(2, 2);
  yCoordinates->SetValue(3, 1);
  yCoordinates->SetValue(4, 0); 

  vtkALBASmartPointer<vtkDoubleArray> zCoordinates;
  zCoordinates->SetNumberOfValues(5);
  zCoordinates->SetValue(0, 0);
  zCoordinates->SetValue(1, 1);
  zCoordinates->SetValue(2, 2);
  zCoordinates->SetValue(3, 1);
  zCoordinates->SetValue(4, 0); 
  
  rectilinearGrid->SetDimensions(5,5,5);
  rectilinearGrid->SetXCoordinates(xCoordinates);
  rectilinearGrid->SetYCoordinates(yCoordinates);
  rectilinearGrid->SetZCoordinates(zCoordinates);


  albaVMEGeneric *vmeGeneric;
  albaNEW(vmeGeneric);

  int result = false;
  result = vmeGeneric->SetData(rectilinearGrid,0);
  CPPUNIT_ASSERT(result == ALBA_OK);

  vtkALBASmartPointer<vtkRectilinearGrid> rectilinearGridA;
  rectilinearGridA = (vtkRectilinearGrid*)vmeGeneric->GetOutput()->GetVTKData();

  result = true;
  for (int i = 0 ; i < 5; i++)
  {
    result = result && albaEquals(((vtkDoubleArray*)rectilinearGridA->GetXCoordinates())->GetValue(i),xCoordinates->GetValue(i)) &&
      albaEquals(((vtkDoubleArray*)rectilinearGridA->GetYCoordinates())->GetValue(i),yCoordinates->GetValue(i)) &&
      albaEquals(((vtkDoubleArray*)rectilinearGridA->GetZCoordinates())->GetValue(i),zCoordinates->GetValue(i)); 
  }
  CPPUNIT_ASSERT(result);


  result = false;
  result = vmeGeneric->SetDataByReference(rectilinearGrid,0);
  CPPUNIT_ASSERT(result == ALBA_OK);

  vtkALBASmartPointer<vtkRectilinearGrid> rectilinearGridB;
  rectilinearGridB = (vtkRectilinearGrid*)vmeGeneric->GetOutput()->GetVTKData();

  result = true;
  for (int i = 0 ; i < 5; i++)
  {
    result = result && albaEquals(((vtkDoubleArray*)rectilinearGridB->GetXCoordinates())->GetValue(i),xCoordinates->GetValue(i)) &&
      albaEquals(((vtkDoubleArray*)rectilinearGridB->GetYCoordinates())->GetValue(i),yCoordinates->GetValue(i)) &&
      albaEquals(((vtkDoubleArray*)rectilinearGridB->GetZCoordinates())->GetValue(i),zCoordinates->GetValue(i)); 
  }
  CPPUNIT_ASSERT(result);

  result = false;
  result = vmeGeneric->SetDataByDetaching(rectilinearGrid,0);
  CPPUNIT_ASSERT(result == ALBA_OK);

  vtkALBASmartPointer<vtkRectilinearGrid> rectilinearGridC;
  rectilinearGridC = (vtkRectilinearGrid*)vmeGeneric->GetOutput()->GetVTKData();

  result = true;
  for (int i = 0 ; i < 5; i++)
  {
    result = result && albaEquals(((vtkDoubleArray*)rectilinearGridC->GetXCoordinates())->GetValue(i),xCoordinates->GetValue(i)) &&
    albaEquals(((vtkDoubleArray*)rectilinearGridC->GetYCoordinates())->GetValue(i),yCoordinates->GetValue(i)) &&
    albaEquals(((vtkDoubleArray*)rectilinearGridC->GetZCoordinates())->GetValue(i),zCoordinates->GetValue(i)); 
  }

  CPPUNIT_ASSERT(result);
  vtkDEL(rectilinearGrid);
  albaDEL(vmeGeneric);
}

//----------------------------------------------------------------------------
void albaVMEGenericTest::TestGetVisualPipe()
//----------------------------------------------------------------------------
{
  albaVMEGeneric *vmeGeneric;
  albaNEW(vmeGeneric);
  int result =  false;
  result = vmeGeneric->GetVisualPipe() == "albaPipeBox";
  
  CPPUNIT_ASSERT(result);

  albaDEL(vmeGeneric);
}
