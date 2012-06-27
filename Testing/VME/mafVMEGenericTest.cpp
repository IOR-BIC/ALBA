/*=========================================================================

 Program: MAF2
 Module: mafVMEGenericTest
 Authors: Roberto Mucci
 
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

#include "mafVMEGenericTest.h"
#include <wx/dir.h>

#include "mafVMEGeneric.h"
#include "mafVMEFactory.h"
#include "mafCoreFactory.h"
#include "mafNodeIterator.h"
#include "mafVMEPolyline.h"
#include "mafTransform.h"
#include "mafVMEItem.h"
#include "mafDataVector.h"
#include "mafSmartPointer.h"


#include "vtkPointData.h"
#include "vtkMAFSmartPointer.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "vtkDoubleArray.h"
#include "vtkPolyData.h"

#include <iostream>
#include <set>

//----------------------------------------------------------------------------
void mafVMEGenericTest::setUp()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafVMEGenericTest::tearDown()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafVMEGenericTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEGeneric *vmeGeneric;
  mafNEW(vmeGeneric);
  mafDEL(vmeGeneric);
}

//----------------------------------------------------------------------------
void mafVMEGenericTest::TestSetData()
//----------------------------------------------------------------------------
{
  // create some VTK data
  vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::New() ;

  // create rectilinear grid data
   vtkMAFSmartPointer<vtkDoubleArray> xCoordinates;
  xCoordinates->SetNumberOfValues(5);
  xCoordinates->SetValue(0, 0);
  xCoordinates->SetValue(1, 1);
  xCoordinates->SetValue(2, 2);
  xCoordinates->SetValue(3, 1);
  xCoordinates->SetValue(4, 0); 

  vtkMAFSmartPointer<vtkDoubleArray> yCoordinates;
  yCoordinates->SetNumberOfValues(5);
  yCoordinates->SetValue(0, 0);
  yCoordinates->SetValue(1, 1);
  yCoordinates->SetValue(2, 2);
  yCoordinates->SetValue(3, 1);
  yCoordinates->SetValue(4, 0); 

  vtkMAFSmartPointer<vtkDoubleArray> zCoordinates;
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


  mafVMEGeneric *vmeGeneric;
  mafNEW(vmeGeneric);

  int result = false;
  result = vmeGeneric->SetData(rectilinearGrid,0);
  CPPUNIT_ASSERT(result == MAF_OK);

  vtkMAFSmartPointer<vtkRectilinearGrid> rectilinearGridA;
  rectilinearGridA = (vtkRectilinearGrid*)vmeGeneric->GetOutput()->GetVTKData();
  rectilinearGridA->Update();

  result = true;
  for (int i = 0 ; i < 5; i++)
  {
    result = result && mafEquals(((vtkDoubleArray*)rectilinearGridA->GetXCoordinates())->GetValue(i),xCoordinates->GetValue(i)) &&
      mafEquals(((vtkDoubleArray*)rectilinearGridA->GetYCoordinates())->GetValue(i),yCoordinates->GetValue(i)) &&
      mafEquals(((vtkDoubleArray*)rectilinearGridA->GetZCoordinates())->GetValue(i),zCoordinates->GetValue(i)); 
  }
  CPPUNIT_ASSERT(result);


  result = false;
  result = vmeGeneric->SetDataByReference(rectilinearGrid,0);
  CPPUNIT_ASSERT(result == MAF_OK);

  vtkMAFSmartPointer<vtkRectilinearGrid> rectilinearGridB;
  rectilinearGridB = (vtkRectilinearGrid*)vmeGeneric->GetOutput()->GetVTKData();
  rectilinearGridB->Update();

  result = true;
  for (int i = 0 ; i < 5; i++)
  {
    result = result && mafEquals(((vtkDoubleArray*)rectilinearGridB->GetXCoordinates())->GetValue(i),xCoordinates->GetValue(i)) &&
      mafEquals(((vtkDoubleArray*)rectilinearGridB->GetYCoordinates())->GetValue(i),yCoordinates->GetValue(i)) &&
      mafEquals(((vtkDoubleArray*)rectilinearGridB->GetZCoordinates())->GetValue(i),zCoordinates->GetValue(i)); 
  }
  CPPUNIT_ASSERT(result);

  result = false;
  result = vmeGeneric->SetDataByDetaching(rectilinearGrid,0);
  CPPUNIT_ASSERT(result == MAF_OK);

  vtkMAFSmartPointer<vtkRectilinearGrid> rectilinearGridC;
  rectilinearGridC = (vtkRectilinearGrid*)vmeGeneric->GetOutput()->GetVTKData();
  rectilinearGridC->Update();

  result = true;
  for (int i = 0 ; i < 5; i++)
  {
    result = result && mafEquals(((vtkDoubleArray*)rectilinearGridC->GetXCoordinates())->GetValue(i),xCoordinates->GetValue(i)) &&
    mafEquals(((vtkDoubleArray*)rectilinearGridC->GetYCoordinates())->GetValue(i),yCoordinates->GetValue(i)) &&
    mafEquals(((vtkDoubleArray*)rectilinearGridC->GetZCoordinates())->GetValue(i),zCoordinates->GetValue(i)); 
  }

  CPPUNIT_ASSERT(result);
  vtkDEL(rectilinearGrid);
  mafDEL(vmeGeneric);
}

//----------------------------------------------------------------------------
void mafVMEGenericTest::TestGetVisualPipe()
//----------------------------------------------------------------------------
{
  mafVMEGeneric *vmeGeneric;
  mafNEW(vmeGeneric);
  int result =  false;
  result = vmeGeneric->GetVisualPipe() == "mafPipeBox";
  
  CPPUNIT_ASSERT(result);

  mafDEL(vmeGeneric);
}
