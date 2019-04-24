/*=========================================================================

 Program: MAF2
 Module: mafOpSegmentationHelper
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafOpSegmentationHelper.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "vtkRectilinearGrid.h"


//----------------------------------------------------------------------------
mafOpSegmentationHelper::mafOpSegmentationHelper()
{
	m_Segmentation = m_Volume = NULL;
	m_SegmetationSlice = m_SegmetationSlice = NULL;
}

//----------------------------------------------------------------------------
mafOpSegmentationHelper::~mafOpSegmentationHelper()
{
}

//----------------------------------------------------------------------------
void mafOpSegmentationHelper::SetVolumes(mafVMEVolumeGray *volume, mafVMEVolumeGray *segmentation)
{
	m_Volume = volume;
	m_Segmentation = segmentation;
}

//----------------------------------------------------------------------------
void mafOpSegmentationHelper::SetSlices(vtkImageData *volumeSlice, vtkImageData *segmentationSlice)
{
	m_VolumeSlice = volumeSlice;
	m_SegmetationSlice = segmentationSlice;
}

//----------------------------------------------------------------------------
void mafOpSegmentationHelper::SliceThreshold(double *threshold)
{
	m_VolumeSlice->Update();
	m_SegmetationSlice->Update();
	vtkDataArray 			*inputScalars = m_VolumeSlice->GetPointData()->GetScalars();
	void *inputPointer = inputScalars->GetVoidPointer(0);
	int nTuples = inputScalars->GetNumberOfTuples();
	int dataType = inputScalars->GetDataType();


	vtkUnsignedCharArray *outputScalars;
	vtkNEW(outputScalars);
	outputScalars->SetNumberOfTuples(nTuples);
	unsigned char *outputPointer = (unsigned char*)outputScalars->GetVoidPointer(0);

	InternalTheshold(dataType, threshold, nTuples, inputPointer, outputPointer);

	m_SegmetationSlice->GetPointData()->SetScalars(outputScalars);

	vtkDEL(outputScalars);
}
//----------------------------------------------------------------------------
void mafOpSegmentationHelper::VolumeThreshold(double *threshold)
{
	m_VolumeSlice->Update();
	vtkDataSet * segmentation = m_Segmentation->GetOutput()->GetVTKData();
	vtkDataArray 	*inputScalars = m_Volume->GetOutput()->GetVTKData()->GetPointData()->GetScalars();
	void *inputPointer = inputScalars->GetVoidPointer(0);
	int nTuples = inputScalars->GetNumberOfTuples();
	int dataType = inputScalars->GetDataType();


	vtkUnsignedCharArray *outputScalars;
	vtkNEW(outputScalars);
	outputScalars->SetNumberOfTuples(nTuples);
	unsigned char *outputPointer = (unsigned char*)outputScalars->GetVoidPointer(0);

	InternalTheshold(dataType,threshold, nTuples, inputPointer, outputPointer);

	segmentation->GetPointData()->SetScalars(outputScalars);

	vtkDEL(outputScalars);
}

//----------------------------------------------------------------------------
void mafOpSegmentationHelper::VolumeThreshold(std::vector<AutomaticInfoRange> *rangesVector)
{
	vtkRectilinearGrid *volRG;
	m_VolumeSlice->Update();
	vtkDataSet * segmentation = m_Segmentation->GetOutput()->GetVTKData();
	vtkDataSet * volumeVtkData = m_Volume->GetOutput()->GetVTKData();
	vtkDataArray 	*inputScalars =volumeVtkData->GetPointData()->GetScalars();
	void *inputPointer = inputScalars->GetVoidPointer(0);
	int nTuples = inputScalars->GetNumberOfTuples();
	int dataType = inputScalars->GetDataType();

	vtkUnsignedCharArray *outputScalars;
	vtkNEW(outputScalars);
	outputScalars->SetNumberOfTuples(nTuples);
	unsigned char *outputPointer = (unsigned char*)outputScalars->GetVoidPointer(0);

	int dims[3];
	if (volRG = vtkRectilinearGrid::SafeDownCast(volumeVtkData))
	{
		dims[0] = volRG->GetXCoordinates()->GetNumberOfTuples();
		dims[1] = volRG->GetYCoordinates()->GetNumberOfTuples();
		dims[2] = volRG->GetZCoordinates()->GetNumberOfTuples();
	}
	else
	{
		((vtkImageData *)volumeVtkData)->GetDimensions(dims);
	}

	int sliceSize = dims[0] * dims[1];

	for (int i = 0; i < rangesVector->size(); i++)
	{
		AutomaticInfoRange range = (*rangesVector)[i];
		int areaStart = (range.m_StartSlice - 1)*sliceSize;
		int areaSize = (range.m_EndSlice+1 - range.m_StartSlice)*sliceSize;

		InternalTheshold(dataType, range.m_Threshold, areaSize, inputPointer, outputPointer, areaStart);
	}

	segmentation->GetPointData()->SetScalars(outputScalars);

	vtkDEL(outputScalars);
}

//----------------------------------------------------------------------------
void mafOpSegmentationHelper::InternalTheshold(int dataType, double *threshold, int n, void * inputPointer, unsigned char * outputPointer, int offset)
{
	switch (dataType)
	{
		case VTK_CHAR:
			InternalThreshold(threshold, n, (char*)inputPointer, outputPointer, offset);
			break;
		case VTK_UNSIGNED_CHAR:
			InternalThreshold(threshold, n, (unsigned char*)inputPointer, outputPointer, offset);
			break;
		case VTK_SHORT:
			InternalThreshold(threshold, n, (short*)inputPointer, outputPointer, offset);
			break;
		case VTK_UNSIGNED_SHORT:
			InternalThreshold(threshold, n, (unsigned short*)inputPointer, outputPointer, offset);
			break;
		case VTK_INT:
			InternalThreshold(threshold, n, (int *)inputPointer, outputPointer, offset);
			break;
		case VTK_UNSIGNED_INT:
			InternalThreshold(threshold, n, (unsigned int*)inputPointer, outputPointer, offset);
			break;
		case VTK_FLOAT:
			InternalThreshold(threshold, n, (float*)inputPointer, outputPointer, offset);
			break;
		case VTK_DOUBLE:  //NOTE: GPU is not allowed
			InternalThreshold(threshold, n, (double*)inputPointer, outputPointer, offset);
			break;
		default:
			return;
	}
}

// ----------------------------------------------------------------------------
template<typename DataType>
void mafOpSegmentationHelper::InternalThreshold(double *threshold, int n, DataType *inputScalars, unsigned char *outputScalars, int offset)
{
	for (int i = offset; i < offset+n; i++)
	{
		if (inputScalars[i] >= threshold[0] && inputScalars[i] <= threshold[1])
			outputScalars[i] = FULL;
		else
			outputScalars[i] = EMPTY;
	}
}

