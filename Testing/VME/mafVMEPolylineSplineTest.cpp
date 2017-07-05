/*=========================================================================

 Program: MAF2
 Module: mafVMEPolylineSplineTest
 Authors: Daniele Giunchi - Matteo Giacomoni
 
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
#include "mafVMEPolylineSplineTest.h"


#include "vtkMAFSmartPointer.h"
#include "mafVMEPolyline.h"
#include "mafVMEPolylineSpline.h"
#include "mafVMEOutputPolyline.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkMath.h"

#include <iostream>


//----------------------------------------------------------------------------
void mafVMEPolylineSplineTest::TestSetData()
//----------------------------------------------------------------------------
{

  vtkMAFSmartPointer<vtkPolyData> in_data;
	vtkMAFSmartPointer<vtkPoints> in_points;
	vtkMAFSmartPointer<vtkCellArray> in_cells;
  
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

  //in_points->InsertPoint(5,point);
  /* When Generate an array of points if you use InsertPoint instead of InsertNextPoint
  skipping one or more point from the last inserted, VTK insert these new points for you
  and initialize them with a value*/

  in_points->InsertNextPoint(point);

  
  in_points->Modified();
  
  /*
  Cell order is not important for points order.
  This create a reversal order cellarray. in this test we must prove that the OrderPolyline
  method affects the cellarray, with the reorder of the cells based on the points sequence.
  */
	int pointId[2];
	for(int i = in_points->GetNumberOfPoints()-1; i>=0 ;i--)
	{
		if (i < in_points->GetNumberOfPoints()-1)
		{             
			pointId[0] = i+1;
			pointId[1] = i;
			in_cells->InsertNextCell(2 , pointId);  
		}
	}

	in_data->SetPoints(in_points);
	in_data->SetLines(in_cells);
  in_data->Modified();

  vtkCellArray *cellArrayOLD = in_data->GetLines();
  mafVMEPolylineSpline *polylineSpline;
  mafNEW(polylineSpline);

  //order test
  polylineSpline->OrderPolyline(in_data);
  vtkCellArray *cellArray = in_data->GetLines();
  int num;
  int *id;
  int ntps;
  for(int i=0 ; i<cellArray->GetNumberOfCells(); i++)
  {
    num = cellArray->GetNextCell(ntps,id);
    printf("\t%d", (*id));
  }
  
  // spline test
  // base value 5 (Spline Coefficient) so point must increment from 4 to 20
  //printf("\n%d", in_data->GetNumberOfPoints());
  int data_num_points = in_data->GetNumberOfPoints();
  CPPUNIT_ASSERT(data_num_points == 4);
  polylineSpline->SplinePolyline(in_data);
  int polyline_coefficient = polylineSpline->GetSplineCoefficient();
  int result_points_number = data_num_points * polyline_coefficient;
  CPPUNIT_ASSERT(in_data->GetNumberOfPoints() == result_points_number);
 
	/*We use this emulation of mafVMEPolylineOutput function (calculate length) because of
	there is problem using vme set Data. The problem consist in the failure of retrieving vtk data from the vme
	after setting it with set data method. It occurs only in test class.*/
	double length = CalculateLengthSubstitute(in_data);

	CPPUNIT_ASSERT(length >= 3.0 && length <= 3.5);
	
	mafDEL(polylineSpline);
}
//----------------------------------------------------------------------------
double mafVMEPolylineSplineTest::CalculateLengthSubstitute(vtkPolyData *data)
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
