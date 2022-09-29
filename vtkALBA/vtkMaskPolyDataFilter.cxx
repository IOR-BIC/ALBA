/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMaskPolyDataFilter.cxx
  Language:  C++
  Date:      2001/01/26
  Version:   1.1


Copyright (c) 1993-1998 Ken Martin, Will Schroeder, Bill Lorensen.

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

vtkStandardNewMacro(vtkMaskPolyDataFilter);

// Construct with user-specified polygonal dataset, initial distance of 0.0,
// capping is turned on with CapValue equal to a large positive number and 
// generating mask scalars turned off.
vtkMaskPolyDataFilter::vtkMaskPolyDataFilter(vtkPolyData *mask)
{
	this->CurrentSliceMask=NULL;
  this->SetMask(mask);
//  this->GenerateMaskScalars = 0;
  this->Distance = 0;
  this->InsideOut = this->Binarize = 0;
  this->InsideValue=this->OutsideValue=0.0;

}

vtkMaskPolyDataFilter::~vtkMaskPolyDataFilter()
{
	vtkDEL(this->CurrentSliceMask);
	delete [] IdConversionTable;
  this->SetMask(NULL);  
}



//
// Mask through data generating surface.
//
void vtkMaskPolyDataFilter::Execute()
{
	int i,j,k, abortExecute=0;
	vtkDataSet *output = this->GetOutput();
	vtkDataSet *input = this->GetInput();
	int numPts=input->GetNumberOfPoints();
	vtkPointData *inPointData=input->GetPointData(), *outPointData=output->GetPointData();
	vtkCellData *inCellData=input->GetCellData(), *outCellData=output->GetCellData();
	vtkIdType idx, cellId;
	int subId;
	double  closestPoint[3];
	double *currentPoint,*x1;
	x1 = new double[3];
	double distance; 
	double distance2;
	double dot, n[3];
	vtkCell *cell;
	vtkIdList *cellIds = vtkIdList::New();
	double pcoords[3];
	double bounds[6];
	double currentZ=VTK_DOUBLE_MIN;
	
	//  vtkDebugMacro(<< "Executing MaskPolyDataFilter");

	//
	// Initialize self; create output objects
	//
	if ( !this->GetMask() )
	{
		//	vtkErrorMacro(<<"No mask specified");
		return;
	}

	Mask=this->GetMask();

	Mask->GetBounds(bounds);

	double *weights=new double[4];    
	cellIds->SetNumberOfIds(Mask->GetMaxCellSize());

	if ( numPts < 1 )
	{
		//vtkErrorMacro(<<"No data to Mask");
		return;
	}


	//
	// Create objects to hold output of contour operation
	//
	output->DeepCopy(input);

	outPointData->DeepCopy(inPointData);
	outCellData->DeepCopy(inCellData);

	int numcomp=outPointData->GetScalars()->GetNumberOfComponents();

	float *tuple=new float[numcomp];

	int oldProgress=0;
	int progress=0;

	InitCurrentSliceMask();


	vtkALBASmartPointer<vtkCellLocator> cellLocator;

	for (i = 0; i < numPts && !abortExecute; i++) 
	{
		
		progress=((float)i*100/(float)numPts);

		if (progress!=oldProgress)
		{
			oldProgress=progress;
			this->UpdateProgress((float)progress/100.0);
			abortExecute = this->GetAbortExecute();
		}

		dot=1;
		currentPoint = output->GetPoint(i);



		// use XNOR table to exclude points that not respect the InsideOut and bounding box condition
		// i.e. if InsideOut is 1 points inside the sourface must be sets to the FillValue, so points upside the bounding 
		// box should be skipped
		if ( (currentPoint[0] <= bounds[0]) || (currentPoint[0] >= bounds[1]) || (currentPoint[1] <= bounds[2]) || (currentPoint[1] >= bounds[3]) || (currentPoint[2] <= bounds[4]) || (currentPoint[2] >= bounds[5])) 
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
				for (int n=0;n<numcomp;n++)
				{
					tuple[n]=this->OutsideValue;
				}
				outPointData->GetScalars()->SetTuple(i,tuple);
				// done: go to next point
				continue;
			}
		}
		// at this point the cell is inside the bounds
		
		if(currentZ!=currentPoint[2])
		{
			currentZ=currentPoint[2];
			UpdateCurrentSliceMask(currentZ);
			cellLocator->SetDataSet(CurrentSliceMask);
			cellLocator->BuildLocator();
			cellLocator->Update();
		}
				

		cellLocator->FindClosestPoint(currentPoint, closestPoint, cellId, subId, distance2);


		if (cellId >= 0) 
		{
			
				cell = CurrentSliceMask->GetCell(cellId);
					// Find the normal  
				vtkPolygon::ComputeNormal(cell->Points,n);	  
	
				// Vector from x to x1
				for (k=0; k<3; k++) 
					x1[k] = currentPoint[k] - closestPoint[k];

			// Projection of the distance vector on the normal
			dot = vtkMath::Dot(x1,n);     
		} 

		distance = sqrt(distance2);

		// find if the x point is inside or outside of the polygon: sign of the distance
		if (dot < 0) {
			distance = -distance;
		}

		if ( this->InsideOut || this->Binarize)
		{
			if (distance <= this->Distance) 
			{
				for (int n=0;n<numcomp;n++)
				{
					tuple[n]=this->InsideValue;
				}
				outPointData->GetScalars()->SetTuple(i,tuple);
			}
		}
		
		if(!this->InsideOut || this->Binarize)
		{
			if (distance > this->Distance) 
			{
				for (int n=0;n<numcomp;n++)
				{
					tuple[n]=this->OutsideValue;
				}
				outPointData->GetScalars()->SetTuple(i,tuple);
			}
		}  
	}


	//
	// Loop over all points creating scalar output determined by evaluating 
	// points using mask geometry.
	//

	delete [] x1;
	delete [] weights;
	delete tuple;
	cellIds->Delete();
}



void vtkMaskPolyDataFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataSetToDataSetFilter::PrintSelf(os,indent);

  os << indent << "Mask Function: " << this->GetMask() << "\n";

  os << indent << "Distance: " << this->Distance << "\n";

}

void vtkMaskPolyDataFilter::InitCurrentSliceMask()
{
	//Creating current slice mask
	vtkNEW(CurrentSliceMask);
	//init data by coping mask data
	CurrentSliceMask->DeepCopy(Mask);
	CurrentSliceMask->Update();
	//allocating memory for id conversion table
	IdConversionTable= new vtkIdType[Mask->GetNumberOfPoints()];
}

void vtkMaskPolyDataFilter::UpdateCurrentSliceMask(double z)
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
			if(point[2]<=z)
				pointUnderBound=true;
			if(point[2]>=z)
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

	CurrentSliceMask->Update();
}
