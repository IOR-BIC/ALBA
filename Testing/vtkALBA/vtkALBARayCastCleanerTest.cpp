/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBARayCastCleanerTest
 Authors: Gianluigi Crimi
 
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
#include "albaString.h"
#include "vtkALBARayCastCleaner.h"
#include "vtkALBARayCastCleanerTest.h"

#include "vtkALBASmartPointer.h"
#include "vtkStructuredPointsReader.h"
#include "vtkStructuredPointsWriter.h"
#include "vtkImageData.h"
#include "vtkStructuredPoints.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"

//-------------------------------------------------------------------------
void vtkALBARayCastCleanerTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  //Testing with smart pointer
  vtkALBASmartPointer<vtkALBARayCastCleaner> filter1;

  //Test with new/Delete
  vtkALBARayCastCleaner *filter2;
  vtkNEW(filter2);
  vtkDEL(filter2);
}

//-------------------------------------------------------------------------
void vtkALBARayCastCleanerTest::TestFilter()
//-------------------------------------------------------------------------
{

  vtkImageData *oldOutput,*newOutput;

  //Loading unfiltered data
  vtkALBASmartPointer<vtkStructuredPointsReader> r;
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/VTK_Volumes/volumeRayCastTest.vtk";
  r->SetFileName(filename.GetCStr());
  r->Update();

  //Loading prefiltered data
  vtkALBASmartPointer<vtkStructuredPointsReader> r2;
  albaString filteredFilename=ALBA_DATA_ROOT;
  filteredFilename<<"/Test_vtkALBARayCastCleaner/outputVolume.vtk";
  r2->SetFileName(filteredFilename.GetCStr());
  r2->Update();

  //creating filter
  vtkALBASmartPointer<vtkALBARayCastCleaner> filter;
  filter->SetInputConnection(r->GetOutputPort());
  filter->SetModalityToCT();
  filter->Update();
  
  //getting output
  oldOutput= vtkImageData::SafeDownCast(r2->GetOutput());
  newOutput=vtkImageData::SafeDownCast(filter->GetOutput());
  
  //base tests
  CPPUNIT_ASSERT( (oldOutput!= NULL) && (newOutput != NULL));
  CPPUNIT_ASSERT(oldOutput->GetNumberOfPoints() == newOutput->GetNumberOfPoints());

  vtkDataArray* oldScalars = (vtkDataArray*)oldOutput->GetPointData()->GetScalars();
  vtkDataArray* newScalars = (vtkDataArray*)newOutput->GetPointData()->GetScalars();

  int nPoints=newOutput->GetNumberOfPoints();
    
  int equalScalars=true;

  //scalars compare one by one
  for (int i=0;i<nPoints;i++)
  {
    if (oldScalars->GetTuple1(i) != newScalars->GetTuple1(i))
    {
      equalScalars=false;
      break;
    }
  }
  
  CPPUNIT_ASSERT(equalScalars);


}


//-------------------------------------------------------------------------
void vtkALBARayCastCleanerTest::TestFilterMR()
//-------------------------------------------------------------------------
{

  vtkImageData *oldOutput,*newOutput;
  
  //Loading unfiltered data
  vtkALBASmartPointer<vtkStructuredPointsReader> r;
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/VTK_Volumes/volumeRayCastTestMR.vtk";
  r->SetFileName(filename.GetCStr());
  r->Update();

  //Loading prefiltered data
  vtkALBASmartPointer<vtkStructuredPointsReader> r2;
  albaString filteredFilename=ALBA_DATA_ROOT;
  filteredFilename<<"/Test_vtkALBARayCastCleaner/outputVolumeMR.vtk";
  r2->SetFileName(filteredFilename.GetCStr());
  r2->Update();
  
  //creating filter
  vtkALBASmartPointer<vtkALBARayCastCleaner> filter;
  filter->SetInputConnection(r->GetOutputPort());
  filter->SetModalityToMR();
  filter->Update();
  
  //getting output
  oldOutput=r2->GetOutput();
  newOutput=vtkImageData::SafeDownCast(filter->GetOutput());
  
  //base tests
  CPPUNIT_ASSERT( (oldOutput!= NULL) && (newOutput != NULL));
  CPPUNIT_ASSERT(oldOutput->GetNumberOfPoints() == newOutput->GetNumberOfPoints());

  vtkDataArray* oldScalars = (vtkDataArray*)oldOutput->GetPointData()->GetScalars();
  vtkDataArray* newScalars = (vtkDataArray*)newOutput->GetPointData()->GetScalars();

  int nPoints=newOutput->GetNumberOfPoints();
    
  int equalScalars=true;

  //scalars compare one by one
  for (int i=0;i<nPoints;i++)
  {
    if (oldScalars->GetTuple1(i) != newScalars->GetTuple1(i))
    {
      equalScalars=false;
      break;
    }
  }
  
  CPPUNIT_ASSERT(equalScalars);
}

//-------------------------------------------------------------------------
void vtkALBARayCastCleanerTest::TestSetterGetter()
//-------------------------------------------------------------------------
{

  //testing setter/getter 

  vtkALBASmartPointer<vtkALBARayCastCleaner> filter;
  
  filter->SetBloodLowerThreshold(145.0);
  CPPUNIT_ASSERT(filter->GetBloodLowerThreshold() == 145.0);
  
  filter->SetBloodUpperThreshold(255.0);
  CPPUNIT_ASSERT(filter->GetBloodUpperThreshold() == 255.0);
  
  filter->SetBoneLowerThreshold(745.0);
  CPPUNIT_ASSERT(filter->GetBoneLowerThreshold() == 745.0);

  filter->SetModalityToCT();
  CPPUNIT_ASSERT(filter->IsModalityCT() && !filter->IsModalityMR());

  filter->SetModalityToMR();
  CPPUNIT_ASSERT(filter->IsModalityMR() && !filter->IsModalityCT());

}
