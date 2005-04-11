/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testPicFactoryLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:25:17 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "testPicFactoryLogic.h"
#include "mmgMDIFrame.h"
#include "mmgNamedPanel.h"
#include "mmgSashPanel.h"
#include "mafWXLog.h"

#include "mafPics.h"
#include <wx/statbmp.h>

#include "mmgGui.h"
#include "mafSideBar.h"
//----------------------------------------------------------------------------
testPicFactoryLogic::testPicFactoryLogic()
//----------------------------------------------------------------------------
{
  m_win = new mmgMDIFrame("testPicFactory", wxDefaultPosition, wxSize(800, 600));
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
  m_side_bar = new mmgSashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,220,"Side Bar \tCtrl+S");
  
  mmgGui *gui = new mmgGui(this); 

  gui->Divider();
  gui->Label("all the icons");
  gui->Label("note: Open and Save were overwritten");

  
  #define FOO(n) { wxStaticBitmap *bmp = new wxStaticBitmap(gui,-1, mafPics.GetBmp(#n));  gui->Add(bmp,0,wxLEFT); }
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
void testPicFactoryLogic::OnEvent(mafEvent& e)
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







