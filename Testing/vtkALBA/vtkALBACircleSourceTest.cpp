/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBACircleSourceTest
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "albaDefines.h"

#include "vtkALBACircleSource.h"
#include "vtkALBACircleSourceTest.h"
#include "vtkALBASmartPointer.h"

//------------------------------------------------------------
void vtkALBACircleSourceTest::TestDynamicAllocation()
{
	vtkALBACircleSource *to = vtkALBACircleSource::New();
  to->Delete();
}

//------------------------------------------------------------
void vtkALBACircleSourceTest::TestParameters()
{
	vtkALBASmartPointer<vtkALBACircleSource> circle;

	double point[3]{ 1.0, 2.0, 0.0 };
	double radius = 5;
	double angle1 = 0.0, angle2 = 90.0;

	circle->SetCenter(point);
	circle->SetRadius(radius);
	circle->SetAngleRange(angle1, angle2);
	circle->Update();

	double expPoint[3];
	double expRadius;
	double expAngleRange[2];

	circle->GetCenter(expPoint);
	CPPUNIT_ASSERT(expPoint[0] == point[0] && expPoint[1] == point[1]);

	expRadius = circle->GetRadius();
	CPPUNIT_ASSERT(expRadius == radius);

	circle->GetAngleRange(expAngleRange);
	CPPUNIT_ASSERT(expAngleRange[0] == angle1 && expAngleRange[1] == angle2);
}

//------------------------------------------------------------
void vtkALBACircleSourceTest::TestClassName()
{
	vtkALBASmartPointer<vtkALBACircleSource> circle;

	wxString className = circle->GetClassName();

	CPPUNIT_ASSERT(className == "vtkALBACircleSource");
}