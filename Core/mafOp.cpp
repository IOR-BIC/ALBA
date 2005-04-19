/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-19 12:32:44 $
  Version:   $Revision: 1.3 $
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


#include "mafOp.h"
#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgGuiHolder.h"
#include "mafNode.h"
//----------------------------------------------------------------------------
mafOp::mafOp(wxString label)
//----------------------------------------------------------------------------
{
	m_gui = NULL;
	m_Listener = NULL;
	m_next = NULL;
	m_optype = OPTYPE_OP;
  m_opMenuPath = "";
	m_canundo = false;
	m_natural_preserving = true;
	m_label = label;
  m_guih = NULL;
	m_input = NULL;
  m_menu_item = NULL;
}
//----------------------------------------------------------------------------
mafOp::mafOp()
//----------------------------------------------------------------------------
{
	m_gui = NULL;
	m_Listener = NULL;
	m_next = NULL;
	m_optype = OPTYPE_OP;
	m_canundo = false;
	m_label = "default op name";
  m_guih = NULL;
	m_input = NULL;
	m_compatibility = 0xFFFF;
}
//----------------------------------------------------------------------------
mafOp::~mafOp() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOp::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
	mafEventMacro(e);
}
//----------------------------------------------------------------------------
bool mafOp::Accept(mafNode* vme)   
/**  */
//----------------------------------------------------------------------------
{
  return false;
}
//----------------------------------------------------------------------------
void mafOp::OpRun()   
/**  */
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOp::OpDo()   
/**  */
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOp::OpUndo()   
/**  */
//----------------------------------------------------------------------------
{
  assert(false);
}
//----------------------------------------------------------------------------
mafOp* mafOp::Copy()   
/** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
//----------------------------------------------------------------------------
{
   return NULL;
}
//----------------------------------------------------------------------------
bool mafOp::CanUndo()   
/**  */
//----------------------------------------------------------------------------
{
  return m_canundo;
}
//----------------------------------------------------------------------------
bool mafOp::IsCompatible(long state)
//----------------------------------------------------------------------------
{
  return (m_compatibility & state) != 0;
}

/*
//----------------------------------------------------------------------------
bool mafOp::IsImporter()   
//----------------------------------------------------------------------------
{
  return m_optype == OPTYPE_IMPORTER;
}
//----------------------------------------------------------------------------
bool mafOp::IsExporter()   
//----------------------------------------------------------------------------
{
  return m_optype == OPTYPE_EXPORTER;
}
//----------------------------------------------------------------------------
bool mafOp::IsOp()   
//----------------------------------------------------------------------------
{
  return m_optype == OPTYPE_OP;
}
*/
//----------------------------------------------------------------------------
int mafOp::GetType()   
/**  */
//----------------------------------------------------------------------------
{
  return m_optype;
}
//----------------------------------------------------------------------------
void mafOp::ShowGui()
/**  */
//----------------------------------------------------------------------------
{
  assert(m_gui); 
  m_guih = new mmgGuiHolder(mafGetFrame(),-1);
  m_guih->Put(m_gui);
	
  wxString title;
  wxString menu_codes=wxStripMenuCodes(m_label);
  title.Format(" %s parameters:",menu_codes.c_str());
  m_guih->SetTitle(title);
  mafEventMacro(mafEvent(this,OP_SHOW_GUI,(wxWindow *)m_guih));
}
//----------------------------------------------------------------------------
void mafOp::HideGui()
/**  */
//----------------------------------------------------------------------------
{
   assert(m_gui); 
   mafEventMacro(mafEvent(this,OP_HIDE_GUI,(wxWindow *)m_guih));
   delete m_guih;
   m_guih = NULL;
   m_gui = NULL;
}
//----------------------------------------------------------------------------
bool mafOp::OkEnabled()
//----------------------------------------------------------------------------
{
  if(!m_gui) return false;
  wxWindow* win = m_gui->FindWindow(wxOK);
  if (!win) return false;
  return win->IsEnabled();
}
//----------------------------------------------------------------------------
void mafOp::ForceStopWithOk()
//----------------------------------------------------------------------------
{
  mafEvent e(this,wxOK);
  OnEvent(e);
}
//----------------------------------------------------------------------------
void mafOp::ForceStopWithCancel()
//----------------------------------------------------------------------------
{
  mafEvent e(this,wxCANCEL);
  OnEvent(e);
}
//----------------------------------------------------------------------------
void mafOp::SetMouseAction(mafAction *action)
//----------------------------------------------------------------------------
{
  m_MouseAction = action;
}
