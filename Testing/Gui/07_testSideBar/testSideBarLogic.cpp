/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testSideBarLogic
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


#include "testSideBarLogic.h" 
#include "albaGUIMDIFrame.h"
#include "albaGUINamedPanel.h"
#include "albaGUISashPanel.h"
#include "albaWXLog.h"
#include "testSideBarGui.h" 

#include "albaSideBar.h"
#include "albaView.h"
//----------------------------------------------------------------------------
testSideBarLogic::testSideBarLogic()
//----------------------------------------------------------------------------
{
  m_gui = NULL;
  m_win = new albaGUIMDIFrame("testSideBar", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  CreateMenu();
  CreateSideBar();
  CreateLogBar();

}
//----------------------------------------------------------------------------
void testSideBarLogic::Show()
//----------------------------------------------------------------------------
{
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void testSideBarLogic::OnQuit()
//----------------------------------------------------------------------------
{
  cppDEL(m_gui);
  m_win->Destroy();
}
//----------------------------------------------------------------------------
wxWindow* testSideBarLogic::GetTopWin()
//----------------------------------------------------------------------------
{
  return m_win;
}
//----------------------------------------------------------------------------
void testSideBarLogic::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event)) 
  {
    switch(e->GetId())
    {
    case MENU_FILE_QUIT:
      OnQuit();		
      break; 
    case MENU_VIEW_LOGBAR:
      if(m_log_bar) m_log_bar->Show(!m_log_bar->IsShown());
      break; 
    case MENU_VIEW_SIDEBAR:
      if(m_side_bar) m_side_bar->Show(!m_side_bar->IsShown());
      break; 
    case UPDATE_UI:
      break; 
    default:
      e->Log();
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void testSideBarLogic::CreateMenu()
//----------------------------------------------------------------------------
{
  m_menu_bar  = new wxMenuBar;
  wxMenu    *file_menu = new wxMenu;
  file_menu->Append(MENU_FILE_QUIT,  "&Quit");
  m_menu_bar->Append(file_menu, "&File");
  wxMenu    *view_menu = new wxMenu;
  m_menu_bar->Append(view_menu, "&View");
  m_win->SetMenuBar(m_menu_bar);
}
//----------------------------------------------------------------------------
void testSideBarLogic::CreateLogBar()
//----------------------------------------------------------------------------
{
  wxTextCtrl *log  = new wxTextCtrl( m_win, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
  albaWXLog *m_logger = new albaWXLog(log);
  wxLog *old_log = wxLog::SetActiveTarget( m_logger );
  cppDEL(old_log);

  albaGUINamedPanel *log_panel = new albaGUINamedPanel(m_win,-1,true);
  log_panel->SetTitle(" Log Area:");
  log_panel->Add(log,1,wxEXPAND);

  m_log_bar = new albaGUISashPanel(m_win, MENU_VIEW_LOGBAR, wxBOTTOM,80,"Log Bar \tCtrl+L");
  m_log_bar->Put(log_panel);
  //m_log_bar->Show(false);
  wxLogMessage("buongiorno");
}
//----------------------------------------------------------------------------
void testSideBarLogic::CreateSideBar()
//----------------------------------------------------------------------------
{
  m_side_bar = NULL;
  //m_side_bar = new albaGUISashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,330,"Side Bar \tCtrl+S");

  albaSideBar *sb = new albaSideBar(m_win,-1,this);

  /*
  m_notebook = new wxNotebook(m_side_bar,-1);
  m_side_bar->Put(m_notebook);

  albaGUINamedPanel *p1 = new albaGUINamedPanel(m_notebook,-1,true);
  p1->SetTitle(" p1:");
  m_notebook->AddPage(p1,"page 1",true);

  albaGUINamedPanel *p2 = new albaGUINamedPanel(m_notebook,-1,true);
  p2->SetTitle(" p2:");
  m_notebook->AddPage(p2,"page 2",true);

  //albaGUINamedPanel *p3 = new albaGUINamedPanel(m_notebook,-1,true);
  //p3->SetTitle(" p3:");
  m_gui = new testSideBarGui(m_notebook);
  //p3->Add(m_gui->GetGui());

  //m_notebook->AddPage(p3,"page 3",true);
  m_notebook->AddPage(m_gui->GetGui(),"gui",true);
  
  //p3->SetTitleColor();
  */

  albaView *v = new albaView("pippo");
  m_gui = new testSideBarGui(m_win);
  albaGUI *view_gui = v->GetGui();   // Paolo 2005-04-22
  view_gui = m_gui->m_gui;          // Paolo 2005-04-22
  sb->ViewSelect(v);
}





