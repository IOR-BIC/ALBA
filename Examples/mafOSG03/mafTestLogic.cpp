/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTestLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2006-02-13 15:50:49 $
  Version:   $Revision: 1.1 $
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

#include "wx/utils.h"
#include "wx/datetime.h"
#include "wx/busyinfo.h"
#include "wx/notebook.h"
#include "wx/textctrl.h"

#include "mafWXLog.h"
#include "mafSideBar.h"
#include "mmgViewFrame.h"
#include "mmgFrame.h"
#include "mmgPicButton.h"
#include "mmgSplittedPanel.h"
#include "mmgSashPanel.h"
#include "mmgNamedPanel.h"
#include "mmgTimeBar.h"
#include "mmgFileHistory.h"

#include "mafPics.h"    // sostituisce il vecchio "mmgBitmaps.h"

#include "mmgGuiHolder.h"
#include "mafViewSettings.h"
#include "mafTestLogic.h"

#include "mafOsgCanvas.h"
//#include "Performer.h"
//#include "IniFiles.h"

//#include "mmgHTMLDialog.h"

/*
// Funzione di call-back per la ridirezione dello stdout di Performer
//----------------------------------------------------------------------------
void NotifyFunc(pfNotifyData *Data)
//----------------------------------------------------------------------------
{
  wxLogMessage(Data->emsg);
}
*/

//----------------------------------------------------------------------------
mafTestLogic::mafTestLogic()
//----------------------------------------------------------------------------
{
  m_menu_bar       = NULL;
  m_toolbar        = NULL;
  m_log_bar        = NULL;
  m_side_bar       = NULL;
  m_settings       = NULL;
  m_settings_panel = NULL;
  m_nb             = NULL; 
  m_canvas         = NULL;
  m_logger         = NULL;

  m_app_title = "mafOsg3";

  m_win = new mmgFrame(m_app_title , wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  //wxTextCtrl *foo = new wxTextCtrl(m_win,-1);  //?
  //m_win->Put(foo); //?

  m_config = wxConfigBase::Get();
}
//----------------------------------------------------------------------------
mafTestLogic::~mafTestLogic( ) 
//----------------------------------------------------------------------------
{
  StopLogging();
}
//----------------------------------------------------------------------------
void mafTestLogic::Show()
//----------------------------------------------------------------------------
{
  CreateMenu();
  CreateToolbar();
  CreateSidebar();
  CreateLogbar();
  StartLogging();

  m_canvas = new mafOsgCanvas(m_win);
  m_win->Put(m_canvas);
  m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void mafTestLogic::Init(int argc, char **argv)
//----------------------------------------------------------------------------
{
	if(argc > 1)
	{
		wxString file = argv[1];
		if(wxFileExists(file))
		{
			FileOpen(file);
		}
	}
}
//----------------------------------------------------------------------------
void mafTestLogic::CreateMenu()
//----------------------------------------------------------------------------
{
  m_menu_bar  = new wxMenuBar;
  wxMenu    *file_menu = new wxMenu;
  file_menu->Append(MENU_FILE_OPEN,  "&Open ..");
  file_menu->AppendSeparator();

	wxMenu  *menu_history = new wxMenu;
  file_menu->Append(0,"Recent Files",menu_history);
  file_menu->AppendSeparator();
  file_menu->Append(MENU_FILE_QUIT,  "&Quit");
  m_menu_bar->Append(file_menu, "&File");

  wxMenu    *view_menu = new wxMenu;
	view_menu->Append(CAMERA_RESET,     "Reset Camera \tCtrl+F");
  view_menu->AppendSeparator();
	view_menu->Append(MENU_VIEW_TOOLBAR, "Toolbar \tCtrl+T","",true);
  m_menu_bar->Append(view_menu, "&View");

  wxMenu    *option_menu = new wxMenu;
  m_menu_bar->Append(option_menu, "&Preferences");
	option_menu->Append(MENU_USER_START, "Test HTML","",true);

  m_win->SetMenuBar(m_menu_bar);
 	m_file_history.UseMenu(menu_history);
	m_file_history.Load(*m_config);
}
//----------------------------------------------------------------------------
void mafTestLogic::CreateToolbar()
//----------------------------------------------------------------------------
{
  m_toolbar = new wxToolBar(m_win,-1,wxPoint(0,0),wxSize(-1,-1),wxHORIZONTAL|wxNO_BORDER|wxTB_FLAT  );
  m_toolbar->SetMargins(0,0);
  m_toolbar->SetToolSeparation(0);

  //e' cambiata la sintassi per ottenere le immagini
  #include "open.xpm"
  #include "home.xpm"
  mafPics.Add("home",home_xpm);
  mafPics.Add("open",open_xpm);

  m_toolbar->SetToolBitmapSize(wxSize(24,24));
  m_toolbar->AddTool(MENU_FILE_OPEN, mafPics.GetBmp("open"),  "open file");
  //m_toolbar->AddSeparator();
  m_toolbar->AddTool(CAMERA_RESET,mafPics.GetBmp("home"),"reset camera to fit all (ctrl+f)");
  m_toolbar->Realize();

  m_win->SetToolBar(m_toolbar);
	m_menu_bar->FindItem(MENU_VIEW_TOOLBAR)->Check(true);
}
//----------------------------------------------------------------------------
void mafTestLogic::CreateSidebar()
//----------------------------------------------------------------------------
{
	m_side_bar = new mmgSashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,277, "Side Bar \tCtrl+S"); // 245 is the width of the sideBar

  m_nb = new wxNotebook(m_side_bar,-1);

  m_settings = new mafViewSettings(this);
  m_settings_panel = new mmgGuiHolder(m_nb,-1,false,true);
  m_settings_panel->SetTitle(" settings:");
  m_settings_panel->Put(m_settings->GetGui());
  m_nb->AddPage(m_settings_panel,"settings",true);

  m_side_bar->Put(m_nb);
  m_side_bar->Show(true);
}
//----------------------------------------------------------------------------
void mafTestLogic::CreateLogbar()
//----------------------------------------------------------------------------
{
  m_log_bar = new mmgSashPanel(m_win, MENU_VIEW_LOGBAR, wxBOTTOM,80,"Log Bar \tCtrl+L");

  wxNotebook *nb = new wxNotebook(m_log_bar,-1);

  wxTextCtrl *m_log  = new wxTextCtrl( nb, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
  wxLog *old_log = wxLog::SetActiveTarget( new wxLogTextCtrl( m_log ) );
  cppDEL(old_log);
  nb->AddPage(m_log,"log window",true);
  
  // modo veloce di redirezionare cout e cerr su un TextCtrl 
  // richiede di compilare wxWin con NO_TEXT_WINDOW_STREAM=0
  // vedere wx/include/textctrl.h
  // vedere anche wxStreamToTextRedirector

  //std::cout.rdbuf(m_log);
  //std::cerr.rdbuf(m_log);
  
  //mmgNamedPanel *log_panel = new mmgNamedPanel(m_win,-1,true);
  //log_panel->SetTitle(" Log Area:");
  //log_panel->Add(log,1,wxEXPAND);

  m_log_bar->Put(nb);
	m_log_bar->Show(true);
}
//----------------------------------------------------------------------------
void mafTestLogic::MenuFileOpen()
//----------------------------------------------------------------------------
{
	wxWindowDisabler disableAll;

  wxString  dir   = mafGetApplicationDirectory().c_str(); //adesso ritorna una std::string
	wxString  wildc = "OpenSceneGraph ascii file(*.osg)|*.osg| OpenSceneGraph binary file (*.ive)|*.osg" ;
  wxString  file  = mafGetOpenFile(dir, wildc).c_str(); //adesso ritorna una std::string
  if(file != "")  FileOpen(file);
}
//----------------------------------------------------------------------------
void mafTestLogic::FileOpen(wxString filename)
//----------------------------------------------------------------------------
{
	wxWindowDisabler disableAll;
  wxBusyCursor wait_cursor;
  
  if(!::wxFileExists(filename))
	{
		wxString msg("File ");
		msg += filename;
		msg += " not found!";
		wxMessageBox( msg,"Warning", wxOK|wxICON_WARNING , NULL);
		return;
	}

  m_canvas->LoadModel(filename);

  /*
  wxString path, name, ext, file_dir;
	wxSplitPath(filename,&path,&name,&ext);
  wxString inifile = path;
  inifile += INI_FILE;

  // do load
  pfFilePath(path);
  m_pf->PerformerLoadObject(filename.c_str());
  m_pf->PerformerInitSceneScan();
  PerformerReadIni(inifile);
  ReadPViewsData(inifile);
  
  TCameraData *Data;
  Data = new TCameraData;
  m_pf->PerformerRender(false);
  m_pf->PerformerGetCameraView(Data);
  m_settings->SetEngine(m_pf);  
  m_settings->LoadEditsFromCamera(Data);
  m_settings->LoadEditsFromFog();
  delete Data;
  m_settings_panel->Put(m_settings->GetGui());

  m_points->SetEngine(m_pf);
  m_points_panel->Put(m_points->GetGui());

  if (m_pf->pf_SwitchNum > 0)
  {
    m_switches->SetEngine(m_pf); //SIL. 11-11-2004: --- reads switches as a side effect
    m_switches_panel->Put(m_switches->GetGui());
  }
  else
    nb->DeletePage(nb->GetPageCount() - 2);

  if (m_pf->pf_NumStory > 0)
  {
    m_evolution->SetEngine(m_pf);
    m_evolution_panel->Put(m_evolution->GetGui());
  }
  else
    nb->DeletePage(nb->GetPageCount() - 1);
  */

  m_file_history.AddFileToHistory(filename);
	m_file_history.Save(*m_config);

  // update frame title
  wxString path, name, ext, file_dir;
  wxSplitPath(filename,&path,&name,&ext);

  wxString title;
	title = m_app_title;
  title += ": " + name;
	m_win->SetTitle(title);
}
//----------------------------------------------------------------------------
void mafTestLogic::HistoryFileOpen(int menuId)
//----------------------------------------------------------------------------
{
  int idx = menuId - wxID_FILE1;
	wxString file = "";
	file = m_file_history.GetHistoryFile(idx);
  FileOpen(file);
}
//----------------------------------------------------------------------------
void mafTestLogic::OnQuit()
//----------------------------------------------------------------------------
{
  //prevent nested OnQuit
  static bool quitting = false;
  if(quitting) return;  
  quitting = true;  

  /*
  //ask confirmation
	int answer = wxMessageBox("quit program?", "Confirm", wxYES_NO | wxICON_QUESTION , m_win);
	bool quit = answer == wxYES;
	if(!quit) 
  {
    quitting = false;
    return;
  }
  */

  // destroy the Log
  delete wxLog::SetActiveTarget(NULL); 

  // destroy the config
  cppDEL(m_config);

  // destroy UI components
  cppDEL(m_settings);
  cppDEL(m_log_bar);
  cppDEL(m_side_bar);
  cppDEL(m_canvas);
  
  //LAST - destroy the Main Window
  m_win->Destroy();
  
  quitting = false;
}
//----------------------------------------------------------------------------
void mafTestLogic::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event)) //SIL - adesso si deve fare un cast dell'evento
  {
    switch(e->GetId())
    {
      case MENU_FILE_OPEN:
		    MenuFileOpen();
	    break; 
	    case wxID_FILE1:
	    case wxID_FILE2:
	    case wxID_FILE3:
	    case wxID_FILE4:
		    HistoryFileOpen(e->GetId());
	    break;
      case MENU_FILE_QUIT:
		    OnQuit();		
      break; 
      case MENU_VIEW_TOOLBAR:
      {
	      bool show = !m_toolbar->IsShown();
	      m_toolbar->Show(show);
	      m_menu_bar->FindItem(MENU_VIEW_TOOLBAR)->Check(show);
	      m_win->Update();
      }
      break; 
      case MENU_VIEW_LOGBAR:
        m_log_bar->Show(!m_log_bar->IsShown());
      break; 
      case MENU_VIEW_SIDEBAR:
        m_side_bar->Show(!m_side_bar->IsShown());
      break; 
      case MENU_USER_START:
        //Test(); 
      break; 
      case CAMERA_RESET:
        m_canvas->Home();
      break;
      default:
        e->Log();
      break; 
    }
  }
}
//-----------------------------------------------------------------------------
void mafTestLogic::StartLogging()
//-----------------------------------------------------------------------------
{
  // open the logfile
  std::string filename = mafGetApplicationDirectory();
  filename += "\\";
  filename += m_app_title;
  filename += ".txt";
  wxLogMessage("also logging to: %s",filename.c_str() );
  
  std::freopen(filename.c_str(), "w", stdout);
  std::freopen(filename.c_str(), "w", stderr);
  
  /*  
  // open the logfile
  std::string filename = mafGetApplicationDirectory();
  filename += "\\";
  filename += m_app_title;
  filename += ".txt";

  m_logfile.open(filename.c_str()); 
  wxLogMessage("also logging to: %s",filename.c_str() );

  // store previous buffer
  m_stdout = std::cout.rdbuf();
  m_stderr = std::cerr.rdbuf();

  //redirect
  std::cout.rdbuf(m_logfile.rdbuf());
  std::cerr.rdbuf(m_logfile.rdbuf());
  osg::setNotifyLevel(osg::DEBUG_FP);

  */
}
//-----------------------------------------------------------------------------
void mafTestLogic::StopLogging()
//-----------------------------------------------------------------------------
{
  std::fclose(stdout);
  std::fclose(stderr);

  /*
  assert(m_stdout && m_stderr); // Ensure StartLogging has been called
  m_logfile.close(); 
  std::cout.rdbuf(m_stdout);
  std::cerr.rdbuf(m_stderr);
  */
}
