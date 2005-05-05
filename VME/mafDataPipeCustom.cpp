/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataPipeCustom.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-05 15:29:47 $
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


#include "mafDataPipeCustom.h"

#include "mafVME.h"
#include "vtkMAFDataPipe.h"
#include "mafEventBase.h"
#include "vtkDataSet.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDataPipeCustom)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDataPipeCustom::mafDataPipeCustom()
//------------------------------------------------------------------------------
{
  vtkNEW(m_VTKDataPipe);
  m_VTKDataPipe->SetDataPipe(this);
}

//------------------------------------------------------------------------------
mafDataPipeCustom::~mafDataPipeCustom()
//------------------------------------------------------------------------------
{
  vtkDEL(m_VTKDataPipe);
} 

//------------------------------------------------------------------------------
vtkDataSet *mafDataPipeCustom::GetVTKData()
//------------------------------------------------------------------------------
{
  m_VTKDataPipe->UpdateInformation();
  return m_VTKDataPipe->GetOutput();
}

//----------------------------------------------------------------------------
vtkMAFDataPipe *mafDataPipeCustom::GetVTKDataPipe()
//----------------------------------------------------------------------------
{
  return m_VTKDataPipe;
}

//----------------------------------------------------------------------------
void mafDataPipeCustom::Update()
//----------------------------------------------------------------------------
{
  m_VTKDataPipe->Update();
}

//------------------------------------------------------------------------------
void mafDataPipeCustom::UpdateBounds()
//------------------------------------------------------------------------------
{
  m_VTKDataPipe->GetOutput()->Update();
  m_VTKDataPipe->GetOutput()->ComputeBounds();
  m_Bounds.DeepCopy(m_VTKDataPipe->GetOutput()->GetBounds());
}
//------------------------------------------------------------------------------
void mafDataPipeCustom::OnEvent(mafEventBase *e)
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