/*=========================================================================

Program: ALBA
Module: vtkALBAVolumeSlicer
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include "vtkALBAProjectVolume.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkRectilinearGrid.h"
#include "vtkALBASmartPointer.h"
#include "vtkProbeFilter.h"
#include "vtkDataSetWriter.h"
#include "vtkALBARGtoSPImageFilter.h"

vtkCxxRevisionMacro(vtkALBAProjectVolume, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkALBAProjectVolume);

#define AIR_LIMIT -500

#define PROJECT_MEAN (ProjectionModality == VTK_PROJECT_MEAN)

//----------------------------------------------------------------------------
void vtkALBAProjectVolume::PropagateUpdateExtent(vtkDataObject *output)
{
}

//=========================================================================
vtkALBAProjectVolume::vtkALBAProjectVolume()
{
  ProjectionSide = VTK_PROJECT_FROM_X;
	ProjectionModality = VTK_PROJECT_MEAN;
	ProjectSubRange = false;
	vtkSource::SetNthOutput(0, vtkImageData::New());
	// Releasing data
	Outputs[0]->ReleaseData();
	Outputs[0]->Delete();
}

//=========================================================================
void vtkALBAProjectVolume::ExecuteInformation()
{
	vtkRectilinearGrid *inputRG = vtkRectilinearGrid::SafeDownCast(GetInput());
	vtkImageData *inputID = vtkImageData::SafeDownCast(GetInput());
	vtkImageData *output = vtkImageData::SafeDownCast(GetOutput());
  int dims[3], outDims[3], wholeExtent[6];
  
	if (inputID == NULL && inputRG == NULL)
	{
		vtkErrorMacro("Missing input");
		return;
	}

	if (output == NULL)
	{
		vtkErrorMacro("Output error");
		return;
	}

	if (inputRG)
		inputRG->GetWholeExtent(wholeExtent);
	else
		inputID->GetWholeExtent(wholeExtent);

	dims[0] = wholeExtent[1] - wholeExtent[0] + 1;
	dims[1] = wholeExtent[3] - wholeExtent[2] + 1;
	dims[2] = wholeExtent[5] - wholeExtent[4] + 1;

	switch (this->ProjectionSide) {
		case VTK_PROJECT_FROM_X:
			outDims[0] = dims[1];
			outDims[1] = dims[2];
			outDims[2] = 1;
			break;
		case VTK_PROJECT_FROM_Y:
			outDims[0] = dims[0];
			outDims[1] = dims[2];
			outDims[2] = 1;
			break;
		case VTK_PROJECT_FROM_Z:
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
  output->SetWholeExtent( wholeExtent );
}

//=========================================================================
void vtkALBAProjectVolume::Execute()
{
	int inputDims[3], projectedDims[3];

	vtkRectilinearGrid *inputRG = vtkRectilinearGrid::SafeDownCast(GetInput());
	vtkImageData *inputID = vtkImageData::SafeDownCast(GetInput());
	vtkImageData *output = vtkImageData::SafeDownCast(GetOutput());


	vtkPointData 			*inputPd = inputRG ? inputRG->GetPointData() : inputID->GetPointData();
	vtkDataArray 			*inputScalars = inputPd->GetScalars();
	vtkDataArray 			*projScalars = inputScalars->NewInstance();

	inputRG ? inputRG->GetDimensions(inputDims) : inputID->GetDimensions(inputDims);

	switch (this->ProjectionSide) {
		case VTK_PROJECT_FROM_X:
			projectedDims[0] = inputDims[1];
			projectedDims[1] = inputDims[2];
			projectedDims[2] = 1;
			break;
		case VTK_PROJECT_FROM_Y:
			projectedDims[0] = inputDims[0];
			projectedDims[1] = inputDims[2];
			projectedDims[2] = 1;
			break;
		case VTK_PROJECT_FROM_Z:
			projectedDims[0] = inputDims[0];
			projectedDims[1] = inputDims[1];
			projectedDims[2] = 1;
			break;
	}
	projScalars->SetNumberOfTuples(projectedDims[0] * projectedDims[1]);

	void *inputPointer = inputScalars->GetVoidPointer(0);
	void *outputPointer = projScalars->GetVoidPointer(0);

	switch (inputScalars->GetDataType())
	{
		case VTK_CHAR:
			ProjectScalars(inputDims, (char*)inputPointer, (char*)outputPointer);
			break;
		case VTK_UNSIGNED_CHAR:
			ProjectScalars(inputDims, (unsigned char*)inputPointer, (unsigned char*)outputPointer);
			break;
		case VTK_SHORT:
			ProjectScalars(inputDims, (short*)inputPointer, (short*)outputPointer);
			break;
		case VTK_UNSIGNED_SHORT:
			ProjectScalars(inputDims, (unsigned short*)inputPointer, (unsigned short*)outputPointer);
			break;
		case VTK_INT:
			ProjectScalars(inputDims, (int*)inputPointer, (int*)outputPointer);
			break;
		case VTK_UNSIGNED_INT:
			ProjectScalars(inputDims, (unsigned int*)inputPointer, (unsigned int*)outputPointer);
			break;
		case VTK_FLOAT:
			ProjectScalars(inputDims, (float*)inputPointer, (float*)outputPointer);
			break;
		case VTK_DOUBLE:
			ProjectScalars(inputDims, (double*)inputPointer, (double*)outputPointer);
			break;
		default:
			vtkErrorMacro(<< "vtkALBAVolumeSlicer: Scalar type is not supported");
			return;
	}

	if (inputRG)
		GenerateOutputFromRG(inputRG, projectedDims, projScalars);
	else
		GenerateOutputFromID(inputID, projectedDims, projScalars);

	vtkDEL(projScalars);
}

//----------------------------------------------------------------------------
template<typename DataType>
void vtkALBAProjectVolume::ProjectScalars(int * inputDims, DataType * inputScalars, DataType * projScalars)
{
	int x, y, z, idx, newIdx, range[2];
	int sliceSize, jOffset, kOffset;
	float acc, rangeSize;

	//Set Projection Range
	if (ProjectSubRange)
	{
		range[0] = MAX(ProjectionRange[0], 0);
		range[1] = MIN(ProjectionRange[1], inputDims[ProjectionSide - 1]);
		rangeSize = range[1] - range[0];
	}
	else
	{
		range[0] = 0;
		range[1] = inputDims[ProjectionSide - 1];
		rangeSize = inputDims[ProjectionSide - 1];
	}


	sliceSize = inputDims[0] * inputDims[1];
	newIdx = 0;

	switch (this->ProjectionSide)
	{
		case VTK_PROJECT_FROM_X:
			for (z = 0; z < inputDims[2]; z++)
			{
				kOffset = z * sliceSize;
				for (y = 0; y < inputDims[1]; y++)
				{
					jOffset = y * inputDims[0];
					acc = PROJECT_MEAN ? 0 : VTK_FLOAT_MIN;
					for (x = range[0]; x < range[1]; x++)
					{
						idx = x + jOffset + kOffset;
						if(PROJECT_MEAN)
							acc += MAX(AIR_LIMIT, inputScalars[idx]);
						else
							acc = MAX(acc, inputScalars[idx]);
					}
					projScalars[newIdx] = PROJECT_MEAN ? (acc / rangeSize) : acc;
					newIdx++;
				}
			}
			break;
		case VTK_PROJECT_FROM_Y:
			for (z = 0; z < inputDims[2]; z++)
			{
				kOffset = z * sliceSize;
				for (x = 0; x < inputDims[0]; x++)
				{
					acc = PROJECT_MEAN ? 0 : VTK_FLOAT_MIN;
					for (y = range[0]; y < range[1]; y++)
					{
						jOffset = y * inputDims[0];
						idx = x + jOffset + kOffset;
						if(PROJECT_MEAN)
							acc += MAX(AIR_LIMIT, inputScalars[idx]);
						else
							acc = MAX(acc, inputScalars[idx]);
					}
					projScalars[newIdx] = PROJECT_MEAN ? (acc / rangeSize) : acc;
					newIdx++;
				}
			}
			break;
		case VTK_PROJECT_FROM_Z:
			for (y = 0; y < inputDims[1]; y++)
			{
				jOffset = y * inputDims[0];
				for (x = 0; x < inputDims[0]; x++)
				{
					acc = PROJECT_MEAN ? 0 : VTK_FLOAT_MIN;
					for (z = range[0]; z < range[1]; z++)
					{
						kOffset = z * sliceSize;
						idx = x + jOffset + kOffset;
						if(PROJECT_MEAN)
							acc += MAX(AIR_LIMIT, inputScalars[idx]);
						else
							acc = MAX(acc, inputScalars[idx]);
					}
					projScalars[newIdx] = PROJECT_MEAN ? (acc / rangeSize) : acc;
					newIdx++;
				}
			}
			break;
	}
}

//----------------------------------------------------------------------------
void vtkALBAProjectVolume::GenerateOutputFromID(vtkImageData * inputSP, int * projectedDims, vtkDataArray * projScalars)
{
	double inputSpacing[3];
	double outputSpacing[3];
	vtkImageData *output = vtkImageData::SafeDownCast(GetOutput());
	
	inputSP->GetSpacing(inputSpacing);
	switch (this->ProjectionSide) {
		case VTK_PROJECT_FROM_X:
			outputSpacing[0] = inputSpacing[1];
			outputSpacing[1] = inputSpacing[2];
			outputSpacing[2] = 1;
			break;
		case VTK_PROJECT_FROM_Y:
			outputSpacing[0] = inputSpacing[0];
			outputSpacing[1] = inputSpacing[2];
			outputSpacing[2] = 1;
			break;
		case VTK_PROJECT_FROM_Z:
			outputSpacing[0] = inputSpacing[0];
			outputSpacing[1] = inputSpacing[1];
			outputSpacing[2] = 1;
	}

	output->SetScalarType(inputSP->GetScalarType());
	output->SetNumberOfScalarComponents(inputSP->GetNumberOfScalarComponents());
	output->SetDimensions(projectedDims);
	output->SetSpacing(outputSpacing);
	output->GetPointData()->SetScalars(projScalars);
}

//----------------------------------------------------------------------------
void vtkALBAProjectVolume::GenerateOutputFromRG(vtkRectilinearGrid * inputRG, int * projectedDims, vtkDataArray * projScalars)
{
	//Generate temporary rectilinear grid output
	vtkRectilinearGrid *rgOut = vtkRectilinearGrid::New();
	vtkDataArray 			*XCoordinates, *YCoordinates, *ZCoordinates;

	rgOut->SetDimensions(projectedDims);

	XCoordinates = inputRG->GetXCoordinates()->NewInstance();
	YCoordinates = inputRG->GetYCoordinates()->NewInstance();
	ZCoordinates = inputRG->GetZCoordinates()->NewInstance();

	XCoordinates->SetNumberOfTuples(projectedDims[0]);
	YCoordinates->SetNumberOfTuples(projectedDims[1]);
	ZCoordinates->SetNumberOfTuples(projectedDims[2]);

	ZCoordinates->InsertComponent(0, 0, 0);

	switch (this->ProjectionSide) {
		case VTK_PROJECT_FROM_X:
			XCoordinates->DeepCopy(inputRG->GetYCoordinates());
			YCoordinates->DeepCopy(inputRG->GetZCoordinates());
			break;
		case VTK_PROJECT_FROM_Y:
			XCoordinates->DeepCopy(inputRG->GetXCoordinates());
			YCoordinates->DeepCopy(inputRG->GetZCoordinates());
			break;
		case VTK_PROJECT_FROM_Z:
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
	rgtosoFilter->SetInput(rgOut);
	rgtosoFilter->Update();
	
	GetOutput()->DeepCopy(rgtosoFilter->GetOutput());

	vtkDEL(rgtosoFilter);
	vtkDEL(rgOut);
}
