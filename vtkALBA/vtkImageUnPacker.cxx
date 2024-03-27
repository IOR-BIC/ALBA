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
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkStreamingDemandDrivenPipeline.h"

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
	SetNumberOfInputPorts(0);
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
  vtkImageAlgorithm::PrintSelf(os,indent);

  os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "(none)") << "\n";
}

//----------------------------------------------------------------------------
// This method returns the largest data that can be generated.
int vtkImageUnPacker::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//----------------------------------------------------------------------------
{
	 vtkInformation* outInfo = outputVector->GetInformationObject(0);
	 vtkImageData *output = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
	
	 // Here information on the image are read and set in the Output cache.
	
	if (ReadImageInformation(this->GetInput()))
	{
		vtkGenericWarningMacro("Problems extracting the image information. ");
	}

	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), GetDataExtent(), 6);
	outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), GetDataExtent(), 6);
	
	output->SetExtent(GetDataExtent());
	output->AllocateScalars(GetDataScalarType(),GetNumberOfScalarComponents());

	if (VtkImageUnPackerUpdate(this->Input,output))
	{
		vtkErrorMacro("Cannot Unpack Image!");
	}

	return 1;
}
