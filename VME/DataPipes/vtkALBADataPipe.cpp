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
//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkALBADataPipe)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkALBADataPipe::vtkALBADataPipe()
//------------------------------------------------------------------------------
{
  m_DataPipe = NULL;
}

//------------------------------------------------------------------------------
vtkALBADataPipe::~vtkALBADataPipe()
//------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void vtkALBADataPipe::SetDataPipe(albaDataPipe *dpipe)
//----------------------------------------------------------------------------
{
  m_DataPipe=dpipe;
}

//----------------------------------------------------------------------------
void vtkALBADataPipe::SetNthInput(int num, vtkDataSet *input)
//----------------------------------------------------------------------------
{
  Superclass::SetNthInput(num,input);
}

//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the ALBA data pipe
unsigned long vtkALBADataPipe::GetMTime()
//------------------------------------------------------------------------------
{
  unsigned long mtime = this->Superclass::GetMTime();

  if (m_DataPipe)
  {
    unsigned long dpipeMTime = m_DataPipe->GetMTime();
    if (dpipeMTime > mtime)
    {
      mtime = dpipeMTime;
    }
  }

  return mtime;
}

//------------------------------------------------------------------------------
unsigned long vtkALBADataPipe::GetInformationTime()
//------------------------------------------------------------------------------
{
  return InformationTime.GetMTime();
}

//------------------------------------------------------------------------------
vtkDataSet *vtkALBADataPipe::GetOutput(int idx)
//------------------------------------------------------------------------------
{
  if (this->NumberOfOutputs < idx+1)
  {
    UpdateInformation(); // force creating the outputs
  }
  return Superclass::GetOutput(idx);
}

//------------------------------------------------------------------------------
vtkDataSet *vtkALBADataPipe::GetOutput()
//------------------------------------------------------------------------------
{
  if (this->NumberOfOutputs == 0)
  {
    UpdateInformation(); // force creating the outputs
  }
  return Superclass::GetOutput();
}

//------------------------------------------------------------------------------
void vtkALBADataPipe::UpdateInformation()
//------------------------------------------------------------------------------
{
  // forward event to ALBA data pipe
  if (m_DataPipe)
    m_DataPipe->OnEvent(&albaEventBase(this,VME_OUTPUT_DATA_PREUPDATE));

  this->Superclass::UpdateInformation();
}

//------------------------------------------------------------------------------
void vtkALBADataPipe::ExecuteInformation()
//------------------------------------------------------------------------------
{
  this->SetErrorCode( vtkErrorCode::NoError );
  
  // check if output array is still empty
  if (this->Outputs==NULL||this->Outputs[0]==NULL)
  {
    // create a new object of the same type of those in the array
    if (GetNumberOfInputs()>0)
    {
      for (int i=0;i<GetNumberOfInputs();i++)
      {
        
        vtkDataSet *data=(vtkDataSet *)GetInputs()[i];
        if (data)
        {
          data->UpdateInformation();
          vtkDataSet *new_data=data->NewInstance();
          new_data->CopyInformation(data);
          this->SetNthOutput(i,new_data);
          new_data->Delete();
        }
      }
    }
  } 
  
  if (GetNumberOfInputs()>0&&GetInput()) // work around to skip vtkDataSet bug with zero inputs
    Superclass::ExecuteInformation(); 
}

//------------------------------------------------------------------------------
void vtkALBADataPipe::Execute()
//------------------------------------------------------------------------------
{
  if (GetInput())
  {
    if(m_DataPipe->IsA("albaDataPipeCustom"))
      m_DataPipe->OnEvent(&albaEventBase(this,VME_OUTPUT_DATA_UPDATE));
    for (int i=0;i<GetNumberOfInputs();i++)
    {
      if (GetNumberOfOutputs()>i)
      {
        vtkDataSet *input=(vtkDataSet *)GetInputs()[i];
        input->Update();
        this->Outputs[i]->ShallowCopy(input);
      }
      else
      {
        vtkErrorMacro("DEBUG: NULL output pointer!");
      }
    }
    // forward event to ALBA data pipe
    if(!m_DataPipe->IsA("albaDataPipeCustom"))
      m_DataPipe->OnEvent(&albaEventBase(this,VME_OUTPUT_DATA_UPDATE));
  }
}
