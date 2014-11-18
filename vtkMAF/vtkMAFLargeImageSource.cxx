/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeImageSource.cxx,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:51:01 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "vtkMAFLargeImageSource.h"
#include "vtkMAFLargeImageData.h"
#include "vtkMAFLargeDataProvider.h"

#include "vtkObjectFactory.h"

#include "mafMemDbg.h"
vtkCxxRevisionMacro(vtkMAFLargeImageSource, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(vtkMAFLargeImageSource);

//----------------------------------------------------------------------------
vtkMAFLargeImageSource::vtkMAFLargeImageSource()
{
	this->vtkSource::SetNthOutput(0,vtkMAFLargeImageData::New());
	// Releasing data for pipeline parallism.
	// Filters will know it is empty. 
	this->Outputs[0]->ReleaseData();
	this->Outputs[0]->Delete();
}

//----------------------------------------------------------------------------
// Specify the input data or filter.
void vtkMAFLargeImageSource::SetOutput(vtkMAFLargeImageData *output)
{
	this->vtkSource::SetNthOutput(0, output);
}

//----------------------------------------------------------------------------
// Specify the input data or filter.
vtkMAFLargeImageData *vtkMAFLargeImageSource::GetOutput()
{
	if (this->NumberOfOutputs < 1)
	{
		return NULL;
	}

	return (vtkMAFLargeImageData *)(this->Outputs[0]);
}


//----------------------------------------------------------------------------
// Convert to Imaging API
void vtkMAFLargeImageSource::Execute()
{
	vtkMAFLargeImageData *output = this->GetOutput();

	// If we have multiple Outputs, they need to be allocate
	// in a subclass.  We cannot be sure all outputs are images.
	output->SetExtent(output->GetUpdateExtent());
	output->AllocateScalars();

	this->Execute(this->GetOutput());
}

//----------------------------------------------------------------------------
// This function can be defined in a subclass to generate the data
// for a region.
void vtkMAFLargeImageSource::Execute(vtkMAFLargeImageData *)
{
	vtkErrorMacro(<< "Execute(): Method not defined.");
}


//----------------------------------------------------------------------------
vtkMAFLargeImageData *vtkMAFLargeImageSource::AllocateOutputData(vtkDataObject *out)
{
	vtkMAFLargeImageData *res = vtkMAFLargeImageData::SafeDownCast(out);
	if (!res)
	{
		vtkWarningMacro("Call to AllocateOutputData with non vtkMAFLargeImageData output");
		return NULL;
	}

	// I would like to eliminate this method which requires extra "information"
	// That is not computed in the graphics pipeline.
	// Until I can eliminate the method, I will reexecute the ExecuteInformation
	// before the execute.
	this->ExecuteInformation();

	res->SetExtent(res->GetUpdateExtent());
	res->AllocateScalars();
	return res;
}

//----------------------------------------------------------------------------
vtkMAFLargeImageData *vtkMAFLargeImageSource::GetOutput(int idx)
{
	return (vtkMAFLargeImageData *) this->vtkSource::GetOutput(idx);
}

//----------------------------------------------------------------------------
void vtkMAFLargeImageSource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}
