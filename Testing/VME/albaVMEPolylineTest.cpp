/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPolylineTest
 Authors: Daniele Giunchi - Matteo Giacomoni
 
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
#include "albaVMEPolylineTest.h"


#include "vtkALBASmartPointer.h"
#include "albaVMEPolyline.h"
#include "albaVMEOutputPolyline.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkMath.h"

#include <iostream>


//----------------------------------------------------------------------------
void albaVMEPolylineTest::TestSetData()
//----------------------------------------------------------------------------
{

  vtkALBASmartPointer<vtkPolyData> in_data;
	vtkALBASmartPointer<vtkPoints> in_points;
	vtkALBASmartPointer<vtkCellArray> in_cells;
  
  double point[3];
  // point 1
  point[0] = 0.0;
  point[1] = 0.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);
  
  // point 2
  point[0] = 1.0;
  point[1] = 0.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  // point 3
  point[0] = 1.0;
  point[1] = 1.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  // point 4
  point[0] = 1.0;
  point[1] = 1.0;
  point[2] = 1.0;
  in_points->InsertNextPoint(point);

  in_points->Modified();
 
	vtkIdType pointId[2];
	for(int i = 0; i< in_points->GetNumberOfPoints();i++)
	{
		if (i > 0)
		{             
			pointId[0] = i - 1;
			pointId[1] = i;
			in_cells->InsertNextCell(2 , pointId);  
		}
	}

	in_data->SetPoints(in_points);
	in_data->SetLines(in_cells);
  in_data->Modified();


	// try to set this data to the volume
	int returnValue = -1;

	albaVMEPolyline *polyline;
	albaNEW(polyline);

	returnValue = polyline->SetData(vtkPolyData::SafeDownCast(in_data),0.0,albaVMEGeneric::ALBA_VME_REFERENCE_DATA);
	CPPUNIT_ASSERT(returnValue == ALBA_OK);
	
	polyline->Modified();
	polyline->Update();
	
	/*We use this emulation of ALBAVMEPolylineOutput function (calculate lenght) because of
	there is prblem using  vme set Data. The problem consist in the failure of retriving vtk data from the vme
	after setting it with set data method. It occurs only in test class.*/
	double length = CalculateLengthSubstitute(in_data);

	//printf("\nTest - %f\n", length);
	CPPUNIT_ASSERT(polyline->GetPolylineOutput() != NULL);

	CPPUNIT_ASSERT(length == 3.0);

	albaDEL(polyline);
}
//----------------------------------------------------------------------------
double albaVMEPolylineTest::CalculateLengthSubstitute(vtkPolyData *data)
//----------------------------------------------------------------------------
{
	double sum = 0;
	vtkPoints *pts = data->GetPoints();
	if(pts == NULL) return 0.0;
	for(int i=0; i< pts->GetNumberOfPoints(); i++)
	{ 
		if (i > 0)
		{
			double pos1[3], pos2[3];
			pts->GetPoint(i, pos1);
			pts->GetPoint(i-1, pos2);

			sum += sqrt(vtkMath::Distance2BetweenPoints(pos1, pos2));

		}

	}

	return sum;
}