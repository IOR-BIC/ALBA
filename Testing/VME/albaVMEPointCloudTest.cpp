/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPointCloudTest
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

#include "albaVMEPointCloudTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "albaVMEGeneric.h"
#include "albaVMEPointCloud.h"
#include "vtkALBASmartPointer.h"
#include "vtkPoints.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"


void albaVMEPointCloudTest::TestFixture()
{

}

void albaVMEPointCloudTest::TestSetData()
{
  vtkALBASmartPointer<vtkPoints> points;

	//cell structure
	vtkALBASmartPointer<vtkCellArray> polys;

  for (int i = 0; i < 25; i++)
  {
    points->InsertPoint(i, vtkMath::Random(0,1), vtkMath::Random(0,1), vtkMath::Random(0,1));	  


		polys->InsertNextCell(3);
		polys->InsertCellPoint(i);
		polys->InsertCellPoint(i);
		polys->InsertCellPoint(i);
  }

  vtkALBASmartPointer<vtkPolyData> pointsVTK;
  pointsVTK->SetPoints(points);
	pointsVTK->SetPolys(polys);


 
  albaVMEPointCloud *vmePointCloud;

  // create vme volume
  albaNEW(vmePointCloud);

  // try to set this data to the volume
  int returnValue = -1;

  // no leaks in this one...
  returnValue = vmePointCloud->SetData(pointsVTK, -1);
  CPPUNIT_ASSERT(returnValue == ALBA_OK);

  vtkDataSet *data = pointsVTK;
  CPPUNIT_ASSERT(data);

  // update the data... 
  data->Update();
  // ... otherwise this will fail!
  CPPUNIT_ASSERT_EQUAL(25, (int)data->GetNumberOfCells());

  // destroy vme
  albaDEL(vmePointCloud);
  
}
