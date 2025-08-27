/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOp.h"
#include "albaDecl.h"
#include "albaSmartPointer.h"
#include "albaGUI.h"
#include "albaGUIHolder.h"
#include "albaVME.h"
#include "albaGUIPicButton.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOp);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOp::albaOp(const wxString &label)
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
  m_TestMode = false;
	m_EnableAccept = true;
}
//----------------------------------------------------------------------------
albaOp::albaOp()
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
  m_SettingPanel = NULL;
}
//----------------------------------------------------------------------------
albaOp::~albaOp()
{
	HideGui();
  m_Output = NULL;
}
//----------------------------------------------------------------------------
void albaOp::OnEvent(albaEventBase *alba_event)
{
	albaEvent *event = albaEvent::SafeDownCast(alba_event);
	if (event	&& event->GetId() == ID_HELP)
	{
		albaEvent helpEvent;
		helpEvent.SetSender(this);
		albaString opTypeName = this->GetTypeName();
		helpEvent.SetString(&opTypeName);
		helpEvent.SetId(OPEN_HELP_PAGE);
		albaEventMacro(helpEvent);
	}
	else
		albaEventMacro(*alba_event);
}
//----------------------------------------------------------------------------
bool albaOp::Accept(albaVME*vme)
{
	if (!m_EnableAccept) return false;

  return InternalAccept(vme);
}
//----------------------------------------------------------------------------
bool albaOp::InternalAccept(albaVME*vme)
{
	return false;
}
//----------------------------------------------------------------------------
void albaOp::OpRun()
{
}
//----------------------------------------------------------------------------
void albaOp::OpDo()
{
	albaLogMessage("Op Do :%s", m_Label.ToAscii());

  if (m_Output)
  {
    m_Output->ReparentTo(m_Input);
		GetLogicManager()->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
void albaOp::OpUndo()
{
  if (m_Output)
  {
    GetLogicManager()->VmeRemove(m_Output);
		GetLogicManager()->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
albaOp* albaOp::Copy()
{
   return NULL;
}
//----------------------------------------------------------------------------
bool albaOp::CanUndo()
{
  return m_Canundo;
}
//----------------------------------------------------------------------------
bool albaOp::IsCompatible(long state)
{
  return (m_Compatibility & state) != 0;
}

//----------------------------------------------------------------------------
int albaOp::GetType()
{
  return m_OpType;
}
//----------------------------------------------------------------------------
void albaOp::ShowGui()
{
  assert(m_Gui); 

  m_Guih = new albaGUIHolder(albaGetFrame(),-1);
	wxBoxSizer * topSizer = m_Guih->GetTopSizer();
	wxPanel * topPanel = m_Guih->GetTopPanel();

	albaString type_name = GetTypeName();
	albaEvent buildHelpGui;
	buildHelpGui.SetSender(this);
	buildHelpGui.SetString(&type_name);
	buildHelpGui.SetId(GET_BUILD_HELP_GUI);
	albaEventMacro(buildHelpGui);

	if (buildHelpGui.GetArg() == true)
	{
		albaGUIPicButton *b = new albaGUIPicButton(topPanel, "OP_HELP", ID_HELP);
		b->SetEventId(ID_HELP);  //SIL. 7-4-2005: 
		b->SetMinSize(wxSize(16, 16));
		b->SetListener(this);
		topSizer->Add(b, 0, wxRIGHT, 2);
	}

  m_Guih->Put(m_Gui);
	
  wxString menu_codes=wxStripMenuCodes(m_Label);
	wxString title=" ";
	title	+= menu_codes +" parameters:";
  m_Guih->SetTitle(title);
  albaEventMacro(albaEvent(this,OP_SHOW_GUI,(wxWindow *)m_Guih));
}
//----------------------------------------------------------------------------
void albaOp::HideGui()
{
  if(m_Gui)
	{
		albaEventMacro(albaEvent(this,OP_HIDE_GUI,(wxWindow *)m_Guih));
		delete m_Guih;
		m_Guih = NULL;
		m_Gui = NULL;
	}
}
//----------------------------------------------------------------------------
bool albaOp::OkEnabled()
{
  if(!m_Gui) return false;
  wxWindow* win = m_Gui->FindWindow(wxOK);
  if (!win) return false;
  return win->IsEnabled();
}
//----------------------------------------------------------------------------
void albaOp::ForceStopWithOk()
{
  albaEvent e(this, OP_RUN_OK);
  OnEvent(&e);
}
//----------------------------------------------------------------------------
void albaOp::ForceStopWithCancel()
{
  albaEvent e(this, OP_RUN_CANCEL);
  OnEvent(&e);
}
//----------------------------------------------------------------------------
void albaOp::SetMouse(albaDeviceButtonsPadMouse *mouse)
{
  m_Mouse = mouse;
}
//----------------------------------------------------------------------------
void albaOp::OpStop(int result)
{
	albaLogMessage("Stopping Op :%s\n", m_Label.ToAscii());
	if (m_Gui)
		m_Gui->DisableRecursive();
	if (result == OP_RUN_CANCEL || !CanUndo()) //on cancel or operation that cannot do undo the op will be deleted so the HideGui method will crash 
	{
		albaEventMacro(albaEvent(this, result));
	}
	else
	{
		albaEventMacro(albaEvent(this, result));
		HideGui();
	}
}
//----------------------------------------------------------------------------
char ** albaOp::GetIcon()
{
#include "pic/MENU_OP_EMPTY.xpm"
	return MENU_OP_EMPTY_xpm;
}
