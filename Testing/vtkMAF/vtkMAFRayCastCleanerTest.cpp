/*=========================================================================

 Program: MAF2
 Module: vtkMAFRayCastCleanerTest
 Authors: Gianluigi Crimi
 
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
#include "mafString.h"
#include "vtkMAFRayCastCleaner.h"
#include "vtkMAFRayCastCleanerTest.h"

#include "vtkMAFSmartPointer.h"
#include "vtkStructuredPointsReader.h"
#include "vtkStructuredPointsWriter.h"
#include "vtkStructuredPoints.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"

//-------------------------------------------------------------------------
void vtkMAFRayCastCleanerTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  //Testing with smart pointer
  vtkMAFSmartPointer<vtkMAFRayCastCleaner> filter1;

  //Test with new/Delete
  vtkMAFRayCastCleaner *filter2;
  vtkNEW(filter2);
  vtkDEL(filter2);
}

//-------------------------------------------------------------------------
void vtkMAFRayCastCleanerTest::TestFilter()
//-------------------------------------------------------------------------
{

  vtkStructuredPoints *oldOutput,*newOutput;

  //Loading unfiltered data
  vtkMAFSmartPointer<vtkStructuredPointsReader> r;
  mafString filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/volumeRayCastTest.vtk";
  r->SetFileName(filename.GetCStr());
  r->Update();

  //Loading prefiltered data
  vtkMAFSmartPointer<vtkStructuredPointsReader> r2;
  mafString filteredFilename=MAF_DATA_ROOT;
  filteredFilename<<"/Test_vtkMAFRayCastCleaner/outputVolume.vtk";
  r2->SetFileName(filteredFilename.GetCStr());
  r2->Update();

  //creating filter
  vtkMAFSmartPointer<vtkMAFRayCastCleaner> filter;
  filter->SetInput(r->GetOutput());
  filter->SetModalityToCT();
  filter->Update();
  
  //getting output
  oldOutput=r2->GetOutput();
  newOutput=filter->GetOutput();
  
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


  /* 
  //Used to generate initial test file 
  
  vtkMAFSmartPointer<vtkStructuredPointsWriter> w;
  mafString outFilename=MAF_DATA_ROOT;
  outFilename<<"/Test_vtkMAFRayCastCleaner/outputVolume.vtk";
  w->SetFileName(outFilename.GetCStr());
  w->SetFileTypeToBinary();
  w->SetInput(filter->GetOutput());
  w->Update();
  */
  
}


//-------------------------------------------------------------------------
void vtkMAFRayCastCleanerTest::TestFilterMR()
//-------------------------------------------------------------------------
{

  vtkStructuredPoints *oldOutput,*newOutput;
  
  //Loading unfiltered data
  vtkMAFSmartPointer<vtkStructuredPointsReader> r;
  mafString filename=MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/volumeRayCastTestMR.vtk";
  r->SetFileName(filename.GetCStr());
  r->Update();

  //Loading prefiltered data
  vtkMAFSmartPointer<vtkStructuredPointsReader> r2;
  mafString filteredFilename=MAF_DATA_ROOT;
  filteredFilename<<"/Test_vtkMAFRayCastCleaner/outputVolumeMR.vtk";
  r2->SetFileName(filteredFilename.GetCStr());
  r2->Update();
  
  //creating filter
  vtkMAFSmartPointer<vtkMAFRayCastCleaner> filter;
  filter->SetInput(r->GetOutput());
  filter->SetModalityToMR();
  filter->Update();
  
  //getting output
  oldOutput=r2->GetOutput();
  newOutput=filter->GetOutput();
  
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
     
  //Used to generate initial test file 
  /*
  vtkMAFSmartPointer<vtkStructuredPointsWriter> w;
  mafString outFilename=MAF_DATA_ROOT;
  outFilename<<"/Test_vtkMAFRayCastCleaner/outputVolumeMR.vtk";
  w->SetFileName(outFilename.GetCStr());
  w->SetFileTypeToBinary();
  w->SetInput(filter->GetOutput());
  w->Update();
  */
  
}

//-------------------------------------------------------------------------
void vtkMAFRayCastCleanerTest::TestSetterGetter()
//-------------------------------------------------------------------------
{

  //testing setter/getter 

  vtkMAFSmartPointer<vtkMAFRayCastCleaner> filter;
  
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
