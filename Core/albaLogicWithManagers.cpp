/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaLogicWithManagers
 Authors: Silvano Imboden, Paolo Quadrani, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "albaDecl.h"

#include "albaLogicWithManagers.h"
#include <wx/config.h>

#include "albaView.h"
#include "albaViewCompound.h"

#include "albaViewManager.h"
#include "albaOp.h"
#include "albaOpManager.h"
#include "albaTagArray.h"
#include "albaTagItem.h"
#include "albaPrintManager.h"

#ifdef ALBA_USE_VTK
  #include "albaViewVTK.h"
  
  #include "albaInteractionManager.h"
  #include "albaInteractionBehavior.h"
  #include "albaPositionalEventRouter.h"
  #include "albaGUITreeContextualMenu.h"
  #include "albaGUIContextualMenu.h"
  #include "vtkCamera.h"
#endif

#include "albaSideBar.h"
#include "albaUser.h"
#include "albaGUIMainFrame.h"
#include "albaGUIViewArea.h"
#include "albaGUIViewPage.h"
#include "albaGUIVMETree.h"
#include "albaGUITimeBar.h"
#include "albaGUIViewFrame.h"
#include "albaGUIApplicationSettings.h"
#include "albaGUISettingsTimeBar.h"
#include "albaGUISettingsDialog.h"
#include "albaGUIAboutDialog.h""

#include "mmaApplicationLayout.h"
#include "albaDataVector.h"
#include "albaVMEStorage.h"
#include "albaWizardManager.h"
#include "albaRWIBase.h"
#include <wx/dir.h>
#include "wx/splash.h"
#include "albaVTKLog.h"
#include "vtkTimerLog.h"
#include "albaWXLog.h"
#include "albaVMERoot.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaHelpManager.h"
#include "albaServiceLocator.h"
#include "albaGUIPicButton.h"
#include "albaGUIButton.h"
#include "albaVMEGroup.h"
#include "vtkUnsignedCharArray.h"
#include "vtkDataSet.h"
#include "albaVMEImage.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataSetAttributes.h"
#include "albaSnapshotManager.h"
#include "albaOpSelect.h"
#include "albaGUIVMEChooserTree.h"
#include "albaGUI.h"
#include "albaGUISplashScreen.h"
#include "albaProsthesesDBManager.h"
#include "albaCursor.h"

#include "albaLogicStandardPlugs.h"

// VIEWS
#include "albaViewCompound.h"
#include "albaViewVTK.h"
#include "albaProgressBarHelper.h"


//----------------------------------------------------------------------------
albaLogicWithManagers::albaLogicWithManagers(albaGUIMainFrame *mainFrame/*=NULL*/)
{
	if (mainFrame == NULL)
		m_Win = new albaGUIMainFrame("alba", wxDefaultPosition, wxSize(800, 600));
	else
		m_Win = mainFrame;

	m_Win->SetListener(this);
	m_Win->GetViewArea()->SetListener(this);
	albaProgressBarHelper::SetMainWindow(m_Win);

	m_ViewListSeparator = NULL;
	m_ApplicationSettings = new albaGUIApplicationSettings(this);
	m_TimeBarSettings = NULL;

	m_ToolBar = NULL;
	m_MenuBar = NULL;

	m_LogToFile = m_ApplicationSettings->GetLogToFileStatus();
	m_Logger = NULL;
	m_VtkLog = NULL;

	m_AppTitle = "";

	m_Quitting = false;

	m_PlugMenu = true;
	m_PlugToolbar = true;
	m_PlugControlPanel = true;
	m_SidebarStyle = albaSideBar::DOUBLE_NOTEBOOK;
	m_PlugTimebar = false;
	m_PlugLogPanel = true;

	m_SideBar = NULL;
	m_UseVMEManager = true;
	m_UseViewManager = true;
	m_UseOpManager = true;
	m_UseInteractionManager = true;
	m_UseHelpManager = true;
	m_UseSnapshotManager = false;

	m_VMEManager = NULL;
	m_ViewManager = NULL;
	m_OpManager = NULL;
	m_InteractionManager = NULL;
	m_SnapshotManager = NULL;
	m_ProsthesisDBManager = NULL;
	m_ImportMenu = NULL;
	m_ExportMenu = NULL;
	m_OpMenu = NULL;
	m_ViewMenu = NULL;
	m_WindowMenu = NULL;
	m_RecentFileMenu = NULL;

	m_PrintSupport = new albaPrintManager();

	m_SettingsDialog = new albaGUISettingsDialog();
	m_AboutDialog = new albaGUIAboutDialog();

	m_Extension = "alba";

	m_UseWizardManager = false;
	m_WizardRunning = false;
	m_RunningOperation = false;
	m_WizardManager = NULL;

	m_User = new albaUser();

	m_ShowStorageSettings = false;
	m_SelectedLandmark = NULL;
	m_FatalExptionOccurred = false;

	m_OpeningMSF = false;

	m_AppLayout = NULL;

	m_AboutImage = "AlbaMasterAbout.bmp";
}
//----------------------------------------------------------------------------
albaLogicWithManagers::~albaLogicWithManagers()
{
	albaProgressBarHelper::SetMainWindow(NULL);

  //free mem
  if(m_WizardManager)
    delete m_WizardManager;

	albaCursor::DeleteCursors();
	
  // Managers are destruct in the OnClose
  cppDEL(m_User);
  cppDEL(m_PrintSupport);
  cppDEL(m_SettingsDialog);
	cppDEL(m_AboutDialog);
	cppDEL(m_ApplicationSettings);
	cppDEL(m_TimeBarSettings);
	albaDEL(m_AppLayout);
}

// APPLICATION ///////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaLogicWithManagers::Init(int argc, char **argv)
{

	
	if (m_WizardManager)
	{
		m_WizardManager->FillSettingDialog(m_SettingsDialog);
	}

	if (m_VMEManager)
	{
		if (argc > 1)
		{
			wxString file = argv[1];
			if (wxFileExists(file))
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
		m_OpManager->SetBuildNum(m_AppBuildNum);

		if (argc > 1)
		{
			albaString op_type = argv[1];
			albaString op_param = argv[2];
			for (int p = 3; p < argc; p++)
			{
				op_param += " ";
				op_param += argv[p];
			}
			m_OpManager->OpRun(op_type, (void *)op_param.GetCStr());
		}
	}

	

	// Init About Dialog
	m_AboutDialog->SetTitle(m_AppTitle);
	m_AboutDialog->SetAppBuildNum(m_AppBuildNum.GetCStr());
	m_AboutDialog->SetAlbaBuildNum(m_AlbaBuildNum.GetCStr());
	m_AboutDialog->SetWebSite("https://github.com/IOR-BIC/ALBA");
	m_AboutDialog->SetCitationPaper(m_CitationPaper.GetCStr());

	wxString imagePath = albaGetConfigDirectory().ToAscii();
	imagePath += "\\" + m_AboutImage;
	m_AboutDialog->SetImagePath(imagePath);

	// Create and Open View
	//ViewCreate(VIEW_START + 1);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::Configure()
{
	// Fill the SettingsDialog
	m_SettingsDialog->AddPage(m_ApplicationSettings->GetGui(), m_ApplicationSettings->GetLabel());

	if (m_PlugTimebar)
	{
		m_TimeBarSettings = new albaGUISettingsTimeBar(this);

		m_SettingsDialog->AddPage(m_TimeBarSettings->GetGui(), m_TimeBarSettings->GetLabel());
	}

	if (m_UseVMEManager)
	{
		m_VMEManager = new albaVMEManager();
		m_VMEManager->SetMsfFileExtension(m_Extension);
	}

	if (m_UseInteractionManager)
	{
		m_InteractionManager = new albaInteractionManager();
		m_InteractionManager->SetListener(this);

	}
	
	if (m_UseViewManager)
	{
		m_ViewManager = new albaViewManager();
		albaServiceLocator::SetViewManager(m_ViewManager);

		m_SettingsDialog->AddPage(m_ViewManager->GetSettingsGui(), "View");
	}

	if (m_UseOpManager)
	{
		m_OpManager = new albaOpManager();
		albaServiceLocator::SetOpManager(m_OpManager);
		m_OpManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
	}

	if (m_UseHelpManager)
	{
		m_HelpManager = new albaHelpManager();
		albaServiceLocator::SetHelpManager(m_HelpManager);
		//m_HelpManager->SetListener(this);
	}

	if (m_UseSnapshotManager)
	{
		m_SnapshotManager = new albaSnapshotManager();

		m_SettingsDialog->AddPage(m_SnapshotManager->GetSettingsGui(), "Snapshot");
	}

	ConfigureWizardManager();
}

// Window
//----------------------------------------------------------------------------
void albaLogicWithManagers::UpdateFrameTitle()
{
	//Special Window title management during wizards
	wxString title(m_AppTitle);
	if (m_WizardRunning)
		title += "   " + m_WizardManager->GetDescription();
	else if (m_VMEManager && !m_VMEManager->GetFileName().IsEmpty())
		title += "   " + m_VMEManager->GetFileName();

	// the name of the active view, as the MDI frame did with a maximized view
	albaGUIViewPage *page = GetActiveViewPage();
	if (page)
		title += " - [" + m_Win->GetViewArea()->GetViewCaption(page) + "]";
	m_Win->SetTitle(title);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::SetApplicationStamp(albaString &app_stamp)
{
	if (m_VMEManager)
	{
		m_VMEManager->SetApplicationStamp(app_stamp);
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::Show()
{
	if (m_PlugMenu)
		CreateMenu();

	FillMenus();
	CreateToolBarsAndPanels();
	CreateViewAreaMenu();

	m_AppTitle = m_Win->GetTitle().ToAscii();

	RestoreLayout();
	UpdateViewAreaMenu();

	m_Win->Show(true);

	// must be after the albaLogicWithGUI::Show(); because in that method is set the m_AppTitle var
	SetApplicationStamp((albaString)m_AppTitle);
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::PlugProsthesisDBManager(albaString passPhrase /*= ""*/)
{
	if (m_ProsthesisDBManager == NULL)
		m_ProsthesisDBManager = new albaProsthesesDBManager();

	if(!passPhrase.IsEmpty())
		m_ProsthesisDBManager->SetPassPhrase(passPhrase);

	m_ProsthesisDBManager->LoadDB();

	wxString imagesPath = albaGetConfigDirectory().ToAscii();
}

// Plug
//----------------------------------------------------------------------------
void albaLogicWithManagers::Plug(albaView* view, bool visibleInMenu)
{
  if(m_ViewManager) 
    m_ViewManager->ViewAdd(view, visibleInMenu);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::Plug(albaOp *op, wxString menuPath, bool canUndo, albaGUISettings *setting)
{
  if(m_OpManager) 
  {
    m_OpManager->OpAdd(op, menuPath, canUndo, setting);
  }
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::Plug( albaWizard *wizard, wxString menuPath /*= ""*/ )
{
	//Plugging the wizard    
	if(m_WizardManager)
	{
		m_WizardManager->WizardAdd(wizard, menuPath);
	}
	else 
	{
		albaLogMessage("Enable wizard manager to plug wizards"); 
	}
}

// Quit
//----------------------------------------------------------------------------
void albaLogicWithManagers::OnQuit()
{
	if (!m_FatalExptionOccurred && m_WizardManager && m_WizardRunning)
	{
		wxMessageBox(_("Please exit wizard before quit."), _("Wizard running"), wxOK | wxCENTER | wxICON_STOP);
		return;
	}
	if (!m_FatalExptionOccurred &&  m_OpManager && m_OpManager->Running())
	{
		wxMessageBox(_("Please exit operation before quit."), _("Operation running"), wxOK | wxCENTER | wxICON_STOP);
		return;
	}

	if (!m_FatalExptionOccurred && m_OpManager && m_OpManager->Running())
	{
		return;
	}

	if (!m_FatalExptionOccurred &&  m_VMEManager)
	{
		m_Quitting = false;
		if (m_VMEManager->MSFIsModified())
		{
			int answer = wxMessageBox
			(
				_("Would you like to save your work before quitting ?"),
				_("Confirm"),
				wxYES_NO | wxCANCEL | wxICON_QUESTION, m_Win
			);
			if (answer == wxYES)
				m_VMEManager->MSFSave();
			m_Quitting = answer != wxCANCEL;
		}
		else
		{
			m_Quitting = true;
		}
		if (!m_Quitting)
			return;
	}

	StoreLayout();

	albaGUIViewFrame::OnQuit();
	albaGUIViewPage::OnQuit();
	m_Win->OnQuit();

	if (m_SnapshotManager)
		cppDEL(m_SnapshotManager);

	cppDEL(m_VMEManager);
	if (albaServiceLocator::GetHelpManager() == m_HelpManager)
		albaServiceLocator::SetHelpManager(NULL);
	cppDEL(m_HelpManager);

	if (albaServiceLocator::GetViewManager() == m_ViewManager)
		albaServiceLocator::SetViewManager(NULL);
	cppDEL(m_ViewManager);

#ifdef ALBA_USE_VTK
	cppDEL(m_InteractionManager);
#endif
	if (albaServiceLocator::GetOpManager() == m_OpManager)
		albaServiceLocator::SetOpManager(NULL);
	cppDEL(m_OpManager);

	// must be deleted after m_VMEManager
	cppDEL(m_SideBar);

	albaYield();
	if (m_PlugLogPanel)
	{
		delete wxLog::SetActiveTarget(NULL);
	}
#ifdef ALBA_USE_VTK 
	vtkTimerLog::CleanupLog();
	vtkDEL(m_VtkLog);
#endif
	m_Win->Destroy();
}


// EVENT /////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaLogicWithManagers::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
			// ###############################################################
			// view area
		case MENU_VIEW_AREA_MAXIMIZE:
			m_Win->GetViewArea()->SetMaximized(!m_Win->GetViewArea()->IsMaximized());
			break;
		case MENU_VIEW_AREA_CLOSE:
			if (GetActiveViewPage())
				GetActiveViewPage()->Close();
			break;
		case VIEW_AREA_CHANGED:
			UpdateViewAreaMenu();
			break;
			// ###############################################################
			// commands related to the Dockable Panes
		case MENU_VIEW_LOGBAR:
			m_Win->ShowDockPane("logbar", !m_Win->DockPaneIsShown("logbar"));
			break;
		case MENU_VIEW_SIDEBAR:
			m_Win->ShowDockPane("sidebar", !m_Win->DockPaneIsShown("sidebar"));
			break;
		case MENU_VIEW_TIMEBAR:
			m_Win->ShowDockPane("timebar", !m_Win->DockPaneIsShown("timebar"));
			break;
			// ###############################################################
			// commands related to FILE MENU  
		case MENU_FILE_NEW:
			FileNew();
			break;
		case MENU_FILE_OPEN:
		{
			albaString *filename = e->GetString();
			if (filename)
				FileOpen((*filename).GetCStr());
			else
				FileOpen();
		}
		break;
		case IMPORT_FILE:
		{
			albaString *filename = e->GetString();
			if (filename)
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
			FileSave();
			break;
		case MENU_FILE_SAVEAS:
			FileSaveAs();
			break;
		case MENU_FILE_SAVEAS_LEGACY:
			FileSaveAs(true);
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
			m_SideBar->FindVME();
			break;
			// ###############################################################
			// commands related to OP
		case MENU_OP:
			if (m_OpManager)
			{
				m_OpManager->OpRun(e->GetArg());
			}
			break;
			// ###############################################################
			// commands related to VIEWS
		case VIEW_CREATE:
			ViewCreate(e->GetArg());
			break;
		case CAMERA_RESET:
			if (m_ViewManager) m_ViewManager->CameraReset();
			break;
		case CAMERA_FIT:
			if (m_ViewManager) m_ViewManager->CameraReset(true);
			break;
		case CAMERA_FLYTO:
			// currently albaInteraction is strictly dependent on VTK (marco)
#ifdef ALBA_USE_VTK
			if (m_InteractionManager) m_InteractionManager->CameraFlyToMode();  //modified by Marco. 15-9-2004 fly to with devices.
#endif
			break;
			// ###############################################################
			// commands related to interaction manager
		case ID_APP_SETTINGS:
			m_SettingsDialog->ShowModal();
			break;
		case ABOUT_APPLICATION:
		{
			m_AboutDialog->ShowModal();
		}
		break;
		case HELP_HOME:
		{
			m_HelpManager->ShowHelp();
		}
		break;
		//-----from medLogic
		case MENU_FILE_SNAPSHOT:
			CreateSnapshot();
			break;
		case MENU_FILE_MANAGE_SNAPSHOT:
		{
			if (m_SnapshotManager && m_VMEManager)
				m_SnapshotManager->ShowSnapshotPreview();
		}
		break;
		case MENU_WIZARD:
			//The event from the application menu
			if (m_WizardManager)
				m_WizardManager->WizardRun(e->GetArg());
			break;
		case MENU_VIEW_TOOLBAR:
			m_Win->ShowDockPane("wizardgauge", !m_Win->DockPaneIsShown("wizardgauge"));
			m_Win->ShowDockPane("wizardlabel", !m_Win->DockPaneIsShown("wizardlabel"));
			m_Win->ShowDockPane("wizardseparator", !m_Win->DockPaneIsShown("wizardseparator"));
			m_Win->ShowDockPane("toolbar", !m_Win->DockPaneIsShown("toolbar"));
			break;

		default:
			if (e->GetId() >= MENU_VIEW_AREA_LIST_FIRST && e->GetId() <= MENU_VIEW_AREA_LIST_LAST)
			{
				std::vector<albaGUIViewPage *> pages = m_Win->GetViewArea()->GetPages();
				int index = e->GetId() - MENU_VIEW_AREA_LIST_FIRST;
				if (index < (int)pages.size())
					m_Win->GetViewArea()->ActivateView(pages[index]);
				break;
			}
#ifdef _DEBUG
			e->Log();
#endif 
			break;
		} // end switch case
	} // end if SafeDowncast
}

// FILE //////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
int albaLogicWithManagers::FileNew()
{
	int result = ALBA_ERROR;

	if (m_OpManager)
		m_OpManager->VmeSelected(NULL);

	if(m_VMEManager)
  {
    if(m_VMEManager->AskConfirmAndSave())
	  {
		  m_VMEManager->MSFNew();
			result = ALBA_OK;
	  }
  }

	m_Win->SetTitle(m_AppTitle);
	return result;
}
//----------------------------------------------------------------------------
int albaLogicWithManagers::FileOpen(const char *file_to_open)
{
	int opened = ALBA_ERROR;

	if (m_VMEManager)
	{
		m_OpeningMSF = true;
		if (m_VMEManager->AskConfirmAndSave())
		{
			wxString file;

			albaString wildc;
			const	char *ext = m_Extension;
			wildc = albaString::Format("All Supported File (*.%s;*.z%s;msf;zmsf)|*.%s;*.%s;*.msf;*.zmsf", ext, ext, ext, ext);
			wildc += albaString::Format("|Alba Project File (*.%s)|*.%s", ext, ext);
			wildc += albaString::Format("|Alba Compressed File (*.z%s)|*.z%s", ext, ext);
			wildc += albaString::Format("|MAF Project File (*.msf)|*.msf");
			wildc += albaString::Format("|MAF Compressed File (*.zmsf)|*.zmsf", ext, ext);

			if (file_to_open != NULL)
			{
				file = file_to_open;
			}
			else
			{
				wxString lastFolder = albaGetLastUserFolder().ToAscii();
				file = albaGetOpenFile(lastFolder, wildc).ToAscii();
			}
			
			if (!file.IsEmpty())
				opened = m_VMEManager->MSFOpen(file);
		}
		m_OpeningMSF = false;
		UpdateFrameTitle();
	}
	return opened;
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::OnFileHistory(int menuId)
{
	if(m_VMEManager) 
  {
		m_OpeningMSF = true;
    if(m_VMEManager->AskConfirmAndSave())
    {
      m_VMEManager->MSFOpen(menuId);
      UpdateFrameTitle();
    }
		m_OpeningMSF = false;
  }
}
//----------------------------------------------------------------------------
int albaLogicWithManagers::FileSave()
{
	int saved = ALBA_ERROR;
  if(m_VMEManager)
  {
		albaString save_default_folder = albaGetLastUserFolder();
	  save_default_folder.ParsePathName();
	  m_VMEManager->SetDirName(save_default_folder);
	  saved=m_VMEManager->MSFSave();
	  UpdateFrameTitle();
  }
	return saved;
}
//----------------------------------------------------------------------------
int albaLogicWithManagers::FileSaveAs(bool legacy)
{
	int saved = ALBA_ERROR;
  if(m_VMEManager)
  {
	  albaString save_default_folder = albaGetLastUserFolder();
	  save_default_folder.ParsePathName();
	  m_VMEManager->SetDirName(save_default_folder);
	  saved=m_VMEManager->MSFSaveAs(legacy);
	  UpdateFrameTitle();
  }
	return saved;
}
//----------------------------------------------------------------------------
int albaLogicWithManagers::FileReload()
{
	UpdateFrameTitle();
	if (m_VMEManager == NULL)
		return ALBA_ERROR;

	wxString file = m_VMEManager->GetFileName().GetCStr();
	if (file.IsEmpty())
	{
		albaLogMessage("Reload requested whitout opened MSF");
		return ALBA_ERROR;
	}
	return m_VMEManager->MSFOpen(file);
}
//----------------------------------------------------------------------------
int albaLogicWithManagers::CreateSnapshot()
{
	if (m_SnapshotManager == NULL || m_VMEManager == NULL || m_ViewManager == NULL)
		return ALBA_ERROR;

	m_SnapshotManager->CreateSnapshot(m_VMEManager->GetRoot(), m_ViewManager->GetSelectedView());
	return ALBA_OK;
}

// VME ///////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeShow(albaVME *vme, bool visibility)
{
	if (!vme)
		return;

	if (m_ViewManager)
		m_ViewManager->EnableCameraUpdateSkip();

	if (m_ViewManager)
	{
		albaVMELandmarkCloud *lmc = albaVMELandmarkCloud::SafeDownCast(vme);
		albaVMELandmark *lm = albaVMELandmark::SafeDownCast(vme);
		if (lmc)
		{
			ShowLandmarkCloud(lmc, visibility);
		}
		else if (lm)
		{
			ShowLandmark(lm, visibility);
		}
		else
			m_ViewManager->VmeShow(vme, visibility);
	}

	ShowInSideBar(vme, visibility);

	if (m_ViewManager)
	{
		m_ViewManager->CameraUpdate();
		m_ViewManager->DisableCameraUpdateSkip();
	}
	
	albaYield();
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeModified(albaVME *vme)
{
	if (!vme) 
		return;

	if (m_VMEManager->GetRoot()->IsInTree(vme))
	{
		if (m_PlugTimebar) UpdateTimeBounds();
		bool vme_in_tree = vme->IsVisible();

		if (m_SideBar && vme_in_tree)
			m_SideBar->VmeModified(vme);

		if (m_VMEManager)
			m_VMEManager->MSFModified(true);

		//if a wizard manager was plugged we tell it about vme selection
		if (m_WizardManager)
			m_WizardManager->VmeModified(vme);

		if (m_OpManager)
			m_OpManager->VmeModified(vme);

		if (!m_PlugTimebar && vme != NULL && vme->IsAnimated())
			m_Win->ShowDockPane("timebar", !m_Win->DockPaneIsShown("timebar"));
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeAdd(albaVME *vme)
{
	if (!vme) 
		return;

	if(m_VMEManager) 
    m_VMEManager->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeAdded(albaVME *vme)
{
	if (!vme) 
		return;

  if(m_ViewManager)
    m_ViewManager->VmeAdd(vme);

  bool vme_in_tree =	!vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") || 
											(vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") && 
											vme->GetTagArray()->GetTag("VISIBLE_IN_THE_TREE")->GetValueAsDouble() != 0);
 
	if(m_SideBar && vme_in_tree)
    m_SideBar->VmeAdd(vme);

  if(m_PlugTimebar)
    UpdateTimeBounds();

	EnableMenuAndToolbar();
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeRemove(albaVME *vme)
{
	if (!vme) 
		return;

  if(m_VMEManager)
    m_VMEManager->VmeRemove(vme);

  if(m_PlugTimebar)
    UpdateTimeBounds();

  if (m_ViewManager)
    m_ViewManager->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeRemoving(albaVME *vme)
{
	if (!vme) 
		return;

  bool vme_in_tree = true;
  vme_in_tree = !vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") || 
    (vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") && vme->GetTagArray()->GetTag("VISIBLE_IN_THE_TREE")->GetValueAsDouble() != 0);
  if(m_SideBar && vme_in_tree)
    m_SideBar->VmeRemove(vme);
	if(m_ViewManager)
    m_ViewManager->VmeRemove(vme);
	if (m_OpManager)
		m_OpManager->VmeRemove(vme);

	if (m_SelectedLandmark == vme)
		m_SelectedLandmark = NULL;
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeRemoved()
{
	EnableMenuAndToolbar();
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeVisualModeChanged(albaVME * vme)
{
	if (vme)
	{
		VmeShow(vme, false);
		VmeShow(vme, true);
	}
}
//----------------------------------------------------------------------------
albaVME *albaLogicWithManagers::ChooseVME(albaString title, albaVMEAcceptFunction acceptFunction, albaVME *selected)
{
	if (m_SideBar == NULL)
		return NULL;

	std::vector<albaVME*> chosen = m_SideBar->VmeChoose(acceptFunction, REPRESENTATION_AS_TREE, title, false, selected);
	return chosen.empty() ? NULL : chosen[0];
}
//----------------------------------------------------------------------------
std::vector<albaVME*> albaLogicWithManagers::ChooseVMEs(albaString title, albaVMEAcceptFunction acceptFunction, albaVME *selected)
{
	if (m_SideBar == NULL)
		return std::vector<albaVME*>();

	return m_SideBar->VmeChoose(acceptFunction, REPRESENTATION_AS_TREE, title, true, selected);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeSelect(albaVME *vme)
{
	if (m_ViewManager)
		m_ViewManager->EnableCameraUpdateSkip();
	if (vme != NULL && m_OpManager && m_OpManager->GetSelectedVme()!=vme)
	{
		m_OpManager->OpSelect(vme);

		EnableMenuAndToolbar();

		if (m_ViewManager)
			m_ViewManager->CameraUpdate();
	}
	if (m_ViewManager)
		m_ViewManager->DisableCameraUpdateSkip();
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeSelected(albaVME *vme)
{
	//if a wizard manager was plugged we tell it about vme selection
	if (m_WizardManager)
	{
		m_WizardManager->VmeSelected(vme);
	}

	if (m_ViewManager)
	{
		if (m_SelectedLandmark)
		{
			SelectLandmark(m_SelectedLandmark, false);
			m_SideBar->VmeShow(m_SelectedLandmark, false);
		}

		if (albaVMELandmark::SafeDownCast(vme))
		{
			SelectLandmark((albaVMELandmark *)vme, true);
			m_SelectedLandmark = (albaVMELandmark *)vme;
		}
		else
			m_SelectedLandmark = NULL;

		m_ViewManager->VmeSelect(vme);
	}

	if (m_OpManager)	m_OpManager->VmeSelected(vme);
	if (m_SideBar)
		m_SideBar->VmeSelected(vme);

	EnableMenuAndToolbar();
}



//----------------------------------------------------------------------------
void albaLogicWithManagers::ShowInSideBar(albaVME * vme, bool visibility)
{
	bool vme_in_tree = vme->IsVisible(); //check VisibleToTraverse flag.
	if (m_SideBar && vme_in_tree)
		m_SideBar->VmeShow(vme, visibility);
}

// Landmark
//----------------------------------------------------------------------------
void albaLogicWithManagers::SelectLandmark(albaVMELandmark *lm, bool select)
{
	if (!select)
		for (albaView * view = m_ViewManager->GetList(); view; view = view->m_Next)
			view->VmeShow(lm, false);
	else
	{
		albaVMELandmarkCloud *lmParent = albaVMELandmarkCloud::SafeDownCast(lm->GetParent());

		if (lmParent && lmParent->IsLandmarkShow((albaVMELandmark *)lm))

			for (albaView * view = m_ViewManager->GetList(); view; view = view->m_Next)
				if (view->GetNodeStatus(lmParent) == NODE_VISIBLE_ON)
					view->VmeShow(lm, true);
				else
					view->VmeShow(lm, false);
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::ShowLandmarkCloud(albaVMELandmarkCloud * lmc, bool visibility)
{
	if (visibility)
		lmc->ShowAllLandmarks();

	for (int i = 0; i < lmc->GetNumberOfLandmarks(); i++)
	{
		ShowInSideBar(lmc->GetLandmark(i), visibility);
	}

	m_ViewManager->VmeShow(lmc, visibility);

	if (m_SelectedLandmark && m_SelectedLandmark->GetParent() == lmc)
		SelectLandmark(m_SelectedLandmark, true);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::ShowLandmark(albaVMELandmark * lm, bool visibility)
{
	albaVMELandmarkCloud *lmParent = albaVMELandmarkCloud::SafeDownCast(lm->GetParent());
	if (!lmParent)
		return;

	albaPipe *lmPipe = NULL;
	//show of lm shows also the lmc
	//hide of last lm hides also the lmc
	if (visibility || (!visibility && lmParent->GetLandmarkShowNumber() == 0))
	{
		albaView * selectedView = m_ViewManager->GetSelectedView();
		if (albaViewCompound *compView = albaViewCompound::SafeDownCast(selectedView))
		{
			for (int i = 0; i < compView->GetNumberOfSubView(); i++)
			{
				lmPipe = compView->GetSubView(i)->GetNodePipe(lmParent);
				if (lmPipe)
					break;
			}
		}
		else if (selectedView)
		{
			lmPipe = selectedView->GetNodePipe(lmParent);
		}
		
		//if the lmc is not show and we want to show a lm we show only this landmark
		if (!lmPipe && visibility)
			lmParent->ShowAllLandmarks(false);

		m_ViewManager->VmeShow(lmParent, visibility);
		ShowInSideBar(lmParent, visibility);

		//If the selected landmark is part of lmParent and is different than current lm
		//the landmark is hided in all views and we need to deselect it
		if (m_SelectedLandmark && m_SelectedLandmark != lm && lmParent->GetLandmarkIndex(m_SelectedLandmark) >= 0)
		{
			SelectLandmark(m_SelectedLandmark, false);
		}
	}

	lmParent->ShowLandmark(lm, visibility);

	if (m_SelectedLandmark == lm)
		SelectLandmark(lm, visibility);
}


// OPERATION /////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaLogicWithManagers::OpRunStarting()
{
	m_CancelledBeforeOpStarting = false;
	albaGUIViewPage *c = GetActiveViewPage();
	if (c != NULL)
		c->SetAllowCloseWindow(false);

	m_RunningOperation = true;
  EnableMenuAndToolbar();

  if(m_InteractionManager) 
		m_InteractionManager->EnableSelect(false);

  if(m_SideBar)    
		m_SideBar->EnableSelect(false);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::OpRunTerminated(bool runOk)
{
	m_RunningOperation = false;

	if (m_WizardManager && m_WizardRunning)
	{
		if (!albaOpSelect::SafeDownCast(m_OpManager->GetRunningOperation()))
			m_WizardManager->WizardContinue(runOk);
		return;
	}

	albaGUIViewPage *c = GetActiveViewPage();
	if (c != NULL)
		c->SetAllowCloseWindow(true);

  EnableMenuAndToolbar();

  if(m_InteractionManager) 
    m_InteractionManager->EnableSelect(true);

  if(m_SideBar)
    m_SideBar->EnableSelect(true);

	if(m_ApplicationSettings->IsAutoSaveOn() && !m_OpeningMSF)
		FileSave();
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::OpShowGui(albaGUIPanel *panel)
{
	if (m_SideBar)
		m_SideBar->OpShowGui(panel);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::OpHideGui()
{
	if (m_SideBar)
		m_SideBar->OpHideGui();
}

// VIEW //////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaLogicWithManagers::ViewCreate(int viewId)
{
	if(m_ViewManager)
  {
    albaView* v = m_ViewManager->ViewCreate(viewId);
  }
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::CreateViewFrame(albaView *v)
{
	if (v)
	{
		albaGUIViewPage *page = new albaGUIViewPage(m_Win->GetViewArea(), v);
		v->SetFrame(page);
		m_Win->GetViewArea()->AddView(page);
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::ViewDeleted(albaView *view)
{
	if (m_PlugControlPanel)
		m_SideBar->ViewDeleted(view);
	EnableMenuAndToolbar();

	if (m_OpManager)
		m_OpManager->RefreshMenu();
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::CreateViewAreaMenu()
{
	if (!m_WindowMenu)
		return;
	if (m_WindowMenu->GetMenuItemCount() > 0)
		m_WindowMenu->AppendSeparator();
	m_WindowMenu->AppendCheckItem(MENU_VIEW_AREA_MAXIMIZE, _("Maximized view\tCtrl+M"));
	m_WindowMenu->Append(MENU_VIEW_AREA_CLOSE, _("Close view\tCtrl+F4"));
}
//----------------------------------------------------------------------------
albaGUIViewPage *albaLogicWithManagers::GetActiveViewPage()
{
	return m_Win ? m_Win->GetViewArea()->GetActivePage() : NULL;
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::UpdateViewAreaMenu()
{
	albaGUIViewArea *area = m_Win->GetViewArea();
	albaGUIViewPage *active = area->GetActivePage();
	if (m_ToolBar)
		m_ToolBar->ToggleTool(MENU_VIEW_AREA_MAXIMIZE, area->IsMaximized());
	UpdateFrameTitle();
	if (!m_WindowMenu || !m_MenuBar)
		return;
	m_MenuBar->Check(MENU_VIEW_AREA_MAXIMIZE, area->IsMaximized());
	m_MenuBar->Enable(MENU_VIEW_AREA_CLOSE, active != NULL);

	// The list of the open views follows the Close view item, after a separator
	for (int id = MENU_VIEW_AREA_LIST_FIRST; id <= MENU_VIEW_AREA_LIST_LAST; id++)
		if (m_WindowMenu->FindChildItem(id))
			m_WindowMenu->Destroy(id);
	if (m_ViewListSeparator)
		m_WindowMenu->Destroy(m_ViewListSeparator);
	m_ViewListSeparator = NULL;

	size_t pos;
	std::vector<albaGUIViewPage *> pages = area->GetPages();
	if (pages.empty() || !m_WindowMenu->FindChildItem(MENU_VIEW_AREA_CLOSE, &pos))
		return;
	m_ViewListSeparator = m_WindowMenu->InsertSeparator(++pos);
	int count = wxMin((int)pages.size(), MENU_VIEW_AREA_LIST_LAST - MENU_VIEW_AREA_LIST_FIRST + 1);
	for (int i = 0; i < count; i++)
	{
		wxString caption = area->GetViewCaption(pages[i]);
		wxString label = i < 9 ? wxString::Format("&%d %s", i + 1, caption) : caption;
		m_WindowMenu->InsertCheckItem(pos + 1 + i, MENU_VIEW_AREA_LIST_FIRST + i, label)->Check(pages[i] == active);
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::SelectView(albaView *view)
{
	albaView *oldView = m_ViewManager ? m_ViewManager->GetSelectedView() : NULL;
	if (m_ViewManager && view)
		m_ViewManager->ViewSelected(view);

	if (view == oldView)
		return;

	if (m_SideBar)
		m_SideBar->ViewSelect(view);

	if (m_OpManager)
	{
		// needed to update all the operations that will be enabled on View Creation
		if (!m_OpManager->Running())
			m_OpManager->VmeSelected(m_OpManager->GetSelectedVme());

		albaGUIViewPage *c = GetActiveViewPage();
		if (c != NULL)
			c->SetAllowCloseWindow(!m_OpManager->Running());
	}
	EnableMenuAndToolbar();
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::ShowViewContextualMenu(bool vmeMenu)
{
	albaGUIContextualMenu *contextMenu = new albaGUIContextualMenu();
	albaView *v = m_ViewManager->GetSelectedView();
	albaGUIViewPage *page = GetActiveViewPage();
	if (page != NULL && v != NULL)
		contextMenu->ShowContextualMenu(page, v, vmeMenu);
	cppDEL(contextMenu);
}

// WIZARD ////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaLogicWithManagers::CreateWizardToolbar()
{
	wxToolBar *serparatorBar = new wxToolBar(m_Win, -1, wxPoint(0, 0), wxSize(-1, -1), wxTB_FLAT | wxTB_NODIVIDER);
	serparatorBar->SetMargins(0, 0);
	serparatorBar->SetToolSeparation(2);
	serparatorBar->SetToolBitmapSize(wxSize(20, 20));
	serparatorBar->AddSeparator();
	serparatorBar->Update();
	serparatorBar->Realize();

	m_WizardGauge = new wxGauge(m_Win, -1, 100, wxDefaultPosition, wxDefaultSize, wxGA_SMOOTH);
	m_WizardGauge->SetForegroundColour(*wxBLUE);
	m_WizardGauge->Disable();

	wxWindow *labelWin = new wxWindow(m_Win, -1, wxDefaultPosition, wxSize(50, 20));

	m_WizardLabel = new wxStaticText(labelWin, -1, " Wizard\n Progress", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	//lab->Show(false);
	m_WizardLabel->Disable();
	m_WizardManager->SetProgressGauge(m_WizardGauge, m_WizardLabel);

	m_Win->AddDockPane(m_WizardGauge, wxPaneInfo()
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

	m_Win->AddDockPane(labelWin, wxPaneInfo()
		.Name("wizardlabel")
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

	m_Win->AddDockPane(serparatorBar, wxPaneInfo()
		.Name("wizardseparator")
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
}
albaProsthesesDBManager * albaLogicWithManagers::GetProsthesesDBManager()
{
	return m_ProsthesisDBManager;
}


//----------------------------------------------------------------------------
void albaLogicWithManagers::PlugStandardOperations()
{
	albaLogicStandardPlugs::PlugStandardOperations(this);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::PlugStandardViews()
{
	albaLogicStandardPlugs::PlugStandardViews(this);
}

//----------------------------------------------------------------------------
bool albaLogicWithManagers::IsVMEExpanded(albaVME *vme)
{
	return m_SideBar->IsVMEExpanded(vme);
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::CollapseVME(albaVME *vme)
{
	m_SideBar->CollapseVME(vme);
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::ExpandVME(albaVME *vme)
{
	m_SideBar->ExpandVME(vme);
}

//----------------------------------------------------------------------------
const char* albaLogicWithManagers::GetAppBuildNum()
{
	return m_AppBuildNum;
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::SetAppBuildNum(char* buildNum)
{
	m_AppBuildNum = buildNum;
}

//----------------------------------------------------------------------------
const char* albaLogicWithManagers::GetAlbaBuildNum()
{
	return m_AlbaBuildNum;
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::SetAlbaBuildNum(char* buildNum)
{
	m_AlbaBuildNum = buildNum;
}

//----------------------------------------------------------------------------
bool albaLogicWithManagers::IsTimeVaringEnabled()
{
	return m_PlugTimebar;
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::ConfigureWizardManager()
{
	//Setting wizard specific data
	if(m_UseWizardManager)
	{
		m_WizardManager = new albaWizardManager();
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::WizardRunStarting()
{
	//lock the close button of the view
	albaGUIViewPage *c = GetActiveViewPage();
	if (c != NULL)
		c->SetAllowCloseWindow(false);

	//Disabling menu, toolbars and selection by interacion manager and sidebar
	m_WizardRunning = true;

	EnableMenuAndToolbar();
	// currently albaInteraction is strictly dependent on VTK (marco)
#ifdef ALBA_USE_VTK
	if (m_InteractionManager) m_InteractionManager->EnableSelect(false);
#endif
	if (m_SideBar)	m_SideBar->EnableSelect(false);

}
//----------------------------------------------------------------------------
void albaLogicWithManagers::WizardRunTerminated()
{
	//unlock the close button of the view
	albaGUIViewPage *c = GetActiveViewPage();
	if (c != NULL)
		c->SetAllowCloseWindow(true);

	//Enabling menu, toolbars and selection by interacion manager and sidebar
	m_WizardRunning = false;
	
	EnableMenuAndToolbar();
	// currently albaInteraction is strictly dependent on VTK (marco)
#ifdef ALBA_USE_VTK
	if (m_InteractionManager)
		m_InteractionManager->EnableSelect(true);
#endif
	if (m_SideBar)
		m_SideBar->EnableSelect(true);

	UpdateFrameTitle();
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::WizardRunOp(const char *opName)
{
	albaLogMessage("wiz starting :%s", opName);
	albaString op = opName;
	m_CancelledBeforeOpStarting = true;
	UpdateFrameTitle();
	m_OpManager->OpRun(op);
	WizardContinueIfOpNotStarted();
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::WizardRunOp(albaOp *op)
{
	m_CancelledBeforeOpStarting = true;
	UpdateFrameTitle();
	m_OpManager->OpRun(op);
	WizardContinueIfOpNotStarted();
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::WizardRunOp(int opId)
{
	m_CancelledBeforeOpStarting = true;
	UpdateFrameTitle();
	m_OpManager->OpRun(opId);
	WizardContinueIfOpNotStarted();
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::WizardContinueIfOpNotStarted()
{
	//If the op is started the value of m_CancelledBeforeOpStarting is changed by OpRunStarting
	if (m_CancelledBeforeOpStarting)
	{
		m_CancelledBeforeOpStarting = false;
		m_WizardManager->WizardContinue(false);
	}
}

// MENU-TOOLBAR //////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaLogicWithManagers::CreateMenu()
{
#include "pic/menu/EDIT_COPY.xpm"
#include "pic/menu/EDIT_CUT.xpm"
#include "pic/menu/EDIT_DELETE.xpm"
#include "pic/menu/EDIT_PASTE.xpm"
#include "pic/menu/EDIT_RENAME.xpm"
#include "pic/menu/EDIT_REPARENT.xpm"
#include "pic/menu/EDIT_FIND.xpm"
#include "pic/menu/EDIT_REDO.xpm"
#include "pic/menu/EDIT_SETTINGS.xpm"
#include "pic/menu/EDIT_UNDO.xpm"
#include "pic/menu/FILE_EXIT.xpm"
#include "pic/menu/FILE_NEW.xpm"
#include "pic/menu/FILE_OPEN.xpm"
#include "pic/menu/FILE_PRINT.xpm"
#include "pic/menu/FILE_PRINT_PREVIEW.xpm"
#include "pic/menu/FILE_SAVE.xpm"
#include "pic/menu/HELP_ABOUT.xpm"
#include "pic/menu/HELP_HELP.xpm"
#include "pic/menu/HELP_LICENCE.xpm"

	m_MenuBar = new wxMenuBar;
	wxMenu *file_menu = new wxMenu;
	albaGUI::AddMenuItem(file_menu, MENU_FILE_NEW, _("&New  \tCtrl+N"), FILE_NEW_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_OPEN, _("&Open   \tCtrl+O"), FILE_OPEN_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_SAVE, _("&Save  \tCtrl+S"), FILE_SAVE_xpm);
	file_menu->Append(MENU_FILE_SAVEAS, _("Save &As  \tCtrl+Shift+S"));
	file_menu->Append(MENU_FILE_SAVEAS_LEGACY, _("Save &As  (Legacy mode)"));

	m_ImportMenu = new wxMenu;
	file_menu->AppendSeparator();
	file_menu->Append(0, _("Import"), m_ImportMenu);

	m_ExportMenu = new wxMenu;
	file_menu->Append(0, _("Export"), m_ExportMenu);

	// Print menu item
	file_menu->AppendSeparator();
	albaGUI::AddMenuItem(file_menu, MENU_FILE_PRINT, _("&Print  \tCtrl+P"), FILE_PRINT_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_PRINT_PREVIEW, _("Print Preview"), FILE_PRINT_PREVIEW_xpm);
	file_menu->Append(MENU_FILE_PRINT_SETUP, _("Printer Setup"));
	file_menu->Append(MENU_FILE_PRINT_PAGE_SETUP, _("Page Setup"));

	m_RecentFileMenu = new wxMenu;
	file_menu->AppendSeparator();
	file_menu->Append(0, _("Recent Files"), m_RecentFileMenu);

	file_menu->AppendSeparator();
	albaGUI::AddMenuItem(file_menu, MENU_FILE_QUIT, _("&Quit  \tCtrl+Q"), FILE_EXIT_xpm);

	m_MenuBar->Append(file_menu, _("&File"));

	wxMenu    *edit_menu = new wxMenu;

	albaGUI::AddMenuItem(edit_menu, OP_UNDO, _("Undo  \tCtrl+Z"), EDIT_UNDO_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_REDO, _("Redo  \tCtrl+Shift+Z"), EDIT_REDO_xpm);

	edit_menu->AppendSeparator();
	albaGUI::AddMenuItem(edit_menu, OP_CUT, _("Cut   \tCtrl+Shift+X"), EDIT_CUT_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_COPY, _("Copy  \tCtrl+Shift+C"), EDIT_COPY_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_PASTE, _("Paste \tCtrl+Shift+V"), EDIT_PASTE_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_DELETE, _("Delete  \tCtrl+Shift+Del"), EDIT_DELETE_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_RENAME, _("Rename  \tCtrl+Shift+N"), EDIT_RENAME_xpm);

	edit_menu->AppendSeparator();
	albaGUI::AddMenuItem(edit_menu, OP_REPARENT, _("Reparent to... \tCtrl+Shift+R"), EDIT_REPARENT_xpm);
	albaGUI::AddMenuItem(edit_menu, MENU_EDIT_FIND_VME, _("Find VME \tCtrl+Shift+F"), EDIT_FIND_xpm);

	edit_menu->AppendSeparator();
	albaGUI::AddMenuItem(edit_menu, ID_APP_SETTINGS, _("Settings..."), EDIT_SETTINGS_xpm);

	m_MenuBar->Append(edit_menu, _("&Edit"));

	m_ViewMenu = new wxMenu;
	m_MenuBar->Append(m_ViewMenu, _("&Views"));

	m_OpMenu = new wxMenu;
	m_MenuBar->Append(m_OpMenu, _("&Operations"));

	// Window menu: the dockable panes (added with the panes), the view area commands and the list of the open views
	m_WindowMenu = new wxMenu;
	m_MenuBar->Append(m_WindowMenu, _("&Window"));

	wxMenu    *help_menu = new wxMenu;
	albaGUI::AddMenuItem(help_menu, ABOUT_APPLICATION, _("About"), HELP_ABOUT_xpm);
	albaGUI::AddMenuItem(help_menu, HELP_HOME, _("Help"), HELP_HELP_xpm);


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
		m_MenuBar->Insert(4, m_WizardMenu, _("&Wizard"));
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::CreateToolbar()
{
	//m_ToolBar = new wxToolBar(m_Win,-1,wxPoint(0,0),wxSize(-1,-1),wxHORIZONTAL|wxNO_BORDER|wxTB_FLAT  );
	m_ToolBar = new wxToolBar(m_Win, MENU_VIEW_TOOLBAR, wxPoint(0, 0), wxSize(-1, -1), wxTB_FLAT | wxTB_NODIVIDER);
	m_ToolBar->SetMargins(0, 0);
	m_ToolBar->SetToolSeparation(2);
	m_ToolBar->SetToolBitmapSize(wxSize(20, 20));
	m_ToolBar->AddTool(MENU_FILE_NEW, "New " + m_Extension + " storage file", albaPictureFactory::GetPictureFactory()->GetBmp("FILE_NEW"));
	m_ToolBar->AddTool(MENU_FILE_OPEN, _("Open " + m_Extension + " storage file"), albaPictureFactory::GetPictureFactory()->GetBmp("FILE_OPEN"));
	m_ToolBar->AddTool(MENU_FILE_SAVE, _("Save current " + m_Extension + " storage file"), albaPictureFactory::GetPictureFactory()->GetBmp("FILE_SAVE"));
	m_ToolBar->AddSeparator();

	m_ToolBar->AddTool(MENU_FILE_PRINT, _("Print the selected view"), albaPictureFactory::GetPictureFactory()->GetBmp("PRINT"));
	m_ToolBar->AddTool(MENU_FILE_PRINT_PREVIEW, _("Show the print preview for the selected view"), albaPictureFactory::GetPictureFactory()->GetBmp("PRINT_PREVIEW"));
	m_ToolBar->AddSeparator();

	m_ToolBar->AddTool(OP_UNDO, _("Undo (ctrl+z)"), albaPictureFactory::GetPictureFactory()->GetBmp("OP_UNDO"));
	m_ToolBar->AddTool(OP_REDO, _("Redo (ctrl+shift+z)"), albaPictureFactory::GetPictureFactory()->GetBmp("OP_REDO"));
	m_ToolBar->AddSeparator();

	m_ToolBar->AddTool(OP_CUT, _("Cut selected vme (ctrl+x)"), albaPictureFactory::GetPictureFactory()->GetBmp("OP_CUT"));
	m_ToolBar->AddTool(OP_COPY, _("Copy selected vme (ctrl+c)"), albaPictureFactory::GetPictureFactory()->GetBmp("OP_COPY"));
	m_ToolBar->AddTool(OP_PASTE, _("Paste vme (ctrl+v)"), albaPictureFactory::GetPictureFactory()->GetBmp("OP_PASTE"));
	//m_ToolBar->AddTool(OP_RENAME, albaPictureFactory::GetPictureFactory()->GetBmp("OP_RENAME"), _("Rename vme (ctrl+n)"));
	m_ToolBar->AddSeparator();

	m_ToolBar->AddTool(CAMERA_RESET, _("Reset camera to fit all (ctrl+f)"), albaPictureFactory::GetPictureFactory()->GetBmp("ZOOM_ALL"));
	m_ToolBar->AddTool(CAMERA_FIT, _("Reset camera to fit selected object (ctrl+shift+f)"), albaPictureFactory::GetPictureFactory()->GetBmp("ZOOM_SEL"));
	m_ToolBar->AddTool(CAMERA_FLYTO, _("Fly to object under mouse"), albaPictureFactory::GetPictureFactory()->GetBmp("FLYTO"));
	m_ToolBar->AddSeparator();
	m_ToolBar->AddCheckTool(MENU_VIEW_AREA_MAXIMIZE, _("Maximized view"), albaPictureFactory::GetPictureFactory()->GetBmp("VIEW_MAXIMIZE"), wxNullBitmap, _("Maximized view (ctrl+m)"));

	if (m_UseSnapshotManager)
	{
		m_ToolBar->AddSeparator();
		m_ToolBar->AddTool(MENU_FILE_SNAPSHOT, _("Create Snapshot"), albaPictureFactory::GetPictureFactory()->GetBmp("CAMERA"));
		m_ToolBar->AddTool(MENU_FILE_MANAGE_SNAPSHOT, _("Manage Snapshots"), albaPictureFactory::GetPictureFactory()->GetBmp("IMAGE_PREVIEW"));
	}

	EnableItem(CAMERA_RESET, false);
	EnableItem(CAMERA_FIT, false);
	EnableItem(CAMERA_FLYTO, false);
	EnableItem(MENU_FILE_PRINT, false);
	EnableItem(MENU_FILE_PRINT_PREVIEW, false);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::CreateToolBarsAndPanels()
{
	if (m_PlugToolbar) CreateAndPlugToolbar();
	if (m_PlugTimebar) CreateTimeBar(); //SIL. 23-may-2006 : 
	if (m_PlugLogPanel)	CreateLogPanel();
	else this->CreateNullLog();
	if (this->m_PlugControlPanel) CreateControlPanel();
	if (m_PlugToolbar && m_WizardManager) CreateWizardToolbar();

	if (m_OpManager && m_ToolBar)
		m_OpManager->SetToolbar(m_ToolBar);

	if (m_WizardManager && m_ToolBar)
		m_WizardManager->SetToolbar(m_ToolBar);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::CreateAndPlugToolbar()
{
	CreateToolbar();

	m_ToolBar->Realize();

	//SIL. 23-may-2006 : 
	m_Win->AddDockPane(m_ToolBar, wxPaneInfo()
		.Name("toolbar")
		.Caption(wxT("Toolbar"))
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
void albaLogicWithManagers::CreateTimeBar()
{
	m_TimePanel = new albaGUITimeBar(m_Win,MENU_VIEW_TIMEBAR,true);
	m_TimePanel->SetListener(this);

	// Events coming from settings are forwarded to the time bar.
	m_TimePanel->SetTimeSettings(m_TimeBarSettings);
	m_TimeBarSettings->SetListener(m_TimePanel);

	m_Win->AddDockPane(m_TimePanel, wxPaneInfo()
		.Name("timebar")
		.Caption(wxT("Time Bar"))
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
void albaLogicWithManagers::CreateControlPanel()
{
	m_SideBar = new albaSideBar(m_Win, MENU_VIEW_SIDEBAR, this, m_SidebarStyle);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::FillMenus()
{
	if (m_VMEManager && m_RecentFileMenu)
	{
		m_VMEManager->SetFileHistoryMenu(m_RecentFileMenu);
	}

	if (m_UseViewManager && m_ViewMenu)
	{
		m_ViewManager->FillMenu(m_ViewMenu);
	}

	if (m_OpManager && m_MenuBar && (m_ImportMenu || m_OpMenu || m_ExportMenu))
	{
		m_OpManager->FillMenu(m_ImportMenu, m_ExportMenu, m_OpMenu);
		m_OpManager->SetMenubar(m_MenuBar);
	}

	//setting gui pointers to the Wizard Manager
	if (m_WizardManager && m_MenuBar)
	{
		m_WizardManager->FillMenu(m_WizardMenu);
		m_WizardManager->SetMenubar(m_MenuBar);
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::EnableMenuAndToolbar()
{
	bool enable = !(m_RunningOperation || m_WizardRunning);

	if (!m_VMEManager)
		return;

	albaView * selectedView = m_ViewManager ?m_ViewManager->GetSelectedView() : NULL; 

	EnableItem(MENU_FILE_NEW, enable);
	EnableItem(MENU_FILE_OPEN, enable);
	EnableItem(MENU_FILE_SAVE, enable);
	EnableItem(MENU_FILE_SAVEAS, enable);
	EnableItem(MENU_FILE_SAVEAS_LEGACY, enable);
	EnableItem(MENU_FILE_QUIT, enable);
	EnableItem(wxID_FILE1, enable);
	EnableItem(wxID_FILE2, enable);
	EnableItem(wxID_FILE3, enable);
	EnableItem(wxID_FILE4, enable);
	EnableItem(wxID_FILE5, enable);
	EnableItem(wxID_FILE6, enable);
	EnableItem(wxID_FILE7, enable);
	EnableItem(wxID_FILE8, enable);
	EnableItem(wxID_FILE9, enable);

	EnableItem(CAMERA_RESET, selectedView != NULL);
	EnableItem(CAMERA_FIT, selectedView != NULL);
	EnableItem(CAMERA_FLYTO, selectedView != NULL);

	EnableItem(MENU_FILE_PRINT, selectedView != NULL);
	EnableItem(MENU_FILE_PRINT_PREVIEW, selectedView != NULL);
	EnableItem(MENU_FILE_PRINT_SETUP, selectedView != NULL);
	EnableItem(MENU_FILE_PRINT_PAGE_SETUP, selectedView != NULL);

	if (m_UseSnapshotManager && m_SnapshotManager)
	{
		EnableItem(MENU_FILE_MANAGE_SNAPSHOT, m_SnapshotManager->HasSnapshots(m_VMEManager->GetRoot()));
		EnableItem(MENU_FILE_SNAPSHOT, selectedView);
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::EnableItem(int item, bool enable)
{
	if (m_MenuBar)
		// must always check if a menu item exist because
		// during application shutdown it is not guaranteed
		if (m_MenuBar->FindItem(item))
			m_MenuBar->Enable(item, enable);

	if (m_ToolBar)
		m_ToolBar->EnableTool(item, enable);
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::ShowTreeContextualMenu(albaGUIVMETree *tree, albaVME *vme, bool vmeMenu)
{
	albaGUITreeContextualMenu *contextMenu = new albaGUITreeContextualMenu();
	contextMenu->SetListener(this);
	contextMenu->SetOpManager(m_OpManager);

	albaView *v = m_ViewManager->GetSelectedView();
	contextMenu->CreateContextualMenu(tree, v, vme, vmeMenu);
	contextMenu->ShowContextualMenu();

	cppDEL(contextMenu);
}

// Splash Screen
//----------------------------------------------------------------------------
void albaLogicWithManagers::ShowSplashScreen(wxBitmap &splashImage, wxString message, int pos, wxColour color)
{
	m_SplashScreen = new albaGUISplashScreen(splashImage, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 2000, NULL);

	if (message.IsEmpty()) 
		message = GetTopWin()->GetTitle()  + " is not a medical device\nand it is not approved for medical usage.";

	m_SplashScreen->SetText(message,(albaGUISplashScreen::SPLASH_SCREEN_POSITION)pos, color);
	wxMilliSleep(1500);

	albaYield();
}

// WebSite
//----------------------------------------------------------------------------
void albaLogicWithManagers::ShowWebSite(wxString url)
{
	albaLogMessage("Opening %s", url.ToAscii());

	//WXwidget does not manage urls that contains anchor
	//in this case we need to create a temp file which redirect to the correct url 
	//and open this file
	if (url.Contains("#"))
	{
		wxString tmpHtmlFileName = albaGetAppDataDirectory().ToAscii();
		tmpHtmlFileName += "/loadPage.html";

		FILE *tmpHtmlFile = fopen(tmpHtmlFileName.ToAscii(), "w");

		fprintf(tmpHtmlFile, "<html>\n");
		fprintf(tmpHtmlFile, "<head>\n");
		fprintf(tmpHtmlFile, "	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\" />\n");
		fprintf(tmpHtmlFile, "	<meta http-equiv=\"refresh\" content=\"1;url=%s\" />\n", url.ToAscii());
		fprintf(tmpHtmlFile, "<title></title>\n");
		fprintf(tmpHtmlFile, "</head>\n");
		fprintf(tmpHtmlFile, "<body/>\n");
		fprintf(tmpHtmlFile, "</html>\n");
		fclose(tmpHtmlFile);

		url = tmpHtmlFileName;
	}

	wxString command = "rundll32.exe url.dll,FileProtocolHandler ";
	command = command + "\"" + url + "\"";
	wxExecute(command);
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::PrintImage(albaVMEImage *img)
{
	m_PrintSupport->OnPrint(img);
}

// Log
//----------------------------------------------------------------------------
void albaLogicWithManagers::CreateNullLog()
{
#ifdef ALBA_USE_VTK
	m_VtkLog = albaVTKLog::New();
	m_VtkLog->SetInstance(m_VtkLog);
#endif  
	wxTextCtrl *log = new wxTextCtrl(m_Win, -1, "", wxPoint(0, 0), wxSize(100, 300), wxNO_BORDER | wxTE_MULTILINE);
	m_Logger = new albaWXLog(log);
	log->Show(false);
	wxLog *old_log = wxLog::SetActiveTarget(m_Logger);
	cppDEL(old_log);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::CreateLogPanel()
{
#ifdef ALBA_USE_VTK
	m_VtkLog = albaVTKLog::New();
	m_VtkLog->SetInstance(m_VtkLog);
#endif
	wxTextCtrl *log = new wxTextCtrl(m_Win, MENU_VIEW_LOGBAR, "", wxPoint(0, 0), wxSize(100, 300), /*wxNO_BORDER |*/ wxTE_MULTILINE);
	m_Logger = new albaWXLog(log);
	m_Logger->LogToFile(m_LogToFile);
	if (m_LogToFile)
	{
		wxString s = m_ApplicationSettings->GetLogFolder().GetCStr();
		wxDateTime log_time = wxDateTime::Now();
		s += "\\";
		s += m_Win->GetTitle();
		s += albaString::Format("_%02d_%02d_%d_%02d_%2d", log_time.GetYear(), log_time.GetMonth() + 1, log_time.GetDay(), log_time.GetHour(), log_time.GetMinute());
		s += ".log";
		if (m_Logger->SetFileName(s) == ALBA_ERROR)
		{
			wxMessageBox(albaString::Format("Unable to create log file %s!!", s.ToAscii()), "Warning", wxOK | wxICON_WARNING);
		}
	}

	wxLog *old_log = wxLog::SetActiveTarget(m_Logger);
	cppDEL(old_log);

	m_Win->AddDockPane(log, wxPaneInfo()
		.Name("logbar")
		.Caption(wxT("Log Panel"))
		.Bottom()
		.Layer(0)
		.MinSize(100, 10)
		.TopDockable(false) // prevent docking on top side - otherwise may dock also beside the toolbar -- and it's hugely
	);

	albaLogMessage(_("Welcome"));
}

// Layout
//----------------------------------------------------------------------------
void albaLogicWithManagers::StoreLayout()
{
	if (m_AppLayout)
	{
		int pos[2], size[2];

		albaXMLStorage *xmlStorage = albaXMLStorage::New();
		xmlStorage->SetFileType("MLY");
		xmlStorage->SetVersion("2.0");
		albaVMERoot *root;
		albaNEW(root);
		root->Initialize();
		xmlStorage->SetDocument(root);

		wxString layout_file = albaGetAppDataDirectory().ToAscii();
		layout_file << "\\layout.mly";

		wxFrame *frame = (wxFrame *)albaGetFrame();
		wxRect rect;
		rect = frame->GetRect();
		pos[0] = rect.GetPosition().x;
		pos[1] = rect.GetPosition().y;
		size[0] = rect.GetSize().GetWidth();
		size[1] = rect.GetSize().GetHeight();

	
		m_AppLayout->SetApplicationInfo(m_Win->IsMaximized(), pos, size);
		bool toolbar_vis = m_Win->GetDockManager().GetPane("toolbar").IsShown();
		m_AppLayout->SetInterfaceElementVisibility("toolbar", toolbar_vis);
		bool sidebar_vis = m_Win->GetDockManager().GetPane("sidebar").IsShown();
		m_AppLayout->SetInterfaceElementVisibility("sidebar", sidebar_vis);
		bool logbar_vis = m_Win->GetDockManager().GetPane("logbar").IsShown();
		m_AppLayout->SetInterfaceElementVisibility("logbar", logbar_vis);


		root->SetAttribute("ApplicationLayout", m_AppLayout);

		xmlStorage->SetURL(layout_file);
		xmlStorage->Store();

		cppDEL(xmlStorage);
		albaDEL(root);
	}
} 
//----------------------------------------------------------------------------
void albaLogicWithManagers::RestoreLayout()
{
	albaXMLStorage *xmlStorage = albaXMLStorage::New();
	xmlStorage->SetFileType("MLY");
	xmlStorage->SetVersion("2.0");

	albaVMERoot *root;
	albaNEW(root);
	albaNEW(m_AppLayout);
	root->Initialize();
	xmlStorage->SetDocument(root);

	wxString layout_file  = albaGetAppDataDirectory().ToAscii();
	layout_file << "\\layout.mly";
	xmlStorage->SetURL(layout_file);
	if(wxFileExists(layout_file) && xmlStorage->Restore()==ALBA_OK)
	{
		albaVME *root = (albaVME *)xmlStorage->GetDocument();

		mmaApplicationLayout * app_layout = (mmaApplicationLayout *)root->GetAttribute("ApplicationLayout");

		int maximized, pos[2], size[2];
		app_layout->GetApplicationInfo(maximized, pos, size);

		pos[0]=MAX(0,pos[0]);
		pos[1]=MAX(0,pos[1]);

		wxRect rect(pos[0],pos[1],size[0],size[1]);
		m_Win->SetMinSize(wxSize(800, 580));
		m_Win->SetSize(rect);
		
		if (maximized != 0)
			m_Win->Maximize();
		
		m_Win->ShowDockPane("toolbar", app_layout->GetToolBarVisibility());
		m_Win->ShowDockPane("logbar", app_layout->GetLogBarVisibility());
		m_Win->ShowDockPane("sidebar",  app_layout->GetSideBarVisibility());

		m_AppLayout->DeepCopy(app_layout);
	}

	cppDEL(xmlStorage);
	albaDEL(root);
}

// OTHER /////////////////////////////////////////////////////////////////////
// Config
//----------------------------------------------------------------------------
void albaLogicWithManagers::TimeSet(double t)
{
	if (m_VMEManager)
	{
		m_VMEManager->TimeSet(t);
	}

	if (m_ViewManager && m_TimeBarSettings)
	{
		m_ViewManager->CameraUpdate(m_TimeBarSettings->GetPlayingInActiveViewport() != 0);
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::UpdateTimeBounds()
{
	double min, max;
	if (m_VMEManager)
	{
		m_VMEManager->TimeGetBounds(&min, &max);
	}

	if (m_TimePanel)
	{
		m_TimePanel->SetBounds(min, max);
		m_Win->ShowDockPane("timebar", min < max);
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::UpdateMeasureUnit()
{
	for (albaView* v = m_ViewManager->GetList(); v; v = v->m_Next)
		v->OptionsUpdate();
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::ImportExternalFile(albaString &filename)
{
	m_OpManager->ImportFile(filename);
}

//----------------------------------------------------------------------------
albaVMEStorage *albaLogicWithManagers::CreateStorage(albaVMEStorage *oldStorage)
{
	if (oldStorage)
		m_OpManager->VmeSelected(NULL);
	albaDEL(oldStorage);
	return albaVMEStorage::New();
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::HandleException()
{
	if (!m_FatalExptionOccurred)
	{
		m_FatalExptionOccurred = true;

		int answare = wxMessageBox(_("An Excpetion has occurred and this application must be closed.\nYou can make a attempt to save your work.\nDo You want to proceed?"), _("Fatal Exception!"), wxYES_NO | wxCENTER | wxICON_ERROR | wxYES_DEFAULT);
		if (answare == wxYES)
		{
			FileSaveAs();

			if (m_OpManager->Running())
				m_OpManager->StopCurrentOperation();
		}
	}
	OnQuit();
}

//----------------------------------------------------------------------------
int albaLogicWithManagers::AppEventFilter(wxEvent& event)
{
	// the keys go to the interaction tools: a key used by a tool is not processed further
#ifdef ALBA_USE_VTK
	if (m_InteractionManager && (event.GetEventType() == wxEVT_KEY_DOWN || event.GetEventType() == wxEVT_KEY_UP))
		if (m_InteractionManager->OnKey(((wxKeyEvent &)event).GetKeyCode(), event.GetEventType() == wxEVT_KEY_DOWN))
			return 1;
#endif

	return -1;
}
