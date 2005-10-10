/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImageUnPacker.cxx
  Language:  C++
  Date:      03/1999
  Version:   
  Credits:   This class has been developed by Marco Petrone

=========================================================================*/

#include <stdio.h>
#include <ctype.h>
#include "vtkObjectFactory.h"
#include "vtkImageUnPacker.h"
#include "vtkImageData.h"

vtkCxxRevisionMacro(vtkImageUnPacker, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkImageUnPacker);

//----------------------------------------------------------------------------
vtkImageUnPacker::vtkImageUnPacker()
//----------------------------------------------------------------------------
{
	this->Input=NULL;
	this->FileName=NULL;
  this->DataExtent[0]=0; this->DataExtent[1]=0; this->DataExtent[2]=0;
  this->DataExtent[3]=0; this->DataExtent[4]=0; this->DataExtent[5]=0;
	SetDataScalarType(VTK_UNSIGNED_CHAR);
	SetNumberOfScalarComponents(1);
	UnPackFromFileOff();
}

//----------------------------------------------------------------------------
vtkImageUnPacker::~vtkImageUnPacker()
//----------------------------------------------------------------------------
{
	SetInput(NULL);
	SetFileName(NULL);
}

//----------------------------------------------------------------------------
void vtkImageUnPacker::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  vtkImageSource::PrintSelf(os,indent);

  os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "(none)") << "\n";
}

//----------------------------------------------------------------------------
// This method returns the largest data that can be generated.
void vtkImageUnPacker::ExecuteInformation()
//----------------------------------------------------------------------------
{
	// Here information on the image are read and set in the Output cache.
	
	if (ReadImageInformation(this->GetInput()))
	{
		vtkGenericWarningMacro("Problems extracting the image information. ");
	}

	GetOutput()->SetWholeExtent(GetDataExtent());
	GetOutput()->SetUpdateExtent(GetDataExtent());
	GetOutput()->SetScalarType(GetDataScalarType());
	GetOutput()->SetNumberOfScalarComponents(GetNumberOfScalarComponents());
}

//----------------------------------------------------------------------------
// This function reads an image from a stream.
void vtkImageUnPacker::Execute(vtkImageData *data)
//----------------------------------------------------------------------------
{
	if (vtkImageUnPackerUpdate(this->Input,data))
	{
		vtkErrorMacro("Cannot Unpack Image!");
	}
}
