/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFProjectRG.cxx,v $
  Language:  C++
  Date:      $Date: 2008-07-03 11:27:45 $
  Version:   $Revision: 1.1 $


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
#include "vtkMAFProjectRG.h"
//#include "vtkMath2.h"
#include "vtkObjectFactory.h"
#include "vtkStructuredPoints.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"


#ifndef MIN 
#define MIN( x, y ) ( (x) < (y) ? (x) : (y) )
#endif
#ifndef MAX 
#define MAX( x, y ) ( (x) > (y) ? (x) : (y) )
#endif

vtkCxxRevisionMacro(vtkMAFProjectRG, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMAFProjectRG);



//=========================================================================
vtkMAFProjectRG::vtkMAFProjectRG()
{
  this->ProjectionMode = VTK_PROJECT_FROM_X;
	this->ProjectSubRange = false;
}

//=========================================================================
void vtkMAFProjectRG::ExecuteInformation()
{
  vtkRectilinearGrid *input=this->GetInput();
  vtkRectilinearGrid *output=this->GetOutput();
  int dims[3], outDims[3], wholeExtent[6];
  
  if (this->GetInput() == NULL)
    {
    vtkErrorMacro("Missing input");
    return;
    }
  this->vtkMAFRectilinearGridToRectilinearGridFilter::ExecuteInformation();

  input->GetWholeExtent( wholeExtent );
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
  output->SetUpdateExtent( wholeExtent );

  vtkDebugMacro(<<"Whole Extent is " << wholeExtent[1] << " " << wholeExtent[3] << " " << wholeExtent[5]);
}

//=========================================================================
void vtkMAFProjectRG::Execute()
{
	int x, y, z, dims[3], outDims[3], dim, idx, newIdx, range[2];
	int sliceSize, outSize, jOffset, kOffset;
	float acc, rangeSize;

	vtkRectilinearGrid 	*input = (vtkRectilinearGrid *)this->GetInput();
	vtkRectilinearGrid 	*output = (vtkRectilinearGrid *)this->GetOutput();

	vtkDataArray 			*XCoordinates, *YCoordinates, *ZCoordinates;

	vtkPointData 			*pd = input->GetPointData();
	vtkDataArray 			*sc = pd->GetScalars();

	vtkPointData 			*outPD = output->GetPointData();
	vtkDataArray 			*outSc = pd->GetScalars()->NewInstance();

	input->GetDimensions(dims);
	dim = 2;

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
			break;
	}

	outSize = outDims[0] * outDims[1];
	output->SetDimensions(outDims);

	XCoordinates = input->GetXCoordinates()->NewInstance();
	YCoordinates = input->GetYCoordinates()->NewInstance();
	ZCoordinates = input->GetZCoordinates()->NewInstance();

	XCoordinates->SetNumberOfTuples(outDims[0]);
	YCoordinates->SetNumberOfTuples(outDims[1]);
	ZCoordinates->SetNumberOfTuples(outDims[2]);

	ZCoordinates->InsertComponent(0, 0, 0);

	switch (this->ProjectionMode) {
		case VTK_PROJECT_FROM_X:
			XCoordinates->DeepCopy(input->GetYCoordinates());
			YCoordinates->DeepCopy(input->GetZCoordinates());
			break;
		case VTK_PROJECT_FROM_Y:
			XCoordinates->DeepCopy(input->GetXCoordinates());
			YCoordinates->DeepCopy(input->GetZCoordinates());
			break;
		case VTK_PROJECT_FROM_Z:
			XCoordinates->DeepCopy(input->GetXCoordinates());
			YCoordinates->DeepCopy(input->GetYCoordinates());
			break;
	}

	//outSc->SetDataType(sc->GetDataType());
	outSc->SetNumberOfTuples(outSize);

	sliceSize = dims[0] * dims[1];

	//
	// Traverse input data and project points to output
	//
	newIdx = 0;

	//Set Projection Range
	if (ProjectSubRange)
	{
		range[0] = MAX(ProjectionRange[ProjectionMode - 1], 0);
		range[1] = MIN(ProjectionRange[ProjectionMode], dims[0]);
		rangeSize = range[1] - range[0];
	}
	else
	{
		range[0] = 0;
		range[1] = dims[ProjectionMode - 1];
		rangeSize = dims[ProjectionMode - 1];
	}

	switch (this->ProjectionMode)
	{
		case VTK_PROJECT_FROM_X:
			for (z = 0; z < dims[2]; z++)
			{
				kOffset = z * sliceSize;
				for (y = 0; y < dims[1]; y++)
				{
					jOffset = y * dims[0];
					acc = 0;
					for (x = range[0]; x < range[1]; x++)
					{
						idx = x + jOffset + kOffset;
						acc += sc->GetTuple1(idx);
					}
					outSc->InsertTuple1(newIdx++, acc / rangeSize);
				}
			}
			break;
		case VTK_PROJECT_FROM_Y:
			for (z = 0; z < dims[2]; z++)
			{
				kOffset = z * sliceSize;
				for (x = 0; x < dims[0]; x++)
				{
					acc = 0;
					for (y = range[0]; y < range[1]; y++)
					{
						jOffset = y * dims[0];
						idx = x + jOffset + kOffset;
						acc += sc->GetTuple1(idx);
					}
					outSc->InsertTuple1(newIdx++, acc / rangeSize);
				}
			}
			break;
		case VTK_PROJECT_FROM_Z:
			for (y = 0; y < dims[1]; y++)
			{
				jOffset = y * dims[0];
				for (x = 0; x < dims[0]; x++)
				{
					acc = 0;
					for (z = range[0]; z < range[1]; z++)
					{
						kOffset = z * sliceSize;
						idx = x + jOffset + kOffset;
						acc += sc->GetTuple1(idx);
					}
					outSc->InsertTuple1(newIdx++, acc / rangeSize);
				}
			}
			break;
	}

	output->SetXCoordinates(XCoordinates);
	output->SetYCoordinates(YCoordinates);
	output->SetZCoordinates(ZCoordinates);
	XCoordinates->Delete();
	YCoordinates->Delete();
	ZCoordinates->Delete();

	outPD->SetScalars(outSc);
	outSc->Delete();
}


//=========================================================================
void vtkMAFProjectRG::PrintSelf(ostream& os, vtkIndent indent)
//=========================================================================
{
  vtkMAFRectilinearGridToRectilinearGridFilter::PrintSelf(os,indent);

  os << indent << "ProjectionMode: " <<GetProjectionModeAsString() << "\n";
  
}




