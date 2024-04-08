/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBADataPipe
 Authors: Marco Petrone
 
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
void vtkALBADataPipe::SetNthInput(int num, vtkDataSet *input)
{
	int currentPortNum=this->GetNumberOfInputPorts();
	if (num>currentPortNum-1)
	{
		SetNumberOfInputPorts(num+1);
		SetNumberOfOutputPorts(num+1);
	}
  Superclass::SetInputData(num,input);
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
int vtkALBADataPipe::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  this->SetErrorCode( vtkErrorCode::NoError );
  
	// get the info objects
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkDataObject *output = vtkDataObject::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // check if output array is still empty
  if (output==NULL)
  {
    // create a new object of the same type of those in the array
    if (GetNumberOfInputPorts()>0)
    {
      for (int i=0;i<GetNumberOfInputPorts();i++)
      {
        
				vtkInformation *nthInInfo = inputVector[0]->GetInformationObject(0);
				vtkDataSet  *input = vtkDataSet::SafeDownCast(nthInInfo->Get(vtkDataObject::DATA_OBJECT()));

        if (input)
        {
          UpdateInformation();
          vtkDataSet *new_data=input->NewInstance();
          
					this->GetExecutive()->SetOutputData(i,new_data);
          new_data->Delete();
        }
      }
    }
  } 
  
 return Superclass::RequestInformation(request, inputVector, outputVector);
}

//------------------------------------------------------------------------------
int vtkALBADataPipe::RequestData(vtkInformation *vtkNotUsed(request),	vtkInformationVector **inputVector,	vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkDataObject  *input = vtkDataObject::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));


  if (input)
  {
		if(m_DataPipe && m_DataPipe->IsA("albaDataPipeCustom"))
      m_DataPipe->OnEvent(&albaEventBase(this,VME_OUTPUT_DATA_UPDATE));

    for (int i=0;i<GetNumberOfInputPorts();i++)
    {
      if (GetNumberOfOutputPorts()>i)
      {
				vtkInformation *nthInInfo = inputVector[i]->GetInformationObject(0);
				vtkDataSet  *nthInput = vtkDataSet::SafeDownCast(nthInInfo->Get(vtkDataObject::DATA_OBJECT()));
			
				// get the info objects
				vtkInformation *nthOutInfo = outputVector->GetInformationObject(i);

				// Initialize some frequently used values.
				vtkDataObject *nthOutput = vtkDataObject::SafeDownCast(nthOutInfo->Get(vtkDataObject::DATA_OBJECT()));

				if(nthOutput)
					nthOutput->DeepCopy(nthInput);
      }
      else
      {
        vtkErrorMacro("DEBUG: NULL output pointer!");
      }
    }
    // forward event to ALBA data pipe
    if(m_DataPipe && !m_DataPipe->IsA("albaDataPipeCustom"))
      m_DataPipe->OnEvent(&albaEventBase(this,VME_OUTPUT_DATA_UPDATE));
  }

	return 1;
}
