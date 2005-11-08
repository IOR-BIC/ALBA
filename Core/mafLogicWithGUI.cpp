/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLogicWithGUI.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-08 09:44:18 $
  Version:   $Revision: 1.14 $
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
#include <wx/utils.h>
#include <wx/datetime.h>
#include <wx/busyinfo.h>
#include <wx/splash.h>
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
#include "mafVTKLog.h"

//----------------------------------------------------------------------------
mafLogicWithGUI::mafLogicWithGUI()
//----------------------------------------------------------------------------
{
  m_Win = new mmgMDIFrame("maf", wxDefaultPosition, wxSize(800, 600));
  m_Win->SetListener(this);

  m_LogSash				= NULL;
  m_TimeSash			= NULL;
  m_TimePanel  	  = NULL;
  m_SideSash	    = NULL;

	m_LogToFile			= false;
	m_LogAllEvents	= false;
	m_Logger				= NULL;
  m_VtkLog        = NULL;

	m_AppTitle      = "";

	m_PlugMenu		  = true;
	m_PlugToolbar	  = true;
	m_PlugSidebar	  = true;
	m_PlugTimebar	  = true;
	m_PlugLogbar	  = true;
  
  m_ShowDefaultSplashScreen = false;
  m_ShowSplashScreen        = false;
}
//----------------------------------------------------------------------------
mafLogicWithGUI::~mafLogicWithGUI()
//----------------------------------------------------------------------------
{
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
  if (m_ShowDefaultSplashScreen)
  {
    m_SplashImage = mafPics.GetBmp("MAF");
  }

  if (m_ShowSplashScreen)
  {
    long splash_style = wxSIMPLE_BORDER | wxSTAY_ON_TOP;
    wxSplashScreen* splash = new wxSplashScreen(m_SplashImage,
      wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
      4000, NULL, -1, wxDefaultPosition, wxDefaultSize,
      splash_style);
  }
  mafYield();

  m_AppTitle = m_Win->GetTitle().c_str();
	m_Win->Show(TRUE);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateMenu()
//----------------------------------------------------------------------------
{
  m_MenuBar  = new wxMenuBar;
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(MENU_FILE_QUIT,  "&Quit");
  m_MenuBar->Append(file_menu, "&File");
  wxMenu *edit_menu = new wxMenu;
  m_MenuBar->Append(edit_menu, "&Edit");
  wxMenu *view_menu = new wxMenu;
  if(this->m_PlugToolbar) 
  {
    view_menu->Append(MENU_VIEW_TOOLBAR, "Toolbar","",wxITEM_CHECK);
    view_menu->Check(MENU_VIEW_TOOLBAR,true);
  }
  m_MenuBar->Append(view_menu, "&View");

  m_Win->SetMenuBar(m_MenuBar);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateNullLog()
//----------------------------------------------------------------------------
{
  m_VtkLog = mafVTKLog::New();
  m_VtkLog->SetInstance(m_VtkLog);
  wxTextCtrl *log  = new wxTextCtrl( m_Win, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
	m_Logger = new mafWXLog(log);
	log->Show(false);
	wxLog *old_log = wxLog::SetActiveTarget( m_Logger );
  cppDEL(old_log);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case MENU_FILE_QUIT:
      OnQuit();		
      break; 
      // ###############################################################
      // commands related to the SASH
    case MENU_VIEW_LOGBAR:
      if(m_LogSash) m_LogSash->Show(!m_LogSash->IsShown());
      break; 
    case MENU_VIEW_SIDEBAR:
      if(m_SideSash) m_SideSash->Show(!m_SideSash->IsShown());
      break; 
    case MENU_VIEW_TIMEBAR:
      if(m_TimeSash) m_TimeSash->Show(!m_TimeSash->IsShown());
      break; 
    case MENU_VIEW_TOOLBAR:
      if(m_PlugToolbar)	
      {
        bool show = !m_TooBar->IsShown();
        m_TooBar->Show(show);
        m_MenuBar->FindItem(MENU_VIEW_TOOLBAR)->Check(show);
        m_Win->Update();
      }
      break; 
      // ###############################################################
      // commands related to the STATUSBAR
    case BIND_TO_PROGRESSBAR:
      m_Win->BindToProgressBar(e->GetVtkObj());
      break;
    case PROGRESSBAR_SHOW:
      m_Win->ProgressBarShow();
      break;
    case PROGRESSBAR_HIDE:
      m_Win->ProgressBarHide();
      break;
    case PROGRESSBAR_SET_VALUE:
      m_Win->ProgressBarSetVal(e->GetArg());
      break;
    case PROGRESSBAR_SET_TEXT:
      m_Win->ProgressBarSetText(&wxString(e->GetString()->GetCStr()));
      break;
      // ###############################################################
    case UPDATE_UI:
      break; 
    default:
      e->Log();
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::OnQuit()
//----------------------------------------------------------------------------
{
  // if OnQuit is redefined in a deriver class,  mafLogicWithGUI::OnQuit() must be clalled last

  cppDEL(m_SideSash); //must be after deleting the vme_manager
  if(m_PlugLogbar) delete wxLog::SetActiveTarget(NULL); 
  vtkDEL(m_VtkLog);
  m_Win->Destroy();
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateLogbar()
//----------------------------------------------------------------------------
{
  m_VtkLog = mafVTKLog::New();
  m_VtkLog->SetInstance(m_VtkLog);

  wxTextCtrl *log  = new wxTextCtrl( m_Win, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
  mafWXLog *m_Logger = new mafWXLog(log);
  wxString s = mafGetApplicationDirectory().c_str();
  wxDateTime *log_time = new wxDateTime();
  s += "\\";
  s += m_AppTitle.GetCStr();
//  s += wxString::Format("_%02d_%02d_%d_%02d_%2d",log_time->GetDay(),log_time->GetMonth() + 1,log_time->GetYear(),log_time->GetHour(),log_time->GetMinute());
  s += ".log";
  if(m_LogToFile)
  {
    m_Logger->SetFileName(s);
    m_Logger->LogToFile(m_LogToFile);
  }

  wxLog *old_log = wxLog::SetActiveTarget( m_Logger );
  cppDEL(old_log);
  cppDEL(log_time);

  mmgNamedPanel *log_panel = new mmgNamedPanel(m_Win,-1,true);
  log_panel->SetTitle(" Log Area:");
  log_panel->Add(log,1,wxEXPAND);

  m_LogSash = new mmgSashPanel(m_Win, MENU_VIEW_LOGBAR, wxBOTTOM,80,"Log Bar \tCtrl+L");
  m_LogSash->Put(log_panel);
  //m_LogSash->Show(false);
  wxLogMessage("welcome");
}

//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateToolbar()
//----------------------------------------------------------------------------
{
  m_TooBar = new wxToolBar(m_Win,-1,wxPoint(0,0),wxSize(-1,-1),wxHORIZONTAL|wxNO_BORDER|wxTB_FLAT  );
  m_TooBar->SetMargins(0,0);
  m_TooBar->SetToolSeparation(2);
  m_TooBar->SetToolBitmapSize(wxSize(20,20));
  m_TooBar->AddTool(MENU_FILE_NEW,mafPics.GetBmp("FILE_NEW"),    "new msf storage file");
  m_TooBar->AddTool(MENU_FILE_OPEN,mafPics.GetBmp("FILE_OPEN"),  "open msf storage file");
  m_TooBar->AddTool(MENU_FILE_SAVE,mafPics.GetBmp("FILE_SAVE"),  "save current msf storage file");
  m_TooBar->AddSeparator();

  m_TooBar->AddTool(OP_UNDO,mafPics.GetBmp("OP_UNDO"),  "undo (ctrl+z)"); //correggere tooltip - shortcut sbagliati
  m_TooBar->AddTool(OP_REDO,mafPics.GetBmp("OP_REDO"),  "redo (ctrl+shift+z)");
  m_TooBar->AddSeparator();

  m_TooBar->AddTool(OP_CUT,  mafPics.GetBmp("OP_CUT"),  "cut selected vme (ctrl+x)");
  m_TooBar->AddTool(OP_COPY, mafPics.GetBmp("OP_COPY"), "copy selected vme (ctrl+c)");
  m_TooBar->AddTool(OP_PASTE,mafPics.GetBmp("OP_PASTE"),"paste vme (ctrl+v)");
  m_TooBar->AddSeparator();
  m_TooBar->AddTool(CAMERA_RESET,mafPics.GetBmp("ZOOM_ALL"),"reset camera to fit all (ctrl+f)");
  m_TooBar->AddTool(CAMERA_FIT,  mafPics.GetBmp("ZOOM_SEL"),"reset camera to fit selected object (ctrl+shift+f)");
  m_TooBar->AddTool(CAMERA_FLYTO,mafPics.GetBmp("FLYTO"),"fly to object under mouse (press f inside a 3Dview)");
  m_TooBar->Realize();
  m_Win->SetToolBar(m_TooBar);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateSidebar()
//----------------------------------------------------------------------------
{
  m_SideSash = new mmgSashPanel(m_Win, MENU_VIEW_SIDEBAR, wxRIGHT,280, "Side Bar \tCtrl+S"); // 280 is the width of the sideBar
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateTimebar()
//----------------------------------------------------------------------------
{
  m_TimeSash = new mmgSashPanel(m_Win,MENU_VIEW_TIMEBAR,wxBOTTOM,22,"Time Bar \tCtrl+T",false);
  m_TimePanel = new mmgTimeBar(m_TimeSash,-1,true);
  m_TimePanel->SetListener(this);
  m_TimeSash->Put(m_TimePanel);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::EnableItem(int item, bool enable)
//----------------------------------------------------------------------------
{
  if(m_MenuBar) 
     // must always check if a menu item exist because
     // during application shutdown it is not guaranteed
     if(m_MenuBar->FindItem(item))	
        m_MenuBar->Enable(item,enable );
  if(m_TooBar) 
     m_TooBar->EnableTool(item,enable );
}
