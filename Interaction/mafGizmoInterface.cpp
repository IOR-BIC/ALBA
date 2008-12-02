/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoInterface.cpp,v $
  Language:  C++
  Date:      $Date: 2008-12-02 15:58:35 $
  Version:   $Revision: 1.3.2.2 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include <stdio.h>
#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGizmoInterface.h"
#include "mafGUIGizmoInterface.h"

#include "mafVME.h"

//----------------------------------------------------------------------------
mafGizmoInterface::mafGizmoInterface()
//----------------------------------------------------------------------------
{ 
  m_InputVME = NULL;
  m_Listener = NULL;
  m_Modality = G_LOCAL;
  m_RefSysVME = NULL;
  m_Visibility = false;
}
//----------------------------------------------------------------------------
mafGizmoInterface::~mafGizmoInterface() 
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------  
void mafGizmoInterface::SendTransformMatrix(mafMatrix* matrix, int eventId, long arg)
//----------------------------------------------------------------------------
{
  mafEvent e2s;
  e2s.SetSender(this);
  e2s.SetMatrix(matrix);
  e2s.SetId(eventId);
  e2s.SetArg(arg);

  mafEventMacro(e2s);
}

mafObserver * mafGizmoInterface::GetListener()
{
  return m_Listener;
}

void mafGizmoInterface::SetListener( mafObserver *listener )
{


  m_Listener = listener;
}

void mafGizmoInterface::SetInput( mafVME *vme )
{
  m_InputVME = vme;
}

void mafGizmoInterface::OnEvent( mafEventBase *maf_event )
{
  // Not implemented
}

void mafGizmoInterface::Show( bool show )
{
  // Not implemented
}

mafVME * mafGizmoInterface::GetInput()
{
  return this->m_InputVME;
}

void mafGizmoInterface::SetModalityToLocal()
{
  this->m_Modality = G_LOCAL;
}

void mafGizmoInterface::SetModalityToGlobal()
{
  this->m_Modality = G_GLOBAL;
}

int mafGizmoInterface::GetModality()
{
  return this->m_Modality;
}

void mafGizmoInterface::SetAbsPose( mafMatrix *absPose )
{
  // not implemented
}

mafMatrix * mafGizmoInterface::GetAbsPose()
{
  return NULL;
}

mafGUI * mafGizmoInterface::GetGui()
{
  return NULL;
}