/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBADataPipe
 Authors: Marco Petrone, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaDecl.h"
#include "vtkALBADataPipe.h"
#include "albaEventBase.h"

#include "albaDataPipeInterpolatorVTK.h"
#include "albaVME.h"

#include "vtkDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkErrorCode.h"
#include "vtkAlgorithm.h"
#include "vtkExecutive.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDemandDrivenPipeline.h"

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkALBADataPipe)

class vtkALBADemandDrivenPipeline : public vtkDemandDrivenPipeline
{
	public:
		vtkMTimeType GetInformationTime(){ return this->InformationTime.GetMTime();};
};

//------------------------------------------------------------------------------
vtkALBADataPipe::vtkALBADataPipe()
{
	SetNumberOfOutputPorts(2);
  m_DataPipe = NULL;
}

//------------------------------------------------------------------------------
vtkALBADataPipe::~vtkALBADataPipe()
{
}

//----------------------------------------------------------------------------
void vtkALBADataPipe::SetDataPipe(albaDataPipe *dpipe)
{
  m_DataPipe=dpipe;
}

//----------------------------------------------------------------------------
void vtkALBADataPipe::SetNumberOfInputs(int n)
{
	this->SetNumberOfInputPorts(n);
	this->SetNumberOfOutputPorts(n);
}

//----------------------------------------------------------------------------
void vtkALBADataPipe::SetNthInput(int num, vtkDataSet *input)
{
	int currentPortNum=this->GetNumberOfInputPorts();

	if (num>currentPortNum-1)
    SetNumberOfInputs(num+1);

  SetInputData(num,input);
}

//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the ALBA data pipe
vtkMTimeType vtkALBADataPipe::GetMTime()
{
	vtkMTimeType mtime = this->Superclass::GetMTime();

  if (m_DataPipe)
  {
		vtkMTimeType dpipeMTime = m_DataPipe->GetMTime();
    if (dpipeMTime > mtime)
    {
      mtime = dpipeMTime;
    }
  }

  return mtime;
}

//------------------------------------------------------------------------------
unsigned long vtkALBADataPipe::GetInformationTime()
{
	vtkDemandDrivenPipeline* ddp = vtkDemandDrivenPipeline::SafeDownCast(this->GetExecutive());
	if (ddp)
  {
		return ((vtkALBADemandDrivenPipeline *)ddp)->GetInformationTime();
  }

}

//------------------------------------------------------------------------------
void vtkALBADataPipe::UpdateInformation()
{
  // forward event to ALBA data pipe
  if (m_DataPipe)
    m_DataPipe->OnEvent(&albaEventBase(this,VME_OUTPUT_DATA_PREUPDATE));

  this->Superclass::UpdateInformation();	
}

//------------------------------------------------------------------------------
int vtkALBADataPipe::RequestData(vtkInformation *request,	vtkInformationVector **inputVector,	vtkInformationVector *outputVector)
{
	if(m_DataPipe && m_DataPipe->IsA("albaDataPipeCustom"))
     m_DataPipe->OnEvent(&albaEventBase(this,VME_OUTPUT_DATA_UPDATE));


	for (int i = 0; i < this->GetNumberOfInputPorts(); ++i)
	{
		vtkDataObject *input = vtkDataObject::GetData(inputVector[i]);
		vtkInformation *out_info = outputVector->GetInformationObject(i);
		vtkDataObject *output = vtkDataObject::GetData(out_info);

		if (!input)
			return 0;

		// se il tipo non corrisponde, rimpiazza l'output
		if (!output || !output->IsA(input->GetClassName()))
		{
			vtkSmartPointer<vtkDataObject> new_output;
			new_output.TakeReference(input->NewInstance());
			out_info->Set(vtkDataObject::DATA_OBJECT(), new_output);
			output = new_output;
		}

		output->ShallowCopy(input);
	}


  // forward event to ALBA data pipe
  if(m_DataPipe && !m_DataPipe->IsA("albaDataPipeCustom"))
    m_DataPipe->OnEvent(&albaEventBase(this,VME_OUTPUT_DATA_UPDATE));

  return 1;
}

