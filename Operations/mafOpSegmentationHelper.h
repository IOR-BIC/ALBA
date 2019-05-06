/*=========================================================================

 Program: MAF2
 Module: mafOpSegmentationHelper
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpSegmentationHelper_h
#define __mafOpSegmentationHelper_h

#include "mafDefines.h"
#include "mafString.h"
#include <vector>

//----------------------------------------------------------------------------
// forward declarations
class mafVMEVolumeGray;
class vtkImageData;
class vtkUnsignedCharArray;

#define EMPTY 0
#define FULL 255

struct AutomaticInfoRange
{
	int m_StartSlice;
	int m_EndSlice;
	double m_Threshold[2];
};

enum PLANE_TYPE
{
	YZ = 0,
	XZ,
	XY,
};

enum BRUSH_SHAPES
{
	CIRCLE_BRUSH_SHAPE = 0,
	SQUARE_BRUSH_SHAPE,
};

/** mafOpSegmentationHelper Is an helper for Segmentation Operation
  */
class MAF_EXPORT mafOpSegmentationHelper 
{
public:

	mafOpSegmentationHelper();
	virtual ~mafOpSegmentationHelper();

	void SetVolumes(mafVMEVolumeGray *volume, mafVMEVolumeGray *segmentation);
	void SetSlices(vtkImageData *volumeSlice,  vtkImageData *segmentationSlice);

	void SliceThreshold(double *threshold);
	void VolumeThreshold(double *threshold);
	void VolumeThreshold(std::vector<AutomaticInfoRange> *rangesVector);

	void ApplySliceChangesToVolume(int slicePlane, int sliceIndex);


	void DrawBrush(double *pos, int slicePlane, int brushSize, int brushShape, bool erase);
protected:

	void InternalTheshold(int dataType, double *threshold, int n, void * inputPointer, unsigned char * outputPointer, int offset = 0);

	template<typename DataType>
	void InternalThreshold(double *threshold, int n, DataType *inputScalars, unsigned char *outputScalars, int offset);

	
	mafVMEVolumeGray *m_Volume;
	mafVMEVolumeGray *m_Segmentation;
	vtkImageData     *m_SegmetationSlice;
	vtkImageData     *m_VolumeSlice;
};

#endif
