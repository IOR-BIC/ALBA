/*=========================================================================

Program: MAF2
Module: vtkMAFVolumeSlicer
Authors: Gianluigi Crimi

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "vtkMAFProjectVolume.h"
#include "vtkObjectFactory.h"
#include "vtkStructuredPoints.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkRectilinearGrid.h"
#include "vtkMAFSmartPointer.h"
#include "vtkProbeFilter.h"
#include "vtkDataSetWriter.h"

vtkCxxRevisionMacro(vtkMAFProjectVolume, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMAFProjectVolume);

#define AIR_LIMIT -500

//----------------------------------------------------------------------------
void vtkMAFProjectVolume::PropagateUpdateExtent(vtkDataObject *output)
{
}

//=========================================================================
vtkMAFProjectVolume::vtkMAFProjectVolume()
{
  ProjectionMode = VTK_PROJECT_FROM_X;
	ProjectSubRange = false;
	vtkSource::SetNthOutput(0, vtkStructuredPoints::New());
	// Releasing data
	Outputs[0]->ReleaseData();
	Outputs[0]->Delete();
}

//=========================================================================
void vtkMAFProjectVolume::ExecuteInformation()
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

	switch (this->ProjectionMode) {
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
void vtkMAFProjectVolume::Execute()
{
	int x, y, z, inputDims[3], projectedDims[3], idx, newIdx, range[2];
	int sliceSize, jOffset, kOffset;
	float acc, rangeSize;

	vtkRectilinearGrid *inputRG = vtkRectilinearGrid::SafeDownCast(GetInput());
	vtkImageData *inputID = vtkImageData::SafeDownCast(GetInput());
	vtkImageData *output = vtkImageData::SafeDownCast(GetOutput());


	vtkPointData 			*inputPd = inputRG ? inputRG->GetPointData() : inputID->GetPointData();
	vtkDataArray 			*inputScalars = inputPd->GetScalars();

	vtkDataArray 			*projScalars = inputPd->GetScalars()->NewInstance();

	inputRG ? inputRG->GetDimensions(inputDims) : inputID->GetDimensions(inputDims);
		
	switch (this->ProjectionMode) {
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
		projScalars->SetNumberOfTuples(projectedDims[0]*projectedDims[1]);
		
	//
	// Traverse input data and project points to output
	//
	newIdx = 0;

	//Set Projection Range
	if (ProjectSubRange)
	{
		range[0] = MAX(ProjectionRange[0], 0);
		range[1] = MIN(ProjectionRange[1], inputDims[ProjectionMode-1]);
		rangeSize = range[1] - range[0];
	}
	else
	{
		range[0] = 0;
		range[1] = inputDims[ProjectionMode-1];
		rangeSize = inputDims[ProjectionMode-1];
	}


	sliceSize = inputDims[0] * inputDims[1];

	switch (this->ProjectionMode)
	{
		case VTK_PROJECT_FROM_X:
			for (z = 0; z < inputDims[2]; z++)
			{
				kOffset = z * sliceSize;
				for (y = 0; y < inputDims[1]; y++)
				{
					jOffset = y * inputDims[0];
					acc = 0;
					for (x = range[0]; x < range[1]; x++)
					{
						idx = x + jOffset + kOffset;
						acc+=MAX(AIR_LIMIT,inputScalars->GetTuple1(idx));
					}
					projScalars->SetTuple1(newIdx++, acc / rangeSize);
				}
			}
			break;
		case VTK_PROJECT_FROM_Y:
			for (z = 0; z < inputDims[2]; z++)
			{
				kOffset = z * sliceSize;
				for (x = 0; x < inputDims[0]; x++)
				{
					acc = 0;
					for (y = range[0]; y < range[1]; y++)
					{
						jOffset = y * inputDims[0];
						idx = x + jOffset + kOffset;
						acc+=MAX(AIR_LIMIT,inputScalars->GetTuple1(idx));
					}
					projScalars->SetTuple1(newIdx++, acc / rangeSize);
				}
			}
			break;
		case VTK_PROJECT_FROM_Z:
			for (y = 0; y < inputDims[1]; y++)
			{
				jOffset = y * inputDims[0];
				for (x = 0; x < inputDims[0]; x++)
				{
					acc = 0;
					for (z = range[0]; z < range[1]; z++)
					{
						kOffset = z * sliceSize;
						idx = x + jOffset + kOffset;
						acc+=MAX(AIR_LIMIT,inputScalars->GetTuple1(idx));
					}
					projScalars->SetTuple1(newIdx++, acc / rangeSize);
				}
			}
			break;
	}

	if (inputRG)
		GenerateOutputFromRG(inputRG, projectedDims, projScalars);
	else
		GenerateOutputFromID(inputID, projectedDims, projScalars);

	vtkDEL(projScalars);
}

//----------------------------------------------------------------------------
void vtkMAFProjectVolume::GenerateOutputFromID(vtkImageData * inputSP, int * projectedDims, vtkDataArray * projScalars)
{
	double inputSpacing[3];
	double outputSpacing[3];
	vtkImageData *output = vtkImageData::SafeDownCast(GetOutput());
	
	inputSP->GetSpacing(inputSpacing);
	switch (this->ProjectionMode) {
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
void vtkMAFProjectVolume::GenerateOutputFromRG(vtkRectilinearGrid * inputRG, int * projectedDims, vtkDataArray * projScalars)
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

	switch (this->ProjectionMode) {
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
		
	GetBestSpacing(bestSpacing, rgOut);


	rgOut->GetBounds(bounds);

	outputDims[0] = ((bounds[1] - bounds[0]) / bestSpacing[0]) + 1;
	outputDims[1] = ((bounds[3] - bounds[2]) / bestSpacing[1]) + 1;
	outputDims[2] = 1;

	outputSpacing[0] = (bounds[1] - bounds[0]) / (double)outputDims[0];
	outputSpacing[1] = (bounds[3] - bounds[2]) / (double)outputDims[1];
	outputSpacing[2] = 1;

	vtkStructuredPoints *SP = vtkStructuredPoints::New();
	SP->SetDimensions(outputDims);
	SP->SetSpacing(outputSpacing);
	SP->SetOrigin(bounds[0], bounds[2], bounds[4]);

	vtkProbeFilter *probeFilter = vtkProbeFilter::New();
	probeFilter->SetInput(SP);
	probeFilter->SetSource(rgOut);
	probeFilter->Update();

	GetOutput()->DeepCopy(probeFilter->GetOutput());

	vtkDEL(SP);
	vtkDEL(probeFilter);
	vtkDEL(rgOut);
}

//=========================================================================
void vtkMAFProjectVolume::GetBestSpacing(double * bestSpacing, vtkRectilinearGrid* rGrid)
{
	bestSpacing[0] = VTK_DOUBLE_MAX;
	bestSpacing[1] = VTK_DOUBLE_MAX;
	bestSpacing[2] = VTK_DOUBLE_MAX;

	for (int xi = 1; xi < rGrid->GetXCoordinates()->GetNumberOfTuples(); xi++)
	{
		double spcx = rGrid->GetXCoordinates()->GetTuple1(xi) - rGrid->GetXCoordinates()->GetTuple1(xi - 1);
		if (bestSpacing[0] > spcx && spcx != 0.0)
			bestSpacing[0] = spcx;
	}

	for (int yi = 1; yi < rGrid->GetYCoordinates()->GetNumberOfTuples(); yi++)
	{
		double spcy = rGrid->GetYCoordinates()->GetTuple1(yi) - rGrid->GetYCoordinates()->GetTuple1(yi - 1);
		if (bestSpacing[1] > spcy && spcy != 0.0)
			bestSpacing[1] = spcy;
	}

	bestSpacing[2] = 1;
}

