/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafView.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:43:10 $
  Version:   $Revision: 1.11 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafView.h"
//----------------------------------------------------------------------------
mafView::mafView( wxString label, bool external)
//----------------------------------------------------------------------------
{
	m_Label					= label;
  m_Name					= "";
	m_Win						= NULL;
  m_Frame					= NULL;
	m_Listener			= NULL;
	m_Next					= NULL;
  m_Guih					= NULL;
	m_Gui					 	= NULL;
	m_Plugged				= false;
  m_Id            = 0;
  m_Mult          = 0;
	//m_external_flag = external;
}
//----------------------------------------------------------------------------
mafView::~mafView()
//----------------------------------------------------------------------------
{
  m_PipeMap.clear();

  cppDEL(m_Gui);
}
//----------------------------------------------------------------------------
void mafView::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  mafEventMacro(*event);
}
/*
//----------------------------------------------------------------------------
void mafView::ShowGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui); 
	mafEvent settings_event(this,OP_SHOW_GUI,(wxWindow *)m_Gui);
	settings_event.SetBool(true);
	mafEventMacro(settings_event);
}
//----------------------------------------------------------------------------
void mafView::HideGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui); 
	mafEvent settings_event(this,OP_HIDE_GUI,(wxWindow *)m_Gui);
	settings_event.SetBool(true);
	mafEventMacro(settings_event);
}
*/
//----------------------------------------------------------------------------
void mafView::PlugVisualPipe(mafString vme_type, mafString pipe_type, long visibility)
//----------------------------------------------------------------------------
{
  mafVisualPipeInfo plugged_pipe;
  plugged_pipe.m_PipeName=pipe_type;
  plugged_pipe.m_Visibility=visibility;
  m_PipeMap[vme_type] = plugged_pipe;
}
//-------------------------------------------------------------------------
void mafView::DeleteGui()
//-------------------------------------------------------------------------
{
  cppDEL(m_Gui);
}
//-------------------------------------------------------------------------
mmgGui* mafView::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(NULL); // replace NULL with 'this' ....  //SIL. 22-4-2005: 
  m_Gui->Label("view default gui");
  return m_Gui;
}