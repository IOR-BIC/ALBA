/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVTKInterpolator.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:21:59 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafVTKInterpolator.h"

#include "mafVME.h"
#include "mafVMEItemVTK.h"
#include "mafVMEGenericVTK.h"
#include "vtkMAFDataPipe.h"
#include "mafEventBase.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafVTKInterpolator)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafVTKInterpolator::mafVTKInterpolator()
//------------------------------------------------------------------------------
{
  vtkNEW(m_VTKDataPipe);
  m_VTKDataPipe->SetDataPipe(this);
}

//------------------------------------------------------------------------------
mafVTKInterpolator::~mafVTKInterpolator()
//------------------------------------------------------------------------------
{
  vtkDEL(m_VTKDataPipe);
} 

//------------------------------------------------------------------------------
bool mafVTKInterpolator::Accept(mafVME *vme)
//------------------------------------------------------------------------------
{
  return Superclass::Accept(vme)&&vme->IsA(mafVMEGenericVTK::GetStaticTypeId());
}

//------------------------------------------------------------------------------
vtkDataSet *mafVTKInterpolator::GetVTKData()
//------------------------------------------------------------------------------
{
  m_VTKDataPipe->UpdateInformation();
  return m_VTKDataPipe->GetOutput();
}

//----------------------------------------------------------------------------
void mafVTKInterpolator::Update()
//----------------------------------------------------------------------------
{
  m_VTKDataPipe->Update();
}

//------------------------------------------------------------------------------
void mafVTKInterpolator::PreExecute()
//------------------------------------------------------------------------------
{
  Superclass::PreExecute();

  unsigned long mtime=this->GetMTime();

  // if the current item is changed set the data inside new item as input for the interpolator
  // more specialized interpolators could redefine this to have more inputs (e.g. when 
  // interpolating different items)
  if ( m_CurrentItem && m_CurrentItem!=m_OldItem || mtime>m_UpdateTime.GetMTime() || mtime>m_VTKDataPipe->GetInformationTime())
  {
    m_VTKDataPipe->SetNthInput(0,GetCurrentItem()->GetData());
    m_UpdateTime.Modified();
  } 
}

//------------------------------------------------------------------------------
void mafVTKInterpolator::OnEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  assert(e->GetSender()==m_VTKDataPipe); // should not receive events from other sources

  if (e->GetSender()==m_VTKDataPipe)
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

  Superclass::OnEvent(e); // this also forwards the event to parent class
}