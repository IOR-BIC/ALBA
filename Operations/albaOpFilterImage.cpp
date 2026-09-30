/*=========================================================================
Program:   AssemblerPro
Module:    albaOpFilterImage.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpFilterImage.h"
#include "albaGUI.h"
#include "albaVME.h"

#include "albaVMEImage.h"

#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkPointData.h"
#include "vtkImageToStructuredPoints.h"
#include "vtkDataArray.h"
#include "vtkSmartPointer.h"

#include "vtkImageMedian3D.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageThreshold.h"
#include "vtkImageAccumulate.h"
#include "vtkImageGradientMagnitude.h"
#include "vtkImageSobel2D.h"
#include "vtkImageSobel3D.h"
#include "vtkImageMagnitude.h"
#include "vtkImageLaplacian.h"

#include <cmath>

#include "vtkALBASmartPointer.h"
#include "albaGUIBusyInfo.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpFilterImage);

//----------------------------------------------------------------------------
albaOpFilterImage::albaOpFilterImage(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_ImgOut = NULL;
	m_Canundo = true;

	m_MedianRadius[0] = m_MedianRadius[1] = m_GaussianMaxKernelSize = m_GaussianVariance = m_CannyVariance = 1;
	m_ThresholdOutsideValue = 0;
	m_ThresholdBinaryValues[0] = 0;
	m_ThresholdBinaryValues[1] = 255;
	m_CannyThesholds[0] = 0;
	m_CannyThesholds[1] = 10;
	m_ThesholdBelow = true;
}

//----------------------------------------------------------------------------
albaOpFilterImage::~albaOpFilterImage()
{
}


//----------------------------------------------------------------------------
bool albaOpFilterImage::InternalAccept(albaVME *node)
{
	return node->IsA("albaVMEImage");
}

//----------------------------------------------------------------------------
albaOp *albaOpFilterImage::Copy()
{
	albaOpFilterImage *cp = new albaOpFilterImage(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void albaOpFilterImage::OpRun()
{

	if (m_ImgOut == NULL)
	{
		vtkImageData *im = vtkImageData::SafeDownCast(m_Input->GetOutput()->GetVTKData());

		vtkALBASmartPointer<vtkImageCast> vtkImageToFloat;
		vtkImageToFloat->SetOutputScalarTypeToFloat();
		vtkImageToFloat->SetInputData(im);
		vtkImageToFloat->Modified();
		vtkImageToFloat->Update();

		albaString name = m_Input->GetName();
		name << " - Filtered";

		albaNEW(m_ImgOut);
		m_ImgOut->SetName(name);

		if (vtkImageToFloat->GetOutput()->GetNumberOfScalarComponents() != 1)
		{
			albaLogMessage("Warning the input image is not a monochrome one, it will be converted to apply the filters");
			int inputDimensions[3];
			vtkImageToFloat->GetOutput()->GetDimensions(inputDimensions);
			vtkImageData *outputImageData;
			vtkNEW(outputImageData);
			outputImageData->SetDimensions(inputDimensions[0], inputDimensions[1], inputDimensions[2]);
			outputImageData->AllocateScalars(vtkImageToFloat->GetOutput()->GetScalarType(), 1);


			vtkDataArray *outScalars = outputImageData->GetPointData()->GetScalars();
			vtkDataArray *inScalars = vtkImageToFloat->GetOutput()->GetPointData()->GetScalars();

			for (int i = 0; i < inScalars->GetNumberOfTuples(); i++)
			{
				float grayValue = 0;
				for (int j = 0; j < inScalars->GetNumberOfComponents(); j++)
					grayValue += inScalars->GetComponent(i, j);

				grayValue /= (double)inScalars->GetNumberOfComponents();

				outScalars->SetTuple1(i, grayValue);
			}

			m_ImgOut->SetData(outputImageData, m_Input->GetTimeStamp());
			outputImageData->GetScalarRange(m_ImgRange);

			vtkDEL(outputImageData);
		}
		else
		{
			m_ImgOut->SetData(vtkImageToFloat->GetOutput(), m_Input->GetTimeStamp());
			vtkImageToFloat->GetOutput()->GetScalarRange(m_ImgRange);
		}

		m_ImgOut->Update();
		m_ImgOut->ReparentTo(m_Input);
		m_ImgOut->Delete();

		GetLogicManager()->VmeShow(m_ImgOut, true);


		//Preparing ThesholdLimits values and bounds
		m_ThresholdLimit = (m_ImgRange[0] + m_ImgRange[1]) / 2.0;
		m_ThresholdBinaryLimits[0] = (m_ImgRange[0] + m_ImgRange[1]) / 3.0;
		m_ThresholdBinaryLimits[1] = (m_ImgRange[0] + m_ImgRange[1]) * 2.0 / 3.0;

		m_Output = m_ImgOut;
	}

	if (!m_TestMode)
	{

		CreateGui();
	}

}

//----------------------------------------------------------------------------
void albaOpFilterImage::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	for (int i = 0; i < m_UndoStack.size(); i++)
	{
		vtkDEL(m_UndoStack[i]);
	}

	m_UndoStack.clear();

	if (result == OP_RUN_CANCEL)
		albaDEL(m_ImgOut);

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpFilterImage::OpDo()
{

}

//----------------------------------------------------------------------------
void albaOpFilterImage::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		m_Gui->Update();


		if (e->GetSender() == m_Gui)
		{
			if (e->GetId() >= MEDIAN_FILTER && e->GetId() <= LAPLACIAN_RECURSIVE_FILTER)
				RunFilter((FilterTypes)e->GetId());
			else
				switch (e->GetId())
				{
				case wxOK:
					OpStop(OP_RUN_OK);
					break;

				case wxCANCEL:
					OpStop(OP_RUN_CANCEL);
					break;

				case ID_UNDO:
					UndoFilter();
				default:
					Superclass::OnEvent(alba_event);
					break;
				}
		}
		else
		{
			Superclass::OnEvent(alba_event);
		}
	}
}

//----------------------------------------------------------------------------
int albaOpFilterImage::GetImageDimensionality(vtkImageData *image)
{
	// Single-slice images (Z extent == 1) are treated as 2D so that the
	// 2D-specific filters (Sobel2D, etc.) and the dimensionality-aware
	// ones (GradientMagnitude, Laplacian) operate only in the X/Y plane.
	int dims[3];
	image->GetDimensions(dims);
	return (dims[2] > 1) ? 3 : 2;
}

//----------------------------------------------------------------------------
void albaOpFilterImage::RunFilter(FilterTypes filterType)
{
	albaGUIBusyInfo busy("Applying filter...", m_TestMode);

	vtkImageData *imgSrc = vtkImageData::SafeDownCast(m_ImgOut->GetOutput()->GetVTKData());

	vtkImageData *undoItem;
	vtkNEW(undoItem);
	undoItem->DeepCopy(imgSrc);
	m_UndoStack.push_back(undoItem);

	vtkALBASmartPointer<vtkImageData> outImg;

	switch (filterType)
	{
	default:
		break;
	case MEDIAN_FILTER:
		MedianFilter(imgSrc, outImg);
		break;
	case GAUSSIAN_DISCRETE_FILTER:
		GaussianBlurFilter(imgSrc, outImg);
		break;
	case THRESHOLD_FILTER:
		ThresholdFilter(imgSrc, outImg);
		break;
	case THRESHOLD_BINARY_FILTER:
		ThresholdBinaryFilter(imgSrc, outImg);
		break;
	case OTSU_THRESHOLD_FILTER:
		OtsuThresholdFilter(imgSrc, outImg);
		break;
	case GRADIENT_MAGNITUDE:
		GradientMaglitudeFilter(imgSrc, outImg);
		break;
	case SOBEL_EDGE:
		SobelFilter(imgSrc, outImg);
		break;
	case CANNY_EDGE:
		CannyEdgeFilter(imgSrc, outImg);
		break;
	case LAPLACIAN_RECURSIVE_FILTER:
		LaplacianRecursiveFilter(imgSrc, outImg);
		break;
	}

	vtkALBASmartPointer<vtkImageToStructuredPoints> imTosp;
	imTosp->SetInputData(outImg);
	imTosp->Update();

	m_ImgOut->SetData((vtkImageData *)imTosp->GetOutput(), m_Input->GetTimeStamp());

	GetLogicManager()->VmeShow(m_ImgOut, false);
	GetLogicManager()->VmeShow(m_ImgOut, true);

	if (m_Gui)
		m_Gui->Enable(ID_UNDO, true);
}

//----------------------------------------------------------------------------
void albaOpFilterImage::CreateGui()
{
	// Interface:CreateGui
	m_Gui = new albaGUI(this);

	m_Gui->Label("Filters:", 1);
	m_Gui->Label("");

	m_Gui->Label("Median Filter:", 1);
	m_Gui->VectorN(-1, "Radius", m_MedianRadius, 2, 1, 100, "Radius for the Median Filer in X,Y");
	m_Gui->Button(MEDIAN_FILTER, "Apply");
	m_Gui->Divider(1);
	m_Gui->Divider(0);
	m_Gui->Divider(0);

	m_Gui->Label("Gaussian Filter:", 1);
	m_Gui->Double(-1, "Variance", &m_GaussianVariance, 0, 100, 2);
	m_Gui->Integer(-1, "Max K Size", &m_GaussianMaxKernelSize, 1, 100);
	m_Gui->Button(GAUSSIAN_DISCRETE_FILTER, "Apply");
	m_Gui->Divider(1);
	m_Gui->Divider(0);
	m_Gui->Divider(0);

	m_Gui->Label("Threshold Filter:", 1);
	m_Gui->Double(-1, "Out Value", &m_ThresholdOutsideValue, -3000, 3000, 2);
	m_Gui->Double(-1, "Limit", &m_ThresholdLimit, m_ImgRange[0], m_ImgRange[1], 2);
	m_Gui->Bool(-1, "Threshold Below", &m_ThesholdBelow, 1);
	m_Gui->Button(THRESHOLD_FILTER, "Apply");
	m_Gui->Divider(1);
	m_Gui->Divider(0);
	m_Gui->Divider(0);

	m_Gui->Label("Threshold Binary Filter:", 1);
	m_Gui->VectorN(-1, "Values", m_ThresholdBinaryValues, 2, -3000, 3000);
	m_Gui->VectorN(-1, "Threshold", m_ThresholdBinaryLimits, 2, m_ImgRange[0], m_ImgRange[1]);
	m_Gui->Button(THRESHOLD_BINARY_FILTER, "Apply");
	m_Gui->Divider(1);
	m_Gui->Divider(0);
	m_Gui->Divider(0);


	m_Gui->Label("OTSU Threshold Filter:", 1);
	m_Gui->Button(OTSU_THRESHOLD_FILTER, "Apply");
	m_Gui->Divider(1);
	m_Gui->Divider(0);
	m_Gui->Divider(0);


	m_Gui->Label("Gradient Magnitude Filter:", 1);
	m_Gui->Button(GRADIENT_MAGNITUDE, "Apply");
	m_Gui->Divider(1);
	m_Gui->Divider(0);
	m_Gui->Divider(0);

	m_Gui->Label("Sobel Edge Det. Filter:", 1);
	m_Gui->Button(SOBEL_EDGE, "Apply");
	m_Gui->Divider(1);
	m_Gui->Divider(0);
	m_Gui->Divider(0);

	m_Gui->Label("Canny Edge Det. Filter:", 1);
	m_Gui->Double(-1, "Variance", &m_CannyVariance, 0, 100, 2);
	m_Gui->VectorN(-1, "Threshold", m_CannyThesholds, 2, 0, 100);
	m_Gui->Button(CANNY_EDGE, "Apply");
	m_Gui->Divider(1);
	m_Gui->Divider(0);
	m_Gui->Divider(0);

	m_Gui->Label("Laplacian Recursive Filter:", 1);
	m_Gui->Button(LAPLACIAN_RECURSIVE_FILTER, "Apply");
	m_Gui->Divider(1);
	m_Gui->Divider(0);
	m_Gui->Divider(0);

	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->Button(ID_UNDO, "Undo");
	m_Gui->Divider(1);
	m_Gui->OkCancel();

	ShowGui();
}

//----------------------------------------------------------------------------
void albaOpFilterImage::UndoFilter()
{
	m_ImgOut->SetData(m_UndoStack[m_UndoStack.size() - 1], m_Input->GetTimeStamp());
	vtkDEL(m_UndoStack[m_UndoStack.size() - 1]);
	m_UndoStack.pop_back();

	GetLogicManager()->VmeShow(m_ImgOut, false);
	GetLogicManager()->VmeShow(m_ImgOut, true);

	if (m_Gui)
		m_Gui->Enable(ID_UNDO, m_UndoStack.size());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::MedianFilter(vtkImageData *inputImage, vtkImageData *outputImage)
{
	vtkALBASmartPointer<vtkImageMedian3D> filter;
	filter->SetInputData(inputImage);

	// KernelSize expects full width per axis: width = 2*radius + 1.
	// Z kernel is 1 (no cross-slice filtering).
	filter->SetKernelSize(m_MedianRadius[0] * 2 + 1, m_MedianRadius[1] * 2 + 1, 1);
	filter->Update();

	outputImage->DeepCopy(filter->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::GaussianBlurFilter(vtkImageData *inputImage, vtkImageData *outputImage)
{
	int dimensionality = GetImageDimensionality(inputImage);
	double stdDev = std::sqrt(m_GaussianVariance);

	vtkALBASmartPointer<vtkImageGaussianSmooth> filter;
	filter->SetInputData(inputImage);

	// SetStandardDeviations() accepts standard deviation values per axis.
	filter->SetStandardDeviations(stdDev, stdDev, dimensionality == 3 ? stdDev : 0.0);

	// RadiusFactors controls kernel half-width per axis.
	filter->SetRadiusFactors(m_GaussianMaxKernelSize, m_GaussianMaxKernelSize, dimensionality == 3 ? m_GaussianMaxKernelSize : 0);
	filter->Update();

	outputImage->DeepCopy(filter->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::ThresholdFilter(vtkImageData *inputImage, vtkImageData *outputImage)
{
	vtkALBASmartPointer<vtkImageThreshold> filter;
	filter->SetInputData(inputImage);
	filter->SetReplaceIn(false);
	filter->SetReplaceOut(true);
	filter->SetOutValue(m_ThresholdOutsideValue);

	// ThresholdByUpper keeps values >= limit, replaces values < limit.
	// ThresholdByLower keeps values <= limit, replaces values > limit.
	if (m_ThesholdBelow)
		filter->ThresholdByUpper(m_ThresholdLimit);
	else
		filter->ThresholdByLower(m_ThresholdLimit);

	filter->Update();

	outputImage->DeepCopy(filter->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::ThresholdBinaryFilter(vtkImageData *inputImage, vtkImageData *outputImage)
{
	vtkALBASmartPointer<vtkImageThreshold> filter;
	filter->SetInputData(inputImage);

	filter->ThresholdBetween(m_ThresholdBinaryLimits[0], m_ThresholdBinaryLimits[1]);
	filter->SetReplaceIn(true);
	filter->SetInValue(m_ThresholdBinaryValues[0]);
	filter->SetReplaceOut(true);
	filter->SetOutValue(m_ThresholdBinaryValues[1]);

	filter->Update();

	outputImage->DeepCopy(filter->GetOutput());
}

//----------------------------------------------------------------------------
double albaOpFilterImage::ComputeOtsuThreshold(vtkImageData *inputImage)
{
	const int numBins = 256;
	double range[2];
	inputImage->GetScalarRange(range);

	double spacing = (range[1] - range[0]) / numBins;
	if (spacing <= 0)
		return range[0];

	// Build histogram using vtkImageAccumulate.
	vtkALBASmartPointer<vtkImageAccumulate> histogram;
	histogram->SetInputData(inputImage);
	histogram->SetComponentExtent(0, numBins - 1, 0, 0, 0, 0);
	histogram->SetComponentOrigin(range[0], 0, 0);
	histogram->SetComponentSpacing(spacing, 0, 0);
	histogram->Update();

	vtkDataArray *bins = histogram->GetOutput()->GetPointData()->GetScalars();

	double total = 0.0;
	double sumAll = 0.0;
	for (int i = 0; i < numBins; i++)
	{
		double count = bins->GetComponent(i, 0);
		total += count;
		sumAll += i * count;
	}

	double sumB = 0.0, weightB = 0.0, maxVariance = 0.0;
	int bestBin = 0;

	for (int i = 0; i < numBins; i++)
	{
		weightB += bins->GetComponent(i, 0);
		if (weightB == 0)
			continue;

		double weightF = total - weightB;
		if (weightF <= 0)
			break;

		sumB += i * bins->GetComponent(i, 0);

		double meanB = sumB / weightB;
		double meanF = (sumAll - sumB) / weightF;
		double diff = meanB - meanF;
		double variance = weightB * weightF * diff * diff;

		if (variance > maxVariance)
		{
			maxVariance = variance;
			bestBin = i;
		}
	}

	// Bin center of the best-scoring bin, translated back to image intensity.
	return range[0] + (bestBin + 0.5) * spacing;
}

//----------------------------------------------------------------------------
void albaOpFilterImage::OtsuThresholdFilter(vtkImageData *inputImage, vtkImageData *outputImage)
{
	double otsuThreshold = ComputeOtsuThreshold(inputImage);

	vtkALBASmartPointer<vtkImageThreshold> filter;
	filter->SetInputData(inputImage);
	filter->ThresholdByUpper(otsuThreshold);
	filter->SetReplaceIn(true);
	filter->SetInValue(255);
	filter->SetReplaceOut(true);
	filter->SetOutValue(0);
	filter->Update();

	outputImage->DeepCopy(filter->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::GradientMaglitudeFilter(vtkImageData *inputImage, vtkImageData *outputImage)
{
	vtkALBASmartPointer<vtkImageGradientMagnitude> filter;
	filter->SetInputData(inputImage);
	filter->SetDimensionality(GetImageDimensionality(inputImage));
	filter->Update();

	outputImage->DeepCopy(filter->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::SobelFilter(vtkImageData *inputImage, vtkImageData *outputImage)
{
	// vtkImageSobel2D/3D output a 2 or 3-component gradient vector.
	// vtkImageMagnitude collapses it to a scalar edge-response image.
	vtkSmartPointer<vtkImageData> sobelOutput;

	if (GetImageDimensionality(inputImage) == 2)
	{
		vtkALBASmartPointer<vtkImageSobel2D> sobel;
		sobel->SetInputData(inputImage);
		sobel->Update();
		sobelOutput = sobel->GetOutput();
	}
	else
	{
		vtkALBASmartPointer<vtkImageSobel3D> sobel;
		sobel->SetInputData(inputImage);
		sobel->Update();
		sobelOutput = sobel->GetOutput();
	}

	vtkALBASmartPointer<vtkImageMagnitude> magnitude;
	magnitude->SetInputData(sobelOutput);
	magnitude->Update();

	outputImage->DeepCopy(magnitude->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::CannyEdgeFilter(vtkImageData *inputImage, vtkImageData *outputImage)
{
	// Approximation of Canny edge detection algorithm.
	// Pipeline: Gaussian smoothing -> Sobel gradient -> magnitude -> double threshold.
	// Note: This implementation does NOT perform non-maximum suppression
	// nor hysteresis edge-linking, resulting in thicker edges.
	int dimensionality = GetImageDimensionality(inputImage);
	double sigma = std::sqrt(m_CannyVariance);

	vtkALBASmartPointer<vtkImageGaussianSmooth> smooth;
	smooth->SetInputData(inputImage);
	smooth->SetStandardDeviations(sigma, sigma, dimensionality == 3 ? sigma : 0.0);
	smooth->Update();

	vtkSmartPointer<vtkImageData> sobelOutput;

	if (dimensionality == 2)
	{
		vtkALBASmartPointer<vtkImageSobel2D> sobel;
		sobel->SetInputData(smooth->GetOutput());
		sobel->Update();
		sobelOutput = sobel->GetOutput();
	}
	else
	{
		vtkALBASmartPointer<vtkImageSobel3D> sobel;
		sobel->SetInputData(smooth->GetOutput());
		sobel->Update();
		sobelOutput = sobel->GetOutput();
	}

	vtkALBASmartPointer<vtkImageMagnitude> magnitude;
	magnitude->SetInputData(sobelOutput);
	magnitude->Update();

	vtkALBASmartPointer<vtkImageThreshold> threshold;
	threshold->SetInputData(magnitude->GetOutput());
	threshold->ThresholdBetween(m_CannyThesholds[0], m_CannyThesholds[1]);
	threshold->SetReplaceIn(true);
	threshold->SetInValue(255);
	threshold->SetReplaceOut(true);
	threshold->SetOutValue(0);
	threshold->Update();

	outputImage->DeepCopy(threshold->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::LaplacianRecursiveFilter(vtkImageData *inputImage, vtkImageData *outputImage)
{
	int dimensionality = GetImageDimensionality(inputImage);

	// Apply recursive Gaussian smoothing (sigma = 1.0) before Laplacian operator.
	vtkALBASmartPointer<vtkImageGaussianSmooth> smooth;
	smooth->SetInputData(inputImage);
	smooth->SetStandardDeviations(1.0, 1.0, dimensionality == 3 ? 1.0 : 0.0);
	smooth->Update();

	vtkALBASmartPointer<vtkImageLaplacian> laplacian;
	laplacian->SetInputData(smooth->GetOutput());
	laplacian->SetDimensionality(dimensionality);
	laplacian->Update();

	outputImage->DeepCopy(laplacian->GetOutput());
}