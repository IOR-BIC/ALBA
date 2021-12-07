/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSegmentationHelper
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpSegmentationHelper_h
#define __albaOpSegmentationHelper_h

#include "albaDefines.h"
#include "albaString.h"
#include <vector>

//----------------------------------------------------------------------------
// forward declarations
class albaVMEVolumeGray;
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

typedef struct {
	int x, y;
} slicePoint;

typedef struct {
	int x, y, z;
} volPoint;

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

/** albaOpSegmentationHelper Is an helper for Segmentation Operation
  */
class ALBA_EXPORT albaOpSegmentationHelper 
{
public:

	albaOpSegmentationHelper();
	virtual ~albaOpSegmentationHelper();

	void SetVolumes(albaVMEVolumeGray *volume, albaVMEVolumeGray *segmentation);
	void SetSlices(vtkImageData *volumeSlice,  vtkImageData *segmentationSlice);

	void SliceThreshold(double *threshold);
	void VolumeThreshold(double *threshold);
	void VolumeThreshold(std::vector<AutomaticInfoRange> *rangesVector);

	void ApplySliceChangesToVolume(int slicePlane, int sliceIndex);

	void CopyVolumeDataToSlice(int slicePlane, int sliceIndex);


	void DrawBrush(double *pos, int slicePlane, int brushSize, int brushShape, bool erase);

	void Fill(double *pos, int slicePlane, double thresholdPerc,  bool erase);

	void Connectivity3d(double * pos, int slicePlane, int currentSlice);

protected:

	void InternalTheshold(int dataType, double *threshold, int n, void * inputPointer, unsigned char * outputPointer, int offset = 0);

	template<typename DataType>
	void InternalThreshold(double *threshold, int n, DataType *inputScalars, unsigned char *outputScalars, int offset);

	template<typename DataType>
	void InternalFill(slicePoint startPoint, double minValue, double maxValue, int fillValue, DataType *inputScalars, unsigned char *outputScalars);


	void GetSlicePoint(int slicePlane, double *pos, int *sclicePoint);

	albaVMEVolumeGray *m_Volume;
	albaVMEVolumeGray *m_Segmentation;
	vtkImageData     *m_SegmetationSlice;
	vtkImageData     *m_VolumeSlice;
public:
};



#endif
