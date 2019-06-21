/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIRollOut
 Authors: Paolo Quadrani
 
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

#include "albaGUIRollOut.h"

#include "albaGUI.h"
#include "albaGUIPicButton.h"

//----------------------------------------------------------------------------
// albaGUIRollOut
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIRollOut,wxPanel)
  EVT_COMMAND_RANGE( WIDGETS_START, WIDGETS_END, wxEVT_COMMAND_BUTTON_CLICKED, albaGUIRollOut::OnRollOut)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUIRollOut::albaGUIRollOut(albaGUI *parent, albaString title, albaGUI *roll_gui, int id, bool rollOutOpen)
:wxPanel(parent,id)
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  wxBoxSizer *topsizer =  new wxBoxSizer( wxHORIZONTAL );
  
  wxString b = rollOutOpen ? "ROLLOUT_OPEN" : "ROLLOUT_CLOSE";
  m_RollOutButton = new albaGUIPicButton(this, b, ID_CLOSE_SASH);
  m_RollOutButton->SetEventId(ID_CLOSE_SASH);
  topsizer->Add(m_RollOutButton,0,wxLEFT,2);

  wxStaticText* lab = new wxStaticText(this, -1, title.GetCStr());
  wxFont font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32
  font.SetPointSize(9);
#endif
  font.SetWeight(wxBOLD);
  lab->SetFont(font);
  topsizer->Add(lab,1,wxEXPAND|wxALL);

  this->SetAutoLayout(true);
  this->SetSizer(topsizer);
  topsizer->Fit(this);
  topsizer->SetSizeHints(this);

  m_MainGui = parent;
  m_RollGui = roll_gui;
  m_MainGui->Add(this);
  m_MainGui->AddGui(m_RollGui);
  m_RollGui->Show(rollOutOpen);
  m_MainGui->FitGui();
}
//----------------------------------------------------------------------------
albaGUIRollOut::~albaGUIRollOut()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIRollOut::OnRollOut(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
  int widget_id = this->GetId();
  RollOut(!m_RollGui->IsShown());
  bool isOpen = m_RollGui->IsShown();
  albaEventMacro(albaEvent(this, widget_id, isOpen));
}
//----------------------------------------------------------------------------
void albaGUIRollOut::RollOut(bool open)
//----------------------------------------------------------------------------
{
  wxString b;
  m_RollGui->Show(open);
  b = m_RollGui->IsShown() ? "ROLLOUT_OPEN": "ROLLOUT_CLOSE";
  m_RollOutButton->SetBitmap(b, ID_CLOSE_SASH);
  m_MainGui->Update();
  m_MainGui->FitGui();
  wxWindow *parent = m_MainGui->GetParent();
  if (parent)
  {
    // Refresh the scrollbar (if exists) when open/close
    // the roll out GUI.
    parent->FitInside();
  }
}

