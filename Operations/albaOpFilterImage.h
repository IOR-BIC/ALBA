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

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class vtkImageData;

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
		OTSU_THRESHOLD_FILTER,
		GRADIENT_MAGNITUDE,
		SOBEL_EDGE,
		CANNY_EDGE,
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
	/*virtual*/ albaOp *Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);

	/** Runs Selected Filter */
	void RunFilter(FilterTypes filterType);

	/** Gets the median filter radius parameter */
	int *GetMedianRadius() { return m_MedianRadius; }

	/** Sets the median filter radius parameter */
	void SetMedianRadius(int x, int y) { m_MedianRadius[0] = x, m_MedianRadius[1] = y; }

	/** Sets the threshold filter parameters */
	void SetThreshold(double value, double limit, bool thresholdBelow) { m_ThresholdOutsideValue = value; m_ThresholdLimit = limit; m_ThesholdBelow = thresholdBelow; }

	/** Sets the binary threshold filter parameters */
	void SetBinaryThreshold(double values[2], double limits[2]) { m_ThresholdBinaryValues[0] = values[0]; m_ThresholdBinaryValues[1] = values[1]; m_ThresholdBinaryLimits[0] = limits[0], m_ThresholdBinaryLimits[1] = limits[1]; }

	/** Sets the binary threshold filter parameters */
	void SetCannyEdge(double variance, double thresholds[2]) { m_CannyVariance = variance; m_CannyThesholds[0] = thresholds[0];	m_CannyThesholds[1] = thresholds[1]; }

	/** Undo last filter */
	void UndoFilter();

protected:


	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME *node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);

	/** Create the Operation GUI */
	virtual void CreateGui();

	/** Returns 2 if the image is a single Z-slice, 3 otherwise. Used to keep
	the dimensionality-aware VTK filters operating in the X/Y plane only
	for 2D images. */
	int GetImageDimensionality(vtkImageData *image);

	/** Computes the Otsu threshold value on the image histogram */
	double ComputeOtsuThreshold(vtkImageData *inputImage);

	void MedianFilter(vtkImageData *inputImage, vtkImageData *outputImage);
	void GaussianBlurFilter(vtkImageData *inputImage, vtkImageData *outputImage);
	void ThresholdFilter(vtkImageData *inputImage, vtkImageData *outputImage);
	void ThresholdBinaryFilter(vtkImageData *inputImage, vtkImageData *outputImage);
	void OtsuThresholdFilter(vtkImageData *inputImage, vtkImageData *outputImage);
	void GradientMaglitudeFilter(vtkImageData *inputImage, vtkImageData *outputImage);
	void SobelFilter(vtkImageData *inputImage, vtkImageData *outputImage);
	void CannyEdgeFilter(vtkImageData *inputImage, vtkImageData *outputImage);
	void LaplacianRecursiveFilter(vtkImageData *inputImage, vtkImageData *outputImage);

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

	double m_ImgRange[2];

private:


};

#endif