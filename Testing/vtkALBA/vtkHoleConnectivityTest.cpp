/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkHoleConnectivityTest
 Authors: Matteo Giacomoni
 
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
#include "vtkHoleConnectivity.h"
#include "vtkHoleConnectivityTest.h"
#include "albaString.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkPolyData.h"

//-------------------------------------------------------------------------
void vtkHoleConnectivityTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
	vtkALBASmartPointer<vtkHoleConnectivity> hc;

  vtkHoleConnectivity *hc2 = vtkHoleConnectivity::New();

  vtkDEL(hc2);
}
//-------------------------------------------------------------------------
void vtkHoleConnectivityTest::TestPrintSelf()
//-------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkHoleConnectivity> hc;  
  hc->PrintSelf(std::cout, vtkIndent(5));
}
//-------------------------------------------------------------------------
void vtkHoleConnectivityTest::TestGetClassName()
//-------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkHoleConnectivity> hc;
  albaString check = hc->GetClassName();
  CPPUNIT_ASSERT(check.Equals("vtkHoleConnectivity"));
}
//-------------------------------------------------------------------------
void vtkHoleConnectivityTest::TestSetGetPointID()
//-------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkHoleConnectivity> hc;
  hc->SetPointID(5);
  CPPUNIT_ASSERT(hc->GetPointID() == 5);
}
//-------------------------------------------------------------------------
void vtkHoleConnectivityTest::TestSetGetPoint()
//-------------------------------------------------------------------------
{
  double point[3] = {1.,2.,3.};
  vtkALBASmartPointer<vtkHoleConnectivity> hc;
  hc->SetPoint(point);
  CPPUNIT_ASSERT(hc->GetPoint()[0] == point[0] &&
                 hc->GetPoint()[1] == point[1] &&
                 hc->GetPoint()[2] == point[2]);

}
//-------------------------------------------------------------------------
void vtkHoleConnectivityTest::TestExecution()
//-------------------------------------------------------------------------
{
  albaString surfaceFile=ALBA_DATA_ROOT;
  surfaceFile<<"/Test_HoleConnectivity/test.vtk";

  vtkALBASmartPointer<vtkPolyDataReader> preader;
  preader->SetFileName(surfaceFile);
  preader->Update();


  vtkALBASmartPointer<vtkHoleConnectivity> hc;
  hc->SetInputConnection(preader->GetOutputPort());
  double point[3] = {0.,0.,0.};
  hc->SetPoint(point);

  hc->Update();

  vtkALBASmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
  connectivityFilter->SetInputConnection(preader->GetOutputPort());
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