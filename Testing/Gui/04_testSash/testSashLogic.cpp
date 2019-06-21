/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testSashLogic
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


#include "testSashLogic.h" 
#include "albaGUIMDIFrame.h"
#include "albaGUINamedPanel.h"
#include "albaGUISashPanel.h"
//#include "albaWXLog.h"

//----------------------------------------------------------------------------
testSashLogic::testSashLogic()
//----------------------------------------------------------------------------
{
  m_win = new albaGUIMDIFrame("testSash", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  m_menu_bar  = new wxMenuBar;
    wxMenu    *file_menu = new wxMenu;
    file_menu->Append(MENU_FILE_QUIT,  "&Quit");
  m_menu_bar->Append(file_menu, "&File");
    wxMenu    *view_menu = new wxMenu;
    m_menu_bar->Append(view_menu, "&View");
  m_win->SetMenuBar(m_menu_bar);


  m_time_bar = new albaGUISashPanel(m_win, MENU_VIEW_TIMEBAR, wxLEFT,120,"time Bar \tCtrl+T");
  albaGUINamedPanel *p1 = new albaGUINamedPanel(m_win,-1,true);
  p1->SetTitle(" time bar");
  m_time_bar->Put(p1);

  m_log_bar = new albaGUISashPanel(m_win, MENU_VIEW_LOGBAR, wxBOTTOM,80,"Log Bar \tCtrl+L");
  albaGUINamedPanel *p3 = new albaGUINamedPanel(m_win,-1,true);
  p3->SetTitle(" log bar");
  m_log_bar->Put(p3);

  m_side_bar = new albaGUISashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,220,"Side Bar \tCtrl+S");
  albaGUINamedPanel *p2 = new albaGUINamedPanel(m_win,-1,true);
  p2->SetTitle(" side bar");
  m_side_bar->Put(p2);

}
//----------------------------------------------------------------------------
void testSashLogic::Show()
//----------------------------------------------------------------------------
{
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void testSashLogic::OnQuit()
//----------------------------------------------------------------------------
{
	m_win->Destroy();
}
//----------------------------------------------------------------------------
wxWindow* testSashLogic::GetTopWin()
//----------------------------------------------------------------------------
{
  return m_win;
}
//----------------------------------------------------------------------------
void testSashLogic::OnEvent(albaEventBase *alba_event)
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
    case MENU_VIEW_TIMEBAR:
      if(m_time_bar) m_time_bar->Show(!m_time_bar->IsShown());
      break; 
    case UPDATE_UI:
      break; 
    default:
      e->Log();
      break; 
    }
  }
}

