/*=========================================================================
Program:   AssemblerPro
Module:    albaOpFilterImage.h
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

#ifndef __albaOpFilterImage_H__
#define __albaOpFilterImage_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDecl.h"
#include "albaOp.h"
#include "itkImage.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkImageData;

const unsigned int Dimension = 2;

typedef double InputPixelTypeFloat;
typedef itk::Image< InputPixelTypeFloat, Dimension > ImageType;

//----------------------------------------------------------------------------
// Class Name: albaOpFilterImage
//----------------------------------------------------------------------------
class albaOpFilterImage : public albaOp
{
public:

	enum FilterTypes
	{
		MEDIAN_FILTER = MINID,
		GAUSSIAN_DISCRETE_FILTER,
		THRESHOLD_FILTER,
		THRESHOLD_BINARY_FILTER,
		GRADIENT_MAGNITUDE,
		SOBEL_EDGE,
		CANNY_EDGE,
		ZERO_CROSSING_EDGE,
		LAPLACIAN_RECURSIVE_FILTER,
	};

	enum IDs {
		ID_UNDO = LAPLACIAN_RECURSIVE_FILTER + 1,


	};
	/** Constructor. */
	albaOpFilterImage(wxString label = "Filter Image");

	/** Destructor. */
	~albaOpFilterImage();

	/** RTTI macro. */
	albaTypeMacro(albaOpFilterImage, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);

	void RunFilter(FilterTypes filterType);
  
protected:


	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME *node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();

	void MedianFilter(const ImageType *inputImage, ImageType *outputImage);
	void GaussianBlurFilter(const ImageType *inputImage, ImageType *outputImage);
	void ThresholdFilter(const ImageType *inputImage, ImageType *outputImage);
	void ThresholdBinaryFilter(const ImageType *inputImage, ImageType *outputImage);
	void GradientMaglitudeFilter(const ImageType *inputImage, ImageType *outputImageg);
	void SobelFilter(const ImageType *inputImage, ImageType *outputImage);
	void CannyEdgeFilter(const ImageType *inputImage, ImageType *outputImage);
	void ZeroCrossingEdgeFilter(const ImageType *inputImage, ImageType *outputImage);
	void LaplacianRecursiveFilter(const ImageType *inputImage, ImageType *outputImage);
	void OnUndo();

	albaVMEImage *m_ImgOut;
	std::vector<vtkImageData *> m_UndoStack;
	
	int m_MedianRadius[2];
	
	double m_GaussianVariance;
	int m_GaussianMaxKernelSize;

	double m_ThresholdOutsideValue;
	double m_ThresholdLimit;
	int m_ThesholdBelow;

	double m_ThresholdBinaryValues[2];
	double m_ThresholdBinaryLimits[2];

	double m_CannyThesholds[2];
	double m_CannyVariance;

	double m_ZeroEdgeVariance;

	double m_ImgRange[2];

private:


};
	
#endif
