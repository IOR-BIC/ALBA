/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTimeBarLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-04 15:01:48 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testTimeBarLogic.h" //the class being defined, must be included as first
#include "mmgMDIFrame.h"
#include "mmgTimeBar.h"
#include "mmgNamedPanel.h"
#include "mmgSashPanel.h"
#include "mafWXLog.h"
#include "mafSideBar.h"
#include "mafView.h"
//----------------------------------------------------------------------------
testTimeBarLogic::testTimeBarLogic()
//----------------------------------------------------------------------------
{
  m_win = new mmgMDIFrame("testSideBar", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  m_time_bounds[0] =0;
  m_time_bounds[1] =100;

  CreateMenu();
  CreateSideBar();
  CreateTimeBar();
  CreateLogBar();

}
//----------------------------------------------------------------------------
void testTimeBarLogic::Show()
//----------------------------------------------------------------------------
{
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void testTimeBarLogic::OnQuit()
//----------------------------------------------------------------------------
{
  m_win->Destroy();
}
//----------------------------------------------------------------------------
wxWindow* testTimeBarLogic::GetTopWin()
//----------------------------------------------------------------------------
{
  return m_win;
}
//----------------------------------------------------------------------------
void testTimeBarLogic::CreateMenu()
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
void testTimeBarLogic::CreateLogBar()
//----------------------------------------------------------------------------
{
  wxTextCtrl *log  = new wxTextCtrl( m_win, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
  mafWXLog *m_logger = new mafWXLog(log);
  wxLog *old_log = wxLog::SetActiveTarget( m_logger );
  cppDEL(old_log);

  mmgNamedPanel *log_panel = new mmgNamedPanel(m_win,-1,true);
  log_panel->SetTitle(" Log Area:");
  log_panel->Add(log,1,wxEXPAND);

  m_log_bar = new mmgSashPanel(m_win, MENU_VIEW_LOGBAR, wxBOTTOM,80,"Log Bar \tCtrl+L");
  m_log_bar->Put(log_panel);
  //m_log_bar->Show(false);
  wxLogMessage("buongiorno");
}
//----------------------------------------------------------------------------
void testTimeBarLogic::CreateTimeBar()
//----------------------------------------------------------------------------
{
  m_time_bar = new mmgSashPanel(m_win,MENU_VIEW_TIMEBAR,wxBOTTOM,22,"Time Bar",false);
  m_time_panel = new mmgTimeBar(m_time_bar,-1,true);
  m_time_panel->SetListener(this);
  m_time_bar->Put(m_time_panel);
}

//----------------------------------------------------------------------------
// Event Ids
//----------------------------------------------------------------------------
enum 
  {
  ID_TIME_BOUNDS = MINID,
  ID_TIME_SETTINGS, 
  };
//----------------------------------------------------------------------------
void testTimeBarLogic::CreateSideBar()
//----------------------------------------------------------------------------
  {
  m_side_bar = new mmgSashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,220,"Side Bar \tCtrl+S");
  //mafSideBar *sb = new mafSideBar(m_win,-1,this);

  mmgGui *gui = new mmgGui(this); 
  gui->Divider();
  gui->Label("time bounds",true);
  gui->VectorN(ID_TIME_BOUNDS,"",m_time_bounds,2,-10000,10000); 
  //gui->Button(ID_TIME_SETTINGS,"settings");
  gui->FitGui();
  gui->Reparent(m_side_bar);
  gui->Update();
  gui->Show();
  }
//----------------------------------------------------------------------------
void testTimeBarLogic::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
  switch(e.GetId())
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
    case ID_TIME_BOUNDS:
      m_time_panel->SetBounds(m_time_bounds[0],m_time_bounds[1]);
      break;
    //case ID_TIME_SETTINGS:
    //  m_time_panel->ShowGui();
    //  break;
    case UPDATE_UI:
      break;
    case TIME_SET:
      wxLogMessage("Time Set to = %g", e.GetFloat());
      //e.Log();
      break;
    default:
      e.Log();
      break; 
  }
}




