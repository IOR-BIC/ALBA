/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLogicWithGUI.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-13 13:09:01 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden, Paolo Quadrani
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


#include "mafLogicWithGUI.h"
#include "wx/utils.h"
#include "wx/time.h"
#include "wx/busyinfo.h"
#include "mafDecl.h"
#include "mafView.h"
#include "mmgMDIFrame.h"
#include "mmgMDIChild.h"
#include "mmgFrame.h"
#include "mmgPicButton.h"
#include "mmgSplittedPanel.h"
#include "mmgSashPanel.h"
#include "mmgNamedPanel.h"
#include "mmgCrossSplitter.h"
#include "mmgTimeBar.h"
#include "mafWXLog.h"
#include "mafPics.h"
//----------------------------------------------------------------------------
mafLogicWithGUI::mafLogicWithGUI()
//----------------------------------------------------------------------------
{
  m_win = new mmgMDIFrame("maf", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  m_log_sash				= NULL;
  m_time_sash			= NULL;
  m_time_panel  	= NULL;
  m_side_sash	    = NULL;

	m_log_to_file			= false;
	m_log_all_events	= false;
	m_logger					= NULL;

	m_AppTitle = "";

	m_PlugMenu		= true;
	m_PlugToolbar	= true;
	m_PlugSidebar	= true;
	m_PlugTimebar	= true;
	m_PlugLogbar	= true;

}
//----------------------------------------------------------------------------
mafLogicWithGUI::~mafLogicWithGUI( ) 
//----------------------------------------------------------------------------
{
  //ammazzare il log ???
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::Configure()
//----------------------------------------------------------------------------
{
  if(m_PlugMenu)		this->CreateMenu();
  if(m_PlugToolbar) this->CreateToolbar();
  if(m_PlugSidebar) this->CreateSidebar();
  if(m_PlugTimebar) this->CreateTimebar();
  if(m_PlugLogbar)	this->CreateLogbar();	else this->CreateNullLog();
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::Show()
//----------------------------------------------------------------------------
{
  m_AppTitle = m_win->GetTitle();
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateMenu()
//----------------------------------------------------------------------------
{
  m_menu_bar  = new wxMenuBar;
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(MENU_FILE_QUIT,  "&Quit");
  m_menu_bar->Append(file_menu, "&File");
  wxMenu *edit_menu = new wxMenu;
  m_menu_bar->Append(edit_menu, "&Edit");
  wxMenu *view_menu = new wxMenu;
  if(this->m_PlugToolbar) view_menu->Append(MENU_VIEW_TOOLBAR, "Toolbar","",true);
  m_menu_bar->Append(view_menu, "&View"); //required by he sidebars

  m_win->SetMenuBar(m_menu_bar);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateNullLog()
//----------------------------------------------------------------------------
{
  wxTextCtrl *log  = new wxTextCtrl( m_win, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
	m_logger = new mafWXLog(log);
	log->Show(false);
	wxLog *old_log = wxLog::SetActiveTarget( m_logger );
  cppDEL(old_log);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
  switch(e.GetId())
  {
  case MENU_FILE_QUIT:
			OnQuit();		
  break; 
  // ###############################################################
  // commands related to the SASH
  case MENU_VIEW_LOGBAR:
    if(m_log_sash) m_log_sash->Show(!m_log_sash->IsShown());
    break; 
  case MENU_VIEW_SIDEBAR:
    if(m_side_sash) m_side_sash->Show(!m_side_sash->IsShown());
    break; 
  case MENU_VIEW_TIMEBAR:
    if(m_time_sash) m_time_sash->Show(!m_time_sash->IsShown());
    break; 
  case MENU_VIEW_TOOLBAR:
    if(m_PlugToolbar)	
    {
      bool show = !m_toolbar->IsShown();
      m_toolbar->Show(show);
      m_menu_bar->FindItem(MENU_VIEW_TOOLBAR)->Check(show);
      m_win->Update();
    }
    break; 
  // ###############################################################
  // commands related to the STATUSBAR
  case BIND_TO_PROGRESSBAR:
    m_win->BindToProgressBar(e.GetVtkObj());
    break;
  case PROGRESSBAR_SHOW:
    m_win->ProgressBarShow();
    break;
  case PROGRESSBAR_HIDE:
    m_win->ProgressBarHide();
    break;
  case PROGRESSBAR_SET_VALUE:
    m_win->ProgressBarSetVal(e.GetArg());
    break;
  case PROGRESSBAR_SET_TEXT:
    m_win->ProgressBarSetText(&wxString(e.GetString()->GetCStr()));
    break;
  // ###############################################################
  case UPDATE_UI:
    break; 
  default:
    e.Log();
  break; 
  }
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::OnQuit()
//----------------------------------------------------------------------------
{
  // if OnQuit is redefined in a deriver class,  mafLogicWithGUI::OnQuit() must be clalled last

  cppDEL(m_side_sash); //must be after deleting the vme_manager
  if(m_PlugLogbar) delete wxLog::SetActiveTarget(NULL); 
  m_win->Destroy();
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateLogbar()
//----------------------------------------------------------------------------
{
  wxTextCtrl *log  = new wxTextCtrl( m_win, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
  mafWXLog *m_logger = new mafWXLog(log);
  wxLog *old_log = wxLog::SetActiveTarget( m_logger );
  cppDEL(old_log);

  mmgNamedPanel *log_panel = new mmgNamedPanel(m_win,-1,true);
  log_panel->SetTitle(" Log Area:");
  log_panel->Add(log,1,wxEXPAND);

  m_log_sash = new mmgSashPanel(m_win, MENU_VIEW_LOGBAR, wxBOTTOM,80,"Log Bar \tCtrl+L");
  m_log_sash->Put(log_panel);
  //m_log_sash->Show(false);
  wxLogMessage("welcome");
}

//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateToolbar()
//----------------------------------------------------------------------------
{
  m_toolbar = new wxToolBar(m_win,-1,wxPoint(0,0),wxSize(-1,-1),wxHORIZONTAL|wxNO_BORDER|wxTB_FLAT  );
  m_toolbar->SetMargins(0,0);
  m_toolbar->SetToolSeparation(2);
  m_toolbar->SetToolBitmapSize(wxSize(20,20));
  m_toolbar->AddTool(MENU_FILE_NEW,mafPics.GetBmp("FILE_NEW"),    "new msf storage file");
  m_toolbar->AddTool(MENU_FILE_OPEN,mafPics.GetBmp("FILE_OPEN"),  "open msf storage file");
  m_toolbar->AddTool(MENU_FILE_SAVE,mafPics.GetBmp("FILE_SAVE"),  "save current msf storage file");
  m_toolbar->AddSeparator();

  m_toolbar->AddTool(OP_UNDO,mafPics.GetBmp("OP_UNDO"),  "undo (ctrl+z)"); //correggere tooltip - shortcut sbagliati
  m_toolbar->AddTool(OP_REDO,mafPics.GetBmp("OP_REDO"),  "redo (ctrl+shift+z)");
  m_toolbar->AddSeparator();

  m_toolbar->AddTool(OP_CUT,  mafPics.GetBmp("OP_CUT"),  "cut selected vme (ctrl+x)");
  m_toolbar->AddTool(OP_COPY, mafPics.GetBmp("OP_COPY"), "copy selected vme (ctrl+c)");
  m_toolbar->AddTool(OP_PASTE,mafPics.GetBmp("OP_PASTE"),"paste vme (ctrl+v)");
  m_toolbar->AddSeparator();
  m_toolbar->AddTool(CAMERA_RESET,mafPics.GetBmp("ZOOM_ALL"),"reset camera to fit all (ctrl+f)");
  m_toolbar->AddTool(CAMERA_FIT,  mafPics.GetBmp("ZOOM_SEL"),"reset camera to fit selected object (ctrl+shift+f)");
  m_toolbar->AddTool(CAMERA_FLYTO,mafPics.GetBmp("FLYTO"),"fly to object under mouse (press f inside a 3Dview)");
  m_toolbar->Realize();
  m_win->SetToolBar(m_toolbar);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateSidebar()
//----------------------------------------------------------------------------
{
  m_side_sash = new mmgSashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,245, "Side Bar \tCtrl+S"); // 245 is the width of the sideBar
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateTimebar()
//----------------------------------------------------------------------------
{
  m_time_sash = new mmgSashPanel(m_win,MENU_VIEW_TIMEBAR,wxBOTTOM,22,"Time Bar \tCtrl+T",false);
  m_time_panel = new mmgTimeBar(m_time_sash,-1,true);
  m_time_panel->SetListener(this);
  m_time_sash->Put(m_time_panel);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::EnableItem(int item, bool enable)
//----------------------------------------------------------------------------
{
  if(m_menu_bar) 
     // must always check if a menu item exist because
     // during application shutdown it is not guaranteed
     if(m_menu_bar->FindItem(item))	
        m_menu_bar->Enable(item,enable );
  if(m_toolbar) 
     m_toolbar->EnableTool(item,enable );
}
