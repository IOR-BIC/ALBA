/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFRGSliceAccumulate.cxx,v $
  Language:  C++
  Date:      $Date: 2009-09-02 12:33:20 $
  Version:   $Revision: 1.1.2.4 $


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
#include "vtkMAFRGSliceAccumulate.h"
#include "vtkDoubleArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkCharArray.h"
#include "vtkIntArray.h"
#include "vtkShortArray.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

vtkCxxRevisionMacro(vtkMAFRGSliceAccumulate, "$Revision: 1.1.2.4 $");
vtkStandardNewMacro(vtkMAFRGSliceAccumulate);

//--------------------------------------------------------------------------------------
vtkMAFRGSliceAccumulate::vtkMAFRGSliceAccumulate()
//--------------------------------------------------------------------------------------
{
  this->Slices = NULL;
  this->NumberOfSlices = 0;
  this->Allocated = 0;
  SetDimensions(1,1,1);
  SetSpacing(1,1,1);
  SetDataType(VTK_UNSIGNED_CHAR);
  SetOrigin(0,0,0);
  Slices=vtkRectilinearGrid::New();
}
//--------------------------------------------------------------------------------------
vtkMAFRGSliceAccumulate::~vtkMAFRGSliceAccumulate()
//--------------------------------------------------------------------------------------
{
  SetSlices(NULL);
}
void vtkMAFRGSliceAccumulate::SetSlice(int slice_num,vtkImageData * slice,double *unRotatedOrigin)
//--------------------------------------------------------------------------------------
{
	int dimensions[3];
	int scalar_type = slice->GetPointData()->GetScalars()->GetDataType();

	slice->GetDimensions(dimensions);

	
	if (slice_num >=0 && slice_num < this->NumberOfSlices)
	{
		if (!this->Allocated)
		{
			int extent[6];
			double spacing[3];
			slice->GetSpacing(spacing);
			slice->GetExtent(extent);
			this->SetInputExtent(extent);
			this->SetSpacing(spacing[0], spacing[1], 1); 
			this->SetDimensions(dimensions);
			this->SetOrigin(unRotatedOrigin);
			this->SetDataType(scalar_type);
			this->Allocate();
		}
		if (dimensions[0] != Dimensions[0] || dimensions[1] != Dimensions[1])
		{
			vtkErrorMacro("Bad dimensions of input slice");
			return;
		}

		((vtkDoubleArray *)this->Slices->GetZCoordinates())->SetValue(slice_num, unRotatedOrigin[2]);
					
		int start;
		void *out_dataPointer		= this->Slices->GetPointData()->GetScalars()->GetVoidPointer(0);
		void *input_dataPointer = slice->GetPointData()->GetScalars()->GetVoidPointer(0);
		int numscalars = slice->GetPointData()->GetScalars()->GetNumberOfTuples();
		
    start = numscalars * slice_num;
		
		switch (scalar_type) 
		{
			case VTK_CHAR:
			case VTK_UNSIGNED_CHAR:
				out_dataPointer = ((char *)out_dataPointer) + start;
				memmove(out_dataPointer,input_dataPointer, sizeof(char) * numscalars);
			break;
			case VTK_SHORT:
			case VTK_UNSIGNED_SHORT:
				out_dataPointer = ((short *)out_dataPointer) + start;
				memmove((short *)out_dataPointer,(short *)input_dataPointer, sizeof(short) * numscalars);
			break;
			case VTK_INT:
			case VTK_UNSIGNED_INT:
				out_dataPointer = ((int *)out_dataPointer) + start;
				memmove((int *)out_dataPointer, (int *)input_dataPointer, sizeof(int) * numscalars);
			break;	
			case VTK_FLOAT:
				out_dataPointer = ((float *)out_dataPointer) + start;
				memmove((float *)out_dataPointer,(float *)input_dataPointer, sizeof(float) * numscalars);
			break;
			case VTK_DOUBLE:
				out_dataPointer = ((double *)out_dataPointer) + start;
				memmove((double *)out_dataPointer, (double *)input_dataPointer, sizeof(double) * numscalars);
		  break;

			default:
				vtkErrorMacro(<< "Only 8/16/32 bit integers and 32 bit floats are supported.");
		}
	} 
	else 
	{
		vtkErrorMacro("Slice Number out of range. Allocated Slices = " << this->NumberOfSlices);
	}

	this->Modified();
}
//--------------------------------------------------------------------------------------
void vtkMAFRGSliceAccumulate::Allocate()
//--------------------------------------------------------------------------------------
{
	if (this->NumberOfSlices == 0)
		return;
	
	this->Allocated = 1;

	vtkDoubleArray *vx = vtkDoubleArray::New();
	vtkDoubleArray *vy = vtkDoubleArray::New();
	vtkDoubleArray *vz = vtkDoubleArray::New();

	Dimensions[2]=this->GetNumberOfSlices();

	vx->SetNumberOfValues(Dimensions[0]);
	vy->SetNumberOfValues(Dimensions[1]);
	vz->SetNumberOfValues(Dimensions[2]);

	int xStart = InputExtent[0];
	int yStart = InputExtent[2];

	for (int ix = 0; ix < Dimensions[0]; ix++) {
		vx->SetValue(ix, Origin[0] + ((double)(ix + xStart))*Spacing[0]);
	}
	int iy = 0;
	for (; iy < Dimensions[1]; iy++) {
		vy->SetValue(iy, Origin[1] + ((double)(iy + yStart))*Spacing[1]);
	}
	for (int iz = 0; iz < Dimensions[2]; iz++) {
		vz->SetValue(iz, 0.);
	}


	vtkDataArray *data = 0;
	int scalar_type = this->GetDataType();

	// data array should consistent with scalar type:
	switch (scalar_type) 
		{
			case VTK_CHAR:
				data = vtkCharArray::New();
			break;
			case VTK_UNSIGNED_CHAR:
				data = vtkUnsignedCharArray::New();
			break;
			case VTK_SHORT:
				data = vtkShortArray::New();
			break;
			case VTK_INT:
				data = vtkIntArray::New();
			break;
			case VTK_UNSIGNED_INT:
				data = vtkUnsignedIntArray::New();
			break;
			case VTK_UNSIGNED_SHORT:
				data = vtkUnsignedShortArray::New();
			break;
			case VTK_FLOAT:
				data = vtkFloatArray::New();
			break;
			case VTK_DOUBLE:
				data = vtkDoubleArray::New();
			break;
			default:
				vtkErrorMacro(<< "Only 8/16 bit integers and 32 bit floats are supported.");
		}
	
	data->SetNumberOfTuples(Dimensions[0] * Dimensions[1] * Dimensions[2]);
	this->Slices->SetDimensions(Dimensions);
	this->Slices->SetXCoordinates(vx);
	this->Slices->SetYCoordinates(vy);
	this->Slices->SetZCoordinates(vz);
	this->Slices->GetPointData()->SetScalars(data);

  vx->Delete();
  vy->Delete();
  vz->Delete();
  data->Delete();
}
//--------------------------------------------------------------------------------------
static int recursing = 0;
void vtkMAFRGSliceAccumulate::PrintSelf(ostream& os, vtkIndent indent)
//--------------------------------------------------------------------------------------
{
  // the reader ivar's source will be this Reader. 
  // we must do this to prevent infinite printing
  if (!recursing)
    { 
    vtkMAFRGSliceAccumulate::PrintSelf(os,indent);
    recursing = 1;
    os << indent << "Reader:\n";
    this->Slices->PrintSelf(os,indent.GetNextIndent());
    }
  recursing = 0;
}
