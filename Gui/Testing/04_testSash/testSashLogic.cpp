/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testSashLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:25:05 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testSashLogic.h" 
#include "mmgMDIFrame.h"
#include "mmgNamedPanel.h"
#include "mmgSashPanel.h"
//#include "mafWXLog.h"

//----------------------------------------------------------------------------
testSashLogic::testSashLogic()
//----------------------------------------------------------------------------
{
  m_win = new mmgMDIFrame("testSash", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  m_menu_bar  = new wxMenuBar;
    wxMenu    *file_menu = new wxMenu;
    file_menu->Append(MENU_FILE_QUIT,  "&Quit");
  m_menu_bar->Append(file_menu, "&File");
    wxMenu    *view_menu = new wxMenu;
    m_menu_bar->Append(view_menu, "&View");
  m_win->SetMenuBar(m_menu_bar);


  m_time_bar = new mmgSashPanel(m_win, MENU_VIEW_TIMEBAR, wxLEFT,120,"time Bar \tCtrl+T");
  mmgNamedPanel *p1 = new mmgNamedPanel(m_win,-1,true);
  p1->SetTitle(" time bar");
  m_time_bar->Put(p1);

  m_log_bar = new mmgSashPanel(m_win, MENU_VIEW_LOGBAR, wxBOTTOM,80,"Log Bar \tCtrl+L");
  mmgNamedPanel *p3 = new mmgNamedPanel(m_win,-1,true);
  p3->SetTitle(" log bar");
  m_log_bar->Put(p3);

  m_side_bar = new mmgSashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,220,"Side Bar \tCtrl+S");
  mmgNamedPanel *p2 = new mmgNamedPanel(m_win,-1,true);
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
void testSashLogic::OnEvent(mafEvent& e)
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
  case UPDATE_UI:
    break; 
  default:
    e.Log();
  break; 
  }
}

