/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOp.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:56:04 $
  Version:   $Revision: 1.24 $
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
#include "mafSmartPointer.h"
#include "mafGUI.h"
#include "mafGUIHolder.h"
#include "mafNode.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOp);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOp::mafOp(const wxString &label)
//----------------------------------------------------------------------------
{
	m_Gui       = NULL;
	m_Listener  = NULL;
	m_Next      = NULL;
	m_OpType    = OPTYPE_OP;
  m_OpMenuPath= "";
	m_Canundo   = false;
	m_Label     = label;
  m_Guih      = NULL;
	m_Input     = NULL;
  m_Output    = NULL;
  m_MenuItem  = NULL;
  m_Mouse     = NULL;
  m_SettingPanel = NULL;
  m_Compatibility     = 0xFFFF;
  m_InputPreserving = true;
  m_CollaborateStatus = false;
  m_TestMode = false;
}
//----------------------------------------------------------------------------
mafOp::mafOp()
//----------------------------------------------------------------------------
{
	m_Gui       = NULL;
	m_Listener  = NULL;
	m_Next      = NULL;
	m_OpType    = OPTYPE_OP;
	m_Canundo   = false;
	m_Label     = "default op name";
  m_OpMenuPath= "";
  m_Guih      = NULL;
	m_Input     = NULL;
  m_Output    = NULL;
  m_Mouse     = NULL;
	m_Compatibility     = 0xFFFF;
  m_InputPreserving = true;
  m_CollaborateStatus = false;
  m_SettingPanel = NULL;
}
//----------------------------------------------------------------------------
mafOp::~mafOp()
//----------------------------------------------------------------------------
{
  m_Output = NULL;
}
//----------------------------------------------------------------------------
void mafOp::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	mafEventMacro(*maf_event);
}
//----------------------------------------------------------------------------
bool mafOp::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  return false;
}
//----------------------------------------------------------------------------
void mafOp::OpRun()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOp::OpDo()
//----------------------------------------------------------------------------
{
  if (m_Output)
  {
    m_Output->ReparentTo(m_Input);
    //mafEventMacro(mafEvent(this, VME_ADD, m_Output));
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
void mafOp::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_Output)
  {
    mafEventMacro(mafEvent(this, VME_REMOVE, m_Output));
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
mafOp* mafOp::Copy()
//----------------------------------------------------------------------------
{
   return NULL;
}
//----------------------------------------------------------------------------
bool mafOp::CanUndo()
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
//----------------------------------------------------------------------------
{
  return m_OpType;
}
//----------------------------------------------------------------------------
void mafOp::ShowGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui); 
  m_Gui->Collaborate(m_CollaborateStatus);
  m_Guih = new mafGUIHolder(mafGetFrame(),-1);
  m_Guih->Put(m_Gui);
	
  wxString menu_codes=wxStripMenuCodes(m_Label);
  wxString title = wxString::Format(" %s parameters:",menu_codes.c_str());
  m_Guih->SetTitle(title);
  mafEventMacro(mafEvent(this,OP_SHOW_GUI,(wxWindow *)m_Guih));
}
//----------------------------------------------------------------------------
void mafOp::HideGui()
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
  mafEvent e(this, OP_RUN_OK);
  OnEvent(&e);
}
//----------------------------------------------------------------------------
void mafOp::ForceStopWithCancel()
//----------------------------------------------------------------------------
{
  mafEvent e(this, OP_RUN_CANCEL);
  OnEvent(&e);
}
//----------------------------------------------------------------------------
void mafOp::SetMouse(mmdMouse *mouse)
//----------------------------------------------------------------------------
{
  m_Mouse = mouse;
}
//----------------------------------------------------------------------------
void mafOp::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mafOp::Collaborate(bool status)
//----------------------------------------------------------------------------
{
  m_CollaborateStatus = status; 
}
