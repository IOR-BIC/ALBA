/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testStatusBarLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-04 10:39:34 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testStatusBarLogic.h" //the class being defined, must be included as first
#include "mmgMDIFrame.h"
#include "mmgNamedPanel.h"
#include "mmgSashPanel.h"
#include "mafWXLog.h"

#include "mmgGui.h"
#include "mafSideBar.h"
#include "mafView.h"
#include <wx/gauge.h>
#include <vtkImageMandelbrotSource.h>
//----------------------------------------------------------------------------
testStatusBarLogic::testStatusBarLogic()
//----------------------------------------------------------------------------
{
  m_progress =0;
  m_progress_text = "progress text";

  m_win = new mmgMDIFrame("testStatusBar", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  CreateMenu();
  CreateSideBar();
  CreateLogBar();
}
//----------------------------------------------------------------------------
void testStatusBarLogic::Show()
//----------------------------------------------------------------------------
{
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void testStatusBarLogic::OnQuit()
//----------------------------------------------------------------------------
{
  m_win->Destroy();
}
//----------------------------------------------------------------------------
wxWindow* testStatusBarLogic::GetTopWin()
//----------------------------------------------------------------------------
{
  return m_win;
}
//----------------------------------------------------------------------------
void testStatusBarLogic::CreateMenu()
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
void testStatusBarLogic::CreateLogBar()
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
  ID_READY,
  ID_SHOWPB,
  ID_HIDEPB,
  ID_PB_VALUE,
  ID_PB_TEXT,
  ID_TEST_VTK,
  ID_REN_START,
  ID_REN_END,
};
//----------------------------------------------------------------------------
void testStatusBarLogic::CreateSideBar()
//----------------------------------------------------------------------------
{
  m_side_bar = new mmgSashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,220,"Side Bar \tCtrl+S");
  //mafSideBar *sb = new mafSideBar(m_win,-1,this);
  
  mmgGui *gui = new mmgGui(this); 

  gui->Divider();
  gui->Label("Direct Api Call",true);
  gui->Button(ID_BUSY,"busy");
  gui->Button(ID_READY,"ready");
  gui->Divider();
  gui->Button(ID_REN_START,"render start");
  gui->Button(ID_REN_END,  "render end");
  gui->Divider();
  gui->Button(ID_SHOWPB,"show progress bar");
  gui->Button(ID_HIDEPB,"hide progress bar");
  gui->Label("progress value");
  gui->Slider(ID_PB_VALUE,"",&m_progress,0,100);
  gui->Label("progress text");
  gui->String(ID_PB_TEXT,"",&m_progress_text);
  gui->Divider(2);
  gui->Label("Connect a vtkObject",true);
  gui->Button(ID_TEST_VTK,"test vtkProgress");

  /*
  wxGauge *g = new wxGauge(gui,-1,100,wxPoint(0,100),wxSize(200,10),wxGA_SMOOTH); //SIL. 1-4-2005: 
  g->SetRange(100);
  g->SetValue(50);
  gui->Add(g);
  */

  gui->FitGui();
  gui->Reparent(m_side_bar);
  gui->Update();
  gui->Show();
}
//----------------------------------------------------------------------------
void testStatusBarLogic::OnEvent(mafEvent& e)
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
  case ID_BUSY:
    m_win->Busy();
    break; 
  case ID_READY:
    m_win->Ready();
    break; 
  case ID_SHOWPB:
    m_win->ProgressBarShow();
    m_win->ProgressBarSetVal(m_progress);
    m_win->ProgressBarSetText(&m_progress_text);
    break; 
  case ID_HIDEPB:
    m_win->ProgressBarHide();
    break; 
  case ID_PB_VALUE:
    m_win->ProgressBarSetVal(m_progress);
  break; 
  case ID_PB_TEXT:
    m_win->ProgressBarSetText(&m_progress_text);
  break; 
  case ID_TEST_VTK:
  {
    vtkImageMandelbrotSource *ims = NULL;
    ims = vtkImageMandelbrotSource::New();
    ims->SetWholeExtent(0,1000,0,1000,0,1);
    m_win->BindToProgressBar(ims,&wxString("vtkImageMandelbrotSource"));
    ims->Update();
    ims->Delete();
  }
  break; 
  case ID_REN_START:
    m_win->RenderStart();
  break; 
  case ID_REN_END:
    m_win->RenderEnd();
  break; 
  default:
    e.Log();
    break; 
  }
}






