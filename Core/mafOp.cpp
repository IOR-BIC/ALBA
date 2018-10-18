/*=========================================================================

 Program: MAF2
 Module: mafOp
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
#include "mafVME.h"
#include "mafGUIPicButton.h"

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
	mafEvent *event = mafEvent::SafeDownCast(maf_event);
	if (event	&& event->GetId() == ID_HELP)
	{
		mafEvent helpEvent;
		helpEvent.SetSender(this);
		mafString opTypeName = this->GetTypeName();
		helpEvent.SetString(&opTypeName);
		helpEvent.SetId(OPEN_HELP_PAGE);
		mafEventMacro(helpEvent);
	}
	else
		mafEventMacro(*maf_event);
}
//----------------------------------------------------------------------------
bool mafOp::Accept(mafVME*vme)
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
	mafLogMessage("Op Do :%s", m_Label.c_str());

  if (m_Output)
  {
    m_Output->ReparentTo(m_Input);
		GetLogicManager()->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
void mafOp::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_Output)
  {
    GetLogicManager()->VmeRemove(m_Output);
		GetLogicManager()->CameraUpdate();
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

  m_Guih = new mafGUIHolder(mafGetFrame(),-1);
	wxBoxSizer * topSizer = m_Guih->GetTopSizer();
	wxPanel * topPanel = m_Guih->GetTopPanel();

	mafString type_name = GetTypeName();
	mafEvent buildHelpGui;
	buildHelpGui.SetSender(this);
	buildHelpGui.SetString(&type_name);
	buildHelpGui.SetId(GET_BUILD_HELP_GUI);
	mafEventMacro(buildHelpGui);

	if (buildHelpGui.GetArg() == true)
	{
		mafGUIPicButton *b = new mafGUIPicButton(topPanel, "OP_HELP", ID_HELP);
		b->SetEventId(ID_HELP);  //SIL. 7-4-2005: 
		b->SetMinSize(wxSize(16, 16));
		b->SetListener(this);
		topSizer->Add(b, 0, wxRIGHT| wxADJUST_MINSIZE, 2);
	}

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
  if(m_Gui)
	{
		mafEventMacro(mafEvent(this,OP_HIDE_GUI,(wxWindow *)m_Guih));
		delete m_Guih;
		m_Guih = NULL;
		m_Gui = NULL;
	}
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
void mafOp::SetMouse(mafDeviceButtonsPadMouse *mouse)
//----------------------------------------------------------------------------
{
  m_Mouse = mouse;
}
//----------------------------------------------------------------------------
void mafOp::OpStop(int result)
//----------------------------------------------------------------------------
{
	mafLogMessage("Stopping Op :%s", m_Label.c_str());
  HideGui();
  mafEventMacro(mafEvent(this,result));        
}

//----------------------------------------------------------------------------
char ** mafOp::GetIcon()
{
	return NULL;
}
