/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFRGSliceAccumulate.cxx,v $
  Language:  C++
  Date:      $Date: 2009-02-09 11:31:43 $
  Version:   $Revision: 1.1.2.1 $


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
#include "vtkCharArray.h"
#include "vtkShortArray.h"
#include "vtkFloatArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

vtkCxxRevisionMacro(vtkMAFRGSliceAccumulate, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMAFRGSliceAccumulate);

//--------------------------------------------------------------------------------------
vtkMAFRGSliceAccumulate::vtkMAFRGSliceAccumulate()
//--------------------------------------------------------------------------------------
{
  this->Slices = NULL;
  this->NumberOfSlices = 0;
  this->allocated = 0;
	this->BuildingAxes = 2;
  SetDimensions(1,1,1);
  SetSpacing(1,1,1);
  SetDataType(VTK_UNSIGNED_CHAR);
  SetOrigin(0,0,0);
  Slices=vtkRectilinearGrid::New();
  //SetSlices(vtkRectilinearGrid::New());
}
//--------------------------------------------------------------------------------------
vtkMAFRGSliceAccumulate::~vtkMAFRGSliceAccumulate()
//--------------------------------------------------------------------------------------
{
	//SetSlices(NULL);
}
//--------------------------------------------------------------------------------------
void vtkMAFRGSliceAccumulate::AddSlice(vtkImageData * slice)
//--------------------------------------------------------------------------------------
{
	double origin[3];
	double spacing[3];
	int dimensions[3];
	int scalar_type = slice->GetPointData()->GetScalars()->GetDataType();

	vtkDoubleArray *vz;
	
	slice->GetOrigin(origin);
	slice->GetDimensions(dimensions);
	slice->GetSpacing(spacing);

	if (!this->allocated)
	{
		this->SetSpacing(spacing[0], spacing[1], 1);
		this->SetDimensions(dimensions);
		this->SetOrigin(origin);
		this->SetDataType(slice->GetPointData()->GetScalars()->GetDataType());

		this->NumberOfSlices++;
		this->Allocate();

		this->Slices->GetPointData()->SetScalars(slice->GetPointData()->GetScalars());
		//((vtkFloatArray *)this->Slices->GetZCoordinates())->SetValue(0, origin[2]);
		((vtkDoubleArray *)this->Slices->GetZCoordinates())->SetValue(0, origin[this->BuildingAxes]);
	} else {
		vz=(vtkDoubleArray *)this->Slices->GetZCoordinates();
		
		int start;
		void *out_dataPointer		= (vtkDoubleArray *)this->Slices->GetPointData()->GetScalars()->GetVoidPointer(0);
		void *input_dataPointer = (vtkDoubleArray *)slice->GetPointData()->GetScalars()->GetVoidPointer(0);
		int numscalars = slice->GetPointData()->GetScalars()->GetNumberOfTuples();
		start = numscalars * this->NumberOfSlices;

		switch (scalar_type) 
		{
			case VTK_CHAR:
				out_dataPointer = ((char *)out_dataPointer) + start;
				memmove((char *)out_dataPointer,(char *)input_dataPointer, sizeof(char) * numscalars);
			break;
			case VTK_UNSIGNED_CHAR:
				out_dataPointer = ((unsigned char *)out_dataPointer) + start;
				memmove((unsigned char *)out_dataPointer,(unsigned char *)input_dataPointer, sizeof(unsigned char) * numscalars);
			break;
			case VTK_SHORT:
				out_dataPointer = ((short *)out_dataPointer) + start;
				memmove((short *)out_dataPointer,(short *)input_dataPointer, sizeof(short) * numscalars);
			break;
			case VTK_UNSIGNED_SHORT:
				out_dataPointer = ((unsigned short *)out_dataPointer) + start;
				memmove((unsigned short *)out_dataPointer,(unsigned short *)input_dataPointer, sizeof(unsigned short) * numscalars);
			break;
			case VTK_FLOAT:
				out_dataPointer = ((float *)out_dataPointer) + start;
				memmove((float *)out_dataPointer,(float *)input_dataPointer, sizeof(float) * numscalars);
			break;
			default:
				vtkErrorMacro(<< "Only 8/16 bit integers and 32 bit floats are supported.");
		}
		
		this->Slices->SetDimensions(dimensions[0], dimensions[1], ++this->NumberOfSlices);
		//vz->InsertValue(this->NumberOfSlices - 1, origin[2]);
		vz->InsertValue(this->NumberOfSlices - 1, origin[this->BuildingAxes]);
	}

	this->Modified();
}
//--------------------------------------------------------------------------------------
void vtkMAFRGSliceAccumulate::SetSlice(int slice_num,vtkImageData * slice)
//--------------------------------------------------------------------------------------
{
	int dimensions[3];
	double origin[3];
	double spacing[3];
	int scalar_type = slice->GetPointData()->GetScalars()->GetDataType();
	
	slice->GetDimensions(dimensions);
	slice->GetOrigin(origin);
	slice->GetSpacing(spacing);
	
	if (slice_num >=0 && slice_num < this->NumberOfSlices)
	{
		if (!this->allocated)
		{
			this->SetSpacing(spacing[0], spacing[1], 1);
			this->SetDimensions(dimensions);
			
			this->SetOrigin(origin);
			
			this->SetDataType(scalar_type);
			
			this->Allocate();
		}
		if (dimensions[0] != Dimensions[0] || dimensions[1] != Dimensions[1])
		{
			vtkErrorMacro("Bad dimensions of input slice");
			return;
		}
		//((vtkFloatArray *)this->Slices->GetZCoordinates())->SetValue(slice_num, origin[2]);
		((vtkDoubleArray *)this->Slices->GetZCoordinates())->SetValue(slice_num, origin[this->BuildingAxes]);
		
		int start;
		void *out_dataPointer		= (vtkDoubleArray *)this->Slices->GetPointData()->GetScalars()->GetVoidPointer(0);
		void *input_dataPointer = (vtkDoubleArray *)slice->GetPointData()->GetScalars()->GetVoidPointer(0);
		int numscalars = slice->GetPointData()->GetScalars()->GetNumberOfTuples();
		start = numscalars * slice_num;

		switch (scalar_type) 
		{
			case VTK_CHAR:
				out_dataPointer = ((char *)out_dataPointer) + start;
				memmove((char *)out_dataPointer,(char *)input_dataPointer, sizeof(char) * numscalars);
			break;
			case VTK_UNSIGNED_CHAR:
				out_dataPointer = ((unsigned char *)out_dataPointer) + start;
				memmove((unsigned char *)out_dataPointer,(unsigned char *)input_dataPointer, sizeof(unsigned char) * numscalars);
			break;
			case VTK_SHORT:
				out_dataPointer = ((short *)out_dataPointer) + start;
				memmove((short *)out_dataPointer,(short *)input_dataPointer, sizeof(short) * numscalars);
			break;
			case VTK_UNSIGNED_SHORT:
				out_dataPointer = ((unsigned short *)out_dataPointer) + start;
				memmove((unsigned short *)out_dataPointer,(unsigned short *)input_dataPointer, sizeof(unsigned short) * numscalars);
			break;
			case VTK_FLOAT:
				out_dataPointer = ((float *)out_dataPointer) + start;
				memmove((float *)out_dataPointer,(float *)input_dataPointer, sizeof(float) * numscalars);
			break;
			default:
				vtkErrorMacro(<< "Only 8/16 bit integers and 32 bit floats are supported.");
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
	
	this->allocated = 1;

	vtkDoubleArray *vx = vtkDoubleArray::New();
	vtkDoubleArray *vy = vtkDoubleArray::New();
	vtkDoubleArray *vz = vtkDoubleArray::New();

	Dimensions[2]=this->GetNumberOfSlices();

	vx->SetNumberOfValues(Dimensions[0]);
	vy->SetNumberOfValues(Dimensions[1]);
	vz->SetNumberOfValues(Dimensions[2]);
		
	for (int ix = 0; ix < Dimensions[0]; ix++)
		vx->SetValue(ix, Origin[0]+((double)ix)*Spacing[0]);
	for (int iy = 0; iy < Dimensions[1]; iy++)
		vy->SetValue(iy, Origin[1]+((double)iy)*Spacing[1]);
	for (int iz = 0; iz < Dimensions[2]; iz++)
		vz->SetValue(iz, 0);

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
			case VTK_UNSIGNED_SHORT:
				data = vtkUnsignedShortArray::New();
			break;
			case VTK_FLOAT:
				data = vtkFloatArray::New();
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
