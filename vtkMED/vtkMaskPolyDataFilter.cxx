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
#include "vtkMaskPolyDataFilter.h"
#include "vtkPointData.h"
#include "vtkCellData.h"


// Construct with user-specified polygonal dataset, initial distance of 0.0,
// capping is turned on with CapValue equal to a large positive number and 
// generating mask scalars turned off.
vtkMaskPolyDataFilter::vtkMaskPolyDataFilter(vtkPolyData *mask)
{
  this->SetMask(mask);
//  this->GenerateMaskScalars = 0;
  this->Distance = 0;
  this->InsideOut = 0;
  this->MaximumDistance = sqrt(1.0e29) / 3.0;
  this->FillValue=0.0;
}

vtkMaskPolyDataFilter::~vtkMaskPolyDataFilter()
{
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
	vtkPointData *inPD=input->GetPointData(), *outPD=output->GetPointData();
	vtkCellData *inCD=input->GetCellData(), *outCD=output->GetCellData();
	int idx, cellId, subId;
	double  closestPoint[3];
	double *x,*x1;
	x1 = new double[3];
	double maxDistance2, distance; 
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
	if ( !this->GetMask() )
	{
		//	vtkErrorMacro(<<"No mask specified");
		return;
	}

	vtkPolyData *mask=this->GetMask();

	mask->GetBounds(bounds);

	double *weights=new double[3];    
	cellIds->SetNumberOfIds(mask->GetMaxCellSize());

	if ( numPts < 1 )
	{
		//vtkErrorMacro(<<"No data to Mask");
		return;
	}


	//
	// Create objects to hold output of contour operation
	//
	output->DeepCopy(input);
	//output->DeepCopy(input);	

	outPD->DeepCopy(inPD);
	outCD->DeepCopy(inCD);

	//outSc = outPD->GetScalars();

	int numcomp=outPD->GetScalars()->GetNumberOfComponents();

	float *tuple=new float[numcomp];

	int progress=0;

	for (i = 0; i < numPts && !abortExecute; i++) 
	{
		progress=((float)i*100/(float)numPts);

		if ((progress%2)==0)
		{
			this->UpdateProgress((float)progress/100.0);
			abortExecute = this->GetAbortExecute();
		}

		maxDistance2 = (this->MaximumDistance) * (this->MaximumDistance);
		dot=1;
		x = output->GetPoint(i);

		// use XNOR table to exclude points that not respect the InsideOut and bounding box condition
		// i.e. if InsideOut is 1 points inside the sourface must be sets to the FillValue, so points upside the bounding 
		// box should be skipped
		if ( (x[0] <= bounds[0]) || (x[0] >= bounds[1]) || (x[1] <= bounds[2]) || (x[1] >= bounds[3]) || (x[2] <= bounds[4]) || (x[2] >= bounds[5])) 
		{
			//the point is outside the mask's bounds
			if (this->InsideOut)
			{
				//if InsideOut==1 the point outside should be skipped
				continue;
			}
			else
			{
				// if insideOut==0 the point outside should be filled with the FillValue
				for (int n=0;n<numcomp;n++)
				{
					tuple[n]=this->FillValue;
				}
				outPD->GetScalars()->SetTuple(i,tuple);
				// done: go to next point
				continue;
			}
		}
		// at this point the cell is inside the bounds

		// Find the closest point in the PolyData
		idx = mask->FindPoint(x);

		if (idx >= 0) 
		{
			// find the cells containing the found point
			mask->GetPointCells(idx, cellIds);

			// examine the each cell to find the closest point to the x point
			for (j=0; j < cellIds->GetNumberOfIds(); j++) 
			{
				// extract the cell
				cellId = cellIds->GetId(j);
				cell = mask->GetCell(cellId);
				// find the distance of the cell from the x point
				cell->EvaluatePosition(x, x1, subId, pcoords, distance2, weights);

				// Find the point at minimum distance
				if (distance2 < maxDistance2) {
					maxDistance2 = distance2;
					// Find the normal  
					vtkPolygon::ComputeNormal(cell->Points,n);	  
					for (k=0; k<3; k++) {	    	   
						closestPoint[k]=x1[k];
					}	  
				}
			}

			// Vector from x to x1
			for (k=0; k<3; k++) 
				x1[k] = x[k] - closestPoint[k];

			// Projection of the distance vector on the normal
			dot = vtkMath::Dot(x1,n);     
		} 

		distance = sqrt(maxDistance2);

		// find if the x point is inside or outside of the polygon: sign of the distance
		if (dot < 0) {
			distance = -distance;
		}

		if ( this->InsideOut )
		{
			if (distance <= this->Distance) 
			{
				for (int n=0;n<numcomp;n++)
				{
					tuple[n]=this->FillValue;
				}
				outPD->GetScalars()->SetTuple(i,tuple);
			}
		}
		else
		{
			if (distance > this->Distance) 
			{
				for (int n=0;n<numcomp;n++)
				{
					tuple[n]=this->FillValue;
				}
				outPD->GetScalars()->SetTuple(i,tuple);
			}
		}  
	}


	//
	// Loop over all points creating scalar output determined by evaluating 
	// points using mask geometry.
	//

	delete [] weights;
	delete tuple;
	cellIds->Delete();
}



void vtkMaskPolyDataFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataSetToDataSetFilter::PrintSelf(os,indent);

  os << indent << "Mask Function: " << this->GetMask() << "\n";

  os << indent << "Distance: " << this->Distance << "\n";

//  os << indent << "InsideOut: " << (this->GenerateMaskScalars ? "On\n" : "Off\n");

//  os << indent << "Generate Mask Scalars: " << (this->GenerateMaskScalars ? "On\n" : "Off\n");

  os << indent << "Maximum Distance: " << this->MaximumDistance << "\n";

}
