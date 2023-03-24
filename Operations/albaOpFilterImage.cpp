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
#include "vtkImageData.h"
#include "vtkImageToStructuredPoints.h"

#include "itkVTKImageToImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkAdaptiveHistogramEqualizationImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkSobelEdgeDetectionImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkCannyEdgeDetectionImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkZeroCrossingBasedEdgeDetectionImageFilter.h"
#include "itkLaplacianRecursiveGaussianImageFilter.h"

#include "vtkALBASmartPointer.h"
#include "wx\busyinfo.h"

typedef itk::VTKImageToImageFilter< ImageType > ConvertervtkTOitk;
typedef itk::ImageToVTKImageFilter< ImageType > ConverteritkTOvtk;


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpFilterImage);

//----------------------------------------------------------------------------
albaOpFilterImage::albaOpFilterImage(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_ImgOut = NULL;
	m_Canundo = true;

	m_MedianRadius[0]= m_MedianRadius[1] = m_GaussianMaxKernelSize = m_GaussianVariance = m_CannyVariance = m_ZeroEdgeVariance =1;
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
albaOp* albaOpFilterImage::Copy()
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
		im->Update();

		vtkALBASmartPointer<vtkImageCast> vtkImageToFloat;
		vtkImageToFloat->SetOutputScalarTypeToDouble();
		vtkImageToFloat->SetInput(im);
		vtkImageToFloat->Modified();
		vtkImageToFloat->Update();


		albaString name = m_Input->GetName();
		name << " - Filtered";

		albaNEW(m_ImgOut);
		m_ImgOut->SetName(name);
		m_ImgOut->SetData(vtkImageToFloat->GetOutput(), m_Input->GetTimeStamp());
		m_ImgOut->Update();
		m_ImgOut->ReparentTo(m_Input);
		m_ImgOut->Delete();

		GetLogicManager()->VmeShow(m_ImgOut, true);


		//Preparing ThesholdLimits values and bounds
		vtkImageToFloat->GetOutput()->GetScalarRange(m_ImgRange);
		m_ThresholdLimit = (m_ImgRange[0] + m_ImgRange[1]) / 2.0;
		m_ThresholdBinaryLimits[0] = (m_ImgRange[0] + m_ImgRange[1]) / 3.0;
		m_ThresholdBinaryLimits[1] = (m_ImgRange[0] + m_ImgRange[1]) * 2.0 / 3.0;
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
						OnUndo();
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
void albaOpFilterImage::RunFilter(FilterTypes filterType)
{
	wxBusyInfo busy("Applying filter...");
	
	ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
	vtkDataSet * imgSrc = m_ImgOut->GetOutput()->GetVTKData();
	vtkTOitk->SetInput(vtkImageData::SafeDownCast(imgSrc));
	vtkTOitk->Update();

	vtkImageData *undoItem;
	vtkNEW(undoItem);
	undoItem->DeepCopy(imgSrc);
	m_UndoStack.push_back(undoItem);

	ImageType::Pointer outImg = ImageType::New();
	switch (filterType)
	{
		default:
			break;
		case MEDIAN_FILTER:
			MedianFilter(vtkTOitk->GetOutput(), outImg);
			break;
		case GAUSSIAN_DISCRETE_FILTER:
			GaussianBlurFilter(vtkTOitk->GetOutput(), outImg);
			break;
		case THRESHOLD_FILTER:
			ThresholdFilter(vtkTOitk->GetOutput(), outImg);
			break;
		case THRESHOLD_BINARY_FILTER:
			ThresholdBinaryFilter(vtkTOitk->GetOutput(), outImg);
			break;
		case GRADIENT_MAGNITUDE:
			GradientMaglitudeFilter(vtkTOitk->GetOutput(), outImg);
			break;
		case SOBEL_EDGE:
			SobelFilter(vtkTOitk->GetOutput(), outImg);
			break;
		case CANNY_EDGE:
			CannyEdgeFilter(vtkTOitk->GetOutput(), outImg);
			break;
		case ZERO_CROSSING_EDGE:
			ZeroCrossingEdgeFilter(vtkTOitk->GetOutput(), outImg);
			break;
		case LAPLACIAN_RECURSIVE_FILTER:
			LaplacianRecursiveFilter(vtkTOitk->GetOutput(), outImg);
			break;
	}



	ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
	itkTOvtk->SetInput(outImg);
	itkTOvtk->Update();

	vtkALBASmartPointer<vtkImageData> imOut;
	imOut->DeepCopy(itkTOvtk->GetOutput());
	imOut->Update();

	vtkALBASmartPointer<vtkImageToStructuredPoints> imTosp;
	imTosp->SetInput(imOut);
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

	m_Gui->Label("Median Filter:",1);
	m_Gui->VectorN(-1, "Radius", m_MedianRadius, 2, 1, 100, "Radius for the Median Filer in X,Y");
	m_Gui->Button(MEDIAN_FILTER, "Apply");
	m_Gui->Divider(1);
	m_Gui->Divider(0);
	m_Gui->Divider(0);

	m_Gui->Label("Gaussian Filter:",1);
	m_Gui->Double(-1, "Variance", &m_GaussianVariance, 0, 100, 2);
	m_Gui->Integer(-1, "Max K Size", &m_GaussianMaxKernelSize, 1, 100);
	m_Gui->Button(GAUSSIAN_DISCRETE_FILTER, "Apply");
	m_Gui->Divider(1);
	m_Gui->Divider(0);
	m_Gui->Divider(0);

	m_Gui->Label("Threshold Filter:",1);
	m_Gui->Double(-1, "Out Value", &m_ThresholdOutsideValue, -3000, 3000, 2);
	m_Gui->Double(-1, "Limit", &m_ThresholdLimit, m_ImgRange[0], m_ImgRange[1],2);
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
	m_Gui->VectorN(-1, "Threshold", m_CannyThesholds, 2, 0, 30);
	m_Gui->Button(CANNY_EDGE, "Apply");
	m_Gui->Divider(1);
	m_Gui->Divider(0);
	m_Gui->Divider(0);

	m_Gui->Label("Zero Crossing Det. Filter:", 1);
	m_Gui->Double(-1, "Variance", &m_ZeroEdgeVariance, 0, 100, 2);
	m_Gui->Button(ZERO_CROSSING_EDGE, "Apply");
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
void albaOpFilterImage::OnUndo()
{
	m_ImgOut->SetData(m_UndoStack[m_UndoStack.size() - 1], m_Input->GetTimeStamp());
	vtkDEL(m_UndoStack[m_UndoStack.size() - 1]);
	m_UndoStack.pop_back();

	GetLogicManager()->VmeShow(m_ImgOut, false);
	GetLogicManager()->VmeShow(m_ImgOut, true);

	if(m_Gui)
		m_Gui->Enable(ID_UNDO, m_UndoStack.size());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::MedianFilter(const ImageType *inputImage, ImageType *outputImage)
{
	using FilterType = itk::MedianImageFilter<ImageType, ImageType>;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput(inputImage);

	ImageType::SizeType indexRadius;
	indexRadius[0] = m_MedianRadius[0]; // radius along x
	indexRadius[1] = m_MedianRadius[1]; // radius along y

	filter->SetRadius(indexRadius);
	filter->Update();

	outputImage->Graft(filter->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::GaussianBlurFilter(const ImageType *inputImage, ImageType *outputImage)
{
	using FilterType = itk::DiscreteGaussianImageFilter<ImageType, ImageType>;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput(inputImage);

	filter->SetVariance(m_GaussianVariance);
	filter->SetMaximumKernelWidth(m_GaussianMaxKernelSize);
	filter->Update();

	outputImage->Graft(filter->GetOutput());
}


//----------------------------------------------------------------------------
void albaOpFilterImage::ThresholdFilter(const ImageType *inputImage, ImageType *outputImage)
{
	using FilterType = itk::ThresholdImageFilter<ImageType>;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput(inputImage);

	filter->SetOutsideValue(m_ThresholdOutsideValue);
	if (m_ThesholdBelow)
		filter->ThresholdBelow(m_ThresholdLimit);
	else
		filter->ThresholdAbove(m_ThresholdLimit);

	filter->Update();

	outputImage->Graft(filter->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::ThresholdBinaryFilter(const ImageType *inputImage, ImageType *outputImage)
{
	using FilterType = itk::BinaryThresholdImageFilter<ImageType,ImageType>;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput(inputImage);

	filter->SetOutsideValue(m_ThresholdBinaryValues[0]);
	filter->SetLowerThreshold(m_ThresholdBinaryLimits[0]);
	filter->SetUpperThreshold(m_ThresholdBinaryLimits[1]);

	filter->Update();

	outputImage->Graft(filter->GetOutput());

}

//----------------------------------------------------------------------------
void albaOpFilterImage::GradientMaglitudeFilter(const ImageType *inputImage, ImageType *outputImage)
{
	using FilterType = itk::GradientMagnitudeImageFilter<ImageType, ImageType>;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput(inputImage);

	filter->Update();

	outputImage->Graft(filter->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::SobelFilter(const ImageType *inputImage, ImageType *outputImage)
{
	using FilterType = itk::SobelEdgeDetectionImageFilter<ImageType, ImageType>;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput(inputImage);

	filter->Update();

	outputImage->Graft(filter->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::CannyEdgeFilter(const ImageType *inputImage, ImageType *outputImage)
{
	using FilterType = itk::CannyEdgeDetectionImageFilter<ImageType, ImageType>;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput(inputImage);

 	filter->SetVariance(m_CannyVariance);
 	filter->SetLowerThreshold(m_CannyThesholds[0]);
	filter->SetUpperThreshold(m_CannyThesholds[1]);

	filter->Update();

	outputImage->Graft(filter->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::ZeroCrossingEdgeFilter(const ImageType *inputImage, ImageType *outputImage)
{
	using FilterType = itk::ZeroCrossingBasedEdgeDetectionImageFilter<ImageType, ImageType>;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput(inputImage);

	filter->SetVariance(m_ZeroEdgeVariance);

	filter->Update();

	outputImage->Graft(filter->GetOutput());
}

//----------------------------------------------------------------------------
void albaOpFilterImage::LaplacianRecursiveFilter(const ImageType *inputImage, ImageType *outputImage)
{
	using FilterType = itk::LaplacianRecursiveGaussianImageFilter<ImageType, ImageType>;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput(inputImage);

	filter->Update();

	outputImage->Graft(filter->GetOutput());
}