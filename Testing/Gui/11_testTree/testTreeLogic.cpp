/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTreeLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:41 $
  Version:   $Revision: 1.3 $
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


#include "testTreeLogic.h" 
#include "mafGUIMDIFrame.h"
#include "mafGUINamedPanel.h"
#include "mafGUISashPanel.h"
#include "mafWXLog.h"
//#include "testTreeGui.h" 
#include "mafPics.h"

#include "mafSideBar.h"
#include "mafView.h"

#include "mafGUITree.h"
//----------------------------------------------------------------------------
testTreeLogic::testTreeLogic()
//----------------------------------------------------------------------------
{
  m_win = new mafGUIMDIFrame("testTree", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  CreateMenu();
  CreateLogBar();
  CreateSideBar();
  
}
//----------------------------------------------------------------------------
void testTreeLogic::Show()
//----------------------------------------------------------------------------
{
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void testTreeLogic::OnQuit()
//----------------------------------------------------------------------------
{
  m_win->Destroy();
}
//----------------------------------------------------------------------------
wxWindow* testTreeLogic::GetTopWin()
//----------------------------------------------------------------------------
{
  return m_win;
}
//----------------------------------------------------------------------------
void testTreeLogic::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event)) 
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
void testTreeLogic::CreateMenu()
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
void testTreeLogic::CreateLogBar()
//----------------------------------------------------------------------------
{
  wxTextCtrl *log  = new wxTextCtrl( m_win, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
  mafWXLog *m_logger = new mafWXLog(log);
  wxLog *old_log = wxLog::SetActiveTarget( m_logger );
  cppDEL(old_log);

  mafGUINamedPanel *log_panel = new mafGUINamedPanel(m_win,-1,true);
  log_panel->SetTitle(" Log Area:");
  log_panel->Add(log,1,wxEXPAND);

  m_log_bar = new mafGUISashPanel(m_win, MENU_VIEW_LOGBAR, wxBOTTOM,80,"Log Bar \tCtrl+L");
  m_log_bar->Put(log_panel);
  //m_log_bar->Show(false);
  wxLogMessage("buongiorno");
}
//----------------------------------------------------------------------------
void testTreeLogic::CreateSideBar()
//----------------------------------------------------------------------------
{
  m_side_bar = NULL;
  m_side_bar = new mafGUISashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,330,"Side Bar \tCtrl+S");
  
  mafGUITree *tree = new mafGUITree(m_side_bar,-1,true);
  tree->SetListener(this);
  m_side_bar->Put(tree);

  tree->SetTitle("mafGUITree");
  tree->AddNode(1,0,"1",0);
  tree->AddNode(2,1,"2",1);
  tree->AddNode(3,1,"3",2);
  tree->AddNode(4,2,"4",3);
  tree->AddNode(5,2,"5",3);
  tree->AddNode(6,2,"6",3);


  wxBitmap bmp = mafPics.GetBmp("FILE_OPEN");
  int w = bmp.GetWidth();
  int h = bmp.GetHeight();

/*
  wxImageList *imgs = new wxImageList(w,h,FALSE,4);
  imgs->Add(bmp);
  imgs->Add(mafPics.GetBmp("FILE_SAVE"));
  imgs->Add(mafPics.GetBmp("FILE_OPEN"));
  imgs->Add(mafPics.GetBmp("FILE_SAVE"));
  tree->SetImageList(imgs);
*/

}






