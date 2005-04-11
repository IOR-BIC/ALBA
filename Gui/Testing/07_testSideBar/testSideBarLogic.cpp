/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testSideBarLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:24:47 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "testSideBarLogic.h" 
#include "mmgMDIFrame.h"
#include "mmgNamedPanel.h"
#include "mmgSashPanel.h"
#include "mafWXLog.h"
#include "testSideBarGui.h" 

#include "mafSideBar.h"
#include "mafView.h"
//----------------------------------------------------------------------------
testSideBarLogic::testSideBarLogic()
//----------------------------------------------------------------------------
{
  m_gui = NULL;
  m_win = new mmgMDIFrame("testSideBar", wxDefaultPosition, wxSize(800, 600));
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
void testSideBarLogic::OnEvent(mafEvent& e)
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
  case UPDATE_UI:
  break; 
  default:
    e.Log();
  break; 
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
void testSideBarLogic::CreateSideBar()
//----------------------------------------------------------------------------
{
  m_side_bar = NULL;
  //m_side_bar = new mmgSashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,330,"Side Bar \tCtrl+S");

  mafSideBar *sb = new mafSideBar(m_win,-1,this);

  /*
  m_notebook = new wxNotebook(m_side_bar,-1);
  m_side_bar->Put(m_notebook);

  mmgNamedPanel *p1 = new mmgNamedPanel(m_notebook,-1,true);
  p1->SetTitle(" p1:");
  m_notebook->AddPage(p1,"page 1",true);

  mmgNamedPanel *p2 = new mmgNamedPanel(m_notebook,-1,true);
  p2->SetTitle(" p2:");
  m_notebook->AddPage(p2,"page 2",true);

  //mmgNamedPanel *p3 = new mmgNamedPanel(m_notebook,-1,true);
  //p3->SetTitle(" p3:");
  m_gui = new testSideBarGui(m_notebook);
  //p3->Add(m_gui->GetGui());

  //m_notebook->AddPage(p3,"page 3",true);
  m_notebook->AddPage(m_gui->GetGui(),"gui",true);
  
  //p3->SetTitleColor();
  */

  mafView *v = new mafView("pippo");
  m_gui = new testSideBarGui(m_win);
  v->m_gui = m_gui->m_gui;
  sb->ViewSelect(v);
}





