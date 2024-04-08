/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeCustom
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


#include "albaDataPipeCustom.h"

#include "albaVME.h"
#include "vtkALBADataPipe.h"
#include "albaEventBase.h"
#include "vtkDataSet.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaDataPipeCustom)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaDataPipeCustom::albaDataPipeCustom()
//------------------------------------------------------------------------------
{
  vtkNEW(m_VTKDataPipe);
  m_VTKDataPipe->SetDataPipe(this);
}

//------------------------------------------------------------------------------
albaDataPipeCustom::~albaDataPipeCustom()
//------------------------------------------------------------------------------
{
  vtkDEL(m_VTKDataPipe);
} 

//------------------------------------------------------------------------------
vtkDataSet *albaDataPipeCustom::GetVTKData()
//------------------------------------------------------------------------------
{
  m_VTKDataPipe->UpdateInformation();
  m_VTKDataPipe->Update();
  return m_VTKDataPipe->GetOutput();
}

//----------------------------------------------------------------------------
vtkALBADataPipe *albaDataPipeCustom::GetVTKDataPipe()
//----------------------------------------------------------------------------
{
  return m_VTKDataPipe;
}

//----------------------------------------------------------------------------
void albaDataPipeCustom::Update()
//----------------------------------------------------------------------------
{
	m_VTKDataPipe->UpdateInformation();
  m_VTKDataPipe->Update();
}

//------------------------------------------------------------------------------
void albaDataPipeCustom::UpdateBounds()
//------------------------------------------------------------------------------
{
  if (m_VTKDataPipe->GetOutput())
  {
		m_VTKDataPipe->UpdateInformation();
		m_VTKDataPipe->Update();
	  m_VTKDataPipe->GetOutput()->ComputeBounds();
	  m_Bounds.DeepCopy(m_VTKDataPipe->GetOutput()->GetBounds());
  }
}
//------------------------------------------------------------------------------
void albaDataPipeCustom::OnEvent(albaEventBase *e)
//------------------------------------------------------------------------------
{
  assert(e->GetSender()==m_VTKDataPipe); // should not receive events from other sources

/*  if (e->GetSender()==m_VTKDataPipe)
  {
    switch (e->GetId())
    {
    case VME_OUTPUT_DATA_PREUPDATE:
      PreExecute();
    break;
    case VME_OUTPUT_DATA_UPDATE:
      Execute(); // superclass execute...
    };
  }
*/
  Superclass::OnEvent(e); // this also forwards the event to parent class
}

//------------------------------------------------------------------------------
void albaDataPipeCustom::SetInput(vtkDataSet *input_dataset)
//------------------------------------------------------------------------------
{
  SetNthInput(0,input_dataset);
}

//------------------------------------------------------------------------------
void albaDataPipeCustom::SetNthInput(int n, vtkDataSet *input_dataset)
//------------------------------------------------------------------------------
{
	vtkALBADataPipe * vtkDataPipe = GetVTKDataPipe();
  vtkDataPipe->SetNthInput(n,input_dataset);
}
