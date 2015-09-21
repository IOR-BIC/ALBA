/*=========================================================================

 Program: MAF2
 Module: mafLogicWithManagers
 Authors: Silvano Imboden, Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafDecl.h"

#include "mafLogicWithManagers.h"
#include <wx/config.h>

#include "mafView.h"
#include "mafViewCompound.h"

#include "mafViewManager.h"
#include "mafOp.h"
#include "mafOpManager.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafPrintSupport.h"

#ifdef MAF_USE_VTK
  #include "mafViewVTK.h"
  
  #include "mafOpImporterVTK.h"
  #include "mafOpImporterSTL.h"
  #include "mafInteractionManager.h"
  #include "mafInteractionFactory.h"
  #include "mafInteractor.h"
  #include "mafDeviceManager.h"
  #include "mafAction.h"
  #include "mafDeviceButtonsPadMouse.h"
  #include "mafDeviceClientMAF.h"
  #include "mafInteractorPER.h"
  #include "mafGUITreeContextualMenu.h"
  #include "mafGUIContextualMenu.h"
  #include "vtkCamera.h"
#endif

#include "mafSideBar.h"

#include "mafUser.h"
//#include "mafGUISRBBrowse.h"
#include "mafGUIDialogRemoteFile.h"
#include "mafGUIDialogFindVme.h"
#include "mafGUIMDIFrame.h"
#include "mafGUIMDIChild.h"
#include "mafGUICheckTree.h"
#include "mafGUITimeBar.h"
#include "mafGUIMaterialChooser.h"
#include "mafGUIViewFrame.h"
#include "mafGUILocaleSettings.h"
#include "mafGUIMeasureUnitSettings.h"
#include "mafGUIApplicationSettings.h"
#include "mafGUISettingsStorage.h"
#include "mafGUIApplicationLayoutSettings.h"
#include "mafGUISettingsTimeBar.h"
#include "mafRemoteLogic.h"
#include "mafGUISettingsDialog.h"
#include  "mafGUISettingsHelp.h"

#ifdef WIN32
  #include "mafDeviceClientMAF.h"
#endif

#include "mmdRemoteFileManager.h"

#include "mmaApplicationLayout.h"

#include "mafEventSource.h"
#include "mafDataVector.h"
#include "mafVMEStorage.h"
#include "mafRemoteStorage.h"


#include "mafWizardManager.h"

#include "mafRWIBase.h"
#include <wx/dir.h>



//----------------------------------------------------------------------------
mafLogicWithManagers::mafLogicWithManagers(mafGUIMDIFrame *mdiFrame/*=NULL*/)
: mafLogicWithGUI(mdiFrame)
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
  
  m_SettingsDialog = new mafGUISettingsDialog();

  m_ApplicationLayoutSettings = NULL;

  m_HelpSettings = NULL;

  m_Revision = _("0.1");

  m_Extension = "msf";

  m_UseWizardManager  = false;
  m_WizardRunning = false;
  m_WizardManager = NULL;

  m_User = new mafUser();

}
//----------------------------------------------------------------------------
mafLogicWithManagers::~mafLogicWithManagers()
//----------------------------------------------------------------------------
{  
  //free mem
  if(m_WizardManager)
    delete m_WizardManager;

  // Managers are destruct in the OnClose
  cppDEL(m_User);
  cppDEL(m_ApplicationLayoutSettings);
  cppDEL(m_HelpSettings);
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
    m_VMEManager->SetHost(m_StorageSettings->GetRemoteHostName());
    m_VMEManager->SetRemotePort(m_StorageSettings->GetRemotePort());
    m_VMEManager->SetUser(m_StorageSettings->GetUserName());
    m_VMEManager->SetPassword(m_StorageSettings->GetPassword());
    m_VMEManager->SetLocalCacheFolder(m_StorageSettings->GetCacheFolder());
    m_VMEManager->SetListener(this); 
	m_VMEManager->SetFileExtension(m_Extension);
    //m_VMEManager->SetSingleBinaryFile(m_StorageSettings->GetSingleFileStatus()!= 0);
  }

// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if (m_UseInteractionManager)
  {
    m_InteractionManager = new mafInteractionManager();
    m_InteractionManager->SetListener(this);
    mafPlugDevice<mmdRemoteFileManager>("mmdRemoteFileManager");

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
    m_OpManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
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
  m_SettingsDialog->AddPage( m_ApplicationSettings->GetGui(), m_ApplicationSettings->GetLabel());
  m_SettingsDialog->AddPage( m_StorageSettings->GetGui(), m_StorageSettings->GetLabel());

  if (m_ViewManager)
  {
    m_ApplicationLayoutSettings = new mafGUIApplicationLayoutSettings(this);
    m_ApplicationLayoutSettings->SetViewManager(m_ViewManager);
    m_ApplicationLayoutSettings->SetApplicationFrame(m_Win);
    m_SettingsDialog->AddPage( m_ApplicationLayoutSettings->GetGui(), m_ApplicationLayoutSettings->GetLabel());
  }

  m_SettingsDialog->AddPage( m_Win->GetDockSettingGui(), _("User Interface Preferences"));

  m_HelpSettings = new mafGUISettingsHelp(this);
  m_SettingsDialog->AddPage(m_HelpSettings->GetGui(), m_HelpSettings->GetLabel());

// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if(m_InteractionManager)
    m_SettingsDialog->AddPage(m_InteractionManager->GetGui(), _("Interaction Manager"));
#endif    
  if(m_LocaleSettings)
    m_SettingsDialog->AddPage(m_LocaleSettings->GetGui(), m_LocaleSettings->GetLabel());

  if (m_MeasureUnitSettings)
    m_SettingsDialog->AddPage(m_MeasureUnitSettings->GetGui(), m_MeasureUnitSettings->GetLabel());

  if (m_TimeBarSettings)
    m_SettingsDialog->AddPage(m_TimeBarSettings->GetGui(), m_TimeBarSettings->GetLabel());

  ConfigureWizardManager();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Plug(mafView* view, bool visibleInMenu)
//----------------------------------------------------------------------------
{
  if(m_ViewManager) 
    m_ViewManager->ViewAdd(view, visibleInMenu);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Plug(mafOp *op, wxString menuPath, bool canUndo, mafGUISettings *setting)
//----------------------------------------------------------------------------
{
  if(m_OpManager) 
  {
    m_OpManager->OpAdd(op, menuPath, canUndo, setting);
    
// currently mafInteraction is strictly dependent on VTK
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
void mafLogicWithManagers::Plug( mafWizard *wizard, wxString menuPath /*= ""*/ )
//----------------------------------------------------------------------------
{
	//Plugging the wizard    
	if(m_WizardManager)
	{
		m_WizardManager->WizardAdd(wizard, menuPath);
	}
	else 
	{
		mafLogMessage("Enable wizard pluggin to plug wizards"); 
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

  //setting gui pointers to the Wizard Manager
  if(m_WizardManager)
  {
	  if(m_MenuBar)
	  {
		  m_WizardManager->FillMenu(m_WizardMenu);
		  m_WizardManager->SetMenubar(m_MenuBar);
	  }
	  if(m_ToolBar)
		  m_WizardManager->SetToolbar(m_ToolBar);
  }
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
void mafLogicWithManagers::SetApplicationStamp(std::vector<mafString> app_stamp)
//----------------------------------------------------------------------------
{
  if (m_VMEManager)
  {
    m_VMEManager->SetApplicationStamp(app_stamp);
  }
}
//----------------------------------------------------------------------------
mafUser *mafLogicWithManagers::GetUser()
//----------------------------------------------------------------------------
{
  return m_User;
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Init(int argc, char **argv)
//----------------------------------------------------------------------------
{
  if (m_WizardManager)
  {
    m_WizardManager->FillSettingDialog(m_SettingsDialog);
  }

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
    m_OpManager->FillSettingDialog(m_SettingsDialog);

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

  m_ApplicationLayoutSettings->LoadLayout(true);
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
  if (m_StorageSettings->UseRemoteStorage())
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
  edit_menu->Append(OP_DELETE, _("Delete  \tDel"));
  edit_menu->Append(OP_CUT,   _("Cut   \tCtrl+Shift+X"));
  edit_menu->Append(OP_COPY,  _("Copy  \tCtrl+Shift+C"));
  edit_menu->Append(OP_PASTE, _("Paste \tCtrl+Shift+V"));
  edit_menu->Append(MENU_EDIT_FIND_VME, _("Find VME \tCtrl+F"));
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
	help_menu->Append(HELP_HOME, _("Help"));

	m_MenuBar->Append(help_menu, _("&Help"));

  m_Win->SetMenuBar(m_MenuBar);

  EnableItem(MENU_FILE_PRINT, false);
  EnableItem(MENU_FILE_PRINT_PREVIEW, false);
  EnableItem(MENU_FILE_PRINT_SETUP, false);
  EnableItem(MENU_FILE_PRINT_PAGE_SETUP, false);

  //add the wizard menu between "operation" and "settings"
  if (m_UseWizardManager)
  {
	  m_WizardMenu = new wxMenu;
	  m_MenuBar->Insert(4,m_WizardMenu, _("&Wizard"));
  }
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
  m_ToolBar->AddTool(MENU_FILE_NEW,mafPictureFactory::GetPictureFactory()->GetBmp("FILE_NEW"),    _("new " + m_Extension + " storage file"));
  m_ToolBar->AddTool(MENU_FILE_OPEN,mafPictureFactory::GetPictureFactory()->GetBmp("FILE_OPEN"),  _("open " + m_Extension + " storage file"));
  m_ToolBar->AddTool(MENU_FILE_SAVE,mafPictureFactory::GetPictureFactory()->GetBmp("FILE_SAVE"),  _("save current " + m_Extension + " storage file"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(MENU_FILE_PRINT,mafPictureFactory::GetPictureFactory()->GetBmp("PRINT"),  _("print the selected view"));
  m_ToolBar->AddTool(MENU_FILE_PRINT_PREVIEW,mafPictureFactory::GetPictureFactory()->GetBmp("PRINT_PREVIEW"),  _("show the print preview for the selected view"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(OP_UNDO,mafPictureFactory::GetPictureFactory()->GetBmp("OP_UNDO"),  _("undo (ctrl+z)"));
  m_ToolBar->AddTool(OP_REDO,mafPictureFactory::GetPictureFactory()->GetBmp("OP_REDO"),  _("redo (ctrl+shift+z)"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(OP_CUT,  mafPictureFactory::GetPictureFactory()->GetBmp("OP_CUT"),  _("cut selected vme (ctrl+x)"));
  m_ToolBar->AddTool(OP_COPY, mafPictureFactory::GetPictureFactory()->GetBmp("OP_COPY"), _("copy selected vme (ctrl+c)"));
  m_ToolBar->AddTool(OP_PASTE,mafPictureFactory::GetPictureFactory()->GetBmp("OP_PASTE"),_("paste vme (ctrl+v)"));
  m_ToolBar->AddSeparator();
  m_ToolBar->AddTool(CAMERA_RESET,mafPictureFactory::GetPictureFactory()->GetBmp("ZOOM_ALL"),_("reset camera to fit all (ctrl+f)"));
  m_ToolBar->AddTool(CAMERA_FIT,  mafPictureFactory::GetPictureFactory()->GetBmp("ZOOM_SEL"),_("reset camera to fit selected object (ctrl+shift+f)"));
  m_ToolBar->AddTool(CAMERA_FLYTO,mafPictureFactory::GetPictureFactory()->GetBmp("FLYTO"),_("fly to object under mouse"));


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
  //Special Window title management during wizards
  if (m_WizardRunning)
  {
    wxString title(m_AppTitle);
	title += "   " + m_WizardManager->GetDescription();
    m_Win->SetTitle(title);
  }
  else
  {
    wxString title(m_AppTitle);
	title += "   " + m_VMEManager->GetFileName();
	m_Win->SetTitle(title);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    /*if (e->GetId() == mafDataVector::SINGLE_FILE_DATA)
    {
      e->SetBool(m_StorageSettings->GetSingleFileStatus()!= 0);
      return;
    }*/
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
        wxString msg = _("Insert remote directory on remote host: ") + m_StorageSettings->GetRemoteHostName();
        do 
        {
          remote_path = wxGetTextFromUser(msg,_("Remote directory choose"),"/mafstorage/pub/");
          if (!remote_path.IsEmpty())
          {
            remote_file = m_StorageSettings->GetRemoteHostName();
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
      case MENU_EDIT_FIND_VME:
        FindVME();
      break;
      case VME_SELECT:	
        VmeSelect(*e);		
      break; 
      case VME_SELECTED: 
        VmeSelected(e->GetVme());
      break;
      case VME_DCLICKED:
        VmeDoubleClicked(*e);
      break;
      case VME_SHOW:
        VmeShow(e->GetVme(), e->GetBool());
        if(m_RemoteLogic && (e->GetSender() != m_RemoteLogic) && m_RemoteLogic->IsSocketConnected())
        {
          m_RemoteLogic->VmeShow(e->GetVme(), e->GetBool());
        }
      break;
      case VME_MODIFIED:
        VmeModified(e->GetVme());
				if(!m_PlugTimebar && ((mafVME*)e->GetVme())->IsAnimated())
					m_Win->ShowDockPane("timebar",!m_Win->DockPaneIsShown("timebar") );
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
        {
          std::vector<mafNode*> nodeVector = VmeChoose(e->GetArg(), REPRESENTATION_AS_TREE, *s, e->GetBool());
          if (!e->GetBool())
          {
            if (nodeVector.size() != 0)
            {
              e->SetVme(nodeVector[0]);
            }
            else
            {
              e->SetVme(NULL);
            }
          }
          else
          {
            e->SetVmeVector(nodeVector);
          }
        }
        else
        {
          std::vector<mafNode*> nodeVector = VmeChoose(e->GetArg(), REPRESENTATION_AS_TREE, "Choose Node", e->GetBool());
          if (!e->GetBool())
          {
            if (nodeVector.size() != 0)
            {
              e->SetVme(nodeVector[0]);
            }
            else
            {
              e->SetVme(NULL);
            }
          }
          else
          {
            e->SetVmeVector(nodeVector);
          }
        }
      }
      break;
      case VME_CHOOSE_MATERIAL:
        VmeChooseMaterial((mafVME *)e->GetVme(), e->GetBool());
      break;
      case VME_VISUAL_MODE_CHANGED:
      {
        mafVME *vme = (mafVME *)e->GetVme();
        VmeShow(vme, false);
        VmeShow(vme, true);
      }
      break;
      case UPDATE_PROPERTY:
        VmeUpdateProperties((mafVME *)e->GetVme(), e->GetBool());
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
          if(/*m_OpManager->GetRunningOperation() && */m_RemoteLogic && m_RemoteLogic->IsSocketConnected() && !m_OpManager->m_FromRemote)
          {
            mafEvent re(this, mafOpManager::RUN_OPERATION_EVENT, e->GetArg());
            re.SetChannel(REMOTE_COMMAND_CHANNEL);
            m_RemoteLogic->OnEvent(&re);
          }
        }
      break;
      case PARSE_STRING:
        {
          if(this->m_OpManager->Running())
          {
            wxMessageBox("There is an other operation running!!");
            return;
          }
          int menuId, opId;
          mafString *s = e->GetString();
          menuId = m_MenuBar->FindMenu(_("Operations"));
          opId = m_MenuBar->GetMenu(menuId)->FindItem(s->GetCStr());
          m_OpManager->OpRun(opId);
        }
      break;
      case MENU_OPTION_APPLICATION_SETTINGS:
        if (m_OpManager)
        {
          m_OpManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
        }
      break;
      case CLEAR_UNDO_STACK:
        if (!m_OpManager->Running())
        {
          m_OpManager->ClearUndoStack();
        }
      break;
      case OP_RUN_STARTING:
      {
		mafLogMessage("run starting");
		m_CancelledBeforeOpStarting=false;
        mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
        if (c != NULL)
          c->SetAllowCloseWindow(false);
        OpRunStarting();
      }
      break;
	  case OP_RUN_TERMINATED:
		  {
			  //if the operation was started from the wizard we continue the wizard execution
			  if (m_WizardManager && m_WizardRunning)
			  {
				  m_WizardManager->WizardContinue(e->GetArg());
			  }
			  //else we manage the operation end by unlock the close button and so on
			  else
			  {
				  mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
				  if (c != NULL)
					  c->SetAllowCloseWindow(true);
				  OpRunTerminated();
			  }
		  }
		  break; 
      case OP_SHOW_GUI:
        OpShowGui(!e->GetBool(), (mafGUIPanel*)e->GetWin());
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
	  case VIEW_RESIZE:
		  {
			  mafView *view = NULL;
			  const char *viewStr=e->GetString()->GetCStr();
			  view=m_ViewManager->GetFromList(viewStr);
			  if(view) 
			  {
				  view->GetFrame()->SetSize(e->GetWidth(),e->GetHeight());
				  view->GetFrame()->SetPosition(wxPoint(e->GetX(),e->GetY()));
			  }
		  }
	  break;
      case VIEW_DELETE:
      {

        if(m_PlugSidebar)
		   this->m_SideBar->ViewDeleted(e->GetView());

#ifdef MAF_USE_VTK
        // currently mafInteraction is strictly dependent on VTK (marco)
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
      }
      if (m_OpManager)
      {
        m_OpManager->RefreshMenu();
      }
      break;	
      case VIEW_SELECT:
      {
        ViewSelect();
        if (m_OpManager)
        {
          mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
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
        e->SetView(m_ViewManager->GetSelectedView());
      break;
      case VIEW_SAVE_IMAGE:
      {
				mafViewCompound *v = mafViewCompound::SafeDownCast(m_ViewManager->GetSelectedView());
        if (v && e->GetBool())
        {
          v->GetRWI()->SaveAllImages(v->GetLabel(),v, m_ApplicationSettings->GetImageTypeId());
        }
        else
        {
          mafView *v = m_ViewManager->GetSelectedView();
          if (v)
          {
            v->GetRWI()->SaveImage(v->GetLabel());
          }
        }
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
      case mafGUIMeasureUnitSettings::MEASURE_UNIT_UPDATED:
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
		//PERFORMANCE WARNING : if view manager exist this will cause another camera update!!
		// An else could be added to reduce CAMERA_UPDATE by a 2 factor. This has been done for the HipOp vertical App showing
		// big performance improvement in composite views creation.
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
          mafInteractorPER *per = mafInteractorPER::SafeDownCast(e->GetMafObject());
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
      case DEVICE_ADD:
        m_InteractionManager->AddDeviceToTree((mafDevice *)e->GetMafObject());
      break;
      case DEVICE_REMOVE:
        m_InteractionManager->RemoveDeviceFromTree((mafDevice *)e->GetMafObject());
      break;
      case DEVICE_GET:
      break;
      case CREATE_STORAGE:
        CreateStorage(e);
      break;
      case COLLABORATE_ENABLE:
      {
        bool collaborate = e->GetBool();
        if (collaborate)
        {
          m_RemoteLogic->SetRemoteMouse(m_InteractionManager->GetRemoteMouseDevice());
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
					// trap the ABOUT_APPLICATION event and shows the about window with the application infos
					wxString message = m_AppTitle.GetCStr();
					message += _(" Application ");
					message += m_Revision;
					wxMessageBox(message, "About Application");
					mafLogMessage(wxString::Format("%s",m_Revision.GetCStr()));
				}
				break;

			case HELP_HOME:
				{
					if (m_HelpSettings)
					{
						m_HelpSettings->OpenHelpPage("HELP_HOME");
					}	
				}
				break;

			case GET_BUILD_HELP_GUI:
			{	
				int buildGui = -1;
				if (m_HelpSettings == NULL)
				{
					buildGui = false;
					e->SetArg(buildGui);
				}
				else
				{
					buildGui = m_HelpSettings->GetBuildHelpGui();
				}
				e->SetArg(buildGui);
			}
			break;

			case OPEN_HELP_PAGE:
			{
				// open help for entity
				wxString entity = e->GetString()->GetCStr();
				m_HelpSettings->OpenHelpPage(entity);
			}
			break;
			//-----from medLogic
			case ID_GET_FILENAME:
				{
					e->SetString(&(m_VMEManager->GetFileName()));
				}
				break;
			case MENU_FILE_SNAPSHOT:
				{
					mafString msfFilename = m_VMEManager->GetFileName();
					if (msfFilename.IsEmpty())
					{
						mafString dirName = mafGetDocumentsDirectory().c_str();

						m_VMEManager->SetDirName(dirName);
						this->OnFileSaveAs();
						this->OnEvent((mafEventBase*)&mafEvent(this,CAMERA_UPDATE));
						msfFilename = m_VMEManager->GetFileName();
					}

					wxString path, name, ext;
					wxSplitPath(msfFilename.GetCStr(),&path,&name,&ext);
					wxString imagesDirectoryName = path;
					imagesDirectoryName += "/images";
					if (!::wxDirExists(imagesDirectoryName))
					{
						::wxMkdir(imagesDirectoryName);
					}

					wxDir imagesDirectory(imagesDirectoryName);
					wxString filename;
					int i = 0;
					bool cont = imagesDirectory.GetFirst(&filename);
					while ( cont )
					{
						i++;
						cont = imagesDirectory.GetNext(&filename);
					}

					if (e->GetString() && !(e->GetString()->IsEmpty()))
					{
						mafString *imageFileName = new mafString();
						imageFileName->Append(imagesDirectoryName.c_str());
						imageFileName->Append("/");
						imageFileName->Append(e->GetString()->GetCStr());
						imageFileName->Append(wxString::Format("_%d",i));
						imageFileName->Append(".png");

						/*mafRWIBase::SafeDownCast(e->GetVtkObj())->SaveImage(imageFileName);*/
						e->SetString(imageFileName);

						wxString path,name,ext;
						wxSplitPath(imageFileName->GetCStr(),&path,&name,&ext);
						wxString oldWD = wxGetWorkingDirectory();
						wxSetWorkingDirectory(path);
						wxString command = "START  ";
						command = command + name+"."+ext;
						wxExecute( command );
						wxSetWorkingDirectory(oldWD);


					}
					else
					{
						wxString imageFileName = "";
						mafViewCompound *v = mafViewCompound::SafeDownCast(m_ViewManager->GetSelectedView());
						if (v)
						{
							imageFileName = imagesDirectoryName;
							imageFileName << "/";
							wxString tmpImageFile;
							tmpImageFile << v->GetLabel();
							tmpImageFile << i;
							tmpImageFile << ".png";

							tmpImageFile.Replace(" ","_");

							imageFileName << tmpImageFile;

							v->GetRWI()->SaveAllImages(imageFileName,v, m_ApplicationSettings->GetImageTypeId());

							wxMessageBox(_("Snapshot saved!"));
						}
						else
						{
							mafView *v = m_ViewManager->GetSelectedView();

							imageFileName = imagesDirectoryName;
							imageFileName << "/";
							wxString tmpImageFile;
							tmpImageFile << v->GetLabel();
							tmpImageFile << i;
							tmpImageFile << ".png";

							tmpImageFile.Replace(" ","_");

							imageFileName << tmpImageFile;

							if (v)
							{
								v->GetRWI()->SaveImage(imageFileName);
								wxMessageBox(_("Snapshot saved!"));
							}
						}

						wxString path,name,ext;
						wxSplitPath(imageFileName,&path,&name,&ext);
						wxString oldWD = wxGetWorkingDirectory();
						wxSetWorkingDirectory(path);
						wxString command = "START  ";
						command = command + name+"."+ext;
						wxShell( command );
						wxSetWorkingDirectory(oldWD);
					}

					OnEvent(&mafEvent(this,WIZARD_RUN_CONTINUE,true));
				}
				break;
			case MENU_WIZARD:
				//The event from the application menu
				if(m_WizardManager) 
					m_WizardManager->WizardRun(e->GetArg());
				break;
			case WIZARD_RUN_STARTING:
				{
					//Manage start event from the wizard lock window close button
					//and disabling toolbar
					mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
					if (c != NULL)
						c->SetAllowCloseWindow(false);
					WizardRunStarting();
				}
				break; 
			case WIZARD_RUN_TERMINATED:
				{
					//Manage end event from the wizard unlock window close button
					//and enabling toolbar
					mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
					if (c != NULL)
						c->SetAllowCloseWindow(true);
					WizardRunTerminated();
					UpdateFrameTitle();
				}
				break;
			case WIZARD_RUN_CONTINUE:
				{
					if (m_WizardManager && m_WizardRunning)
						m_WizardManager->WizardContinue(e->GetBool());
				}
				break;
			case WIZARD_UPDATE_WINDOW_TITLE:
				{
					UpdateFrameTitle();
				}
				break;
			case WIZARD_REQUIRED_VIEW:
				{
					//The wizard requires a specific view
					//searching on open views or open a new one
					mafView *view;
					const char *viewStr=e->GetString()->GetCStr();

					view=m_ViewManager->GetFromList(viewStr);
					if (view)
						m_ViewManager->Activate(view);
					else
						m_ViewManager->ViewCreate(viewStr);
				}
				break;
			case WIZARD_DELETE_VIEW:
				{
					mafView *view;
					const char *viewStr=e->GetString()->GetCStr();

					view=m_ViewManager->GetFromList(viewStr);
					if (view) 
					{
						mafGUIMDIChild *c = (mafGUIMDIChild *)view->GetFrame();
						m_ViewManager->ViewDelete(view);
						if (c != NULL)
							c->Destroy();
					}
				}
				break;
			case WIZARD_RUN_OP:
				{
					//Running an op required from the wizard
					mafString *tmp=e->GetString();
					mafLogMessage("wiz starting :%s",tmp->GetCStr());
					m_CancelledBeforeOpStarting=true;
					UpdateFrameTitle();
					m_OpManager->OpRun(*(e->GetString()));
					//If the op is started the value of m_CancelledBeforeOpStarting 
					//is changed by OP_RUN_STARTING event
					if (m_CancelledBeforeOpStarting)
					{
						m_CancelledBeforeOpStarting=false;
						m_WizardManager->WizardContinue(false);
					}

				}
				break;
			case WIZARD_OP_DELETE:
				{
					//Running an op required from the wizard
					m_CancelledBeforeOpStarting=true;
					UpdateFrameTitle();
					m_OpManager->OpRun(OP_DELETE);
					m_WizardManager->WizardContinue(true);
				}
				break;
			case WIZARD_OP_NEW:
				{
					//Running an op required from the wizard
					if(m_VMEManager)
						m_VMEManager->MSFNew();
					m_WizardManager->WizardContinue(true);
				}
				break;
			case WIZARD_PAUSE:
				{
					UpdateFrameTitle();
					m_OpManager->OpRun(e->GetOp());
				}
				break;
			case WIZARD_RELOAD_MSF:
				{
					UpdateFrameTitle();
					wxString file;
					file=m_VMEManager->GetFileName().GetCStr();
					if(file.IsEmpty())
					{
						mafLogMessage ("Reload requested whitout opened MSF");
						//continue wizard with error
						m_WizardManager->WizardContinue(false);
					}
					else
					{
						int opened=m_VMEManager->MSFOpen(file);
						//continue wizard after open operation
						m_WizardManager->WizardContinue(opened!=MAF_ERROR);
					}
				}
				break;
			case MENU_VIEW_TOOLBAR:
				m_Win->ShowDockPane("wizardgauge",!m_Win->DockPaneIsShown("wizardgauge") );
				m_Win->ShowDockPane("tmpwithtest",!m_Win->DockPaneIsShown("tmpwithtest") );
				m_Win->ShowDockPane("separator",!m_Win->DockPaneIsShown("separator") );
				mafLogicWithGUI::OnEvent(maf_event);
				break;
			case PROGRESSBAR_SHOW:
				{
					if (e->GetSender()==m_WizardManager)
					{
						m_WizardLabel->Enable();
						m_WizardGauge->Enable();
					}
					else
						mafLogicWithGUI::OnEvent(maf_event);
				}
				break;
			case PROGRESSBAR_HIDE:
				{
					if (e->GetSender()==m_WizardManager)
					{
						m_WizardGauge->SetValue(0);
						m_WizardGauge->Enable(false);
						m_WizardLabel->Enable(false);
					}
					else
						mafLogicWithGUI::OnEvent(maf_event);
				}
				break;
			case PROGRESSBAR_SET_VALUE:
				{
					if (e->GetSender()==m_WizardManager)
						m_WizardGauge->SetValue(e->GetArg());
					else
						mafLogicWithGUI::OnEvent(maf_event);
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
    if(m_VMEManager->AskConfirmAndSave())
	  {
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
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileOpen(const char *file_to_open)
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
  {
	  if(m_VMEManager->AskConfirmAndSave())
	  {
		  wxString file;
		  if (m_StorageSettings->GetStorageType() == mafGUISettingsStorage::HTTP)
		  {
			  if (file_to_open != NULL)
			  {
				  file = file_to_open;
			  }
			  else
			  {
				  mafGUIDialogRemoteFile remoteFile;
				  remoteFile.ShowModal();
				  file = remoteFile.GetFile().GetCStr();
				  mafString protocol;
				  if (IsRemote(file.c_str(),protocol))
				  {
					  m_VMEManager->SetHost(remoteFile.GetHost());
					  m_VMEManager->SetRemotePort(remoteFile.GetPort());//
					  m_VMEManager->SetUser(remoteFile.GetUser());
					  m_VMEManager->SetPassword(remoteFile.GetPassword());
				  }
			  }
		  }
		  else      
		  {
			  wxString wildc    = _("MAF Storage Format file (*." + m_Extension + ")|*." + m_Extension +"|Compressed file (*.z" + m_Extension + ")|*.z" + m_Extension + "");
			  if (file_to_open != NULL)
			  {
				  file = file_to_open;
			  }
			  else
			  {
				  file = mafGetOpenFile("", wildc).c_str();
			  }
		  }

		  if(file.IsEmpty() && m_WizardManager && m_WizardRunning)
			  m_WizardManager->WizardContinue(false);
		  else if(file.IsEmpty())
			  return;


		  int opened=m_VMEManager->MSFOpen(file);
		  //If there is a wizzard running we need to continue it after open operation
		  if (m_WizardManager && m_WizardRunning)
			  m_WizardManager->WizardContinue(opened!=MAF_ERROR);
	  }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileHistory(int menuId)
//----------------------------------------------------------------------------
{
	if(m_VMEManager) 
  {
    if(m_VMEManager->AskConfirmAndSave())
    {
      m_VMEManager->MSFOpen(menuId);
      UpdateFrameTitle();
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileSave()
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
  {
	  mafString save_default_folder = m_StorageSettings->GetDefaultSaveFolder();
	  save_default_folder.ParsePathName();
	  m_VMEManager->SetDirName(save_default_folder);
	  int saved=m_VMEManager->MSFSave();
	  //If there is a wizard running we need to continue it after save operation
	  if (m_WizardManager && m_WizardRunning)
		  m_WizardManager->WizardContinue(saved!=MAF_ERROR);
	  UpdateFrameTitle();
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileSaveAs()
//----------------------------------------------------------------------------
{
  if(m_VMEManager) 
  {
	  mafString save_default_folder = m_StorageSettings->GetDefaultSaveFolder();
	  save_default_folder.ParsePathName();
	  m_VMEManager->SetDirName(save_default_folder);
	  int saved=m_VMEManager->MSFSaveAs();
	  //If there is a wizard running we need to continue it after save operation
	  if (m_WizardManager && m_WizardRunning)
		  m_WizardManager->WizardContinue(saved!=MAF_ERROR);
	  UpdateFrameTitle();
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnQuit()
//----------------------------------------------------------------------------
{
  if (m_WizardManager && m_WizardRunning)
  {
    wxMessageBox(_("Please exit wizard before quit."), _("Wizard running"), wxOK|wxCENTER|wxICON_STOP);
	return;
  }
  if (m_OpManager && m_OpManager->Running())
  {
    wxMessageBox(_("Please exit operation before quit."), _("Operation running"), wxOK|wxCENTER|wxICON_STOP);
	return;
  }

  if (m_OpManager && m_OpManager->Running())
  {
    return;
  }

	if(m_ApplicationLayoutSettings->GetModifiedLayouts())
	{
		int answer = wxMessageBox
			(
			_("would you like to save your layout list ?"),
			_("Confirm"), 
			wxYES_NO|wxCANCEL|wxICON_QUESTION , m_Win
			);
		if(answer == wxYES) 
			m_ApplicationLayoutSettings->SaveApplicationLayout();
	}

  if(m_VMEManager)
  {
    m_Quitting = false;
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
      m_Quitting = answer != wxCANCEL;
    }
    else 
    {
      int answer = wxMessageBox(_("quit program ?"), _("Confirm"), wxYES_NO | wxICON_QUESTION , m_Win);
      m_Quitting = answer == wxYES;
    }
    if(!m_Quitting) 
      return;
  }

  mafGUIViewFrame::OnQuit();
  mafGUIMDIChild::OnQuit(); 
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
void mafLogicWithManagers::VmeDoubleClicked(mafEvent &e)
//----------------------------------------------------------------------------
{
  mafNode *node = e.GetVme();
  if (node)
  {
    mafLogMessage("Double click on %s", node->GetName());
  }
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
  //if a wizard manager was plugged we tell it about vme selection
  if(m_WizardManager) 
  {
    m_WizardManager->VmeSelected(vme);
  }
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
  if(m_SideBar && vme_in_tree)
    m_SideBar->VmeShow(vme,visibility);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeModified(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_PlugTimebar) UpdateTimeBounds();
  bool vme_in_tree = vme->IsVisible();
  if(m_SideBar && vme_in_tree)
    m_SideBar->VmeModified(vme);
	if(m_VMEManager) m_VMEManager->MSFModified(true);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
	if(m_VMEManager) 
    m_VMEManager->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeAdded(mafNode *vme)
//----------------------------------------------------------------------------
{
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
    
    m_ApplicationLayoutSettings->SetVisibilityVME(true);
    m_ApplicationLayoutSettings->ApplyTreeLayout();
    m_ApplicationLayoutSettings->SetVisibilityVME(false);
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
  if (m_ViewManager)
    m_ViewManager->CameraUpdate();
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
  EnableItem(MENU_FILE_SAVEAS,enable);
  EnableItem(MENU_FILE_MERGE,enable);
  EnableItem(MENU_FILE_QUIT,enable);
  EnableItem(wxID_FILE1,enable);
  EnableItem(wxID_FILE2,enable);
  EnableItem(wxID_FILE3,enable);
  EnableItem(wxID_FILE4,enable);
  EnableItem(wxID_FILE5,enable);
  EnableItem(wxID_FILE6,enable);
  EnableItem(wxID_FILE7,enable);
  EnableItem(wxID_FILE8,enable);
  EnableItem(wxID_FILE9,enable);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpShowGui(bool push_gui, mafGUIPanel *panel)
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
      mafGUIViewFrame *extern_view = new mafGUIViewFrame(m_Win, -1, v->GetLabel(), wxPoint(10,10),wxSize(800,600)/*, wxSIMPLE_BORDER|wxMAXIMIZE*/);
      extern_view->SetView(v);
      extern_view->SetListener(m_ViewManager);
      v->GetFrame()->SetWindowStyleFlag(m_ChildFrameStyle);
      v->SetListener(extern_view);
      v->SetFrame(extern_view);
      extern_view->Refresh();
    }
    else
    {
      // child views
      mafGUIMDIChild *c = new mafGUIMDIChild(m_Win,v);
      c->SetWindowStyleFlag(m_ChildFrameStyle);
      c->SetListener(m_ViewManager);
      v->SetFrame(c);
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::TimeSet(double t)
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
  {
    m_VMEManager->TimeSet(t);
  }
  if(m_ViewManager)
  {
    m_ViewManager->CameraUpdate(m_TimeBarSettings->GetPlayingInActiveViewport() != 0);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::UpdateTimeBounds()
//----------------------------------------------------------------------------
{
  double min, max; 
  if(m_VMEManager)
  {
    m_VMEManager->TimeGetBounds(&min, &max);
  }
  if(m_TimePanel)
  {
    m_TimePanel->SetBounds(min,max);
    m_Win->ShowDockPane("timebar", min<max);
  }
}
//----------------------------------------------------------------------------
std::vector<mafNode*> mafLogicWithManagers::VmeChoose(long vme_accept_function, long style, mafString title, bool multiSelect)
//----------------------------------------------------------------------------
{
  mafGUIVMEChooser vc(m_SideBar->GetTree(),title.GetCStr(), vme_accept_function, style, multiSelect);
  return vc.ShowChooserDialog();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeChooseMaterial(mafVME *vme, bool updateProperty)
//----------------------------------------------------------------------------
{
  if (m_MaterialChooser == NULL)
  {
    m_MaterialChooser = new mafGUIMaterialChooser();
  }
  if(m_MaterialChooser->ShowChooserDialog(vme))
  {
    this->m_ViewManager->PropertyUpdate(updateProperty);
    this->m_ViewManager->CameraUpdate();
    this->m_VMEManager->MSFModified(true);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeUpdateProperties(mafVME *vme, bool updatePropertyFromTag)
//----------------------------------------------------------------------------
{
  this->m_ViewManager->PropertyUpdate(updatePropertyFromTag);
  this->m_ViewManager->CameraUpdate();
  this->m_VMEManager->MSFModified(true);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::FindVME()
//----------------------------------------------------------------------------
{
  mafGUICheckTree *tree = m_SideBar->GetTree();
  mafGUIDialogFindVme fd(_("Find VME"));
  fd.SetTree(tree);
  fd.ShowModal();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewContextualMenu(bool vme_menu)
//----------------------------------------------------------------------------
{
  mafGUIContextualMenu *contextMenu = new mafGUIContextualMenu();
  contextMenu->SetListener(this);
  mafView *v = m_ViewManager->GetSelectedView();
  mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
  if(c != NULL)
    contextMenu->ShowContextualMenu(c,v,vme_menu);
  cppDEL(contextMenu);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::TreeContextualMenu(mafEvent &e)
//----------------------------------------------------------------------------
{
  mafGUITreeContextualMenu *contextMenu = new mafGUITreeContextualMenu();
  contextMenu->SetListener(m_ApplicationLayoutSettings);
  mafView *v = m_ViewManager->GetSelectedView();
  mafVME  *vme = (mafVME *)e.GetVme();
  bool vme_menu = e.GetBool();
  bool autosort = e.GetArg() != 0;
  contextMenu->CreateContextualMenu((mafGUICheckTree *)e.GetSender(),v,vme,vme_menu);
  contextMenu->ShowContextualMenu();
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

	  if (m_OpManager->Running())
		  m_OpManager->StopCurrentOperation();
  }
  OnQuit();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateStorage(mafEvent *e)
//----------------------------------------------------------------------------
{
  if (m_StorageSettings->UseRemoteStorage())
  {
    mafString cache_folder = m_StorageSettings->GetCacheFolder();
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
    storage->SetTmpFolder(cache_folder.GetCStr());
    
    //set default values for remote connection
    storage->SetHostName(m_StorageSettings->GetRemoteHostName());
    storage->SetRemotePort(m_StorageSettings->GetRemotePort());
    storage->SetUsername(m_StorageSettings->GetUserName());
    storage->SetPassword(m_StorageSettings->GetPassword());
    
    storage->GetRoot()->SetName("root");
    storage->SetListener(m_VMEManager);
    storage->Initialize();
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
    mafOpImporterVTK *vtkImporter = new mafOpImporterVTK("importer");
    vtkImporter->SetInput(m_VMEManager->GetRoot());
    vtkImporter->SetListener(m_OpManager);
    vtkImporter->SetFileName(filename.GetCStr());
    vtkImporter->ImportVTK();
    vtkImporter->OpDo();
    cppDEL(vtkImporter);
  }
  else if (ext == "stl")
  {
    mafOpImporterSTL *stlImporter = new mafOpImporterSTL("importer");
    stlImporter->SetInput(m_VMEManager->GetRoot());
    stlImporter->SetListener(m_OpManager);
    stlImporter->SetFileName(filename.GetCStr());
    stlImporter->ImportSTL();
    stlImporter->OpDo();
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

//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateWizardToolbar()
//----------------------------------------------------------------------------
{

	wxToolBar *serparatorBar = new wxToolBar(m_Win,-1,wxPoint(0,0),wxSize(-1,-1),wxTB_FLAT | wxTB_NODIVIDER );
	serparatorBar->SetMargins(0,0);
	serparatorBar->SetToolSeparation(2);
	serparatorBar->SetToolBitmapSize(wxSize(20,20));
	serparatorBar->AddSeparator();
	serparatorBar->Update();
	serparatorBar->Realize();

	m_WizardGauge;
	m_WizardGauge = new wxGauge(m_Win,-1, 100,wxDefaultPosition,wxDefaultSize,wxGA_SMOOTH);
	m_WizardGauge->SetForegroundColour( *wxBLUE );
	m_WizardGauge->Disable();



	wxWindow *tmp=new wxWindow(m_Win,-1, wxDefaultPosition,wxSize(50,20));

	m_WizardLabel = new wxStaticText(tmp, -1, " Wizard\n Progress",wxDefaultPosition,wxDefaultSize,wxALIGN_LEFT);
	//lab->Show(false);
	m_WizardLabel->Disable();


	m_Win->AddDockPane(m_WizardGauge,  wxPaneInfo()
		.Name("wizardgauge")
		//.Caption(wxT("ToolBar1"))
		.Top()
		.Layer(2)
		.ToolbarPane()
		.LeftDockable(false)
		.RightDockable(false)
		.Floatable(false)
		.Movable(false)
		.Gripper(false)
		);


	m_Win->AddDockPane(tmp,  wxPaneInfo()
		.Name("tmpwithtest")
		//.Caption(wxT("ToolBar2"))
		.Top()
		.Layer(2)
		.ToolbarPane()
		.LeftDockable(false)
		.RightDockable(false)
		.Floatable(false)
		.Movable(false)
		.Gripper(false)
		);

	m_Win->AddDockPane(serparatorBar,  wxPaneInfo()
		.Name("separator")
		//.Caption(wxT("ToolBar3"))
		.Top()
		.Layer(2)
		.ToolbarPane()
		.LeftDockable(false)
		.RightDockable(false)
		.Floatable(false)
		.Movable(false)
		.Gripper(false)
		);

	m_WizardGauge->Show(false);
}

//----------------------------------------------------------------------------
void mafLogicWithManagers::ConfigureWizardManager()
	//----------------------------------------------------------------------------
{
	//Setting wizard specific data
	if(m_UseWizardManager)
	{
		CreateWizardToolbar();
		m_WizardManager = new mafWizardManager();
		m_WizardManager->SetListener(this);
		m_WizardManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
	}
}

//----------------------------------------------------------------------------
void mafLogicWithManagers::WizardRunStarting()
//----------------------------------------------------------------------------
{
	//Disabling menu, toolbars and selection by interacion manager and sidebar
	m_WizardRunning=true;

	EnableMenuAndToolbar(false);
	// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
	if(m_InteractionManager) m_InteractionManager->EnableSelect(false);
#endif
	if(m_SideBar)    m_SideBar->EnableSelect(false);

}

//----------------------------------------------------------------------------
void mafLogicWithManagers::WizardRunTerminated()
//----------------------------------------------------------------------------
{
	//Enabling menu, toolbars and selection by interacion manager and sidebar
	m_WizardRunning=false;

	EnableMenuAndToolbar(true);
	// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
	if(m_InteractionManager) 
		m_InteractionManager->EnableSelect(true);
#endif
	if(m_SideBar)
		m_SideBar->EnableSelect(true);

}
