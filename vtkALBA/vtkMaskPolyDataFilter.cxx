/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMaskPolyDataFilter.cxx
  Language:  C++
  Date:      2001/01/26
  Version:   1.1


Copyright (c) 1993-1998 Ken Martin, Will Schroeder, Bill Lorensen, Gianluigi Crimi

This software is copyrighted by Ken Martin, Will Schroeder and Bill Lorensen.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files. This copyright specifically does
not apply to the related textbook "The Visualization Toolkit" ISBN
013199837-4 published by Prentice Hall which is covered by its own copyright.

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any purpose, provided that existing
copyright notices are retained in all copies and that this notice is included
verbatim in any distributions. Additionally, the authors grant permission to
modify this software and its documentation for any purpose, provided that
such modifications are not distributed without the explicit consent of the
authors and that existing copyright notices are retained in all copies. Some
of the algorithms implemented by this software are patented, observe all
applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================*/
#include <math.h>
#include "albaDefines.h"
#include "vtkMaskPolyDataFilter.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkALBASmartPointer.h"
#include "vtkCellLocator.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredPoints.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkAlgorithm.h"
#include "vtkExecutive.h"

vtkStandardNewMacro(vtkMaskPolyDataFilter);

// Construct with user-specified polygonal dataset, initial distance of 0.0,
// capping is turned on with CapValue equal to a large positive number and 
// generating mask scalars turned off.
vtkMaskPolyDataFilter::vtkMaskPolyDataFilter(vtkPolyData *mask)
{
	this->SetNumberOfInputPorts(2);
	this->CurrentSliceMask=NULL;
  this->SetMask(mask);
//  this->GenerateMaskScalars = 0;
  this->Distance = 0;
  this->InsideOut = this->Binarize = this->TriplePass = 0;
  this->InsideValue=this->OutsideValue=0.0;

}

vtkMaskPolyDataFilter::~vtkMaskPolyDataFilter()
{
	vtkDEL(this->CurrentSliceMask);
	delete [] IdConversionTable;
  this->SetMask(NULL);  
}



//------------------------------------------------------------------------------
int vtkMaskPolyDataFilter::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkDataSet  *input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkDataSet *output = vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
	
	if (this->TriplePass)
		return TriplePassAlgorithm(input, output);
	else
		return StandardAlgorithm(input, output);
}



//----------------------------------------------------------------------------
int vtkMaskPolyDataFilter::StandardAlgorithm(vtkDataSet *input, vtkDataSet *output)
{
	

	int i, j, k, abortExecute = 0;

	int numPts = input->GetNumberOfPoints();
	vtkPointData *inPointData = input->GetPointData(), *outPointData = output->GetPointData();
	vtkCellData *inCellData = input->GetCellData(), *outCellData = output->GetCellData();
	vtkIdType idx, cellId;
	int subId;
	double  closestPoint[3];
	double *currentPoint, *x1;
	x1 = new double[3];
	double distance2;
	double dot, n[3];
	vtkCell *cell;
	vtkIdList *cellIds = vtkIdList::New();
	double pcoords[3];
	double bounds[6];
	double currentZ = VTK_DOUBLE_MIN;


	//  vtkDebugMacro(<< "Executing MaskPolyDataFilter");

	//
	// Initialize self; create output objects
	//
	if (!this->GetMask())
	{
		//	vtkErrorMacro(<<"No mask specified");
		return 1;
	}

	Mask = this->GetMask();

	Mask->GetBounds(bounds);

	double *weights = new double[4];
	cellIds->SetNumberOfIds(Mask->GetMaxCellSize());

	if (numPts < 1)
	{
		//vtkErrorMacro(<<"No data to Mask");
		return 1;
	}


	//
	// Create objects to hold output of contour operation
	//
	output->DeepCopy(input);

	outPointData->DeepCopy(inPointData);
	outCellData->DeepCopy(inCellData);

	int numcomp = outPointData->GetScalars()->GetNumberOfComponents();

	float *inTuple = new float[numcomp];
	float *outTuple = new float[numcomp];

	for (int n = 0; n < numcomp; n++)
	{
		inTuple[n] = this->InsideValue;
		outTuple[n] = this->OutsideValue;
	}

	this->Distance2 = this->Distance * this->Distance;

	int oldProgress = 0;
	int progress = 0;

	InitCurrentSliceMask();


	vtkALBASmartPointer<vtkCellLocator> cellLocator;

	for (i = 0; i < numPts && !abortExecute; i++)
	{

		progress = ((float)i * 100 / (float)numPts);

		if (progress != oldProgress)
		{
			oldProgress = progress;
			this->UpdateProgress((float)progress / 100.0);
			abortExecute = this->GetAbortExecute();
		}

		dot = 1;
		currentPoint = output->GetPoint(i);



		// use XNOR table to exclude points that not respect the InsideOut and bounding box condition
		// i.e. if InsideOut is 1 points inside the sourface must be sets to the FillValue, so points upside the bounding 
		// box should be skipped
		if ((currentPoint[0] <= bounds[0]) || (currentPoint[0] >= bounds[1]) || (currentPoint[1] <= bounds[2]) || (currentPoint[1] >= bounds[3]) || (currentPoint[2] <= bounds[4]) || (currentPoint[2] >= bounds[5]))
		{
			//the point is outside the mask's bounds
			if (this->InsideOut && !this->Binarize)
			{
				//if InsideOut==1 the point outside should be skipped
				continue;
			}
			else
			{
				// if insideOut==0 the point outside should be filled with the FillValue
				outPointData->GetScalars()->SetTuple(i, outTuple);
				// done: go to next point
				continue;
			}
		}
		// at this point the cell is inside the bounds

		if (currentZ != currentPoint[2])
		{
			currentZ = currentPoint[2];
			UpdateCurrentSliceMask(currentZ,2);
			cellLocator->SetDataSet(CurrentSliceMask);
			cellLocator->BuildLocator();
			cellLocator->Update();
		}


		cellLocator->FindClosestPoint(currentPoint, closestPoint, cellId, subId, distance2);


		if (cellId >= 0)
		{

			cell = CurrentSliceMask->GetCell(cellId);
			// Find the normal  
			vtkPolygon::ComputeNormal(cell->Points, n);

			// Vector from x to x1
			for (k = 0; k < 3; k++)
				x1[k] = currentPoint[k] - closestPoint[k];

			// Projection of the distance vector on the normal
			dot = vtkMath::Dot(x1, n);
		}

		// find if the x point is inside or outside of the polygon: sign of the distance
		if (dot < 0) {
			distance2 = -distance2;
		}

		if ((this->InsideOut || this->Binarize) && (distance2 <= this->Distance2))
			outPointData->GetScalars()->SetTuple(i, inTuple);


		if ((!this->InsideOut || this->Binarize) && (distance2 > this->Distance2))
			outPointData->GetScalars()->SetTuple(i, outTuple);
	}


	//
	// Loop over all points creating scalar output determined by evaluating 
	// points using mask geometry.
	//

	delete[] x1;
	delete[] weights;
	delete inTuple;
	delete outTuple;
	cellIds->Delete();

	return 0;
}

//----------------------------------------------------------------------------
int vtkMaskPolyDataFilter::TriplePassAlgorithm(vtkDataSet *input, vtkDataSet *output)
{
	int i, j, k, abortExecute = 0;
	int numPts = input->GetNumberOfPoints();
	vtkPointData *inPointData = input->GetPointData(), *outPointData = output->GetPointData();
	vtkCellData *inCellData = input->GetCellData(), *outCellData = output->GetCellData();
	vtkIdType idx, cellId;
	int dims[3], voxel[3], plane2, plane3, currentPointId;
	int subId;
	double  closestPoint[3];
	double *currentPoint, *x1;
	x1 = new double[3];
	double distance2;
	double dot, n[3];
	vtkCell *cell;
	vtkIdList *cellIds = vtkIdList::New();
	double pcoords[3];
	double bounds[6];

	//  vtkDebugMacro(<< "Executing MaskPolyDataFilter");

	//
	// Initialize self; create output objects
	//
	if (!this->GetMask())
	{
		//	vtkErrorMacro(<<"No mask specified");
		return 1;
	}

	Mask = this->GetMask();

	Mask->GetBounds(bounds);

	double *weights = new double[4];
	cellIds->SetNumberOfIds(Mask->GetMaxCellSize());

	if (numPts < 1)
	{
		//vtkErrorMacro(<<"No data to Mask");
		return 1;
	}


	//
	// Create objects to hold output of contour operation
	//
	output->DeepCopy(input);

	outCellData->DeepCopy(inCellData);

	int numcomp = outPointData->GetScalars()->GetNumberOfComponents();

	float *inTuple = new float[numcomp];
	float *outTuple = new float[numcomp];

	for (int n = 0; n < numcomp; n++)
	{
		inTuple[n] = this->InsideValue;
		outTuple[n] = this->OutsideValue;
	}

	this->Distance2 = this->Distance * this->Distance;

	int oldProgress = 0;
	int progress = 0;

	InitCurrentSliceMask();


	vtkALBASmartPointer<vtkCellLocator> cellLocator;

	if (vtkRectilinearGrid::SafeDownCast(input))
		vtkRectilinearGrid::SafeDownCast(input)->GetDimensions(dims);
	else if (vtkStructuredPoints::SafeDownCast(input))
		vtkStructuredPoints::SafeDownCast(input)->GetDimensions(dims);

	for (i = 0; i < numPts && !abortExecute; i++)
			outPointData->GetScalars()->SetTuple1(i, 0);

	i = 0;
	for (int plane = 0; plane <= 2; plane++)
	{
		double currentHeight = VTK_DOUBLE_MIN;

		plane2 = (plane + 1) % 3;
		plane3 = (plane + 2) % 3;
		for (voxel[plane] = 0; voxel[plane] < dims[plane]; voxel[plane]++)
			for (voxel[plane2] = 0; voxel[plane2] < dims[plane2]; voxel[plane2]++)
				for (voxel[plane3] = 0; voxel[plane3] < dims[plane3]; voxel[plane3]++)
				{
					i++;
					progress = ((float)i * 100 / ((float)numPts * 3));

					if (progress != oldProgress)
					{
						oldProgress = progress;
						this->UpdateProgress((float)progress / 100.0);
						abortExecute = this->GetAbortExecute();
					}

					dot = 1;

					currentPointId = voxel[0] + (voxel[1] * dims[0]) + voxel[2] * dims[0] * dims[1];
					currentPoint = output->GetPoint(currentPointId);



					// use XNOR table to exclude points that not respect the InsideOut and bounding box condition
					// i.e. if InsideOut is 1 points inside the sourface must be sets to the FillValue, so points upside the bounding 
					// box should be skipped
					if ((currentPoint[0] <= bounds[0]) || (currentPoint[0] >= bounds[1]) || (currentPoint[1] <= bounds[2]) || (currentPoint[1] >= bounds[3]) || (currentPoint[2] <= bounds[4]) || (currentPoint[2] >= bounds[5]))
					{
						//the point is outside the mask's bounds
						continue;
					}
					// at this point the cell is inside the bounds

					if (currentHeight != currentPoint[plane])
					{
						currentHeight = currentPoint[plane];
						UpdateCurrentSliceMask(currentHeight, plane);
						cellLocator->SetDataSet(CurrentSliceMask);
						cellLocator->BuildLocator();
						cellLocator->Update();
					}


					cellLocator->FindClosestPoint(currentPoint, closestPoint, cellId, subId, distance2);


					if (cellId >= 0)
					{

						cell = CurrentSliceMask->GetCell(cellId);
						// Find the normal  
						vtkPolygon::ComputeNormal(cell->Points, n);

						// Vector from x to x1
						for (k = 0; k < 3; k++)
							x1[k] = currentPoint[k] - closestPoint[k];

						// Projection of the distance vector on the normal
						dot = vtkMath::Dot(x1, n);
					}

					// find if the x point is inside or outside of the polygon: sign of the distance
					if (dot < 0) {
						distance2 = -distance2;
					}

					if (distance2 <= this->Distance2)
						outPointData->GetScalars()->SetTuple1(currentPointId, outPointData->GetScalars()->GetTuple1(currentPointId)+1);
				}
	}



	//
	// Loop over all points creating scalar output determined by evaluating 
	// points using mask geometry.
	//

	for (i = 0; i < numPts && !abortExecute; i++)
	{
		double value =  outPointData->GetScalars()->GetTuple1(i);

		if ((this->InsideOut || this->Binarize) && (value>=2))
			outPointData->GetScalars()->SetTuple(i, inTuple);
		else if ((!this->InsideOut || this->Binarize) && (value < 2))
			outPointData->GetScalars()->SetTuple(i, outTuple);
		else 
			outPointData->GetScalars()->SetTuple(i, inPointData->GetScalars()->GetTuple(i));
	}

	delete[] x1;
	delete[] weights;
	delete inTuple;
	delete outTuple;
	cellIds->Delete();

  return 0;
}

//----------------------------------------------------------------------------
void vtkMaskPolyDataFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataSetAlgorithm ::PrintSelf(os,indent);

  os << indent << "Mask Function: " << this->GetMask() << "\n";

  os << indent << "Distance: " << this->Distance << "\n";

}

vtkPolyData * vtkMaskPolyDataFilter::GetMask()
{
	return (vtkPolyData *)(this->GetExecutive()->GetInputData(1,0));
}

void vtkMaskPolyDataFilter::InitCurrentSliceMask()
{
	//Creating current slice mask
	vtkNEW(CurrentSliceMask);
	//init data by coping mask data
	CurrentSliceMask->DeepCopy(Mask);
	//allocating memory for id conversion table
	IdConversionTable= new vtkIdType[Mask->GetNumberOfPoints()];
}

//----------------------------------------------------------------------------
void vtkMaskPolyDataFilter::UpdateCurrentSliceMask(double value, int plane)
{
	int nPoints=Mask->GetNumberOfPoints();
	vtkIdType *cellIds;
	
	//generate points
	vtkPoints *new_points;
	vtkNEW(new_points);
	//generate polydata structure
	vtkCellArray * new_cells;
	vtkNEW(new_cells);


	//reset conversion table values
	memset(IdConversionTable,-1,sizeof(vtkIdType)*nPoints);
	
	vtkIdType cellId;
	vtkIdType pointOverBound, pointUnderBound, cellNPoints, addedPoints;

	cellId=addedPoints=0;

	vtkCellArray *polys;
	polys=Mask->GetPolys();
	int nCell=polys->GetNumberOfCells();
	
	//Searching cells that intersect current z-plane
	for(int i=0;i<nCell;i++)
	{
		polys->GetCell(cellId,cellNPoints,cellIds);
		cellId+=cellNPoints+1;
		pointOverBound=pointUnderBound=0;
	
		//If there is at least one point under current z value and a point over
		//the cell will intersect the plane
		for(int j=0;j<cellNPoints;j++)
		{
			double *point=Mask->GetPoint(cellIds[j]);
			if(point[plane]<=value)
				pointUnderBound=true;
			if(point[plane]>=value)
				pointOverBound=true;
			if(pointUnderBound && pointOverBound)
				break;
		}

		if(pointOverBound && pointUnderBound)
		{
			//the current cell is intersecting the plane adding it to the new cells
			new_cells->InsertNextCell(cellNPoints);

			//We relocate the cell points in order to obtain an output with only required points
			for(int j=0;j<cellNPoints;j++)
			{
				//get the Mask point Id
				int pointId=cellIds[j];
				
				//if the conversion table of that point is < 0 the point is not inserted to the new points
				if(IdConversionTable[pointId]<0)
				{
					//inserting the point in the new points structure
					new_points->InsertNextPoint(Mask->GetPoint(pointId));
					//updating conversion table for next operations
					IdConversionTable[pointId]=addedPoints;
					addedPoints++;
				}
				//insert point in the cell
				new_cells->InsertCellPoint(IdConversionTable[pointId]);
			}
		}
	}

	//deleting current links to rebuild structures
	CurrentSliceMask->DeleteCells();
	CurrentSliceMask->DeleteLinks();

	CurrentSliceMask->SetPolys(new_cells);
	vtkDEL(new_cells);
	CurrentSliceMask->SetPoints(new_points);
	vtkDEL(new_points);
}
