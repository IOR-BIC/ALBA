/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLogicWithManagers.cpp,v $
  Language:  C++
  Date:      $Date: 2006-11-28 14:41:34 $
  Version:   $Revision: 1.86 $
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


#include "mafLogicWithManagers.h"
#include <wx/config.h>

#include "mafView.h"

#include "mafViewManager.h"
#include "mafOp.h"
#include "mafOpManager.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafPrintSupport.h"

#ifdef MAF_USE_VTK
  #include "mafViewVTK.h"
  
  #include "mmoVTKImporter.h"
  #include "mmoSTLImporter.h"
  #include "mafInteractionManager.h"
  #include "mafInteractor.h"
  #include "mafDeviceManager.h"
  #include "mafAction.h"
  #include "mmdMouse.h"
  #include "mmdClientMAF.h"
  #include "mmiPER.h"
  #include "mmiPER.h"
  #include "mmgTreeContextualMenu.h"
  #include "mmgContextualMenu.h"
  #include "vtkCamera.h"
#endif

#include "mafSideBar.h"

#include "mmgDialogRemoteFile.h"
#include "mmgMDIFrame.h"
#include "mmgMDIChild.h"
#include "mmgCheckTree.h"
#include "mmgTimeBar.h"
#include "mmgMaterialChooser.h"
#include "mmgViewFrame.h"
#include "mmgLocaleSettings.h"
#include "mmgMeasureUnitSettings.h"
#include "mmgApplicationSettings.h"
#include "mmgApplicationLayoutSettings.h"
#include "mafRemoteLogic.h"
#include "mmgSettingsDialog.h"

#ifdef WIN32
  #include "mmdClientMAF.h"
#endif

#include "mmaApplicationLayout.h"

#include "mafVMEStorage.h"
#include "mafRemoteStorage.h"
//#include "vtkCamera.h"


//----------------------------------------------------------------------------
mafLogicWithManagers::mafLogicWithManagers()
: mafLogicWithGUI()
//----------------------------------------------------------------------------
{
  m_SideBar     = NULL;
  m_UseVMEManager  = true;
  m_UseViewManager = true;
  m_UseOpManager   = true;
  m_UseInteractionManager = true;
  
  m_ExternalViewFlag  = false;

  m_CameraLinkingObserverFlag = false;
  
  m_VMEManager  = NULL;
  m_ViewManager = NULL;
  m_OpManager   = NULL;
  m_InteractionManager = NULL;
  m_RemoteLogic = NULL;

  m_ImportMenu  = NULL; 
  m_ExportMenu  = NULL; 
  m_OpMenu      = NULL;
  m_ViewMenu    = NULL; 
  m_RecentFileMenu = NULL;

  m_MaterialChooser = NULL;

  // this is needed to manage events coming from the widget
  // when the user change the unit settings.
  m_MeasureUnitSettings->SetListener(this);

  m_PrintSupport = new mafPrintSupport();
  
  m_SettingsDialog = new mmgSettingsDialog();

  m_ApplicationLayoutSettings = NULL;

	m_Revision = _("0.1");
}
//----------------------------------------------------------------------------
mafLogicWithManagers::~mafLogicWithManagers()
//----------------------------------------------------------------------------
{
  // Managers are destruct in the OnClose 
  cppDEL(m_ApplicationLayoutSettings);
  cppDEL(m_PrintSupport);
  cppDEL(m_SettingsDialog); 
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Configure()
//----------------------------------------------------------------------------
{
  mafLogicWithGUI::Configure(); // create the GUI - and calls CreateMenu

  if(this->m_PlugSidebar)
  {
    m_SideBar = new mafSideBar(m_Win,MENU_VIEW_SIDEBAR,this,m_SidebarStyle);
    m_Win->AddDockPane(m_SideBar->m_Notebook , wxPaneInfo()
      .Name("sidebar")
      .Caption(wxT("ControlBar"))
      .Right()
      .Layer(2)
      .MinSize(240,450)
      .TopDockable(false)
      .BottomDockable(false)
      );
  }

  if(m_UseVMEManager)
  {
    m_VMEManager = new mafVMEManager();
    m_VMEManager->SetListener(this); 
  }

// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if (m_UseInteractionManager)
  {
    m_InteractionManager = new mafInteractionManager();
    m_InteractionManager->SetListener(this);

    m_Mouse = m_InteractionManager->GetMouseDevice();
    //SIL m_InteractionManager->GetClientDevice()->AddObserver(this, MCH_INPUT);
  }
#endif

  if(m_UseViewManager)
  {
    m_ViewManager = new mafViewManager();
    m_ViewManager->SetListener(this);
    m_ViewManager->SetMouse(m_Mouse);
  }

  if(m_UseOpManager)
  {
    m_OpManager = new mafOpManager();
    m_OpManager->SetListener(this);
    m_OpManager->SetMouse(m_Mouse);
  }
  
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if (m_UseInteractionManager && m_UseViewManager && m_UseOpManager)
  {
#ifdef __WIN32__
    m_RemoteLogic = new mafRemoteLogic(this, m_ViewManager, m_OpManager);

    m_RemoteLogic->SetClientUnit(m_InteractionManager->GetClientDevice());
#endif

  }
#endif

  // Fill the SettingsDialog
  m_SettingsDialog->AddPage( m_ApplicationSettings->GetGui(), _("Application Settings"));

  if (m_ViewManager)
  {
    m_ApplicationLayoutSettings = new mmgApplicationLayoutSettings(this);
    m_ApplicationLayoutSettings->SetViewManager(m_ViewManager);
    m_ApplicationLayoutSettings->SetApplicationFrame(m_Win);
    m_SettingsDialog->AddPage( m_ApplicationLayoutSettings->GetGui(), _("Application Layout"));
  }
  m_SettingsDialog->AddPage( m_Win->GetDockSettingGui(), _("User Interface Preferences"));

// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if(m_InteractionManager)
    m_SettingsDialog->AddPage(m_InteractionManager->GetGui(), _("Interaction Manager"));
#endif    
  if(m_LocaleSettings)
    m_SettingsDialog->AddPage(m_LocaleSettings->GetGui(), _("Interface language"));

  if (m_MeasureUnitSettings)
    m_SettingsDialog->AddPage(m_MeasureUnitSettings->GetGui(), _("Measure Unit"));
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Plug(mafView* view) 
//----------------------------------------------------------------------------
{
  if(m_ViewManager) 
    m_ViewManager->ViewAdd(view);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Plug(mafOp *op, wxString menuPath)
//----------------------------------------------------------------------------
{
  if(m_OpManager) 
  {
    m_OpManager->OpAdd(op, menuPath);
    
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK    
    if (m_InteractionManager)
    {
      if (const char **actions = op->GetActions())
      {
        const char *action;
        for (int i=0;action=actions[i];i++)
        {
          m_InteractionManager->AddAction(action);
        }
      }
    }
#endif
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Show()
//----------------------------------------------------------------------------
{
  if(m_VMEManager && m_RecentFileMenu)
  {
    m_VMEManager->SetFileHistoryMenu(m_RecentFileMenu);
  }

  if(m_UseViewManager && m_ViewMenu)
  {
    wxMenu *view_list = new wxMenu;
    m_ViewMenu->AppendSeparator();
    m_ViewMenu->Append(0,_("Add View"),view_list);
    m_ViewManager->FillMenu(view_list);
  }

  if(m_OpManager)
  {
    if(m_MenuBar && (m_ImportMenu || m_OpMenu || m_ExportMenu))
    {
      m_OpManager->FillMenu(m_ImportMenu, m_ExportMenu, m_OpMenu);
      m_OpManager->SetMenubar(m_MenuBar);
    }
    if(m_ToolBar)
      m_OpManager->SetToolbar(m_ToolBar);
  }

  mafLogicWithGUI::Show();

  // must be after the mafLogicWithGUI::Show(); because in that method is set the m_AppTitle var
  SetApplicationStamp(m_AppTitle);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::SetApplicationStamp(mafString &app_stamp)
//----------------------------------------------------------------------------
{
  if (m_VMEManager)
  {
    m_VMEManager->SetApplicationStamp(app_stamp);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Init(int argc, char **argv)
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
  {
    if(argc > 1 )
	  {
		  wxString file = argv[1];
		  if(wxFileExists(file))
		  {
			  m_VMEManager->MSFOpen(file);
			  UpdateFrameTitle();
		  }
      else
      {
        m_VMEManager->MSFNew();
      }
	  }
    else
    {
      m_VMEManager->MSFNew();
    }
  }
  if (m_OpManager)
  {
    if(argc > 1 )
    {
      mafString op_type = argv[1];
      mafString op_param = argv[2];
      for (int p = 3; p < argc; p++)
      {
        op_param += " ";
        op_param += argv[p];
      }
      m_OpManager->OpRun(op_type, (void *)op_param.GetCStr());
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateMenu()
//----------------------------------------------------------------------------
{
  m_MenuBar  = new wxMenuBar;
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(MENU_FILE_NEW,   _("&New  \tCtrl+N"));
  file_menu->Append(MENU_FILE_OPEN,  _("&Open   \tCtrl+O"));
  file_menu->Append(MENU_FILE_SAVE,  _("&Save  \tCtrl+S"));
  file_menu->Append(MENU_FILE_SAVEAS,_("Save &As  \tCtrl+Shift+S"));
  if (m_ApplicationSettings->UseRemoteStorage())
  {
    //file_menu->Append(MENU_FILE_UPLOAD, _("&Upload"));
  }
  m_ImportMenu = new wxMenu;
  file_menu->AppendSeparator();
  file_menu->Append(0,_("Import"),m_ImportMenu );

  m_ExportMenu = new wxMenu;
  file_menu->Append(0,_("Export"),m_ExportMenu);

  // Print menu item
  file_menu->AppendSeparator();
  file_menu->Append(MENU_FILE_PRINT, _("&Print  \tCtrl+P"));
  file_menu->Append(MENU_FILE_PRINT_PREVIEW, _("Print Preview"));
  file_menu->Append(MENU_FILE_PRINT_SETUP, _("Printer Setup"));
  file_menu->Append(MENU_FILE_PRINT_PAGE_SETUP, _("Page Setup"));

  m_RecentFileMenu = new wxMenu;
  file_menu->AppendSeparator();
  file_menu->Append(0,_("Recent Files"),m_RecentFileMenu);

  file_menu->AppendSeparator();
  file_menu->Append(MENU_FILE_QUIT,  _("&Quit  \tCtrl+Q"));

  m_MenuBar->Append(file_menu, _("&File"));

  wxMenu    *edit_menu = new wxMenu;
  edit_menu->Append(OP_UNDO,   _("Undo  \tCtrl+Z"));
  edit_menu->Append(OP_REDO,   _("Redo  \tCtrl+Shift+Z"));
  edit_menu->AppendSeparator();
  edit_menu->Append(OP_DELETE, _("Delete"));
  edit_menu->Append(OP_CUT,   _("Cut   \tCtrl+Shift+X"));
  edit_menu->Append(OP_COPY,  _("Copy  \tCtrl+Shift+C"));
  edit_menu->Append(OP_PASTE, _("Paste \tCtrl+Shift+V"));
  m_MenuBar->Append(edit_menu, _("&Edit"));

  m_ViewMenu = new wxMenu;
  m_MenuBar->Append(m_ViewMenu, _("&View"));

  m_OpMenu = new wxMenu;
  m_MenuBar->Append(m_OpMenu, _("&Operations"));

  wxMenu    *option_menu = new wxMenu;
  option_menu->Append(ID_APP_SETTINGS, _("Options..."));
  m_MenuBar->Append(option_menu, _("Tools"));

	wxMenu    *help_menu = new wxMenu;
	help_menu->Append(ABOUT_APPLICATION,_("About"));
  m_MenuBar->Append(help_menu, _("&Help"));

  m_Win->SetMenuBar(m_MenuBar);

  EnableItem(MENU_FILE_PRINT, false);
  EnableItem(MENU_FILE_PRINT_PREVIEW, false);
  EnableItem(MENU_FILE_PRINT_SETUP, false);
  EnableItem(MENU_FILE_PRINT_PAGE_SETUP, false);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateToolbar()
//----------------------------------------------------------------------------
{
  
  //m_ToolBar = new wxToolBar(m_Win,-1,wxPoint(0,0),wxSize(-1,-1),wxHORIZONTAL|wxNO_BORDER|wxTB_FLAT  );
  m_ToolBar = new wxToolBar(m_Win,MENU_VIEW_TOOLBAR,wxPoint(0,0),wxSize(-1,-1),wxTB_FLAT | wxTB_NODIVIDER );
  m_ToolBar->SetMargins(0,0);
  m_ToolBar->SetToolSeparation(2);
  m_ToolBar->SetToolBitmapSize(wxSize(20,20));
  m_ToolBar->AddTool(MENU_FILE_NEW,mafPics.GetBmp("FILE_NEW"),    _("new msf storage file"));
  m_ToolBar->AddTool(MENU_FILE_OPEN,mafPics.GetBmp("FILE_OPEN"),  _("open msf storage file"));
  m_ToolBar->AddTool(MENU_FILE_SAVE,mafPics.GetBmp("FILE_SAVE"),  _("save current msf storage file"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(MENU_FILE_PRINT,mafPics.GetBmp("PRINT"),  _("print the selected view"));
  m_ToolBar->AddTool(MENU_FILE_PRINT_PREVIEW,mafPics.GetBmp("PRINT_PREVIEW"),  _("show the print preview for the selected view"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(OP_UNDO,mafPics.GetBmp("OP_UNDO"),  _("undo (ctrl+z)"));
  m_ToolBar->AddTool(OP_REDO,mafPics.GetBmp("OP_REDO"),  _("redo (ctrl+shift+z)"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(OP_CUT,  mafPics.GetBmp("OP_CUT"),  _("cut selected vme (ctrl+x)"));
  m_ToolBar->AddTool(OP_COPY, mafPics.GetBmp("OP_COPY"), _("copy selected vme (ctrl+c)"));
  m_ToolBar->AddTool(OP_PASTE,mafPics.GetBmp("OP_PASTE"),_("paste vme (ctrl+v)"));
  m_ToolBar->AddSeparator();
  m_ToolBar->AddTool(CAMERA_RESET,mafPics.GetBmp("ZOOM_ALL"),_("reset camera to fit all (ctrl+f)"));
  m_ToolBar->AddTool(CAMERA_FIT,  mafPics.GetBmp("ZOOM_SEL"),_("reset camera to fit selected object (ctrl+shift+f)"));
  m_ToolBar->AddTool(CAMERA_FLYTO,mafPics.GetBmp("FLYTO"),_("fly to object under mouse"));


  EnableItem(CAMERA_RESET, false);
  EnableItem(CAMERA_FIT,   false);
  EnableItem(CAMERA_FLYTO, false);
  EnableItem(MENU_FILE_PRINT, false);
  EnableItem(MENU_FILE_PRINT_PREVIEW, false);

  m_ToolBar->Realize();

  //SIL. 23-may-2006 : 
  m_Win->AddDockPane(m_ToolBar,  wxPaneInfo()
    .Name("toolbar")
    .Caption(wxT("ToolBar"))
    .Top()
    .Layer(2)
    .ToolbarPane()
    .LeftDockable(false)
    .RightDockable(false)
    .Floatable(false)
    .Movable(false)
    .Gripper(false)
    );
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::UpdateFrameTitle()
//----------------------------------------------------------------------------
{
  wxString title(m_AppTitle);
  title += "   " + m_VMEManager->GetFileName();
  m_Win->SetTitle(title);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      // ###############################################################
      // commands related to FILE MENU  
      case MENU_FILE_NEW:
        OnFileNew();
      break; 
      case MENU_FILE_OPEN:
      {
        mafString *filename = e->GetString();
        if(filename)
          OnFileOpen((*filename).GetCStr());
        else
          OnFileOpen();
        UpdateFrameTitle();
      }
      break; 
      case IMPORT_FILE:
      {
        mafString *filename = e->GetString();
        if(filename)
        {
          ImportExternalFile(*filename);
        }
      }
      break;
      case wxID_FILE1:
      case wxID_FILE2:
      case wxID_FILE3:
      case wxID_FILE4:
      case wxID_FILE5:
      case wxID_FILE6:
      case wxID_FILE7:
      case wxID_FILE8:
      case wxID_FILE9:
        OnFileHistory(e->GetId());
      break;
      case MENU_FILE_SAVE:
        OnFileSave();
      break; 
      case MENU_FILE_SAVEAS:
        OnFileSaveAs();
      break;
      case MENU_FILE_UPLOAD:
      {
        /*  Re-think about it!!
        wxString remote_path = "";
        wxString remote_file = "";
        bool valid_dir = false;
        wxString msg = _("Insert remote directory on remote host: ") + m_ApplicationSettings->GetRemoteHostName();
        do 
        {
          remote_path = wxGetTextFromUser(msg,_("Remote directory choose"),"/mafstorage/pub/");
          if (!remote_path.IsEmpty())
          {
            remote_file = m_ApplicationSettings->GetRemoteHostName();
            remote_file << remote_path;
          }
          else
            return;
          wxString dir_check = remote_file[remote_file.Length()-1];
          valid_dir = dir_check.IsSameAs("/") || dir_check.IsSameAs("\\");
          if (!valid_dir)
          {
            wxMessageBox(_("Not valid path!! It should ends with '/'"), _("Warning"));
          }
        } while(!valid_dir);

        // for now upload the entire msf
        remote_file += wxFileNameFromPath(m_VMEManager->GetFileName());
        OnFileUpload(remote_file.c_str());
        */
      }
      break;
      case MENU_FILE_PRINT:
        if (m_ViewManager && m_PrintSupport)
          m_PrintSupport->OnPrint(m_ViewManager->GetSelectedView());
      break;
      case MENU_FILE_PRINT_PREVIEW:
        if (m_ViewManager && m_PrintSupport)
          m_PrintSupport->OnPrintPreview(m_ViewManager->GetSelectedView());
      break;
      case MENU_FILE_PRINT_SETUP:
        if (m_PrintSupport)
          m_PrintSupport->OnPrintSetup();
      break;
      case MENU_FILE_PRINT_PAGE_SETUP:
        if (m_PrintSupport)
          m_PrintSupport->OnPageSetup();
      break;
      case MENU_FILE_QUIT:
        OnQuit();		
      break; 
        // ###############################################################
        // commands related to VME
      case VME_SELECT:	
        VmeSelect(*e);		
      break; 
      case VME_SELECTED: 
        VmeSelected(e->GetVme());
      break;
      case VME_SHOW:
        VmeShow(e->GetVme(), e->GetBool());
      break;
      case VME_MODIFIED:
        VmeModified(e->GetVme());
      break; 
      case VME_ADD:
        VmeAdd(e->GetVme());
      break; 
      case VME_ADDED:
        VmeAdded(e->GetVme());
      break; 
      case VME_REMOVE:
        VmeRemove(e->GetVme());
      break; 
      case VME_REMOVING:
        VmeRemoving(e->GetVme());
      break; 
      case VME_CHOOSE:
        {
          mafString *s = e->GetString();
          if(s != NULL)
            e->SetVme(VmeChoose(e->GetArg(), e->GetBool(), *s));
          else
            e->SetVme(VmeChoose(e->GetArg(), e->GetBool()));
        }
      break;
      case VME_CHOOSE_MATERIAL:
        VmeChooseMaterial((mafVME *)e->GetVme(), e->GetBool());
      break;
      case SHOW_CONTEXTUAL_MENU:
        if (e->GetSender() == m_SideBar->GetTree())
          TreeContextualMenu(*e);
        else
          ViewContextualMenu(e->GetBool());
      break;
        // ###############################################################
        // commands related to OP
      case MENU_OP:
        if(m_OpManager) 
        {
          m_OpManager->OpRun(e->GetArg());
          if(m_RemoteLogic && m_RemoteLogic->IsSocketConnected() && !m_OpManager->m_FromRemote)
          {
            mafEvent re(this, mafOpManager::RUN_OPERATION_EVENT, e->GetArg());
            re.SetChannel(REMOTE_COMMAND_CHANNEL);
            m_RemoteLogic->OnEvent(&re);
          }
        }
      break;
      case OP_RUN_STARTING:
      {
        mmgMDIChild *c = (mmgMDIChild *)m_Win->GetActiveChild();
        if (c != NULL)
          c->SetAllowCloseWindow(false);
        OpRunStarting();
      }
      break; 
      case OP_RUN_TERMINATED:
      {
        mmgMDIChild *c = (mmgMDIChild *)m_Win->GetActiveChild();
        if (c != NULL)
          c->SetAllowCloseWindow(true);
        OpRunTerminated();
      }
      break; 
      case OP_SHOW_GUI:
        OpShowGui(!e->GetBool(), (mmgPanel*)e->GetWin());
      break; 
      case OP_HIDE_GUI:
        OpHideGui(e->GetBool());
      break; 
      case OP_FORCE_STOP:
        m_OpManager->StopCurrentOperation();
      break;
        // ###############################################################
        // commands related to VIEWS
      case VIEW_CREATE:
        ViewCreate(e->GetArg());
      break;
      case VIEW_CREATED:
        ViewCreated(e->GetView());
      break;
      case VIEW_DELETE:
        if(m_PlugSidebar)
          this->m_SideBar->ViewDeleted(e->GetView()); // changed By Marco (is it correct?)
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
        if(m_InteractionManager)
          m_InteractionManager->ViewSelected(NULL);
#endif
        if (m_ViewManager)
        {
          EnableItem(CAMERA_RESET, false);
          EnableItem(CAMERA_FIT,   false);
          EnableItem(CAMERA_FLYTO, false);

          EnableItem(MENU_FILE_PRINT, false);
          EnableItem(MENU_FILE_PRINT_PREVIEW, false);
          EnableItem(MENU_FILE_PRINT_SETUP, false);
          EnableItem(MENU_FILE_PRINT_PAGE_SETUP, false);
        }
      break;	
      case VIEW_SELECT:
      {
        ViewSelect();
        if (m_OpManager)
        {
          mmgMDIChild *c = (mmgMDIChild *)m_Win->GetActiveChild();
          if (c != NULL)
            c->SetAllowCloseWindow(!m_OpManager->Running());
        }
      }
      break;
      case VIEW_MAXIMIZE:
        if (m_RemoteLogic && m_RemoteLogic->IsSocketConnected() && !m_ViewManager->m_FromRemote)
        {
          m_RemoteLogic->RemoteMessage(*e->GetString());
        }
      break;
      case VIEW_SELECTED:
        e->SetBool(m_ViewManager->GetSelectedView() != NULL);
      break;
      case VIEW_SAVE_IMAGE:
      {
        mafView *v = m_ViewManager->GetSelectedView();
        if (v)
          v->GetRWI()->SaveImage(v->GetLabel());
      }
      break;
      case LAYOUT_LOAD:
        RestoreLayout();
      break;
      case CAMERA_RESET:
        if(m_ViewManager) m_ViewManager->CameraReset();
      break; 
      case CAMERA_FIT:
        if(m_ViewManager) m_ViewManager->CameraReset(true);
      break;
      case CAMERA_FLYTO:
        if(m_ViewManager) m_ViewManager->CameraFlyToMode();
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
        if(m_InteractionManager) m_InteractionManager->CameraFlyToMode();  //modified by Marco. 15-9-2004 fly to with devices.
#endif
      break;
      case LINK_CAMERA_TO_INTERACTOR:
      {
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
        if (m_InteractionManager == NULL || m_InteractionManager->GetPER() == NULL)
          return;

        vtkCamera *cam = vtkCamera::SafeDownCast(e->GetVtkObj());
        bool link_camera = e->GetBool();
        if (!link_camera) 
        {
          if (cam) 
            m_InteractionManager->GetPER()->LinkCameraRemove(cam);
          else
            m_InteractionManager->GetPER()->LinkCameraRemoveAll();

          if (m_CameraLinkingObserverFlag) 
          {
            m_InteractionManager->GetPER()->GetCameraMouseInteractor()->RemoveObserver(this);
            m_CameraLinkingObserverFlag = false;
          }
        }
        else if (cam) 
        {
          if (!m_CameraLinkingObserverFlag) 
          {
            m_InteractionManager->GetPER()->GetCameraMouseInteractor()->AddObserver(this);
            m_CameraLinkingObserverFlag = true;
          }
          m_InteractionManager->GetPER()->LinkCameraAdd(cam);
        }
      }
#endif
      break;
      case TIME_SET:
        TimeSet(e->GetDouble());
      break;
      // ###############################################################
      // commands related to interaction manager
      case ID_APP_SETTINGS:
        m_SettingsDialog->ShowModal();
      break;
      case mmgMeasureUnitSettings::MEASURE_UNIT_UPDATED:
        UpdateMeasureUnit();
      break;
      case CAMERA_PRE_RESET:
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
        if(m_InteractionManager) 
        {
          vtkRenderer *ren = (vtkRenderer*)e->GetVtkObj();
          //assert(ren);
          m_InteractionManager->PreResetCamera(ren);
          //mafLogMessage("CAMERA_PRE_RESET");
        }
#endif
      break;
      case CAMERA_POST_RESET:
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
        if(m_InteractionManager) 
        {
          vtkRenderer *ren = (vtkRenderer*)e->GetVtkObj();
          //assert(ren); //modified by Marco. 2-11-2004 Commented out to allow reset camera of all cameras.
          m_InteractionManager->PostResetCamera(ren);
          //mafLogMessage("CAMERA_POST_RESET");
        }
#endif
      break;
      case CAMERA_UPDATE:
        if(m_ViewManager) m_ViewManager->CameraUpdate();
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
        if(m_InteractionManager) m_InteractionManager->CameraUpdate(e->GetView());
#endif
      break;
      case CAMERA_SYNCHRONOUS_UPDATE:     
        m_ViewManager->CameraUpdate();
      break;
      case INTERACTOR_ADD:
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
        if(m_InteractionManager)
        {
          mafInteractor *interactor = mafInteractor::SafeDownCast(e->GetMafObject());
          assert(interactor);
          mafString *action_name = e->GetString();
          m_InteractionManager->BindAction(*action_name,interactor);
        }
#endif
      break;
      case INTERACTOR_REMOVE:
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
        if(m_InteractionManager) 
        {
          mafInteractor *interactor = mafInteractor::SafeDownCast(e->GetMafObject());
          assert(interactor);
          mafString *action_name = e->GetString();
          m_InteractionManager->UnBindAction(*action_name,interactor);
        }
#endif
      break;
      case PER_PUSH:
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
        if(m_InteractionManager)
        {
          mmiPER *per = mmiPER::SafeDownCast(e->GetMafObject());
          assert(per);
          m_InteractionManager->PushPER(per);
        }
#endif
      break; 
      case PER_POP:
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
        if(m_InteractionManager) m_InteractionManager->PopPER();
#endif
      break;
      case CREATE_STORAGE:
        CreateStorage(e);
      break;
      case COLLABORATE_ENABLE:
      {
        bool collaborate = e->GetBool();
        if (collaborate)
        {
          //m_RemoteLogic->SetRemoteMouse(m_InteractionManager->GetRemoteMouseDevice());
          m_Mouse->AddObserver(m_RemoteLogic, REMOTE_COMMAND_CHANNEL);
          if(m_RemoteLogic->IsSocketConnected())  //check again, because if no server is present
          {                                       //no synchronization is necessary
            m_RemoteLogic->SynchronizeApplication();
          }
        }
        else
        {
          if (m_RemoteLogic != NULL)
          {
            m_RemoteLogic->SetRemoteMouse(NULL);
            m_RemoteLogic->Disconnect();
            m_Mouse->RemoveObserver(m_RemoteLogic);
          }
        }
        m_ViewManager->Collaborate(collaborate);
        m_OpManager->Collaborate(collaborate);
        m_Mouse->Collaborate(collaborate);
      }
      break;
			case ABOUT_APPLICATION:
			{
				mafString message=m_AppTitle;
				message+=_(" Application ");
				message+=m_Revision;
				wxString msg_dlg = wxString::Format("%s",message);
				wxMessageBox(msg_dlg, "About Application");
			}
			break;
      default:
        mafLogicWithGUI::OnEvent(maf_event);
      break; 
    } // end switch case
  } // end if SafeDowncast
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileNew()
//----------------------------------------------------------------------------
{
	if(m_VMEManager)
  {
    if( m_VMEManager->AskConfirmAndSave())
	  {
      if(m_OpManager)
      {
        m_OpManager->GetSelectedVme()->GetRoot()->CleanTree();
        m_OpManager->ClearUndoStack(); 
      }
		  m_VMEManager->MSFNew();
	  }
  }
	m_Win->SetTitle(wxString(m_AppTitle.GetCStr()));
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileUpload(const char *remote_file, unsigned int upload_flag)
//----------------------------------------------------------------------------
{
  if (remote_file == NULL)
  {
    wxMessageBox(_("remote filename not valid!!"), _("Warning"));
    return;
  }

  wxMessageBox(_("Not implemented: think about it!!"), _("Warning"));
  /*
  if(m_VMEManager)
  {
    if(m_VMEManager->AskConfirmAndSave())
    {
      // Upload the file to remote host.
      m_VMEManager->Upload(remote_file,upload_flag);
    }
  }
  */
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileOpen(const char *file_to_open)
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
  {
	  if(m_VMEManager->AskConfirmAndSave())
	  {
      if(m_OpManager)
      {
        m_OpManager->GetSelectedVme()->GetRoot()->CleanTree();
        m_OpManager->ClearUndoStack(); 
      }
      wxString file;
      if (m_ApplicationSettings->UseRemoteStorage())
      {
        if (file_to_open != NULL)
        {
          file = file_to_open;
        }
        else
        {
          mmgDialogRemoteFile remoteFile;
          remoteFile.ShowModal();
          file = remoteFile.GetFile().GetCStr();
          mafString protocol;
          if (IsRemote(file.c_str(),protocol))
          {
            m_VMEManager->SetHost(remoteFile.GetHost());
            m_VMEManager->SetRemotePort(remoteFile.GetPort());
            m_VMEManager->SetUser(remoteFile.GetUser());
            m_VMEManager->SetPassword(remoteFile.GetPassword());
          }
        }
      }
      else
      {
		    wxString wildc    = _("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
		    wxString msf_dir  = wxGetCwd().c_str();
        if (file_to_open != NULL)
        {
          file = file_to_open;
        }
        else
        {
          file = mafGetOpenFile(msf_dir, wildc).c_str();
        }
      }

      if(file.IsEmpty())
        return;

		  m_VMEManager->MSFOpen(file);
	  }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileHistory(int menuId)
//----------------------------------------------------------------------------
{
	if(m_VMEManager) 
  {
    this->m_VMEManager->MSFOpen(menuId);
    UpdateFrameTitle();
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileSave()
//----------------------------------------------------------------------------
{
  if(m_VMEManager) m_VMEManager->MSFSave();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileSaveAs()
//----------------------------------------------------------------------------
{
  if(m_VMEManager) 
  {
    m_VMEManager->MSFSaveAs();
    UpdateFrameTitle();
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnQuit()
//----------------------------------------------------------------------------
{
  if (m_OpManager && m_OpManager->Running())
  {
    return;
  }

  if(m_VMEManager)
  {
    bool quit = false;
    if (m_VMEManager->MSFIsModified())
    {
      int answer = wxMessageBox
          (
          _("would you like to save your work before quitting ?"),
          _("Confirm"), 
          wxYES_NO|wxCANCEL|wxICON_QUESTION , m_Win
          );
      if(answer == wxYES) 
        m_VMEManager->MSFSave();
      quit = answer != wxCANCEL;
    }
    else 
    {
      int answer = wxMessageBox(_("quit program ?"), _("Confirm"), wxYES_NO | wxICON_QUESTION , m_Win);
      quit = answer == wxYES;
    }
    if(!quit) 
      return;
  }
  
  mmgMDIChild::OnQuit(); 
  m_Win->OnQuit(); 

  cppDEL(m_RemoteLogic);
  cppDEL(m_VMEManager);
  cppDEL(m_MaterialChooser);
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  cppDEL(m_InteractionManager);
#endif
  cppDEL(m_ViewManager);
  cppDEL(m_OpManager);

  // must be deleted after m_VMEManager
  cppDEL(m_SideBar);

  mafLogicWithGUI::OnQuit();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeSelect(mafEvent& e)	//modified by Paolo 10-9-2003
//----------------------------------------------------------------------------
{
  mafNode *node = NULL;

	if(m_PlugSidebar && (e.GetSender() == this->m_SideBar->GetTree()))
    node = (mafNode*)e.GetArg();//sender == tree => the node is in e.arg
  else
    node = e.GetVme();          //sender == PER  => the node is in e.node  

  if(node == NULL)
  {
    //node can be selected by its ID
    if(m_VMEManager)
    {
		  long vme_id = e.GetArg();
		  mafVMERoot *root = this->m_VMEManager->GetRoot();
		  if (root)
		  {
			  node = root->FindInTreeById(vme_id);
			  e.SetVme(node);
      }
    }
  }
  if(node != NULL && m_OpManager)
    m_OpManager->OpSelect(node);
    
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if (m_InteractionManager)
    m_InteractionManager->VmeSelected(node);
#endif

  if(m_RemoteLogic && (e.GetSender() != m_RemoteLogic) && m_RemoteLogic->IsSocketConnected())
  {
    m_RemoteLogic->VmeSelected(node);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeSelected(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_ViewManager) m_ViewManager->VmeSelect(vme);
  if(m_OpManager)   m_OpManager->VmeSelected(vme);
	if(m_SideBar)     m_SideBar->VmeSelected(vme);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeShow(mafNode *vme, bool visibility)
//----------------------------------------------------------------------------
{
	if(m_ViewManager) m_ViewManager->VmeShow(vme, visibility);
  bool vme_in_tree = vme->IsVisible(); //check VisibleToTraverse flag.
/*  vme_in_tree = !vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") || 
    (vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") && vme->GetTagArray()->GetTag("VISIBLE_IN_THE_TREE")->GetValueAsDouble() != 0);*/
  if(m_SideBar && vme_in_tree)
    m_SideBar->VmeShow(vme,visibility);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeModified(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_PlugTimebar) UpdateTimeBounds();
  bool vme_in_tree = vme->IsVisible();
/*  vme_in_tree = !vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") || 
    (vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") && vme->GetTagArray()->GetTag("VISIBLE_IN_THE_TREE")->GetValueAsDouble() != 0);*/
  if(m_SideBar && vme_in_tree)
    m_SideBar->VmeModified(vme);
	if(m_VMEManager) m_VMEManager->MSFModified(true);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
	if(m_VMEManager) m_VMEManager->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeAdded(mafNode *vme)
//----------------------------------------------------------------------------
{
  //vme->Initialize();

  if(m_ViewManager)
    m_ViewManager->VmeAdd(vme);
  bool vme_in_tree = true;
  vme_in_tree = !vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") || 
    (vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") && vme->GetTagArray()->GetTag("VISIBLE_IN_THE_TREE")->GetValueAsDouble() != 0);
  if(m_SideBar && vme_in_tree)
    m_SideBar->VmeAdd(vme);
  if(m_PlugTimebar)
    UpdateTimeBounds();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::RestoreLayout()
//----------------------------------------------------------------------------
{
  // Retrieve the saved layout.
  mafNode *vme = m_VMEManager->GetRoot();
  mmaApplicationLayout *app_layout = mmaApplicationLayout::SafeDownCast(vme->GetAttribute("ApplicationLayout"));
  if (app_layout)
  {
    int answer = wxMessageBox(_("Do you want to load the layout?"), _("Warning"), wxYES_NO);
    if (answer == wxNO)
    {
      return;
    }
    m_ViewManager->ViewDeleteAll();
    int maximized, pos[2], size[2];
    app_layout->GetApplicationInfo(maximized, pos, size);
    if (maximized != 0)
    {
      m_Win->Maximize();
    }
    else
    {
      wxRect rect(pos[0],pos[1],size[0],size[1]);
      m_Win->SetSize(rect);
    }
    bool tb_vis = app_layout->GetToolBarVisibility() != 0;
    bool sb_vis = app_layout->GetSideBarVisibility() != 0;
    bool lb_vis = app_layout->GetLogBarVisibility() != 0;
    m_Win->ShowDockPane("toolbar", tb_vis);
    m_Win->ShowDockPane("logbar", lb_vis);
    m_Win->ShowDockPane("sidebar", sb_vis);
    int num = app_layout->GetNumberOfViewsInLayout();
    std::vector<mmaApplicationLayout::ViewLayoutInfo>::iterator iter = app_layout->GetLayoutList();
    mafView *v = NULL;
    for (int i = 0; i < num; i++, iter++)
    {
      ViewCreate((*iter).m_Id);
      mafYield();
      v = m_ViewManager->GetSelectedView();
      if (v)
      {
        v->SetName((*iter).m_Label.GetCStr());
        pos[0] = (*iter).m_Position[0];
        pos[1] = (*iter).m_Position[1];
        size[0] = (*iter).m_Size[0];
        size[1] = (*iter).m_Size[1];
        wxRect rect(pos[0],pos[1],size[0],size[1]);
        v->GetFrame()->SetSize(rect);
      }
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
    m_VMEManager->VmeRemove(vme);
  if(m_PlugTimebar)
    UpdateTimeBounds();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeRemoving(mafNode *vme)
//----------------------------------------------------------------------------
{
  bool vme_in_tree = true;
  vme_in_tree = !vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") || 
    (vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") && vme->GetTagArray()->GetTag("VISIBLE_IN_THE_TREE")->GetValueAsDouble() != 0);
  if(m_SideBar && vme_in_tree)
    m_SideBar->VmeRemove(vme);
	if(m_ViewManager)
    m_ViewManager->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpRunStarting()
//----------------------------------------------------------------------------
{
  EnableMenuAndToolbar(false);
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if(m_InteractionManager) m_InteractionManager->EnableSelect(false);
#endif
  if(m_SideBar)    m_SideBar->EnableSelect(false);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpRunTerminated()
//----------------------------------------------------------------------------
{
  EnableMenuAndToolbar(true);
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if(m_InteractionManager) 
    m_InteractionManager->EnableSelect(true);
#endif
  if(m_SideBar)
    m_SideBar->EnableSelect(true);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::EnableMenuAndToolbar(bool enable)
//----------------------------------------------------------------------------
{
  EnableItem(MENU_FILE_NEW,enable);
  EnableItem(MENU_FILE_OPEN,enable);
  EnableItem(MENU_FILE_SAVE,enable);
  EnableItem(MENU_FILE_SAVEAS,enable );
  EnableItem(MENU_FILE_MERGE,enable );
  EnableItem(MENU_FILE_QUIT,enable );
  EnableItem(wxID_FILE1,enable );
  EnableItem(wxID_FILE2,enable );
  EnableItem(wxID_FILE3,enable );
  EnableItem(wxID_FILE4,enable );
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpShowGui(bool push_gui, mmgPanel *panel)
//----------------------------------------------------------------------------
{
	if(m_SideBar) m_SideBar->OpShowGui(push_gui, panel);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpHideGui(bool view_closed)
//----------------------------------------------------------------------------
{
	if(m_SideBar) m_SideBar->OpHideGui(view_closed);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewCreate(int viewId)
//----------------------------------------------------------------------------
{
	if(m_ViewManager)
  {
    mafView* v = m_ViewManager->ViewCreate(viewId);

    /*
    if(m_OpManager) 
    {
      VmeShow(m_OpManager->GetSelectedVme(),true);
    }
    */
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewSelect()
//----------------------------------------------------------------------------
{
  if(m_ViewManager) 
  {
    mafView *view = m_ViewManager->GetSelectedView();
    if(m_SideBar)	m_SideBar->ViewSelect(view);

    EnableItem(CAMERA_RESET, view!=NULL);
    EnableItem(CAMERA_FIT,   view!=NULL);
    EnableItem(CAMERA_FLYTO, view!=NULL);

    EnableItem(MENU_FILE_PRINT, view != NULL);
    EnableItem(MENU_FILE_PRINT_PREVIEW, view != NULL);
    EnableItem(MENU_FILE_PRINT_SETUP, view != NULL);
    EnableItem(MENU_FILE_PRINT_PAGE_SETUP, view != NULL);

// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if (m_InteractionManager)
    {
      m_InteractionManager->ViewSelected(view);
    }
#endif

    if(m_OpManager && !m_OpManager->Running()) 
    {
      // needed to update all the operations that will be enabled on View Creation
      m_OpManager->VmeSelected(m_OpManager->GetSelectedVme());
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewCreated(mafView *v)
//----------------------------------------------------------------------------
{
  // removed temporarily support for external Views
  if(v) 
	{
    if(m_RemoteLogic && m_RemoteLogic->IsSocketConnected() && !m_ViewManager->m_FromRemote)
    {
      mafEvent ev(this,VIEW_CREATE,v);
      ev.SetChannel(REMOTE_COMMAND_CHANNEL);
      m_RemoteLogic->OnEvent(&ev);
    }

    if (GetExternalViewFlag())
    {
      // external views
      mmgViewFrame *extern_view = new mmgViewFrame(m_Win, -1, v->GetLabel(), wxPoint(10,10),wxSize(800,600)/*, wxSIMPLE_BORDER|wxMAXIMIZE*/);
      extern_view->SetView(v);
      extern_view->SetListener(m_ViewManager);
      v->SetListener(extern_view);
      v->SetFrame(extern_view);
      extern_view->Refresh();
    }
    else
    {
      // child views
      mmgMDIChild *c = new mmgMDIChild(m_Win,v);   
      c->SetListener(m_ViewManager);
      v->SetFrame(c);
    }
	}
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::TimeSet(float t)
//----------------------------------------------------------------------------
{
  if(m_VMEManager) m_VMEManager->TimeSet(t);
  if(m_ViewManager) m_ViewManager->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::UpdateTimeBounds()
//----------------------------------------------------------------------------
{
  double min,max; 
  if(m_VMEManager) m_VMEManager->TimeGetBounds(&min,&max);
  if(m_TimePanel)
  {
    m_TimePanel->SetBounds(min,max);
    m_Win->ShowDockPane("timebar", min<max);
  }
}
//----------------------------------------------------------------------------
mafNode* mafLogicWithManagers::VmeChoose(long vme_accept_function, long style, mafString title)
//----------------------------------------------------------------------------
{
  mmgVMEChooser vc(m_SideBar->GetTree(),title.GetCStr(), vme_accept_function, style);
  return vc.ShowChooserDialog();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeChooseMaterial(mafVME *vme, bool updateProperty)
//----------------------------------------------------------------------------
{
  if (m_MaterialChooser == NULL)
  {
    m_MaterialChooser = new mmgMaterialChooser();
  }
  if(m_MaterialChooser->ShowChooserDialog(vme))
  {
    this->m_ViewManager->PropertyUpdate(updateProperty);
    this->m_ViewManager->CameraUpdate();
    this->m_VMEManager->MSFModified(true);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewContextualMenu(bool vme_menu)
//----------------------------------------------------------------------------
{
  mmgContextualMenu *contextMenu = new mmgContextualMenu();
  contextMenu->SetListener(this);
  mafView *v = m_ViewManager->GetSelectedView();
  mmgMDIChild *c = (mmgMDIChild *)m_Win->GetActiveChild();
  if(c != NULL)
    contextMenu->ShowContextualMenu(c,v,vme_menu);
/*  else
    //m_extern_view->ShowContextualMenu(vme_menu);
    contextMenu->ShowContextualMenu(m_extern_view,v,vme_menu);*/
  cppDEL(contextMenu);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::TreeContextualMenu(mafEvent &e)
//----------------------------------------------------------------------------
{
  mmgTreeContextualMenu *contextMenu = new mmgTreeContextualMenu();
  contextMenu->SetListener(this);
  mafView *v = m_ViewManager->GetSelectedView();
  mafVME  *vme = (mafVME *)e.GetVme();
  bool vme_menu = e.GetBool();
  bool autosort = e.GetArg() != 0;
  contextMenu->ShowContextualMenu((mmgCheckTree *)e.GetSender(),v,vme,vme_menu);
  cppDEL(contextMenu);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::HandleException()
//----------------------------------------------------------------------------
{
  int answare = wxMessageBox(_("Do you want to try to save the unsaved work ?"), _("Fatal Exception!!"), wxYES_NO|wxCENTER);
  if(answare == wxYES)
  {
    OnFileSaveAs();
    m_OpManager->StopCurrentOperation();
  }
  OnQuit();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateStorage(mafEvent *e)
//----------------------------------------------------------------------------
{
  if (m_ApplicationSettings->UseRemoteStorage())
  {
    mafString cache_folder = m_ApplicationSettings->GetCacheFolder();
    if (!wxDirExists(cache_folder.GetCStr()))
    {
      wxMkdir(cache_folder.GetCStr());
    }
    mafRemoteStorage *storage;
    storage = (mafRemoteStorage *)e->GetMafObject();
    if (storage)
    {
      m_VMEManager->NotifyRemove(storage->GetRoot());
      storage->Delete();
    }
    storage = mafRemoteStorage::New();
    storage->SetLocalCacheFolder(cache_folder);
    
    //set default values for remote connection
    storage->SetHostName(m_ApplicationSettings->GetRemoteHostName());
    storage->SetRemotePort(m_ApplicationSettings->GetRemotePort());
    storage->SetUsername(m_ApplicationSettings->GetUserName());
    storage->SetPassword(m_ApplicationSettings->GetPassword());
    
    storage->GetRoot()->SetName("root");
    storage->SetListener(m_VMEManager);
    storage->GetRoot()->Initialize();
    e->SetMafObject(storage);
  }
  else
  {
    mafVMEStorage *storage;
    storage = (mafVMEStorage *)e->GetMafObject();
    if (storage)
    {
      m_VMEManager->NotifyRemove(storage->GetRoot());
      storage->Delete();
    }
    storage = mafVMEStorage::New();
    storage->GetRoot()->SetName("root");
    storage->SetListener(m_VMEManager);
    storage->GetRoot()->Initialize();
    e->SetMafObject(storage);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::SetExternalViewFlag(bool external)
//----------------------------------------------------------------------------
{
  m_ExternalViewFlag = external;
  wxConfig *config = new wxConfig(wxEmptyString);
  config->Write("ExternalViewFlag",m_ExternalViewFlag);
  cppDEL(config);
}
//----------------------------------------------------------------------------
bool mafLogicWithManagers::GetExternalViewFlag()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->Read("ExternalViewFlag", &m_ExternalViewFlag, false);
  cppDEL(config);
  return m_ExternalViewFlag;
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::UpdateMeasureUnit()
//----------------------------------------------------------------------------
{
  for(mafView* v = m_ViewManager->GetList(); v; v=v->m_Next) 
    v->OptionsUpdate();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ImportExternalFile(mafString &filename)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
  wxSplitPath(filename.GetCStr(),&path,&name,&ext);
  ext.MakeLower();
  if (ext == "vtk")
  {
    mmoVTKImporter *vtkImporter = new mmoVTKImporter("importer");
    vtkImporter->SetListener(m_OpManager);
    vtkImporter->SetFileName(filename.GetCStr());
    vtkImporter->ImportVTK();
    vtkImporter->GetOutput()->ReparentTo(m_VMEManager->GetRoot());
    cppDEL(vtkImporter);
  }
  else if (ext == "stl")
  {
    mmoSTLImporter *stlImporter = new mmoSTLImporter("importer");
    stlImporter->SetListener(m_OpManager);
    stlImporter->SetFileName(filename.GetCStr());
    stlImporter->ImportSTL();
    stlImporter->GetOutput()->ReparentTo(m_VMEManager->GetRoot());
    cppDEL(stlImporter);
  }
  else
    wxMessageBox(_("Can not import this type of file!"), _("Warning"));
}

//----------------------------------------------------------------------------
void mafLogicWithManagers::SetRevision(mafString revision)
//----------------------------------------------------------------------------
{
	m_Revision=revision;
}
