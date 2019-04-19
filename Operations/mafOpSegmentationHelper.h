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

//----------------------------------------------------------------------------
// forward declarations
class mafVMEVolumeGray;
class vtkImageData;

#define EMPTY 0
#define FULL 255

struct AutomaticInfoRange
{
	int m_StartSlice;
	int m_EndSlice;
	double m_Threshold[2];
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

	void SliceThreshold(double low, double high);


protected:

	void InternalTheshold(int dataType, double low, double high, int n, void * inputPointer, unsigned char * outputPointer);

	template<typename DataType>
	void InternalThreshold(double low, double high, int n, DataType *inputScalars, unsigned char *outputScalars);

	mafVMEVolumeGray *m_Volume;
	mafVMEVolumeGray *m_Segmentation;
	vtkImageData     *m_SegmetationSlice;
	vtkImageData     *m_VolumeSlice;



	int m_VolumeDimensions[3];          //<Dimensions of the volumes (number of slices)

	

};

#endif
