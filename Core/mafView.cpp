/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafView.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:02:55 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

//----------------------------------------------------------------------------
// Include: - include the class being defined first
//----------------------------------------------------------------------------
#include "mafView.h"

//#include "mafVME.h"
#include "mafDecl.h"
//#include "mafSceneNode.h"
#include "mmgGuiHolder.h"
#include "mmgGui.h"
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
	m_external_flag = external;
}
//----------------------------------------------------------------------------
mafView::~mafView( ) 
//----------------------------------------------------------------------------
{
  if(m_gui != NULL) HideGui();
}
//----------------------------------------------------------------------------
void mafView::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
  //@@@ mafEventMacro(e); 
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

