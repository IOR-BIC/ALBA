/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISashPanel
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


#include "albaDecl.h"
#include "albaGUISashPanel.h"
//----------------------------------------------------------------------------
// albaGUISashPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUISashPanel,wxSashLayoutWindow)
  EVT_BUTTON (ID_CLOSE_SASH, albaGUISashPanel::OnHide)
END_EVENT_TABLE()

#define albaGUISashPanelStyle wxNO_BORDER|wxSW_3D|wxCLIP_CHILDREN
//----------------------------------------------------------------------------
albaGUISashPanel::albaGUISashPanel (wxWindow* parent,wxWindowID id, wxDirection side , int defaultsize, wxString menu_string, bool sizable ) 
 : wxSashLayoutWindow(parent,id,wxDefaultPosition,wxSize(100, 100), albaGUISashPanelStyle)         
//----------------------------------------------------------------------------
{
  m_CurrentGui = new albaGUIPanel(this,-1);
  m_Frame = (wxFrame*)parent;
  m_MenuBar = NULL;

  switch (side)
  {
		case wxRIGHT:
			SetAlignment(wxLAYOUT_RIGHT);
			SetOrientation(wxLAYOUT_VERTICAL);
			SetSashVisible(wxSASH_LEFT, true);
  		SetDefaultSize(wxSize(defaultsize, 1000));
		break;
		case wxLEFT:
			SetAlignment(wxLAYOUT_LEFT);
			SetOrientation(wxLAYOUT_VERTICAL);
			SetSashVisible(wxSASH_RIGHT, true);
  		SetDefaultSize(wxSize(defaultsize, 1000));
		break;
		case wxTOP:
			SetAlignment(wxLAYOUT_TOP);
			SetOrientation(wxLAYOUT_HORIZONTAL);
			SetSashVisible(wxSASH_BOTTOM, true);
  		SetDefaultSize(wxSize(1000, defaultsize));
		break;
		case wxBOTTOM:
			SetAlignment(wxLAYOUT_BOTTOM);
			SetOrientation(wxLAYOUT_HORIZONTAL);
			SetSashVisible(wxSASH_TOP, true);
  		SetDefaultSize(wxSize(1000, defaultsize));
		break;
		default:
			albaLogMessage("albaGUISashPanel: unrecognized side");
  }
  SetExtraBorderSize(0);

  if(!sizable)
  {
    SetSashVisible(wxSASH_LEFT,  false);
    SetSashVisible(wxSASH_RIGHT, false);
    SetSashVisible(wxSASH_BOTTOM,false);
    SetSashVisible(wxSASH_TOP,   false);
  }

  // store the menu in m_MenuBar;
  // create the menu item 
  if (menu_string != "")
  {
    if ( m_MenuBar = m_Frame->GetMenuBar() )
    {
      int idx = m_MenuBar->FindMenu("View");
      if( idx != wxNOT_FOUND)
      {
        wxMenu *m = m_MenuBar->GetMenu(idx);
        m->Append(id, menu_string, "", wxITEM_CHECK);
        m_MenuBar->Check(id,true);
      }
      else
      {
        m_MenuBar=NULL; //prevent later check/uncheck  //SIL. 30-3-2005: 
      }
    }
  }
}
//----------------------------------------------------------------------------
albaGUISashPanel::~albaGUISashPanel( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUISashPanel::OnHide( wxCommandEvent &event )
//----------------------------------------------------------------------------
{
  // notify Logic
  wxCommandEvent c(wxEVT_COMMAND_MENU_SELECTED,this->GetId());
	ProcessEvent(c);
}
//----------------------------------------------------------------------------
bool albaGUISashPanel::Show(bool show)
//----------------------------------------------------------------------------
{
  // call show/hide
  wxSashLayoutWindow::Show(show);   

  // check menu item
  if(m_MenuBar) 
    //SIL. 7-4-2005: 
    // when the application close,
    // when this is about to be destroyed a Show(false) is received
    // at that time the menubar is already there, but the menuitem was destroyed.
    // Calling Check without controlling for the menuitem raise an exception.
    if(m_MenuBar->FindItem(this->GetId())) 
        m_MenuBar->Check(this->GetId(),show);

  // event for Layout
  wxCommandEvent c(wxEVT_COMMAND_BUTTON_CLICKED ,ID_LAYOUT);
  ProcessEvent(c);
  return true;
}
//----------------------------------------------------------------------------
bool albaGUISashPanel::Put(wxWindow* win)
//----------------------------------------------------------------------------
{
  assert(m_CurrentGui);
  m_CurrentGui->Reparent(albaGetFrame());
  m_CurrentGui->Show(false);

  m_CurrentGui = win;
  m_CurrentGui->Reparent(this);
  m_CurrentGui->Show(true);
  Refresh();
  wxLayoutAlgorithm layout;
  layout.LayoutWindow(this,m_CurrentGui);

  return true;
}
