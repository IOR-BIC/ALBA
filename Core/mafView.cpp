/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafView.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-23 09:52:01 $
  Version:   $Revision: 1.6 $
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
	m_label					= label;
  m_name					= "";
	m_win						= NULL;
  m_frame					= NULL;
	m_Listener			= NULL;
	m_next					= NULL;
  m_guih					= NULL;
	m_gui					 	= NULL;
	m_plugged				= false;
  m_id            = 0;
  m_mult          = 0;
	//m_external_flag = external;
}
//----------------------------------------------------------------------------
mafView::~mafView()
//----------------------------------------------------------------------------
{
  m_PipeMap.clear();

  if(m_gui != NULL) 
    HideGui();
}
//----------------------------------------------------------------------------
void mafView::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
  mafEventMacro(e);
}
//----------------------------------------------------------------------------
void mafView::ShowGui()
//----------------------------------------------------------------------------
{
  assert(m_gui); 
	mafEvent settings_event(this,OP_SHOW_GUI,(wxWindow *)m_gui);
	settings_event.SetBool(true);
	mafEventMacro(settings_event);
}
//----------------------------------------------------------------------------
void mafView::HideGui()
//----------------------------------------------------------------------------
{
	assert(m_gui); 
	mafEvent settings_event(this,OP_HIDE_GUI,(wxWindow *)m_gui);
	settings_event.SetBool(true);
	mafEventMacro(settings_event);
}
//----------------------------------------------------------------------------
void mafView::PlugVisualPipe(mafID vme_type_id, mafVisualPipeInfo plugged_pipe)
//----------------------------------------------------------------------------
{
  m_PipeMap[vme_type_id] = plugged_pipe;
}