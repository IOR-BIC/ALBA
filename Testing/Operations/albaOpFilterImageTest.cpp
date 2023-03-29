/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFilterImageTest
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
#include "albaOpFilterImageTest.h"

#include "albaOpFilterImage.h"
#include "vtkALBASmartPointer.h"
#include "albaVMEImage.h"
#include "albaOpFilterImage.h"
#include "vtkStructuredPointsReader.h"
#include "vtkStructuredPoints.h"

//----------------------------------------------------------------------------
void albaOpFilterImageTest::BeforeTest()
{
	vtkStructuredPointsReader *preader = NULL;

	albaString fileName = ALBA_DATA_ROOT;
	fileName << "/ImageFilters\\TestImage.vtk";

	vtkNEW(preader);
	preader->SetFileName(fileName);
	preader->Update();

	albaNEW(m_TestImage);
	m_TestImage->SetDataByDetaching(preader->GetOutput(), 0);

	m_TestOp=new albaOpFilterImage;
	m_TestOp->SetInput(m_TestImage);
	m_TestOp->TestModeOn();
	m_TestOp->OpRun();

	vtkDEL(preader);
}

//----------------------------------------------------------------------------
void  albaOpFilterImageTest::AfterTest()
{
 	albaDEL(m_TestOp);
 	albaDEL(m_TestImage);
}

//----------------------------------------------------------------------------
void albaOpFilterImageTest::TestGaussianDiscrete()
{
	albaVMEImage *imgout = albaVMEImage::SafeDownCast(m_TestOp->GetOutput());

	m_TestOp->RunFilter(albaOpFilterImage::GAUSSIAN_DISCRETE_FILTER);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "GAUSSIAN_DISCRETE_FILTER");

}

//----------------------------------------------------------------------------
void albaOpFilterImageTest::TestThreshold()
{
	albaVMEImage *imgout = albaVMEImage::SafeDownCast(m_TestOp->GetOutput());

	m_TestOp->SetThreshold(0, 100, true);
	m_TestOp->RunFilter(albaOpFilterImage::THRESHOLD_FILTER);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "THRESHOLD_FILTER_0_100_Below");

	m_TestOp->UndoFilter();
	m_TestOp->SetThreshold(255, 150, false);
	m_TestOp->RunFilter(albaOpFilterImage::THRESHOLD_FILTER);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "THRESHOLD_FILTER_255_155_Above");

}

//----------------------------------------------------------------------------
void albaOpFilterImageTest::TestThresholdBinary()
{
	albaVMEImage *imgout = albaVMEImage::SafeDownCast(m_TestOp->GetOutput());

	double values[2] = { 0,255 }, limits[2] = { 50,150 };
	m_TestOp->SetBinaryThreshold(values, limits);
	m_TestOp->RunFilter(albaOpFilterImage::THRESHOLD_BINARY_FILTER);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "THRESHOLD_BINARY_FILTER_0_255_50_150");

	double values2[2] = { 150,30 }, limits2[2] = { 45,200 };
	m_TestOp->UndoFilter();
	m_TestOp->SetBinaryThreshold(values2, limits2);
	m_TestOp->RunFilter(albaOpFilterImage::THRESHOLD_BINARY_FILTER);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "THRESHOLD_BINARY_FILTER_150_30_45_200");

}

//----------------------------------------------------------------------------
void albaOpFilterImageTest::TestGradientMagnitude()
{
	albaVMEImage *imgout = albaVMEImage::SafeDownCast(m_TestOp->GetOutput());

	m_TestOp->RunFilter(albaOpFilterImage::GRADIENT_MAGNITUDE);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "GRADIENT_MAGNITUDE");

}

//----------------------------------------------------------------------------
void albaOpFilterImageTest::TestSobel()
{
	albaVMEImage *imgout = albaVMEImage::SafeDownCast(m_TestOp->GetOutput());

	m_TestOp->RunFilter(albaOpFilterImage::SOBEL_EDGE);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "SOBEL_EDGE");

}

//----------------------------------------------------------------------------
void albaOpFilterImageTest::TestCanny()
{
	albaVMEImage *imgout = albaVMEImage::SafeDownCast(m_TestOp->GetOutput());


	double thresholds[2] = { 0,10 }, variance=1;

	m_TestOp->SetCannyEdge(variance, thresholds);
	m_TestOp->RunFilter(albaOpFilterImage::CANNY_EDGE);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "CANNY_EDGE_1_0_10");

	m_TestOp->UndoFilter();

	double thresholds2[2] = { 5,15 }, variance2 = 2;

	m_TestOp->SetCannyEdge(variance2, thresholds);
	m_TestOp->RunFilter(albaOpFilterImage::CANNY_EDGE);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "CANNY_EDGE_2_5_15");


}

//----------------------------------------------------------------------------
void albaOpFilterImageTest::TestZeroCrossing()
{
	albaVMEImage *imgout = albaVMEImage::SafeDownCast(m_TestOp->GetOutput());

	m_TestOp->SetZeroEdgeVariance(1);
	m_TestOp->RunFilter(albaOpFilterImage::ZERO_CROSSING_EDGE);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "ZERO_CROSSING_EDGE_1");

	m_TestOp->UndoFilter();
	m_TestOp->SetZeroEdgeVariance(50);
	m_TestOp->RunFilter(albaOpFilterImage::ZERO_CROSSING_EDGE);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "ZERO_CROSSING_EDGE_50");
}

//----------------------------------------------------------------------------
void albaOpFilterImageTest::TestLaplacianRecursive()
{
	albaVMEImage *imgout = albaVMEImage::SafeDownCast(m_TestOp->GetOutput());

	m_TestOp->RunFilter(albaOpFilterImage::LAPLACIAN_RECURSIVE_FILTER);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "LAPLACIAN_RECURSIVE_FILTER");

}

//-----------------------------------------------------------
void albaOpFilterImageTest::TestMedian() 
{
	albaVMEImage *imgout = albaVMEImage::SafeDownCast(m_TestOp->GetOutput());

	m_TestOp->RunFilter(albaOpFilterImage::MEDIAN_FILTER);

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "MEDIAN");

	m_TestOp->UndoFilter();

	m_TestOp->SetMedianRadius(3, 5);

	m_TestOp->RunFilter(albaOpFilterImage::MEDIAN_FILTER);
	
	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "MEDIAN_3x5");

}

//----------------------------------------------------------------------------
void albaOpFilterImageTest::TestUndo()
{
	albaVMEImage *imgout = albaVMEImage::SafeDownCast(m_TestOp->GetOutput());

	m_TestOp->OpRun();
	m_TestOp->RunFilter(albaOpFilterImage::MEDIAN_FILTER);
	m_TestOp->RunFilter(albaOpFilterImage::MEDIAN_FILTER);

	m_TestOp->UndoFilter();

	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "UNDO_Median");

	m_TestOp->UndoFilter();
	COMPARE_VTK_IMAGES((vtkImageData *)imgout->GetOutput()->GetVTKData(), "UNDO_Plain");

}
