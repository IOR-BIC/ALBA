/*=========================================================================

Program: ALBA
Module: vtkALBAVolumeOrthoSlicer
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include "vtkALBAVolumeOrthoSlicer.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkRectilinearGrid.h"
#include "vtkALBASmartPointer.h"
#include "vtkProbeFilter.h"
#include "vtkDataSetWriter.h"
#include "vtkMath.h"
#include "vtkALBARGtoSPImageFilter.h"
#include "vtkInformation.h"

vtkStandardNewMacro(vtkALBAVolumeOrthoSlicer);

#define EPSILON 1e-6

//----------------------------------------------------------------------------
void vtkALBAVolumeOrthoSlicer::SetPlaneOrigin(double *origin)
{
	Origin[0] = origin[0];
	Origin[1] = origin[1];
	Origin[2] = origin[2];
	Modified();
}

//----------------------------------------------------------------------------
void vtkALBAVolumeOrthoSlicer::PropagateUpdateExtent(vtkDataObject *output)
{
}

//=========================================================================
vtkALBAVolumeOrthoSlicer::vtkALBAVolumeOrthoSlicer()
{
  SclicingMode = ORTHOSLICER_X_SLICE;
	Origin[0] = Origin[1] = Origin[2] = 0;
}

//----------------------------------------------------------------------------
int vtkALBAVolumeOrthoSlicer::FillOutputPortInformation(int port, vtkInformation* info)
{
	// now add our info
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
	return 1;
}


//=========================================================================
int vtkALBAVolumeOrthoSlicer::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	vtkRectilinearGrid *inputRG = vtkRectilinearGrid::SafeDownCast(GetInput());
	vtkImageData *inputID = vtkImageData::SafeDownCast(GetInput());
	vtkImageData *output = vtkImageData::SafeDownCast(GetOutput());
  int dims[3], outDims[3], wholeExtent[6];
  
	if (inputID == NULL && inputRG == NULL)
	{
		vtkErrorMacro(<<"Missing input");
		return 0;
	}

	if (output == NULL)
	{
		vtkErrorMacro(<<"Output error");
		return 0;
	}

	if (inputRG)
		inputRG->GetExtent(wholeExtent);
	else
		inputID->GetExtent(wholeExtent);

	dims[0] = wholeExtent[1] - wholeExtent[0] + 1;
	dims[1] = wholeExtent[3] - wholeExtent[2] + 1;
	dims[2] = wholeExtent[5] - wholeExtent[4] + 1;

	switch (this->SclicingMode) {
		case ORTHOSLICER_X_SLICE:
			outDims[0] = dims[1];
			outDims[1] = dims[2];
			outDims[2] = 1;
			break;
		case ORTHOSLICER_Y_SLICE:
			outDims[0] = dims[0];
			outDims[1] = dims[2];
			outDims[2] = 1;
			break;
		case ORTHOSLICER_Z_SLICE:
			outDims[0] = dims[0];
			outDims[1] = dims[1];
			outDims[2] = 1;
	}

	wholeExtent[0] = 0;
	wholeExtent[1] = outDims[0] - 1;
	wholeExtent[2] = 0;
	wholeExtent[3] = outDims[1] - 1;
	wholeExtent[4] = 0;
	wholeExtent[5] = outDims[2] - 1;
  output->SetExtent( wholeExtent );

	return 1;
}

//=========================================================================
int vtkALBAVolumeOrthoSlicer::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	int inputDims[3], projectedDims[3];
	vtkRectilinearGrid *inputRG = vtkRectilinearGrid::SafeDownCast(GetInput());
	vtkImageData *inputID = vtkImageData::SafeDownCast(GetInput());
	vtkImageData *output = vtkImageData::SafeDownCast(GetOutput());
	
	vtkPointData 			*inputPd = inputRG ? inputRG->GetPointData() : inputID->GetPointData();
	vtkDataArray 			*inputScalars = inputPd->GetScalars();
	vtkDataArray 			*slicedScalars = inputScalars->NewInstance();

	inputRG ? inputRG->GetDimensions(inputDims) : inputID->GetDimensions(inputDims);
		
	switch (this->SclicingMode) {
		case ORTHOSLICER_X_SLICE:
			projectedDims[0] = inputDims[1];
			projectedDims[1] = inputDims[2];
			projectedDims[2] = 1;
			break;
		case ORTHOSLICER_Y_SLICE:
			projectedDims[0] = inputDims[0];
			projectedDims[1] = inputDims[2];
			projectedDims[2] = 1;
			break;
		case ORTHOSLICER_Z_SLICE:
			projectedDims[0] = inputDims[0];
			projectedDims[1] = inputDims[1];
			projectedDims[2] = 1;
			break;
	}

	slicedScalars->SetNumberOfTuples(projectedDims[0]*projectedDims[1]);
	void *inputPointer = inputScalars->GetVoidPointer(0);
	void *outputPointer = slicedScalars->GetVoidPointer(0);

	switch (inputScalars->GetDataType())
	{
		case VTK_CHAR:
			SliceScalars(inputDims, (char*)inputPointer, (char*)outputPointer);
			break;
		case VTK_UNSIGNED_CHAR:
			SliceScalars(inputDims, (unsigned char*)inputPointer, (unsigned char*)outputPointer);
			break;
		case VTK_SHORT:
			SliceScalars(inputDims, (short*)inputPointer, (short*)outputPointer);
			break;
		case VTK_UNSIGNED_SHORT:
			SliceScalars(inputDims, (unsigned short*)inputPointer, (unsigned short*)outputPointer);
			break;
		case VTK_INT:
			SliceScalars(inputDims, (int *)inputPointer, (int *)outputPointer);
			break;
		case VTK_UNSIGNED_INT:
			SliceScalars(inputDims, (unsigned int*)inputPointer, (unsigned int*)outputPointer);
			break;
		case VTK_FLOAT:
			SliceScalars(inputDims, (float*)inputPointer, (float*)outputPointer);
			break;
		case VTK_DOUBLE:  //NOTE: GPU is not allowed
			SliceScalars(inputDims, (double*)inputPointer, (double*)outputPointer);
			break;
		default:
			vtkErrorMacro(<< "vtkALBAVolumeSlicer: Scalar type is not supported");
			return 0;
	}

	if (inputRG)
		GenerateOutputFromRG(request, inputRG, projectedDims, slicedScalars);
	else
		GenerateOutputFromID(request, inputID, projectedDims, slicedScalars);

	vtkDEL(slicedScalars);

	return 1;
}

//----------------------------------------------------------------------------
template<typename DataType>
void vtkALBAVolumeOrthoSlicer::SliceScalars(int *inputDims, DataType *inputScalars, DataType *slicedScalars)
{
	vtkIdType x, y, z, idx1, idx2, newIdx, range[2], sliceSize, jOffset1, jOffset2, kOffset1, kOffset2;
	int plane1, plane2;
	double acc;
	double ratio1, ratio2;
	
	newIdx = 0;
	sliceSize = (vtkIdType)inputDims[0] * (vtkIdType)inputDims[1];
	GetSlicingInfo(&plane1, &plane2, &ratio1, &ratio2);
	
	switch (this->SclicingMode)
	{
		case ORTHOSLICER_X_SLICE:
			for (z = 0; z < inputDims[2]; z++)
			{
				kOffset1 = z * sliceSize;
				for (y = 0; y < inputDims[1]; y++)
				{
					jOffset1 = y * inputDims[0];
					idx1 = plane1 + jOffset1 + kOffset1;
					idx2 = plane2 + jOffset1 + kOffset1;
					acc = inputScalars[idx1]*ratio1 + inputScalars[idx2]*ratio2;
					slicedScalars[newIdx] = acc;
					newIdx++;
				}
			}
			break;
		case ORTHOSLICER_Y_SLICE:
			jOffset1 = plane1 * inputDims[0];
			jOffset2 = plane2 * inputDims[0];
			for (z = 0; z < inputDims[2]; z++)
			{
				kOffset1 = z * sliceSize;
				for (x = 0; x < inputDims[0]; x++)
				{
					idx1 = x + jOffset1 + kOffset1;
					idx2 = x + jOffset2 + kOffset1;
					acc = inputScalars[idx1]*ratio1 + inputScalars[idx2]*ratio2;
					slicedScalars[newIdx] = acc;
					newIdx++;
				}
			}
			break;
		case ORTHOSLICER_Z_SLICE:
			kOffset1 = plane1 * sliceSize;
			kOffset2 = plane2 * sliceSize;
			if (ratio1 == 1)
				memcpy(slicedScalars, inputScalars + kOffset1,  sizeof(DataType)*inputDims[0] * inputDims[1]);
			else if(ratio2 ==1)
				memcpy(slicedScalars, inputScalars + kOffset2, sizeof(DataType)*inputDims[0] * inputDims[1]);
			else
				for (y = 0; y < inputDims[1]; y++)
				{
					jOffset1 = y * inputDims[0];
					for (x = 0; x < inputDims[0]; x++)
					{
						idx1 = x + jOffset1 + kOffset1;
						idx2 = x + jOffset1 + kOffset2;
						acc = inputScalars[idx1]*ratio1 + inputScalars[idx2]*ratio2;
						slicedScalars[newIdx] = acc;
						newIdx++;
					}
				}
			break;
	}
}

//----------------------------------------------------------------------------
void vtkALBAVolumeOrthoSlicer::GenerateOutputFromID(vtkInformation *request, vtkImageData * inputSP, int * projectedDims, vtkDataArray * projScalars)
{
	double inputSpacing[3];
	double outputSpacing[3];
	vtkImageData *output = vtkImageData::SafeDownCast(GetOutput());
	
	inputSP->GetSpacing(inputSpacing);
	switch (this->SclicingMode) {
		case ORTHOSLICER_X_SLICE:
			outputSpacing[0] = inputSpacing[1];
			outputSpacing[1] = inputSpacing[2];
			outputSpacing[2] = 1;
			break;
		case ORTHOSLICER_Y_SLICE:
			outputSpacing[0] = inputSpacing[0];
			outputSpacing[1] = inputSpacing[2];
			outputSpacing[2] = 1;
			break;
		case ORTHOSLICER_Z_SLICE:
			outputSpacing[0] = inputSpacing[0];
			outputSpacing[1] = inputSpacing[1];
			outputSpacing[2] = 1;
	}

	output->SetScalarType(inputSP->GetScalarType(),request);
	output->SetNumberOfScalarComponents(inputSP->GetNumberOfScalarComponents(),request);
	output->SetDimensions(projectedDims);
	output->SetSpacing(outputSpacing);
	output->GetPointData()->SetScalars(projScalars);
}

//----------------------------------------------------------------------------
void vtkALBAVolumeOrthoSlicer::GenerateOutputFromRG(vtkInformation *request, vtkRectilinearGrid * inputRG, int * projectedDims, vtkDataArray * projScalars)
{
	//Generate temporary rectilinear grid output
	vtkRectilinearGrid *rgOut = vtkRectilinearGrid::New();
	vtkDataArray 			*XCoordinates, *YCoordinates, *ZCoordinates;
	double outputSpacing[3], bounds[6], bestSpacing[3];
	int outputDims[3];


	rgOut->SetDimensions(projectedDims);

	XCoordinates = inputRG->GetXCoordinates()->NewInstance();
	YCoordinates = inputRG->GetYCoordinates()->NewInstance();
	ZCoordinates = inputRG->GetZCoordinates()->NewInstance();

	XCoordinates->SetNumberOfTuples(projectedDims[0]);
	YCoordinates->SetNumberOfTuples(projectedDims[1]);
	ZCoordinates->SetNumberOfTuples(projectedDims[2]);

	ZCoordinates->InsertComponent(0, 0, 0);

	switch (this->SclicingMode) {
		case ORTHOSLICER_X_SLICE:
			XCoordinates->DeepCopy(inputRG->GetYCoordinates());
			YCoordinates->DeepCopy(inputRG->GetZCoordinates());
			break;
		case ORTHOSLICER_Y_SLICE:
			XCoordinates->DeepCopy(inputRG->GetXCoordinates());
			YCoordinates->DeepCopy(inputRG->GetZCoordinates());
			break;
		case ORTHOSLICER_Z_SLICE:
			XCoordinates->DeepCopy(inputRG->GetXCoordinates());
			YCoordinates->DeepCopy(inputRG->GetYCoordinates());
			break;
	}

	rgOut->SetXCoordinates(XCoordinates);
	rgOut->SetYCoordinates(YCoordinates);
	rgOut->SetZCoordinates(ZCoordinates);
	XCoordinates->Delete();
	YCoordinates->Delete();
	ZCoordinates->Delete();

	rgOut->GetPointData()->SetScalars(projScalars);
	
	
	vtkALBARGtoSPImageFilter *rgtosoFilter = vtkALBARGtoSPImageFilter::New();
	rgtosoFilter->SetInputData(rgOut);
	rgtosoFilter->Update();
	
	GetOutput()->DeepCopy(rgtosoFilter->GetOutput());

	vtkDEL(rgtosoFilter);
	vtkDEL(rgOut);
}

//----------------------------------------------------------------------------
void vtkALBAVolumeOrthoSlicer::GetSlicingInfo(int* plane1, int* plane2, double* ratio1, double* ratio2)
{
	vtkImageData *inputID = vtkImageData::SafeDownCast(GetInput());
	vtkRectilinearGrid *inputRG = vtkRectilinearGrid::SafeDownCast(GetInput());
	double bounds[6];

	if (inputID)
		inputID->GetBounds(bounds);
	else
		inputRG->GetBounds(bounds);

	if (Origin[SclicingMode]<=bounds[SclicingMode * 2])
	{
		*plane1 = *plane2 = 0;
		*ratio1 = 1.0;
		*ratio2 = 0.0;
		return;
	}
	
	if (Origin[SclicingMode] >= bounds[SclicingMode * 2 + 1])
	{
		int dims[3];
		if (inputID)
			inputID->GetDimensions(dims);
		else
			inputRG->GetDimensions(dims);

		*plane1 = *plane2 = dims[SclicingMode]-1;
		*ratio1 = 0.0;
		*ratio2 = 1.0;
		return;
	}

	if (inputID)
	{
		double spacing[3];
		inputID->GetSpacing(spacing);

		double slicingPercentage = (Origin[SclicingMode] - bounds[SclicingMode * 2]) / spacing[SclicingMode];
		*plane1 = floor(slicingPercentage);
		*plane2 = ceil(slicingPercentage);
		*ratio2 = slicingPercentage - *plane1;
		*ratio1 = 1.0 - *ratio2;
		return;
	}
	else
	{
		double prevPos, postPos;
		vtkDataArray * coordinates;
		switch (this->SclicingMode)
		{
			case ORTHOSLICER_X_SLICE:
				coordinates = inputRG->GetXCoordinates();
				break;
			case ORTHOSLICER_Y_SLICE:
				coordinates = inputRG->GetYCoordinates();
				break;
			case ORTHOSLICER_Z_SLICE:
				coordinates = inputRG->GetZCoordinates();
				break;
		}
		int nCoordinates = coordinates->GetNumberOfTuples();
		double *coordPointer = (double *) coordinates->GetVoidPointer(0);
		double n0, n1;
		double l, r;
		n1 = coordPointer[0];
		for (int i = 1; i < nCoordinates; i++)
		{
			//n0 was last n1
			n0 = n1;
			n1 = coordPointer[i];

			l = MIN(n0, n1);
			r = MAX(n0, n1);

			if ((Origin[SclicingMode] >= l && Origin[SclicingMode] <= r))
			{
				double spacing = r - l;
				double slicingPercentage = (Origin[SclicingMode] - l) / spacing;
				*plane1 = i - 1;
				*plane2 = i;
				*ratio2 = slicingPercentage;
				*ratio1 = 1.0 - *ratio2;
				return;
			}
		}
	}
}



