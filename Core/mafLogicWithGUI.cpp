/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLogicWithGUI.cpp,v $
  Language:  C++
  Date:      $Date: 2007-10-26 08:10:57 $
  Version:   $Revision: 1.40 $
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
#include "mmgNamedPanel.h"
#include "mmgCrossSplitter.h"
#include "mmgTimeBar.h"
#include "mmgLocaleSettings.h"
#include "mmgMeasureUnitSettings.h"
#include "mmgApplicationSettings.h"
#include "mafGUISettingsStorage.h"
#include "mafGUISettingsTimeBar.h"

#include "mafWXLog.h"
#include "mafPics.h"
#ifdef MAF_USE_VTK
  #include "mafVTKLog.h"
#endif
//----------------------------------------------------------------------------
mafLogicWithGUI::mafLogicWithGUI()
//----------------------------------------------------------------------------
{
  m_Win = new mmgMDIFrame("maf", wxDefaultPosition, wxSize(800, 600));
  m_Win->SetListener(this);

  m_ChildFrameStyle = wxCAPTION | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxTHICK_FRAME; // Default style
  m_LocaleSettings = new mmgLocaleSettings(this);
  m_MeasureUnitSettings = new mmgMeasureUnitSettings(this);
  m_ApplicationSettings = new mmgApplicationSettings(this);
  m_StorageSettings = new mafGUISettingsStorage(this);
  m_TimeBarSettings = new mafGUISettingsTimeBar(this);

  m_ToolBar       = NULL;
  m_MenuBar       = NULL;

	m_LogToFile			= m_ApplicationSettings->GetLogToFileStatus();
	m_LogAllEvents	= m_ApplicationSettings->GetLogVerboseStatus();
	m_Logger				= NULL;
  m_VtkLog        = NULL;

	m_AppTitle      = "";

	m_PlugMenu		  = true;
	m_PlugToolbar	  = true;
	m_PlugSidebar	  = true;
  m_SidebarStyle  = mafSideBar::DOUBLE_NOTEBOOK;
	m_PlugTimebar	  = true;
	m_PlugLogbar	  = true;
}
//----------------------------------------------------------------------------
mafLogicWithGUI::~mafLogicWithGUI()
//----------------------------------------------------------------------------
{
  cppDEL(m_LocaleSettings);
  cppDEL(m_MeasureUnitSettings);
  cppDEL(m_ApplicationSettings);
  cppDEL(m_StorageSettings);
  cppDEL(m_TimeBarSettings);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::SetParentFrameStyle(long style)
//----------------------------------------------------------------------------
{
  m_Win->SetWindowStyleFlag(style);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::Configure()
//----------------------------------------------------------------------------
{
  if(m_PlugMenu)		this->CreateMenu();
  if(m_PlugToolbar) this->CreateToolbar();
  if(m_PlugTimebar) this->CreateTimebar(); //SIL. 23-may-2006 : 
  if(m_PlugLogbar)	this->CreateLogbar();	else this->CreateNullLog();
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::Show()
//----------------------------------------------------------------------------
{
  m_AppTitle = m_Win->GetTitle().c_str();
	m_Win->Show(TRUE);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::ShowSplashScreen()
//----------------------------------------------------------------------------
{
  wxBitmap splashImage = mafPics.GetBmp("SPLASH_SCREEN");
  ShowSplashScreen(splashImage);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::ShowSplashScreen(wxBitmap &splashImage)
//----------------------------------------------------------------------------
{
  long splash_style = wxSIMPLE_BORDER | wxSTAY_ON_TOP;
  wxSplashScreen* splash = new wxSplashScreen(splashImage,
    wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
    4000, NULL, -1, wxDefaultPosition, wxDefaultSize,
    splash_style);
  mafYield();
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateMenu()
//----------------------------------------------------------------------------
{
  m_MenuBar  = new wxMenuBar;
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(MENU_FILE_QUIT,  _("&Quit"));
  m_MenuBar->Append(file_menu, _("&File"));
  wxMenu *edit_menu = new wxMenu;
  m_MenuBar->Append(edit_menu, _("&Edit"));
  wxMenu *view_menu = new wxMenu;
  if(this->m_PlugToolbar) 
  {
    view_menu->Append(MENU_VIEW_TOOLBAR, _("Toolbar"),"",wxITEM_CHECK);
    view_menu->Check(MENU_VIEW_TOOLBAR,true);
  }
  m_MenuBar->Append(view_menu, _("&View"));

  m_Win->SetMenuBar(m_MenuBar);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateNullLog()
//----------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  m_VtkLog = mafVTKLog::New();
  m_VtkLog->SetInstance(m_VtkLog);
#endif  
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
      // commands related to the Dockable Panes
    case MENU_VIEW_LOGBAR:
        m_Win->ShowDockPane("logbar",!m_Win->DockPaneIsShown("logbar") );
    break; 
    case MENU_VIEW_SIDEBAR:
      m_Win->ShowDockPane("sidebar",!m_Win->DockPaneIsShown("sidebar") );
    break; 
    case MENU_VIEW_TIMEBAR:
      m_Win->ShowDockPane("timebar",!m_Win->DockPaneIsShown("timebar") );
    break; 
    case MENU_VIEW_TOOLBAR:
      m_Win->ShowDockPane("toolbar",!m_Win->DockPaneIsShown("toolbar") );
    break; 
      // ###############################################################
      // commands related to the STATUSBAR
    case BIND_TO_PROGRESSBAR:
#ifdef MAF_USE_VTK
      m_Win->BindToProgressBar(e->GetVtkObj());
#endif
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
  // if OnQuit is redefined in a derived class,  mafLogicWithGUI::OnQuit() must be clalled last

  mafYield();
  if(m_PlugLogbar) delete wxLog::SetActiveTarget(NULL);
#ifdef MAF_USE_VTK 
  vtkDEL(m_VtkLog);
#endif
  m_Win->Destroy();
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateLogbar()
//----------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  m_VtkLog = mafVTKLog::New();
  m_VtkLog->SetInstance(m_VtkLog);
#endif
  wxTextCtrl *log  = new wxTextCtrl( m_Win, MENU_VIEW_LOGBAR, "", wxPoint(0,0), wxSize(100,300), /*wxNO_BORDER |*/ wxTE_MULTILINE );
  m_Logger = new mafWXLog(log);
  m_Logger->LogToFile(m_LogToFile);
  if(m_LogToFile)
  {
    wxString s = m_ApplicationSettings->GetLogFolder().GetCStr();
    wxDateTime log_time = wxDateTime::Now();
    s += "\\";
    s += m_Win->GetTitle();
    s += wxString::Format("_%02d_%02d_%d_%02d_%2d",log_time.GetYear(),log_time.GetMonth() + 1,log_time.GetDay(),log_time.GetHour(),log_time.GetMinute());
    s += ".log";
    m_Logger->SetFileName(s);
  }
  m_Logger->SetVerbose(m_LogAllEvents);

  wxLog *old_log = wxLog::SetActiveTarget( m_Logger );
  cppDEL(old_log);

  m_Win->AddDockPane(log, wxPaneInfo()
    .Name("logbar")
    .Caption(wxT("LogBar"))
    .Bottom()
    .Layer(0)
    .MinSize(100,10)
    .TopDockable(false) // prevent docking on top side - otherwise may dock also beside the toolbar -- and it's hugely
  );
  
  mafLogMessage(_("welcome"));
}

//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateToolbar()
//----------------------------------------------------------------------------
{
  m_ToolBar = new wxToolBar(m_Win,-1,wxPoint(0,0),wxSize(-1,-1),wxHORIZONTAL|wxNO_BORDER|wxTB_FLAT  );
  m_ToolBar->SetMargins(0,0);
  m_ToolBar->SetToolSeparation(2);
  m_ToolBar->SetToolBitmapSize(wxSize(20,20));
  m_ToolBar->AddTool(MENU_FILE_NEW,mafPics.GetBmp("FILE_NEW"),    _("new msf storage file"));
  m_ToolBar->AddTool(MENU_FILE_OPEN,mafPics.GetBmp("FILE_OPEN"),  _("open msf storage file"));
  m_ToolBar->AddTool(MENU_FILE_SAVE,mafPics.GetBmp("FILE_SAVE"),  _("save current msf storage file"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(OP_UNDO,mafPics.GetBmp("OP_UNDO"),  _("undo (ctrl+z)")); //correggere tooltip - shortcut sbagliati
  m_ToolBar->AddTool(OP_REDO,mafPics.GetBmp("OP_REDO"),  _("redo (ctrl+shift+z)"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(OP_CUT,  mafPics.GetBmp("OP_CUT"),  _("cut selected vme (ctrl+x)"));
  m_ToolBar->AddTool(OP_COPY, mafPics.GetBmp("OP_COPY"), _("copy selected vme (ctrl+c)"));
  m_ToolBar->AddTool(OP_PASTE,mafPics.GetBmp("OP_PASTE"),_("paste vme (ctrl+v)"));
  m_ToolBar->AddSeparator();
  m_ToolBar->AddTool(CAMERA_RESET,mafPics.GetBmp("ZOOM_ALL"),_("reset camera to fit all (ctrl+f)"));
  m_ToolBar->AddTool(CAMERA_FIT,  mafPics.GetBmp("ZOOM_SEL"),_("reset camera to fit selected object (ctrl+shift+f)"));
  m_ToolBar->AddTool(CAMERA_FLYTO,mafPics.GetBmp("FLYTO"),_("fly to object under mouse (press f inside a 3Dview)"));
  m_ToolBar->Realize();
  m_Win->SetToolBar(m_ToolBar);

  EnableItem(CAMERA_RESET, false);
  EnableItem(CAMERA_FIT,   false);
  EnableItem(CAMERA_FLYTO, false);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateTimebar()
//----------------------------------------------------------------------------
{
  m_TimePanel = new mmgTimeBar(m_Win,MENU_VIEW_TIMEBAR,true);
  m_TimePanel->SetListener(this);

  // Events coming from settings are forwarded to the time bar.
  m_TimePanel->SetTimeSettings(m_TimeBarSettings);
  m_TimeBarSettings->SetListener(m_TimePanel);

  m_Win->AddDockPane(m_TimePanel, wxPaneInfo()
    .Name("timebar")
    .Caption(wxT("TimeBar"))
    .Bottom()
    .Row(1)
    .Layer(2)
    .ToolbarPane()
    .LeftDockable(false)
    .RightDockable(false)
    .MinSize(100,22)
    .Floatable(false)
    .Gripper(false)
    .Resizable(false)
    .Movable(false)
    );
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
  if(m_ToolBar)
     m_ToolBar->EnableTool(item,enable );
}
