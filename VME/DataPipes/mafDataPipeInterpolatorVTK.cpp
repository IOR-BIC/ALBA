/*=========================================================================

 Program: MAF2
 Module: mafDataPipeInterpolatorVTK
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafDataPipeInterpolatorVTK.h"

#include "mafVME.h"
#include "mafVMEItemVTK.h"
#include "mafVMEGeneric.h"
#include "vtkMAFDataPipe.h"
#include "mafEventBase.h"

#include <assert.h>
#include "vtkDataSet.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDataPipeInterpolatorVTK)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDataPipeInterpolatorVTK::mafDataPipeInterpolatorVTK()
//------------------------------------------------------------------------------
{
  vtkNEW(m_VTKDataPipe);
  m_VTKDataPipe->SetDataPipe(this);
}

//------------------------------------------------------------------------------
mafDataPipeInterpolatorVTK::~mafDataPipeInterpolatorVTK()
//------------------------------------------------------------------------------
{
  vtkDEL(m_VTKDataPipe);
} 

//------------------------------------------------------------------------------
bool mafDataPipeInterpolatorVTK::Accept(mafVME *vme)
//------------------------------------------------------------------------------
{
  return Superclass::Accept(vme)&&vme->IsA(mafVMEGeneric::GetStaticTypeId());
}

//------------------------------------------------------------------------------
vtkDataSet *mafDataPipeInterpolatorVTK::GetVTKData()
//------------------------------------------------------------------------------
{
  m_VTKDataPipe->UpdateInformation();
  vtkDataSet *data = vtkDataSet::SafeDownCast(m_VTKDataPipe->GetInput());
	m_VTKDataPipe->Update();
  return (data != NULL) ? m_VTKDataPipe->GetOutput() : NULL;
}

//----------------------------------------------------------------------------
void mafDataPipeInterpolatorVTK::Update()
//----------------------------------------------------------------------------
{
  m_VTKDataPipe->Update();
}

//------------------------------------------------------------------------------
void mafDataPipeInterpolatorVTK::PreExecute()
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
void mafDataPipeInterpolatorVTK::OnEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  assert(e->GetSender()==m_VTKDataPipe); // should not receive events from other sources

  Superclass::OnEvent(e); // this also forwards the event to parent class
}