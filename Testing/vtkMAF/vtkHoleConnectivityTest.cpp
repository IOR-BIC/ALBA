/*=========================================================================

 Program: MAF2Medical
 Module: vtkHoleConnectivityTest
 Authors: Matteo Giacomoni
 
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
#include "vtkHoleConnectivity.h"
#include "vtkHoleConnectivityTest.h"
#include "mafString.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkPolyData.h"

//-------------------------------------------------------------------------
void vtkHoleConnectivityTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
	vtkMAFSmartPointer<vtkHoleConnectivity> hc;

  vtkHoleConnectivity *hc2 = vtkHoleConnectivity::New();

  vtkDEL(hc2);
}
//-------------------------------------------------------------------------
void vtkHoleConnectivityTest::TestPrintSelf()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkHoleConnectivity> hc;  
  hc->PrintSelf(std::cout, 5);
}
//-------------------------------------------------------------------------
void vtkHoleConnectivityTest::TestGetClassName()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkHoleConnectivity> hc;
  mafString check = hc->GetClassName();
  CPPUNIT_ASSERT(check.Equals("vtkHoleConnectivity"));
}
//-------------------------------------------------------------------------
void vtkHoleConnectivityTest::TestSetGetPointID()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkHoleConnectivity> hc;
  hc->SetPointID(5);
  CPPUNIT_ASSERT(hc->GetPointID() == 5);
}
//-------------------------------------------------------------------------
void vtkHoleConnectivityTest::TestSetGetPoint()
//-------------------------------------------------------------------------
{
  double point[3] = {1.,2.,3.};
  vtkMAFSmartPointer<vtkHoleConnectivity> hc;
  hc->SetPoint(point);
  CPPUNIT_ASSERT(hc->GetPoint()[0] == point[0] &&
                 hc->GetPoint()[1] == point[1] &&
                 hc->GetPoint()[2] == point[2]);

}
//-------------------------------------------------------------------------
void vtkHoleConnectivityTest::TestExecution()
//-------------------------------------------------------------------------
{
  mafString surfaceFile=MAF_DATA_ROOT;
  surfaceFile<<"/Test_HoleConnectivity/test.vtk";

  vtkMAFSmartPointer<vtkPolyDataReader> preader;
  preader->SetFileName(surfaceFile);
  preader->Update();


  vtkMAFSmartPointer<vtkHoleConnectivity> hc;
  hc->SetInput(preader->GetOutput());
  double point[3] = {0.,0.,0.};
  hc->SetPoint(point);

  hc->Update();

  vtkMAFSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
  connectivityFilter->SetInput(preader->GetOutput());
  connectivityFilter->SetExtractionModeToClosestPointRegion ();
  connectivityFilter->SetClosestPoint(point);
  connectivityFilter->Modified();
  connectivityFilter->Update();

  //check
  int num1 = connectivityFilter->GetOutput()->GetNumberOfPoints();
  int num2 = hc->GetOutput()->GetNumberOfPoints();

  CPPUNIT_ASSERT(num1 == num2);

  num1 = connectivityFilter->GetOutput()->GetNumberOfCells();
  num2 = hc->GetOutput()->GetNumberOfCells();

  CPPUNIT_ASSERT(num1 == num2);

  int i = 0, size = connectivityFilter->GetOutput()->GetNumberOfPoints();
  bool result = true;
  for(;i<size; i++)
  {
    double point1[3], point2[3];
    connectivityFilter->GetOutput()->GetPoint(i, point1);
    hc->GetOutput()->GetPoint(i, point2);

    if(point1[0] == point2[0] && point1[1] == point2[1] && point1[2] == point2[2])
    {
      //equal
    }
    else
    {
      //not equal, exit from cycle
      result = false;
      break;
    }
  }

  CPPUNIT_ASSERT(result);
}