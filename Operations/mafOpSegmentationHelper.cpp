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
void mafOpSegmentationHelper::SliceThreshold(double low, double high)
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

	InternalTheshold(dataType, low, high, nTuples, inputPointer, outputPointer);

	m_SegmetationSlice->GetPointData()->SetScalars(outputScalars);

	vtkDEL(outputScalars);
}

//----------------------------------------------------------------------------
void mafOpSegmentationHelper::InternalTheshold(int dataType, double low, double high, int n, void * inputPointer, unsigned char * outputPointer)
{
	switch (dataType)
	{
		case VTK_CHAR:
			InternalThreshold(low, high, n, (char*)inputPointer, outputPointer);
			break;
		case VTK_UNSIGNED_CHAR:
			InternalThreshold(low, high, n, (unsigned char*)inputPointer, outputPointer);
			break;
		case VTK_SHORT:
			InternalThreshold(low, high, n, (short*)inputPointer, outputPointer);
			break;
		case VTK_UNSIGNED_SHORT:
			InternalThreshold(low, high, n, (unsigned short*)inputPointer, outputPointer);
			break;
		case VTK_INT:
			InternalThreshold(low, high, n, (int *)inputPointer, outputPointer);
			break;
		case VTK_UNSIGNED_INT:
			InternalThreshold(low, high, n, (unsigned int*)inputPointer, outputPointer);
			break;
		case VTK_FLOAT:
			InternalThreshold(low, high, n, (float*)inputPointer, outputPointer);
			break;
		case VTK_DOUBLE:  //NOTE: GPU is not allowed
			InternalThreshold(low, high, n, (double*)inputPointer, outputPointer);
			break;
		default:
			return;
	}
}

// ----------------------------------------------------------------------------
template<typename DataType>
void mafOpSegmentationHelper::InternalThreshold(double low, double high, int n, DataType *inputScalars, unsigned char *outputScalars)
{
	for (int i = 0; i < n; i++)
	{
		if (inputScalars[i] >= low && inputScalars[i] <= high)
			outputScalars[i] = FULL;
		else
			outputScalars[i] = EMPTY;
	}
}

