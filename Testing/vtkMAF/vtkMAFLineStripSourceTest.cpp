/*=========================================================================

 Program: MAF2
 Module: vtkMAFLineStripSourceTest
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "mafDefines.h"

#include "vtkMAFLineStripSource.h"
#include "vtkMAFLineStripSourceTest.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCell.h"

//------------------------------------------------------------
void vtkMAFLineStripSourceTest::TestDynamicAllocation()
{
	vtkMAFLineStripSource *to = vtkMAFLineStripSource::New();
  to->Delete();
}

//------------------------------------------------------------
void vtkMAFLineStripSourceTest::TestPoints()
{
	vtkMAFSmartPointer<vtkMAFLineStripSource> lineStrip;

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
void vtkMAFLineStripSourceTest::TestClassName()
{
	vtkMAFSmartPointer<vtkMAFLineStripSource> lineStrip;

	wxString className = lineStrip->GetClassName();

	CPPUNIT_ASSERT(className == "vtkMAFLineStripSource");
}

//------------------------------------------------------------
void vtkMAFLineStripSourceTest::TestCells()
{
	vtkMAFSmartPointer<vtkMAFLineStripSource> lineStrip;

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