/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBALineStripSourceTest
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "albaDefines.h"

#include "vtkALBALineStripSource.h"
#include "vtkALBALineStripSourceTest.h"
#include "vtkALBASmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCell.h"

//------------------------------------------------------------
void vtkALBALineStripSourceTest::TestDynamicAllocation()
{
	vtkALBALineStripSource *to = vtkALBALineStripSource::New();
  to->Delete();
}

//------------------------------------------------------------
void vtkALBALineStripSourceTest::TestPoints()
{
	vtkALBASmartPointer<vtkALBALineStripSource> lineStrip;

	double point[3];
	point[1] = 0.0;
	point[2] = 0.0;

	int nPoints = 5;

	for (int i = 0; i < nPoints; i++)
	{
		point[0] = i + 0.2;
		lineStrip->AddPoint(point);
	}

	lineStrip->Update();

	CPPUNIT_ASSERT(lineStrip->GetNumberOfPoints() == nPoints);

	lineStrip->ClearPoints();
	lineStrip->Update();

	CPPUNIT_ASSERT(lineStrip->GetNumberOfPoints() == 0);
}

//------------------------------------------------------------
void vtkALBALineStripSourceTest::TestClassName()
{
	vtkALBASmartPointer<vtkALBALineStripSource> lineStrip;

	wxString className = lineStrip->GetClassName();

	CPPUNIT_ASSERT(className == "vtkALBALineStripSource");
}

//------------------------------------------------------------
void vtkALBALineStripSourceTest::TestCells()
{
	vtkALBASmartPointer<vtkALBALineStripSource> lineStrip;

	double point[3];
	point[1] = 0.0;
	point[2] = 0.0;

	int nPoints = 5;

	for (int i = 0; i < nPoints; i++)
	{
		point[0] = i + 0.2;
		lineStrip->AddPoint(point);
	}

	lineStrip->Update();
	int nCells = lineStrip->GetOutput()->GetNumberOfCells();

	CPPUNIT_ASSERT(nCells == 1);

	vtkCell *cell = lineStrip->GetOutput()->GetCell(0);
	int cellPnts = cell->GetNumberOfPoints();

	CPPUNIT_ASSERT(cellPnts == nPoints);

	for (int i = 0; i < cellPnts; i++)
	{
		CPPUNIT_ASSERT(cell->GetPointId(i) == i);
	}
}