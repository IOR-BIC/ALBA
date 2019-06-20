/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testPicFactoryLogic
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


#include "testPicFactoryLogic.h"
#include "albaGUIMDIFrame.h"
#include "albaGUINamedPanel.h"
#include "albaGUISashPanel.h"
#include "albaWXLog.h"

#include "albaPics.h"
#include <wx/statbmp.h>

#include "albaGUI.h"
#include "albaSideBar.h"
//----------------------------------------------------------------------------
testPicFactoryLogic::testPicFactoryLogic()
//----------------------------------------------------------------------------
{
  m_win = new albaGUIMDIFrame("testPicFactory", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  CreateMenu();
  CreateSideBar();
  CreateLogBar();
}
//----------------------------------------------------------------------------
void testPicFactoryLogic::Show()
//----------------------------------------------------------------------------
{
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void testPicFactoryLogic::OnQuit()
//----------------------------------------------------------------------------
{
  m_win->Destroy();
}
//----------------------------------------------------------------------------
wxWindow* testPicFactoryLogic::GetTopWin()
//----------------------------------------------------------------------------
{
  return m_win;
}
//----------------------------------------------------------------------------
void testPicFactoryLogic::CreateMenu()
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
void testPicFactoryLogic::CreateLogBar()
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
// Event Ids
//----------------------------------------------------------------------------
enum 
{
  ID_BUSY = MINID,
};
//----------------------------------------------------------------------------
void testPicFactoryLogic::CreateSideBar()
//----------------------------------------------------------------------------
{
  m_side_bar = new albaGUISashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,220,"Side Bar \tCtrl+S");
  
  albaGUI *gui = new albaGUI(this); 

  gui->Divider();
  gui->Label("all the icons");
  gui->Label("note: Open and Save were overwritten");

  
  #define FOO(n) { wxStaticBitmap *bmp = new wxStaticBitmap(gui,-1, albaPics.GetBmp(#n));  gui->Add(bmp,0,wxLEFT); }
    FOO(FRAME_ICON16x16)
    FOO(FRAME_ICON32x32)
    FOO(CLOSE_SASH)
    FOO(FILE_NEW)
    FOO(FILE_OPEN)
    FOO(FILE_SAVE)
    FOO(FLYTO)
    FOO(MDICHILD_ICON)
    FOO(OP_COPY)
    FOO(OP_CUT)
    FOO(OP_PASTE)
    FOO(OP_REDO)
    FOO(OP_UNDO)
    FOO(PIC_BACK)
    FOO(PIC_BOTTOM)
    FOO(PIC_FRONT)
    FOO(PIC_LEFT)
    FOO(PIC_RIGHT)
    FOO(PIC_TOP)
    FOO(TIME_BEGIN)
    FOO(TIME_END)
    FOO(TIME_NEXT)
    FOO(TIME_PLAY)
    FOO(TIME_PREV)
    FOO(TIME_STOP)
    FOO(ZOOM)
    FOO(ZOOM_ALL)
    FOO(ZOOM_SEL)
    FOO(NODE_YELLOW)
    FOO(NODE_RED)
    FOO(NODE_BLUE)
    FOO(NODE_GRAY)

  gui->FitGui();
  gui->Reparent(m_side_bar);
  gui->Update();
  gui->Show();
}
//----------------------------------------------------------------------------
void testPicFactoryLogic::OnEvent(albaEventBase *alba_event)
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







