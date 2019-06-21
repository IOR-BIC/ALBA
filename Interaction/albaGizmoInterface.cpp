/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoInterface
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <stdio.h>
#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaGizmoInterface.h"
#include "albaGUIGizmoInterface.h"

#include "albaVME.h"

//----------------------------------------------------------------------------
albaGizmoInterface::albaGizmoInterface()
//----------------------------------------------------------------------------
{ 
  m_InputVME = NULL;
  m_Listener = NULL;
  m_Modality = G_LOCAL;
  m_RefSysVME = NULL;
  m_Visibility = false;
  m_Name = "UNDEFINED_GIZMO_NAME";
  m_Mediator = NULL;
  m_Autoscale = false;
  m_RenderWindowHeightPercentage = 0.20;
  m_AlwaysVisible = false;
}
//----------------------------------------------------------------------------
albaGizmoInterface::~albaGizmoInterface() 
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------  
void albaGizmoInterface::SendTransformMatrix(albaMatrix* matrix, int eventId, long arg)
//----------------------------------------------------------------------------
{
  albaEvent e2s;
  e2s.SetSender(this);
  e2s.SetMatrix(matrix);
  e2s.SetId(eventId);
  e2s.SetArg(arg);

  albaEventMacro(e2s);
}

albaObserver * albaGizmoInterface::GetListener()
{
  return m_Listener;
}

void albaGizmoInterface::SetListener( albaObserver *listener )
{
  m_Listener = listener;
}

void albaGizmoInterface::SetInput( albaVME *vme )
{
  m_InputVME = vme;
}

void albaGizmoInterface::OnEvent( albaEventBase *alba_event )
{
  // Not implemented
}

void albaGizmoInterface::Show( bool show )
{
  // Not implemented
}

albaVME * albaGizmoInterface::GetInput()
{
  return this->m_InputVME;
}

void albaGizmoInterface::SetModalityToLocal()
{
  this->m_Modality = G_LOCAL;
}

void albaGizmoInterface::SetModalityToGlobal()
{
  this->m_Modality = G_GLOBAL;
}

int albaGizmoInterface::GetModality()
{
  return this->m_Modality;
}

void albaGizmoInterface::SetAbsPose( albaMatrix *absPose )
{
  // not implemented
}

albaMatrix * albaGizmoInterface::GetAbsPose()
{
  return NULL;
}

albaGUI * albaGizmoInterface::GetGui()
{
  return NULL;
}