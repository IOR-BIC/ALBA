/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgSashPanel.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-03 13:55:38 $
  Version:   $Revision: 1.8 $
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


#include "mafDecl.h"
#include "mmgSashPanel.h"
//----------------------------------------------------------------------------
// mmgSashPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgSashPanel,wxSashLayoutWindow)
  EVT_BUTTON (ID_CLOSE_SASH, mmgSashPanel::OnHide)
END_EVENT_TABLE()

#define mmgSashPanelStyle wxNO_BORDER|wxSW_3D|wxCLIP_CHILDREN
//----------------------------------------------------------------------------
mmgSashPanel::mmgSashPanel (wxWindow* parent,wxWindowID id, wxDirection side , int defaultsize, wxString menu_string, bool sizable ) 
 : wxSashLayoutWindow(parent,id,wxDefaultPosition,wxSize(100, 100), mmgSashPanelStyle)         
//----------------------------------------------------------------------------
{
  m_CurrentGui = new mmgPanel(this,-1);
  m_Frame = (wxFrame*)parent;
  m_MenuBar = NULL;

  switch (side)
  {
		case wxRIGHT:
			SetAlignment(wxLAYOUT_RIGHT);
			SetOrientation(wxLAYOUT_VERTICAL);
			SetSashVisible(wxSASH_LEFT, TRUE);
  		SetDefaultSize(wxSize(defaultsize, 1000));
		break;
		case wxLEFT:
			SetAlignment(wxLAYOUT_LEFT);
			SetOrientation(wxLAYOUT_VERTICAL);
			SetSashVisible(wxSASH_RIGHT, TRUE);
  		SetDefaultSize(wxSize(defaultsize, 1000));
		break;
		case wxTOP:
			SetAlignment(wxLAYOUT_TOP);
			SetOrientation(wxLAYOUT_HORIZONTAL);
			SetSashVisible(wxSASH_BOTTOM, TRUE);
  		SetDefaultSize(wxSize(1000, defaultsize));
		break;
		case wxBOTTOM:
			SetAlignment(wxLAYOUT_BOTTOM);
			SetOrientation(wxLAYOUT_HORIZONTAL);
			SetSashVisible(wxSASH_TOP, TRUE);
  		SetDefaultSize(wxSize(1000, defaultsize));
		break;
		default:
			wxLogMessage("mmgSashPanel: unrecognized side");
  }
  SetExtraBorderSize(0);

  if(!sizable)
  {
    SetSashVisible(wxSASH_LEFT,  FALSE);
    SetSashVisible(wxSASH_RIGHT, FALSE);
    SetSashVisible(wxSASH_BOTTOM,FALSE);
    SetSashVisible(wxSASH_TOP,   FALSE);
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
mmgSashPanel::~mmgSashPanel( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgSashPanel::OnHide( wxCommandEvent &event )
//----------------------------------------------------------------------------
{
  // notify Logic
  wxCommandEvent c(wxEVT_COMMAND_MENU_SELECTED,this->GetId());
	ProcessEvent(c);
}
//----------------------------------------------------------------------------
bool mmgSashPanel::Show(bool show)
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
bool mmgSashPanel::Put(wxWindow* win)
//----------------------------------------------------------------------------
{
  assert(m_CurrentGui);
  m_CurrentGui->Reparent(mafGetFrame());
  m_CurrentGui->Show(false);

  m_CurrentGui = win;
  m_CurrentGui->Reparent(this);
  m_CurrentGui->Show(true);
  Refresh();
  wxLayoutAlgorithm layout;
  layout.LayoutWindow(this,m_CurrentGui);

  return true;
}
