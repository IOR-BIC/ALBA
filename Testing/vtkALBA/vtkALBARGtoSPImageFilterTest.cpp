/*=========================================================================

Program: ALBA
Module:  vtkALBARGtoSPImageFilterTest.cpp
Authors: Nicola Vanella
Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include "vtkALBARGtoSPImageFilterTest.h"
#include "vtkALBARGtoSPImageFilter.h"

#include "vtkRectilinearGrid.h"
#include "vtkRectilinearGridReader.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"

//----------------------------------------------------------------------------
void vtkALBARGtoSPImageFilterTest::BeforeTest()
{ }
//----------------------------------------------------------------------------
void vtkALBARGtoSPImageFilterTest::AfterTest()
{ }

//------------------------------------------------------------
void vtkALBARGtoSPImageFilterTest::TestDynamicAllocation()
{
	vtkALBARGtoSPImageFilter *filter = vtkALBARGtoSPImageFilter::New();
	filter->Delete();
}

//------------------------------------------------------------
void vtkALBARGtoSPImageFilterTest::FilterTest()
{
	// Load RectilinearGrid
	std::string absPathFilename = ALBA_DATA_ROOT;
	absPathFilename += "/Test_vtkALBARGtoSPImageFilter/";
	absPathFilename.append("test.vtk");

	vtkRectilinearGridReader *reader = vtkRectilinearGridReader::New();

	reader->SetFileName(absPathFilename.c_str());
	reader->Update();

	vtkDataSet *input = reader->GetOutput();
	
	// Use Filter 
	vtkALBARGtoSPImageFilter *RGtoSPImageFilter;
	vtkNEW(RGtoSPImageFilter);
	RGtoSPImageFilter->SetInputConnection(reader->GetOutputPort());
	RGtoSPImageFilter->Update();

	vtkDataSet *output = RGtoSPImageFilter->GetOutput();

	//
	double b1[6], b2[6];
	input->GetBounds(b1);
	output->GetBounds(b2);

	// Bounds
	bool CompareBounds = b1[0] == b2[0] && b1[1] == b2[1] && b1[2] == b2[2] && b1[3] == b2[3] && b1[4] == b2[4] && b1[5] == b2[5];
	CPPUNIT_ASSERT(CompareBounds);
	
	// Point Data
	int nPoints = output->GetNumberOfPoints();
	CPPUNIT_ASSERT(nPoints = 150);

	vtkImageData *imageDataOutput = vtkImageData::SafeDownCast(output);

	// Dimensions
	int dimensions[3];
	imageDataOutput->GetDimensions(dimensions);
	CPPUNIT_ASSERT(dimensions[0]==15 && dimensions[1] == 10 && dimensions[2] == 1);

	// Spacing
	double spacing[3];
	imageDataOutput->GetSpacing(spacing);
	CPPUNIT_ASSERT(spacing[0] == 0.5 && spacing[1] == 1.0 && spacing[2] == 1.0);

	// Scalars
	vtkDataArray *scalars = imageDataOutput->GetPointData()->GetScalars();

	CPPUNIT_ASSERT(scalars->GetTuple1(0) == 1.0);
	CPPUNIT_ASSERT(scalars->GetTuple1(24) == 2.0);
	CPPUNIT_ASSERT(scalars->GetTuple1(56) == 4.0);
	CPPUNIT_ASSERT(scalars->GetTuple1(99) == 7.0);
	CPPUNIT_ASSERT(scalars->GetTuple1(132) == 9.0);
	CPPUNIT_ASSERT(scalars->GetTuple1(149) == 10.);

	//
	vtkDEL(RGtoSPImageFilter);
	reader->Delete();
}

//----------------------------------------------------------------------------
void vtkALBARGtoSPImageFilterTest::TestPrintSelf()
{
	vtkALBARGtoSPImageFilter *filter = vtkALBARGtoSPImageFilter::New();
	filter->PrintSelf(std::cout, vtkIndent(3));
	filter->Delete();
}

