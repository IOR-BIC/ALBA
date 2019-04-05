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
#include "mafPrintManager.h"

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
  #include "mafInteractorPER.h"
  #include "mafGUITreeContextualMenu.h"
  #include "mafGUIContextualMenu.h"
  #include "vtkCamera.h"
#endif

#include "mafSideBar.h"
#include "mafUser.h"
#include "mafGUIMDIFrame.h"
#include "mafGUIMDIChild.h"
#include "mafGUICheckTree.h"
#include "mafGUITimeBar.h"
#include "mafGUIMaterialChooser.h"
#include "mafGUIViewFrame.h"
#include "mafGUIApplicationSettings.h"
#include "mafGUISettingsTimeBar.h"
#include "mafGUISettingsDialog.h"
#include "mafGUIAboutDialog.h""

#include "mmaApplicationLayout.h"
#include "mafDataVector.h"
#include "mafVMEStorage.h"
#include "mafWizardManager.h"
#include "mafRWIBase.h"
#include <wx/dir.h>
#include "wx/splash.h"
#include "mafVTKLog.h"
#include "vtkTimerLog.h"
#include "mafWXLog.h"
#include "mafVMERoot.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafHelpManager.h"
#include "mafServiceLocator.h"
#include "mafGUIPicButton.h"
#include "mafGUIButton.h"
#include "mafVMEGroup.h"
#include "vtkUnsignedCharArray.h"
#include "vtkDataSet.h"
#include "mafVMEImage.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataSetAttributes.h"
#include "mafSnapshotManager.h"
#include "mafOpSelect.h"
#include "mafGUIVMEChooserTree.h"
#include "mafGUI.h"

#define IDM_WINDOWNEXT 4004
#define IDM_WINDOWPREV 4006


//----------------------------------------------------------------------------
mafLogicWithManagers::mafLogicWithManagers(mafGUIMDIFrame *mdiFrame/*=NULL*/)
{
	if (mdiFrame == NULL)
		m_Win = new mafGUIMDIFrame("maf", wxDefaultPosition, wxSize(800, 600), wxFRAME_NO_WINDOW_MENU);
	else
		m_Win = mdiFrame;

	m_Win->SetListener(this);

	m_ChildFrameStyle = 0; wxCAPTION | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxRESIZE_BORDER; //wxTHICK_FRAME; // Default style
	m_ApplicationSettings = new mafGUIApplicationSettings(this);
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
	m_SidebarStyle = mafSideBar::DOUBLE_NOTEBOOK;
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
	m_ImportMenu = NULL;
	m_ExportMenu = NULL;
	m_OpMenu = NULL;
	m_ViewMenu = NULL;
	m_RecentFileMenu = NULL;

	m_MaterialChooser = NULL;

	m_PrintSupport = new mafPrintManager();

	m_SettingsDialog = new mafGUISettingsDialog();
	m_AboutDialog = new mafGUIAboutDialog();

	m_Revision = _("0.1");
	m_Extension = "msf";

	m_UseWizardManager = false;
	m_WizardRunning = false;
	m_RunningOperation = false;
	m_WizardManager = NULL;

	m_User = new mafUser();

	m_ShowStorageSettings = false;
	m_ShowInteractionSettings = false;
	m_SelectedLandmark = NULL;
	m_FatalExptionOccurred = false;
}
//----------------------------------------------------------------------------
mafLogicWithManagers::~mafLogicWithManagers()
{  
  //free mem
  if(m_WizardManager)
    delete m_WizardManager;



  // Managers are destruct in the OnClose
  cppDEL(m_User);
  cppDEL(m_PrintSupport);
  cppDEL(m_SettingsDialog);
	cppDEL(m_AboutDialog);
	cppDEL(m_ApplicationSettings);
	cppDEL(m_TimeBarSettings);
}


// APPLICATION ///////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void mafLogicWithManagers::Init(int argc, char **argv)
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

		if (argc > 1)
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

	// Init About Dialog
	m_AboutDialog->SetTitle(m_AppTitle);
	m_AboutDialog->SetRevision(m_Revision.GetCStr());

	wxString imagePath = mafGetApplicationDirectory().c_str();
	imagePath += "\\Config\\ExampleAppAbout.bmp";
	m_AboutDialog->SetImagePath(imagePath);

	// Create and Open View
	ViewCreate(VIEW_START + 1);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Configure()
{
	if (m_PlugTimebar)
		m_TimeBarSettings = new mafGUISettingsTimeBar(this);

	if (m_UseVMEManager)
	{
		m_VMEManager = new mafVMEManager();
		m_VMEManager->SetListener(this);
		m_VMEManager->SetMsfFileExtension(m_Extension);
		//m_VMEManager->SetSingleBinaryFile(m_StorageSettings->GetSingleFileStatus()!= 0);
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

	if (m_UseViewManager)
	{
		m_ViewManager = new mafViewManager();
		m_ViewManager->SetListener(this);
		m_ViewManager->SetMouse(m_Mouse);
	}

	if (m_UseOpManager)
	{
		m_OpManager = new mafOpManager();
		m_OpManager->SetListener(this);
		m_OpManager->SetMouse(m_Mouse);
		m_OpManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
	}

	if (m_UseHelpManager)
	{
		m_HelpManager = new mafHelpManager();
		//m_HelpManager->SetListener(this);
	}

	if (m_UseSnapshotManager)
	{
		m_SnapshotManager = new mafSnapshotManager();
		m_SnapshotManager->SetMouse(m_Mouse);
	}

	// Fill the SettingsDialog
	m_SettingsDialog->AddPage(m_ApplicationSettings->GetGui(), m_ApplicationSettings->GetLabel());

	if (m_ShowInteractionSettings && m_InteractionManager)
		//m_SettingsDialog->AddPage(m_InteractionManager->GetGui(), _("Interaction Manager"));

		if (m_TimeBarSettings)
			m_SettingsDialog->AddPage(m_TimeBarSettings->GetGui(), m_TimeBarSettings->GetLabel());

	ConfigureWizardManager();
}

// Window
//----------------------------------------------------------------------------
void mafLogicWithManagers::UpdateFrameTitle()
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
void mafLogicWithManagers::SetApplicationStamp(mafString &app_stamp)
{
	if (m_VMEManager)
	{
		m_VMEManager->SetApplicationStamp(app_stamp);
	}
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Show()
{
	if (m_PlugMenu)
		CreateMenu();

	FillMenus();
	CreateToolBarsAndPanels();

	m_AppTitle = m_Win->GetTitle().c_str();

	RestoreLayout();

	m_Win->Show(TRUE);

	// must be after the mafLogicWithGUI::Show(); because in that method is set the m_AppTitle var
	SetApplicationStamp((mafString)m_AppTitle);
}

// Plug
//----------------------------------------------------------------------------
void mafLogicWithManagers::Plug(mafView* view, bool visibleInMenu)
{
  if(m_ViewManager) 
    m_ViewManager->ViewAdd(view, visibleInMenu);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Plug(mafOp *op, wxString menuPath, bool canUndo, mafGUISettings *setting)
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
{
	//Plugging the wizard    
	if(m_WizardManager)
	{
		m_WizardManager->WizardAdd(wizard, menuPath);
	}
	else 
	{
		mafLogMessage("Enable wizard manager to plug wizards"); 
	}
}

// Quit
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnQuit()
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

	mafGUIViewFrame::OnQuit();
	mafGUIMDIChild::OnQuit();
	m_Win->OnQuit();

	if (m_SnapshotManager)
		cppDEL(m_SnapshotManager);

	cppDEL(m_VMEManager);
	cppDEL(m_MaterialChooser);
	cppDEL(m_HelpManager);

	cppDEL(m_ViewManager);

#ifdef MAF_USE_VTK
	cppDEL(m_InteractionManager);
#endif
	cppDEL(m_OpManager);

	// must be deleted after m_VMEManager
	cppDEL(m_SideBar);

	mafYield();
	if (m_PlugLogPanel)
	{
		delete wxLog::SetActiveTarget(NULL);
	}
#ifdef MAF_USE_VTK 
	vtkTimerLog::CleanupLog();
	vtkDEL(m_VtkLog);
#endif
	m_Win->Destroy();
}


// EVENT /////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		/*if (e->GetId() == mafDataVector::SINGLE_FILE_DATA)
		{
			e->SetBool(m_StorageSettings->GetSingleFileStatus()!= 0);
			return;
		}*/
		switch (e->GetId())
		{
			//resize view
		case TILE_WINDOW_CASCADE:
			m_Win->Cascade();
			break;
		case TILE_WINDOW_HORIZONTALLY:
			m_Win->Tile(wxHORIZONTAL);
			break;
		case TILE_WINDOW_VERTICALLY:
			m_Win->Tile(wxVERTICAL);
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
			// commands related to the STATUSBAR
		case BIND_TO_PROGRESSBAR:
#ifdef MAF_USE_VTK
			m_Win->BindToProgressBar(e->GetVtkObj());
#endif
			break;
		case PROGRESSBAR_SET_TEXT:
			m_Win->ProgressBarSetText(&wxString(e->GetString()->GetCStr()));
			break;
			// ###############################################################
		case UPDATE_UI:
			break;
			// ###############################################################
			// commands related to FILE MENU  
		case MENU_FILE_NEW:
			OnFileNew();
			break;
		case MENU_FILE_OPEN:
		{
			mafString *filename = e->GetString();
			if (filename)
				OnFileOpen((*filename).GetCStr());
			else
				OnFileOpen();
			UpdateFrameTitle();
		}
		break;
		case IMPORT_FILE:
		{
			mafString *filename = e->GetString();
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
			OnFileSave();
			break;
		case MENU_FILE_SAVEAS:
			OnFileSaveAs();
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
		case VME_SELECT:
			VmeSelect(e->GetVme());
			EnableMenuAndToolbar();
			break;
		case VME_SELECTED:
			VmeSelected(e->GetVme());
			break;
		case VME_DCLICKED:
			VmeDoubleClicked(*e);
			break;
		case VME_ADDED:
			VmeAdded(e->GetVme());
			break;
		case VME_REMOVING:
			VmeRemoving(e->GetVme());
			break;
		case VME_CHOOSE:
		{
			mafString *s = e->GetString();
			mafString str;

			if (s != NULL)
				str = *s;
			else
				str = "Choose Node";

			std::vector<mafVME*> nodeVector = m_SideBar->VmeChoose(e->GetPointer(), REPRESENTATION_AS_TREE, str, e->GetBool(), e->GetVme());
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
		break;
		case VME_CHOOSE_MATERIAL:
			VmeChooseMaterial(e->GetVme(), e->GetBool());
			break;
		case UPDATE_PROPERTY:
			VmeUpdateProperties(e->GetVme(), e->GetBool());
			break;
		case SHOW_TREE_CONTEXTUAL_MENU:
			TreeContextualMenu(*e);
			break;
		case SHOW_VIEW_CONTEXTUAL_MENU:
			ViewContextualMenu(e->GetBool());
			break;
			// ###############################################################
			// commands related to OP
		case MENU_OP:
			if (m_OpManager)
			{
				m_OpManager->OpRun(e->GetArg());
			}
			break;
		case PARSE_STRING:
		{
			if (this->m_OpManager->Running())
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
			m_CancelledBeforeOpStarting = false;
			mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
			if (c != NULL)
				c->SetAllowCloseWindow(false);
			OpRunStarting();
		}
		break;
		case OP_RUN_TERMINATED:
		{
			if (m_WizardManager && m_WizardRunning)
			{
				m_RunningOperation = false;
				if (!mafOpSelect::SafeDownCast(m_OpManager->GetRunningOperation()))
					m_WizardManager->WizardContinue(e->GetArg());
			}
			//else we manage the operation end by unlock the close button and so on
			else
			{
				mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
				if (c != NULL)
					c->SetAllowCloseWindow(true);
				OpRunTerminated(e->GetArg());
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
			const char *viewStr = e->GetString()->GetCStr();
			view = m_ViewManager->GetFromList(viewStr);
			if (view)
			{
				view->GetFrame()->SetSize(e->GetWidth(), e->GetHeight());
				view->GetFrame()->SetPosition(wxPoint(e->GetX(), e->GetY()));
			}
		}
		break;
		case VIEW_DELETE:
		{

			if (m_PlugControlPanel)
				this->m_SideBar->ViewDeleted(e->GetView());

#ifdef MAF_USE_VTK
			// currently mafInteraction is strictly dependent on VTK (marco)
			if (m_InteractionManager)
				m_InteractionManager->ViewSelected(NULL);
#endif

			EnableMenuAndToolbar();
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
			EnableMenuAndToolbar();
		}
		break;
		case VIEW_MAXIMIZE:
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
				v->GetRWI()->SaveAllImages(v->GetLabel(), v);
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
		case CAMERA_RESET:
			CameraReset();
			break;
		case CAMERA_FIT:
			if (m_ViewManager) m_ViewManager->CameraReset(true);
			break;
		case CAMERA_FLYTO:
			if (m_ViewManager) m_ViewManager->CameraFlyToMode();
			// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
			if (m_InteractionManager) m_InteractionManager->CameraFlyToMode();  //modified by Marco. 15-9-2004 fly to with devices.
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
		case CAMERA_PRE_RESET:
			// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
			if (m_InteractionManager)
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
			if (m_InteractionManager)
			{
				vtkRenderer *ren = (vtkRenderer*)e->GetVtkObj();
				//assert(ren); //modified by Marco. 2-11-2004 Commented out to allow reset camera of all cameras.
				m_InteractionManager->PostResetCamera(ren);
				//mafLogMessage("CAMERA_POST_RESET");
			}
#endif
			break;
		case CAMERA_UPDATE:
			CameraUpdate();

			break;
		case CAMERA_SYNCHRONOUS_UPDATE:
			m_ViewManager->CameraUpdate();
			break;
		case INTERACTOR_ADD:
			// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
			if (m_InteractionManager)
			{
				mafInteractor *interactor = mafInteractor::SafeDownCast(e->GetMafObject());
				assert(interactor);
				mafString *action_name = e->GetString();
				m_InteractionManager->BindAction(*action_name, interactor);
			}
#endif
			break;
		case INTERACTOR_REMOVE:
			// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
			if (m_InteractionManager)
			{
				mafInteractor *interactor = mafInteractor::SafeDownCast(e->GetMafObject());
				assert(interactor);
				mafString *action_name = e->GetString();
				m_InteractionManager->UnBindAction(*action_name, interactor);
			}
#endif
			break;
		case PER_PUSH:
			// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
			if (m_InteractionManager)
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
			if (m_InteractionManager) m_InteractionManager->PopPER();
#endif
			break;
		case GET_CURRENT_PER:
			if (m_InteractionManager)
				e->SetMafObject(m_InteractionManager->GetPER());
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
		case GET_BUILD_HELP_GUI:
		{
			if (e->GetString())
			{
				mafString typeName = e->GetString()->GetCStr();
				bool res = m_HelpManager && m_HelpManager->HasHelpEntry(typeName);
				e->SetArg(m_HelpManager && m_HelpManager->HasHelpEntry(typeName));
			}
			else
			{
				e->SetArg(false);
			}
		}
		break;
		case OPEN_HELP_PAGE:
		{
			// open help for entity
			if (e->GetString())
			{
				wxString typeName = e->GetString()->GetCStr();
				m_HelpManager->ShowHelpEntry(typeName);
			}
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
			if (m_SnapshotManager && m_VMEManager && m_ViewManager)
				m_SnapshotManager->CreateSnapshot(m_VMEManager->GetRoot(), m_ViewManager->GetSelectedView());

			if (m_WizardManager && m_WizardRunning)
				OnEvent(&mafEvent(this, WIZARD_RUN_CONTINUE, true));
		}
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
			const char *viewStr = e->GetString()->GetCStr();

			view = m_ViewManager->GetFromList(viewStr);
			if (view)
				m_ViewManager->Activate(view);
			else
				m_ViewManager->ViewCreate(viewStr);
		}
		break;
		case WIZARD_DELETE_VIEW:
		{
			mafView *view;
			const char *viewStr = e->GetString()->GetCStr();

			view = m_ViewManager->GetFromList(viewStr);
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
			mafString *tmp = e->GetString();
			mafLogMessage("wiz starting :%s", tmp->GetCStr());
			m_CancelledBeforeOpStarting = true;
			UpdateFrameTitle();
			m_OpManager->OpRun(*(e->GetString()));
			//If the op is started the value of m_CancelledBeforeOpStarting 
			//is changed by OP_RUN_STARTING event
			if (m_CancelledBeforeOpStarting)
			{
				m_CancelledBeforeOpStarting = false;
				m_WizardManager->WizardContinue(false);
			}
		}
		break;
		case WIZARD_OP_DELETE:
		{
			//Running an op required from the wizard
			m_CancelledBeforeOpStarting = true;
			UpdateFrameTitle();
			m_OpManager->OpRun(OP_DELETE);
		}
		break;
		case WIZARD_OP_NEW:
		{
			//Running an op required from the wizard
			if (m_VMEManager)
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
			file = m_VMEManager->GetFileName().GetCStr();
			if (file.IsEmpty())
			{
				mafLogMessage("Reload requested whitout opened MSF");
				//continue wizard with error
				m_WizardManager->WizardContinue(false);
			}
			else
			{
				int opened = m_VMEManager->MSFOpen(file);
				//continue wizard after open operation
				m_WizardManager->WizardContinue(opened != MAF_ERROR);
			}
		}
		break;
		case MENU_VIEW_TOOLBAR:
			m_Win->ShowDockPane("wizardgauge", !m_Win->DockPaneIsShown("wizardgauge"));
			m_Win->ShowDockPane("wizardlabel", !m_Win->DockPaneIsShown("wizardlabel"));
			m_Win->ShowDockPane("wizardseparator", !m_Win->DockPaneIsShown("wizardseparator"));
			m_Win->ShowDockPane("toolbar", !m_Win->DockPaneIsShown("toolbar"));
			break;
		case PROGRESSBAR_SHOW:
		{
			if (e->GetSender() == m_WizardManager)
			{
				m_WizardLabel->Enable();
				m_WizardGauge->Enable();
			}
			else
				m_Win->ProgressBarShow();
		}
		break;
		case PROGRESSBAR_HIDE:
		{
			if (e->GetSender() == m_WizardManager)
			{
				m_WizardGauge->SetValue(0);
				m_WizardGauge->Enable(false);
				m_WizardLabel->Enable(false);
			}
			else
				m_Win->ProgressBarHide();
		}
		break;
		case PROGRESSBAR_SET_VALUE:
		{
			if (e->GetSender() == m_WizardManager)
				m_WizardGauge->SetValue(e->GetArg());
			else
				m_Win->ProgressBarSetVal(e->GetArg());
		}
		break;

		default:
			e->Log();
			break;
			break;
		} // end switch case
	} // end if SafeDowncast
}

// FILE //////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileNew()
{
	if (m_OpManager)
		m_OpManager->VmeSelected(NULL);

	if(m_VMEManager)
  {
    if(m_VMEManager->AskConfirmAndSave())
	  {
		  m_VMEManager->MSFNew();
	  }
  }
	
	m_Win->SetTitle(m_AppTitle);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileOpen(const char *file_to_open)
{
	if (m_VMEManager)
	{
		if (m_VMEManager->AskConfirmAndSave())
		{
			wxString file;

			wxString wildc = _("Storage File (*." + m_Extension + ")|*." + m_Extension + "|Compressed file (*.z" + m_Extension + ")|*.z" + m_Extension + "");
			if (file_to_open != NULL)
			{
				file = file_to_open;
			}
			else
			{
				wxString lastFolder = mafGetLastUserFolder().c_str();
				file = mafGetOpenFile(lastFolder, wildc).c_str();
			}
			
			if (file.IsEmpty() && m_WizardManager && m_WizardRunning)
				m_WizardManager->WizardContinue(false);
			else if (file.IsEmpty())
				return;

			int opened = m_VMEManager->MSFOpen(file);
			//If there is a wizzard running we need to continue it after open operation
			if (m_WizardManager && m_WizardRunning)
				m_WizardManager->WizardContinue(opened != MAF_ERROR);
		}
	}
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileHistory(int menuId)
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
{
  if(m_VMEManager)
  {
		mafString save_default_folder = mafGetLastUserFolder().c_str();
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
{
  if(m_VMEManager) 
  {
	  mafString save_default_folder = mafGetLastUserFolder().c_str();
	  save_default_folder.ParsePathName();
	  m_VMEManager->SetDirName(save_default_folder);
	  int saved=m_VMEManager->MSFSaveAs();
	  //If there is a wizard running we need to continue it after save operation
	  if (m_WizardManager && m_WizardRunning)
		  m_WizardManager->WizardContinue(saved!=MAF_ERROR);
	  UpdateFrameTitle();
  }
}

// VME ///////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeShow(mafVME *vme, bool visibility)
{
	if (!vme)
		return;

	if (m_ViewManager)
	{
		mafVMELandmarkCloud *lmc = mafVMELandmarkCloud::SafeDownCast(vme);
		mafVMELandmark *lm = mafVMELandmark::SafeDownCast(vme);
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
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeModified(mafVME *vme)
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
void mafLogicWithManagers::VmeAdd(mafVME *vme)
{
	if (!vme) 
		return;
	if(m_VMEManager) 
    m_VMEManager->VmeAdd(vme);


}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeAdded(mafVME *vme)
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
void mafLogicWithManagers::VmeRemove(mafVME *vme)
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
void mafLogicWithManagers::VmeRemoving(mafVME *vme)
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
void mafLogicWithManagers::VmeRemoved()
{
	EnableMenuAndToolbar();
}

//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeVisualModeChanged(mafVME * vme)
{
	if (vme)
	{
		VmeShow(vme, false);
		VmeShow(vme, true);
	}
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeDoubleClicked(mafEvent &e)
{
	mafVME *node = e.GetVme();
	if (node)
	{
		mafLogMessage("Double click on %s", node->GetName());
	}
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeSelect(mafVME *vme)
{
	if (vme != NULL && m_OpManager)
		m_OpManager->OpSelect(vme);

	// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
	if (m_InteractionManager)
		m_InteractionManager->VmeSelected(vme);
#endif

	EnableMenuAndToolbar();
}

//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeSelected(mafVME *vme)
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

		if (mafVMELandmark::SafeDownCast(vme))
		{
			SelectLandmark((mafVMELandmark *)vme, true);
			m_SelectedLandmark = (mafVMELandmark *)vme;
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
void mafLogicWithManagers::VmeChooseMaterial(mafVME *vme, bool updateProperty)
{
	if (m_MaterialChooser == NULL)
	{
		m_MaterialChooser = new mafGUIMaterialChooser();
	}
	if (m_MaterialChooser->ShowChooserDialog(vme))
	{
		this->m_ViewManager->PropertyUpdate(updateProperty);
		this->m_ViewManager->CameraUpdate();
		this->m_VMEManager->MSFModified(true);
	}
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeUpdateProperties(mafVME *vme, bool updatePropertyFromTag)
{
	this->m_ViewManager->PropertyUpdate(updatePropertyFromTag);
	this->m_ViewManager->CameraUpdate();
	this->m_VMEManager->MSFModified(true);
}


//----------------------------------------------------------------------------
void mafLogicWithManagers::ShowInSideBar(mafVME * vme, bool visibility)
{
	bool vme_in_tree = vme->IsVisible(); //check VisibleToTraverse flag.
	if (m_SideBar && vme_in_tree)
		m_SideBar->VmeShow(vme, visibility);
}

// Landmark
//----------------------------------------------------------------------------
void mafLogicWithManagers::SelectLandmark(mafVMELandmark *lm, bool select)
{
	if (!select)
		for (mafView * view = m_ViewManager->GetList(); view; view = view->m_Next)
			view->VmeShow(lm, false);
	else
	{
		mafVMELandmarkCloud *lmParent = mafVMELandmarkCloud::SafeDownCast(lm->GetParent());

		if (lmParent && lmParent->IsLandmarkShow((mafVMELandmark *)lm))

			for (mafView * view = m_ViewManager->GetList(); view; view = view->m_Next)
				if (view->GetNodeStatus(lmParent) == NODE_VISIBLE_ON)
					view->VmeShow(lm, true);
				else
					view->VmeShow(lm, false);
	}
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ShowLandmarkCloud(mafVMELandmarkCloud * lmc, bool visibility)
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
void mafLogicWithManagers::ShowLandmark(mafVMELandmark * lm, bool visibility)
{
	mafVMELandmarkCloud *lmParent = mafVMELandmarkCloud::SafeDownCast(lm->GetParent());
	if (!lmParent)
		return;

	mafPipe *lmPipe = NULL;
	//show of lm shows also the lmc
	//hide of last lm hides also the lmc
	if (visibility || (!visibility && lmParent->GetLandmarkShowNumber() == 0))
	{
		mafView * selectedView = m_ViewManager->GetSelectedView();
		if (mafViewCompound *compView = mafViewCompound::SafeDownCast(selectedView))
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
void mafLogicWithManagers::OpRunStarting()
//----------------------------------------------------------------------------
{
	m_RunningOperation = true;
  EnableMenuAndToolbar();
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if(m_InteractionManager) m_InteractionManager->EnableSelect(false);
#endif
  if(m_SideBar)    m_SideBar->EnableSelect(false);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpRunTerminated(int runOk)
//----------------------------------------------------------------------------
{
	m_RunningOperation = false;
  EnableMenuAndToolbar();
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if(m_InteractionManager) 
    m_InteractionManager->EnableSelect(true);
#endif
  if(m_SideBar)
    m_SideBar->EnableSelect(true);
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


// VIEW //////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewCreate(int viewId)
{
	if(m_ViewManager)
  {
    mafView* v = m_ViewManager->ViewCreate(viewId);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewCreated(mafView *v)
{
	// removed temporarily support for external Views
	if (v)
	{
		// child views
		mafGUIMDIChild *c = new mafGUIMDIChild(m_Win, v);
		c->SetWindowStyleFlag(m_ChildFrameStyle);
		c->SetListener(m_ViewManager);
		v->SetFrame(c);
	}
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewSelect()
{
	if (m_ViewManager)
	{
		mafView *view = m_ViewManager->GetSelectedView();
		if (m_SideBar)	m_SideBar->ViewSelect(view);

		// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
		if (m_InteractionManager)
		{
			m_InteractionManager->ViewSelected(view);
		}
#endif

		if (m_OpManager && !m_OpManager->Running())
		{
			// needed to update all the operations that will be enabled on View Creation
			m_OpManager->VmeSelected(m_OpManager->GetSelectedVme());
		}
	}
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewContextualMenu(bool vme_menu)
{
	mafGUIContextualMenu *contextMenu = new mafGUIContextualMenu();
	contextMenu->SetListener(this);
	mafView *v = m_ViewManager->GetSelectedView();
	mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
	if (c != NULL)
		contextMenu->ShowContextualMenu(c, v, vme_menu);
	cppDEL(contextMenu);
}

// CAMERA ////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void mafLogicWithManagers::CameraUpdate()
{
	if (m_ViewManager)
		m_ViewManager->CameraUpdate();

#ifdef MAF_USE_VTK
	//PERFORMANCE WARNING : if view manager exist this will cause another camera update!!
	// An else could be added to reduce CAMERA_UPDATE by a 2 factor. This has been done for the HipOp vertical App showing
	// big performance improvement in composite views creation.
	if (m_InteractionManager)
		m_InteractionManager->CameraUpdate(m_ViewManager->GetSelectedView());
#endif
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::CameraReset()
{
	if (m_ViewManager) 
		m_ViewManager->CameraReset();
}


// WIZARD ////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateWizardToolbar()
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
//----------------------------------------------------------------------------
void mafLogicWithManagers::ConfigureWizardManager()
	//----------------------------------------------------------------------------
{
	//Setting wizard specific data
	if(m_UseWizardManager)
	{
		m_WizardManager = new mafWizardManager();
		m_WizardManager->SetListener(this);
		m_WizardManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
	}
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::WizardRunStarting()
{
	//Disabling menu, toolbars and selection by interacion manager and sidebar
	m_WizardRunning = true;

	EnableMenuAndToolbar();
	// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
	if (m_InteractionManager) m_InteractionManager->EnableSelect(false);
#endif
	if (m_SideBar)	m_SideBar->EnableSelect(false);

}
//----------------------------------------------------------------------------
void mafLogicWithManagers::WizardRunTerminated()
{
	//Enabling menu, toolbars and selection by interacion manager and sidebar
	m_WizardRunning = false;
	
	EnableMenuAndToolbar();
	// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
	if (m_InteractionManager)
		m_InteractionManager->EnableSelect(true);
#endif
	if (m_SideBar)
		m_SideBar->EnableSelect(true);
}


// MENU-TOOLBAR //////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateMenu()
{
#include "pic/menu/EDIT_COPY.xpm"
#include "pic/menu/EDIT_CUT.xpm"
#include "pic/menu/EDIT_DELETE.xpm"
#include "pic/menu/EDIT_PASTE.xpm"
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
#include "pic/menu/WINDOW_CASCADE.xpm"
#include "pic/menu/WINDOW_HORIZONTALLY.xpm"
#include "pic/menu/WINDOW_PREV.xpm"
#include "pic/menu/WINDOW_NEXT.xpm"
#include "pic/menu/WINDOW_VERTICALLY.xpm"

	m_MenuBar = new wxMenuBar;
	wxMenu *file_menu = new wxMenu;
	mafGUI::AddMenuItem(file_menu, MENU_FILE_NEW, _("&New  \tCtrl+N"), FILE_NEW_xpm);
	mafGUI::AddMenuItem(file_menu, MENU_FILE_OPEN, _("&Open   \tCtrl+O"), FILE_OPEN_xpm);
	mafGUI::AddMenuItem(file_menu, MENU_FILE_SAVE, _("&Save  \tCtrl+S"), FILE_SAVE_xpm);
	file_menu->Append(MENU_FILE_SAVEAS, _("Save &As  \tCtrl+Shift+S"));

	m_ImportMenu = new wxMenu;
	file_menu->AppendSeparator();
	file_menu->Append(0, _("Import"), m_ImportMenu);

	m_ExportMenu = new wxMenu;
	file_menu->Append(0, _("Export"), m_ExportMenu);

	// Print menu item
	file_menu->AppendSeparator();
	mafGUI::AddMenuItem(file_menu, MENU_FILE_PRINT, _("&Print  \tCtrl+P"), FILE_PRINT_xpm);
	mafGUI::AddMenuItem(file_menu, MENU_FILE_PRINT_PREVIEW, _("Print Preview"), FILE_PRINT_PREVIEW_xpm);
	file_menu->Append(MENU_FILE_PRINT_SETUP, _("Printer Setup"));
	file_menu->Append(MENU_FILE_PRINT_PAGE_SETUP, _("Page Setup"));

	m_RecentFileMenu = new wxMenu;
	file_menu->AppendSeparator();
	file_menu->Append(0, _("Recent Files"), m_RecentFileMenu);

	file_menu->AppendSeparator();
	mafGUI::AddMenuItem(file_menu, MENU_FILE_QUIT, _("&Quit  \tCtrl+Q"), FILE_EXIT_xpm);

	m_MenuBar->Append(file_menu, _("&File"));

	wxMenu    *edit_menu = new wxMenu;

	mafGUI::AddMenuItem(edit_menu, OP_UNDO, _("Undo  \tCtrl+Z"), EDIT_UNDO_xpm);
	mafGUI::AddMenuItem(edit_menu, OP_REDO, _("Redo  \tCtrl+Shift+Z"), EDIT_REDO_xpm);

	edit_menu->AppendSeparator();
	mafGUI::AddMenuItem(edit_menu, OP_CUT, _("Cut   \tCtrl+Shift+X"), EDIT_CUT_xpm);
	mafGUI::AddMenuItem(edit_menu, OP_COPY, _("Copy  \tCtrl+Shift+C"), EDIT_COPY_xpm);
	mafGUI::AddMenuItem(edit_menu, OP_PASTE, _("Paste \tCtrl+Shift+V"), EDIT_PASTE_xpm);
	mafGUI::AddMenuItem(edit_menu, OP_DELETE, _("Delete  \tCtrl+Shift+Del"), EDIT_DELETE_xpm);

	edit_menu->AppendSeparator();
	mafGUI::AddMenuItem(edit_menu, OP_REPARENT, _("Reparent to... \tCtrl+Shift+R"), EDIT_REPARENT_xpm);
	mafGUI::AddMenuItem(edit_menu, MENU_EDIT_FIND_VME, _("Find VME \tCtrl+Shift+F"), EDIT_FIND_xpm);

	edit_menu->AppendSeparator();
	mafGUI::AddMenuItem(edit_menu, ID_APP_SETTINGS, _("Settings..."), EDIT_SETTINGS_xpm);

	m_MenuBar->Append(edit_menu, _("&Edit"));

	m_ViewMenu = new wxMenu;
	m_MenuBar->Append(m_ViewMenu, _("&View"));

	m_OpMenu = new wxMenu;
	m_MenuBar->Append(m_OpMenu, _("&Operations"));

	wxMenu    *windowMenu = new wxMenu;
	m_MenuBar->Append(windowMenu, _("Window"));
	mafGUI::AddMenuItem(windowMenu, TILE_WINDOW_CASCADE, _("&Cascade"), WINDOW_CASCADE_xpm);
	mafGUI::AddMenuItem(windowMenu, TILE_WINDOW_HORIZONTALLY, _("Tile &Horizontally"), WINDOW_HORIZONTALLY_xpm);
	mafGUI::AddMenuItem(windowMenu, TILE_WINDOW_VERTICALLY, _("Tile &Vertically"), WINDOW_VERTICALLY_xpm);

	windowMenu->AppendSeparator();
	mafGUI::AddMenuItem(windowMenu, IDM_WINDOWNEXT, _("&Next"), WINDOW_NEXT_xpm);
	mafGUI::AddMenuItem(windowMenu, IDM_WINDOWPREV, _("&Previous"), WINDOW_PREV_xpm);

	wxMenu    *help_menu = new wxMenu;
	mafGUI::AddMenuItem(help_menu, ABOUT_APPLICATION, _("About"), HELP_ABOUT_xpm);
	mafGUI::AddMenuItem(help_menu, HELP_HOME, _("Help"), HELP_HELP_xpm);


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
void mafLogicWithManagers::CreateToolbar()
{
	//m_ToolBar = new wxToolBar(m_Win,-1,wxPoint(0,0),wxSize(-1,-1),wxHORIZONTAL|wxNO_BORDER|wxTB_FLAT  );
	m_ToolBar = new wxToolBar(m_Win, MENU_VIEW_TOOLBAR, wxPoint(0, 0), wxSize(-1, -1), wxTB_FLAT | wxTB_NODIVIDER);
	m_ToolBar->SetMargins(0, 0);
	m_ToolBar->SetToolSeparation(2);
	m_ToolBar->SetToolBitmapSize(wxSize(20, 20));
	m_ToolBar->AddTool(MENU_FILE_NEW, mafPictureFactory::GetPictureFactory()->GetBmp("FILE_NEW"), _("new " + m_Extension + " storage file"));
	m_ToolBar->AddTool(MENU_FILE_OPEN, mafPictureFactory::GetPictureFactory()->GetBmp("FILE_OPEN"), _("open " + m_Extension + " storage file"));
	m_ToolBar->AddTool(MENU_FILE_SAVE, mafPictureFactory::GetPictureFactory()->GetBmp("FILE_SAVE"), _("save current " + m_Extension + " storage file"));
	m_ToolBar->AddSeparator();

	m_ToolBar->AddTool(MENU_FILE_PRINT, mafPictureFactory::GetPictureFactory()->GetBmp("PRINT"), _("print the selected view"));
	m_ToolBar->AddTool(MENU_FILE_PRINT_PREVIEW, mafPictureFactory::GetPictureFactory()->GetBmp("PRINT_PREVIEW"), _("show the print preview for the selected view"));
	m_ToolBar->AddSeparator();

	m_ToolBar->AddTool(OP_UNDO, mafPictureFactory::GetPictureFactory()->GetBmp("OP_UNDO"), _("undo (ctrl+z)"));
	m_ToolBar->AddTool(OP_REDO, mafPictureFactory::GetPictureFactory()->GetBmp("OP_REDO"), _("redo (ctrl+shift+z)"));
	m_ToolBar->AddSeparator();

	m_ToolBar->AddTool(OP_CUT, mafPictureFactory::GetPictureFactory()->GetBmp("OP_CUT"), _("cut selected vme (ctrl+x)"));
	m_ToolBar->AddTool(OP_COPY, mafPictureFactory::GetPictureFactory()->GetBmp("OP_COPY"), _("copy selected vme (ctrl+c)"));
	m_ToolBar->AddTool(OP_PASTE, mafPictureFactory::GetPictureFactory()->GetBmp("OP_PASTE"), _("paste vme (ctrl+v)"));
	m_ToolBar->AddSeparator();
	m_ToolBar->AddTool(CAMERA_RESET, mafPictureFactory::GetPictureFactory()->GetBmp("ZOOM_ALL"), _("reset camera to fit all (ctrl+f)"));
	m_ToolBar->AddTool(CAMERA_FIT, mafPictureFactory::GetPictureFactory()->GetBmp("ZOOM_SEL"), _("reset camera to fit selected object (ctrl+shift+f)"));
	m_ToolBar->AddTool(CAMERA_FLYTO, mafPictureFactory::GetPictureFactory()->GetBmp("FLYTO"), _("fly to object under mouse"));
	
	if (m_UseSnapshotManager)
	{
		m_ToolBar->AddSeparator();
		m_ToolBar->AddTool(MENU_FILE_SNAPSHOT, mafPictureFactory::GetPictureFactory()->GetBmp("CAMERA"), _("Create Snapshot"));
		m_ToolBar->AddTool(MENU_FILE_MANAGE_SNAPSHOT, mafPictureFactory::GetPictureFactory()->GetBmp("IMAGE_PREVIEW"), _("Manage Snapshots"));
	}

	EnableItem(CAMERA_RESET, false);
	EnableItem(CAMERA_FIT, false);
	EnableItem(CAMERA_FLYTO, false);
	EnableItem(MENU_FILE_PRINT, false);
	EnableItem(MENU_FILE_PRINT_PREVIEW, false);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateToolBarsAndPanels()
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
void mafLogicWithManagers::CreateAndPlugToolbar()
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
void mafLogicWithManagers::CreateTimeBar()
{
	m_TimePanel = new mafGUITimeBar(m_Win,MENU_VIEW_TIMEBAR,true);
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
void mafLogicWithManagers::CreateControlPanel()
{
	m_SideBar = new mafSideBar(m_Win, MENU_VIEW_SIDEBAR, this, m_SidebarStyle);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::FillMenus()
{
	if (m_VMEManager && m_RecentFileMenu)
	{
		m_VMEManager->SetFileHistoryMenu(m_RecentFileMenu);
	}

	if (m_UseViewManager && m_ViewMenu)
	{
		m_ViewManager->FillMenu(m_ViewMenu);
		m_ViewMenu->AppendSeparator();
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
void mafLogicWithManagers::EnableMenuAndToolbar()
{
	bool enable = !(m_RunningOperation || m_WizardRunning);

	if (!m_VMEManager)
		return;

	mafView * selectedView = m_ViewManager ?m_ViewManager->GetSelectedView() : NULL; 

	EnableItem(MENU_FILE_NEW, enable);
	EnableItem(MENU_FILE_OPEN, enable);
	EnableItem(MENU_FILE_SAVE, enable);
	EnableItem(MENU_FILE_SAVEAS, enable);
	EnableItem(MENU_FILE_MERGE, enable);
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
void mafLogicWithManagers::EnableItem(int item, bool enable)
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
void mafLogicWithManagers::TreeContextualMenu(mafEvent &e)
{
	mafGUITreeContextualMenu *contextMenu = new mafGUITreeContextualMenu();
	contextMenu->SetListener(this);
	contextMenu->SetOpManager(m_OpManager);

	mafView *v = m_ViewManager->GetSelectedView();
	mafVME  *vme = e.GetVme();
	bool vme_menu = e.GetBool();
	bool autosort = e.GetArg() != 0;

	contextMenu->CreateContextualMenu((mafGUICheckTree *)e.GetSender(), v, vme, vme_menu);
	contextMenu->ShowContextualMenu();

	cppDEL(contextMenu);
}

// Splash Screen
//----------------------------------------------------------------------------
void mafLogicWithManagers::ShowSplashScreen()
{
	wxBitmap splashImage = mafPictureFactory::GetPictureFactory()->GetBmp("SPLASH_SCREEN");
	ShowSplashScreen(splashImage);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ShowSplashScreen(wxBitmap &splashImage)
{
	long splash_style = wxSIMPLE_BORDER | wxSTAY_ON_TOP;
	wxSplashScreen* splash = new wxSplashScreen(splashImage,
		wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
		2000, NULL, -1, wxDefaultPosition, wxDefaultSize,
		splash_style);
	mafYield();
}

// WebSite
//----------------------------------------------------------------------------
void mafLogicWithManagers::ShowWebSite(wxString url)
{
	mafLogMessage("Opening %s", url.c_str());

	//WXwidget does not manage urls that contains anchor
	//in this case we need to create a temp file which redirect to the correct url 
	//and open this file
	if (url.Contains("#"))
	{
		wxString tmpHtmlFileName = mafGetAppDataDirectory().c_str();
		tmpHtmlFileName += "/loadPage.html";

		FILE *tmpHtmlFile = fopen(tmpHtmlFileName.c_str(), "w");

		fprintf(tmpHtmlFile, "<html>\n");
		fprintf(tmpHtmlFile, "<head>\n");
		fprintf(tmpHtmlFile, "	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\" />\n");
		fprintf(tmpHtmlFile, "	<meta http-equiv=\"refresh\" content=\"1;url=%s\" />\n", url.c_str());
		fprintf(tmpHtmlFile, "<title></title>\n");
		fprintf(tmpHtmlFile, "</head>\n");
		fprintf(tmpHtmlFile, "<body/>\n");
		fprintf(tmpHtmlFile, "</html>\n");
		fclose(tmpHtmlFile);

		url = tmpHtmlFileName;
	}

	wxString command = "rundll32.exe url.dll,FileProtocolHandler ";
	command = command + "\"" + url.c_str() + "/\"";
	wxExecute(command);
}

//----------------------------------------------------------------------------
void mafLogicWithManagers::PrintImage(mafVMEImage *img)
{
	m_PrintSupport->OnPrint(img);
}

// Log
//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateNullLog()
{
#ifdef MAF_USE_VTK
	m_VtkLog = mafVTKLog::New();
	m_VtkLog->SetInstance(m_VtkLog);
#endif  
	wxTextCtrl *log = new wxTextCtrl(m_Win, -1, "", wxPoint(0, 0), wxSize(100, 300), wxNO_BORDER | wxTE_MULTILINE);
	m_Logger = new mafWXLog(log);
	log->Show(false);
	wxLog *old_log = wxLog::SetActiveTarget(m_Logger);
	cppDEL(old_log);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateLogPanel()
{
#ifdef MAF_USE_VTK
	m_VtkLog = mafVTKLog::New();
	m_VtkLog->SetInstance(m_VtkLog);
#endif
	wxTextCtrl *log = new wxTextCtrl(m_Win, MENU_VIEW_LOGBAR, "", wxPoint(0, 0), wxSize(100, 300), /*wxNO_BORDER |*/ wxTE_MULTILINE);
	m_Logger = new mafWXLog(log);
	m_Logger->LogToFile(m_LogToFile);
	if (m_LogToFile)
	{
		wxString s = m_ApplicationSettings->GetLogFolder().GetCStr();
		wxDateTime log_time = wxDateTime::Now();
		s += "\\";
		s += m_Win->GetTitle();
		s += wxString::Format("_%02d_%02d_%d_%02d_%2d", log_time.GetYear(), log_time.GetMonth() + 1, log_time.GetDay(), log_time.GetHour(), log_time.GetMinute());
		s += ".log";
		if (m_Logger->SetFileName(s) == MAF_ERROR)
		{
			wxMessageBox(wxString::Format("Unable to create log file %s!!", s), "Warning", wxOK | wxICON_WARNING);
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

	mafLogMessage(_("Welcome"));
}

// Layout
//----------------------------------------------------------------------------
void mafLogicWithManagers::StoreLayout()
{
	int pos[2], size[2];

	mafXMLStorage *xmlStorage = mafXMLStorage::New();
	xmlStorage->SetFileType("MLY");
	xmlStorage->SetVersion("2.0");
	mafVMERoot *root;
	mafNEW(root);
	root->Initialize();
	xmlStorage->SetDocument(root);

	wxString layout_file  = mafGetAppDataDirectory().c_str();
	layout_file << "\\layout.mly";

	wxFrame *frame = (wxFrame *)mafGetFrame();
	wxRect rect;
	rect = frame->GetRect();
	pos[0] = rect.GetPosition().x;
	pos[1] = rect.GetPosition().y;
	size[0] = rect.GetSize().GetWidth();
	size[1] = rect.GetSize().GetHeight();

	mmaApplicationLayout  *layout;
	mafNEW(layout);

	layout->SetApplicationInfo(m_Win->IsMaximized(), pos, size);
	bool toolbar_vis = m_Win->GetDockManager().GetPane("toolbar").IsShown();
	layout->SetInterfaceElementVisibility("toolbar", toolbar_vis);
	bool sidebar_vis = m_Win->GetDockManager().GetPane("sidebar").IsShown();
	layout->SetInterfaceElementVisibility("sidebar", sidebar_vis);
	bool logbar_vis = m_Win->GetDockManager().GetPane("logbar").IsShown();
	layout->SetInterfaceElementVisibility("logbar", logbar_vis);

	root->SetAttribute("ApplicationLayout", layout);

	xmlStorage->SetURL(layout_file);
	xmlStorage->Store();

	cppDEL(xmlStorage);
	mafDEL(root);
	mafDEL(layout);
} 
//----------------------------------------------------------------------------
void mafLogicWithManagers::RestoreLayout()
{
	mafXMLStorage *xmlStorage = mafXMLStorage::New();
	xmlStorage->SetFileType("MLY");
	xmlStorage->SetVersion("2.0");

	mafVMERoot *root;
	mafNEW(root);
	root->Initialize();
	xmlStorage->SetDocument(root);

	wxString layout_file  = mafGetAppDataDirectory().c_str();
	layout_file << "\\layout.mly";
	xmlStorage->SetURL(layout_file);
	if(wxFileExists(layout_file) && xmlStorage->Restore()==MAF_OK)
	{
		mafVME *root = (mafVME *)xmlStorage->GetDocument();

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
	}


	cppDEL(xmlStorage);
	mafDEL(root);
}

// OTHER /////////////////////////////////////////////////////////////////////
// Config
//----------------------------------------------------------------------------
void mafLogicWithManagers::TimeSet(double t)
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
void mafLogicWithManagers::UpdateTimeBounds()
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
void mafLogicWithManagers::UpdateMeasureUnit()
{
	for (mafView* v = m_ViewManager->GetList(); v; v = v->m_Next)
		v->OptionsUpdate();
}

//----------------------------------------------------------------------------
void mafLogicWithManagers::ImportExternalFile(mafString &filename)
{
	wxString path, name, ext;
	wxSplitPath(filename.GetCStr(), &path, &name, &ext);
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
void mafLogicWithManagers::CreateStorage(mafEvent *e)
{
	mafVMEStorage *oldStorage, *newStorage;
	oldStorage = (mafVMEStorage *)e->GetMafObject();
	if (oldStorage)
	{
		m_VMEManager->NotifyRemove(oldStorage->GetRoot());
		m_OpManager->VmeSelected(NULL);		
	}
	newStorage = mafVMEStorage::New();
	newStorage->SetListener(m_VMEManager);
	newStorage->GetRoot()->Initialize();
	e->SetMafObject(newStorage);

	mafDEL(oldStorage);
}

//----------------------------------------------------------------------------
void mafLogicWithManagers::HandleException()
{

	if (!m_FatalExptionOccurred)
	{
		m_FatalExptionOccurred = true;

		int answare = wxMessageBox(_("An Excpetion has occurred and this application must be closed.\nYou can make a attempt to save your work.\nDo You want to proceed?"), _("Fatal Exception!"), wxYES_NO | wxCENTER | wxICON_ERROR | wxYES_DEFAULT);
		if (answare == wxYES)
		{
			OnFileSaveAs();

			if (m_OpManager->Running())
				m_OpManager->StopCurrentOperation();
		}
	}
	OnQuit();
}
