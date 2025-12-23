/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeInterpolatorVTK
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


#include "albaDataPipeInterpolatorVTK.h"

#include "albaVME.h"
#include "albaVMEItemVTK.h"
#include "albaVMEGeneric.h"
#include "vtkALBADataPipe.h"
#include "albaEventBase.h"

#include <assert.h>
#include "vtkDataSet.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaDataPipeInterpolatorVTK)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaDataPipeInterpolatorVTK::albaDataPipeInterpolatorVTK()
//------------------------------------------------------------------------------
{
  vtkNEW(m_VTKDataPipe);
  m_VTKDataPipe->SetDataPipe(this);
}

//------------------------------------------------------------------------------
albaDataPipeInterpolatorVTK::~albaDataPipeInterpolatorVTK()
//------------------------------------------------------------------------------
{
  vtkDEL(m_VTKDataPipe);
} 

//------------------------------------------------------------------------------
bool albaDataPipeInterpolatorVTK::Accept(albaVME *vme)
//------------------------------------------------------------------------------
{
  return Superclass::Accept(vme)&&vme->IsA(albaVMEGeneric::GetStaticTypeId());
}

//------------------------------------------------------------------------------
vtkDataSet *albaDataPipeInterpolatorVTK::GetVTKData()
//------------------------------------------------------------------------------
{
	m_VTKDataPipe->UpdateInformation();
  m_VTKDataPipe->Update();
  return m_VTKDataPipe->GetOutput();
}

//----------------------------------------------------------------------------
void albaDataPipeInterpolatorVTK::Update()
//----------------------------------------------------------------------------
{
  m_VTKDataPipe->UpdateInformation();
  m_VTKDataPipe->Update();
}

//------------------------------------------------------------------------------
void albaDataPipeInterpolatorVTK::PreExecute()
//------------------------------------------------------------------------------
{
  Superclass::PreExecute();

  unsigned long mtime=this->GetMTime();

  // if the current item is changed set the data inside new item as input for the interpolator
  // more specialized interpolators could redefine this to have more inputs (e.g. when 
  // interpolating different items)
  if ( m_CurrentItem && (m_CurrentItem!=m_OldItem || \
    mtime>m_UpdateTime.GetMTime() || \
    mtime>m_VTKDataPipe->GetInformationTime() ||
    !m_CurrentItem->IsDataPresent()))
  {
    vtkDataSet *data = GetCurrentItem()->GetData();
    if (data != NULL)
    {
      m_VTKDataPipe->SetNthInput(0,data);
      m_UpdateTime.Modified();
    }
  } 
}

//------------------------------------------------------------------------------
void albaDataPipeInterpolatorVTK::OnEvent(albaEventBase *e)
//------------------------------------------------------------------------------
{
  assert(e->GetSender()==m_VTKDataPipe); // should not receive events from other sources

  Superclass::OnEvent(e); // this also forwards the event to parent class
}