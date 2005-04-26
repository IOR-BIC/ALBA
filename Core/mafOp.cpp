/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-26 11:08:33 $
  Version:   $Revision: 1.4 $
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
	m_Gui = NULL;
	m_Listener = NULL;
	m_Next = NULL;
	m_OpType = OPTYPE_OP;
  m_OpMenuPath = "";
	m_Canundo = false;
	m_NaturalPreserving = true;
	m_Label = label;
  m_Guih = NULL;
	m_Input = NULL;
  m_MenuItem = NULL;
}
//----------------------------------------------------------------------------
mafOp::mafOp()
//----------------------------------------------------------------------------
{
	m_Gui = NULL;
	m_Listener = NULL;
	m_Next = NULL;
	m_OpType = OPTYPE_OP;
	m_Canundo = false;
	m_Label = "default op name";
  m_Guih = NULL;
	m_Input = NULL;
	m_Compatibility = 0xFFFF;
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
  return m_Canundo;
}
//----------------------------------------------------------------------------
bool mafOp::IsCompatible(long state)
//----------------------------------------------------------------------------
{
  return (m_Compatibility & state) != 0;
}

/*
//----------------------------------------------------------------------------
bool mafOp::IsImporter()   
//----------------------------------------------------------------------------
{
  return m_OpType == OPTYPE_IMPORTER;
}
//----------------------------------------------------------------------------
bool mafOp::IsExporter()   
//----------------------------------------------------------------------------
{
  return m_OpType == OPTYPE_EXPORTER;
}
//----------------------------------------------------------------------------
bool mafOp::IsOp()   
//----------------------------------------------------------------------------
{
  return m_OpType == OPTYPE_OP;
}
*/
//----------------------------------------------------------------------------
int mafOp::GetType()   
/**  */
//----------------------------------------------------------------------------
{
  return m_OpType;
}
//----------------------------------------------------------------------------
void mafOp::ShowGui()
/**  */
//----------------------------------------------------------------------------
{
  assert(m_Gui); 
  m_Guih = new mmgGuiHolder(mafGetFrame(),-1);
  m_Guih->Put(m_Gui);
	
  wxString title;
  wxString menu_codes=wxStripMenuCodes(m_Label);
  title.Format(" %s parameters:",menu_codes.c_str());
  m_Guih->SetTitle(title);
  mafEventMacro(mafEvent(this,OP_SHOW_GUI,(wxWindow *)m_Guih));
}
//----------------------------------------------------------------------------
void mafOp::HideGui()
/**  */
//----------------------------------------------------------------------------
{
   assert(m_Gui); 
   mafEventMacro(mafEvent(this,OP_HIDE_GUI,(wxWindow *)m_Guih));
   delete m_Guih;
   m_Guih = NULL;
   m_Gui = NULL;
}
//----------------------------------------------------------------------------
bool mafOp::OkEnabled()
//----------------------------------------------------------------------------
{
  if(!m_Gui) return false;
  wxWindow* win = m_Gui->FindWindow(wxOK);
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
