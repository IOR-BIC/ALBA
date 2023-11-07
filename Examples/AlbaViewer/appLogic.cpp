/*=========================================================================
Program:   AlbaViewer
Module:    appLogic.cpp
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appDecl.h"
#include "appUtils.h"
#include "appLogic.h"
#include "appSideBar.h"

#include "albaGUIAboutDialog.h"
#include "albaGUIApplicationSettings.h"
#include "albaLogicWithManagers.h"
#include "albaOpImporterDicom.h"
#include "albaOpManager.h"
#include "albaSnapshotManager.h"
#include "albaVME.h"
#include "albaVMEGroup.h"
#include "albaVMEIterator.h"
#include "albaVMEVolumeGray.h"
#include "albaView.h"
#include "albaViewManager.h"
#include "albaVTKLog.h"

#ifdef USE_WIZARD
#include "albaWizardManager.h"
#endif

#include "vtkObject.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "albaGUIViewFrame.h"
#include "albaGUIMDIChild.h"
#include "vtkTimerLog.h"
#include "albaTagArray.h"

//----------------------------------------------------------------------------
appLogic::appLogic() : albaLogicWithManagers()
{
	m_OpeningMSF = false;

	// Init Operations to plug in Toolbar 
	m_OpImporterDicom = new albaOpImporterDicom("DICOM", true);

	m_Win->Maximize();

	// Set project file extension
	m_Extension = "alba";
}
//----------------------------------------------------------------------------
appLogic::~appLogic()
{
	albaDEL(m_OpImporterDicom);

	delete m_SideBar;
}

//----------------------------------------------------------------------------
void appLogic::Init(int argc, char **argv)
{
	albaLogicWithManagers::Init(argc, argv);
	m_Win->ShowDockPane("logbar", false);

	if (!wxDirExists(albaGetAppDataDirectory().char_str()))
		wxMkDir(albaGetAppDataDirectory().char_str());

	InitAboutDialog();

	SetApplicationStamp((albaString)"AlbaMaster");
	m_VMEManager->IgnoreAppStamp(true);

	// Create and Open View
	ViewCreate(VIEW_START + 1);
}

//----------------------------------------------------------------------------
void appLogic::InitAboutDialog()
{
	// Init About Dialog
	m_AboutDialog->SetTitle(m_AppTitle);
	m_AboutDialog->SetBuildNum(m_BuildNum.GetCStr());
	m_AboutDialog->SetWebSite("https://github.com/IOR-BIC");
	//m_AboutDialog->SetLicenseURL("https://github.com/IOR-BIC");
	//m_AboutDialog->SetExtraMessage("");

	// About Image
	wxString imageName = "AlbaViewerAbout";
	wxString imagesPath = appUtils::GetConfigDirectory().char_str();
	m_AboutDialog->SetImagePath(imagesPath + "/" + imageName + ".bmp");
}

//----------------------------------------------------------------------------
void appLogic::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case VME_PICKED:
		{
			albaVME* selectedVme = m_OpManager->GetSelectedVme();
		}
		break;

		// View Events
		case ID_SHOW_VIEW_VTK_SURFACE:
		{
			ViewCreate(VIEW_START);
		}
		break;
		case ID_SHOW_ORTHOSLICE_VIEW:
		{
			ViewCreate(VIEW_START + 1);
		}
		break;

		// WX Events
		case wxID_FILE1:
		case wxID_FILE2:
		case wxID_FILE3:
		case wxID_FILE4:
		case wxID_FILE5:
		case wxID_FILE6:
		case wxID_FILE7:
		case wxID_FILE8:
		case wxID_FILE9:
		{
			m_OpeningMSF = true;
			OnFileHistory(e->GetId());
			m_OpeningMSF = false;

			ShowVMEOnView();
		}
		break;
		case MENU_FILE_OPEN:
		{
			m_OpeningMSF = true;
			albaString *filename = e->GetString();
			if (filename)
			{
				OnFileOpen((*filename).GetCStr());
			}
			else
			{
				OnFileOpen();
			}

			UpdateFrameTitle();
			m_OpeningMSF = false;

			ShowVMEOnView();
		}
		break;
		case ID_GET_FILENAME:
		{
			e->SetString(&(m_VMEManager->GetFileName()));
		}
		break;

		default:
			albaLogicWithManagers::OnEvent(alba_event);
			break;
		} // end switch case
	} // end if SafeDowncast
	else
		albaLogicWithManagers::OnEvent(alba_event);
}

//----------------------------------------------------------------------------
void appLogic::RunOp(albaOp *op)
{
	m_OpManager->OpRun(op);
}

// Quit
//----------------------------------------------------------------------------
void appLogic::OnQuit()
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
	
	StoreLayout();

	albaGUIViewFrame::OnQuit();
	albaGUIMDIChild::OnQuit();
	m_Win->OnQuit();

	if (m_SnapshotManager)
		cppDEL(m_SnapshotManager);

	cppDEL(m_VMEManager);
	cppDEL(m_MaterialChooser);
	cppDEL(m_HelpManager);
	cppDEL(m_ViewManager);
	cppDEL(m_InteractionManager);
	cppDEL(m_OpManager);

	// must be deleted after m_VMEManager
	cppDEL(m_SideBar);

	albaYield();
	if (m_PlugLogPanel)
	{
		delete wxLog::SetActiveTarget(NULL);
	}

	vtkTimerLog::CleanupLog();
	vtkDEL(m_VtkLog);

	m_Win->Destroy();
}

// FILE //////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void appLogic::OnFileNew()
{
	if (m_OpManager)
		m_OpManager->VmeSelected(NULL);

	if (m_VMEManager)
	{
			m_VMEManager->MSFNew();
	}

	m_Win->SetTitle(m_AppTitle);
}
//----------------------------------------------------------------------------
void appLogic::OnFileOpen(const char *file_to_open)
{
	if (m_VMEManager)
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
			wxString lastFolder = albaGetLastUserFolder().char_str();
			file = albaGetOpenFile(lastFolder, wildc).char_str();
		}

		if (file.IsEmpty())
			return;

		int opened = m_VMEManager->MSFOpen(file);
	}
}

//----------------------------------------------------------------------------
void appLogic::ViewCreate(int viewId)
{
	albaView *currentView = NULL;
	
	//currentView = m_ViewManager->GetSelectedView(); // Create only one view

	if (!currentView)
	{
		currentView = m_ViewManager->ViewCreate(viewId);
		currentView->GetFrame()->SetMaxSize(currentView->GetFrame()->GetSize());
		ShowVMEOnView();
	}
}

//----------------------------------------------------------------------------
void appLogic::ShowVMEOnView()
{
	albaVMERoot *root = this->m_VMEManager->GetRoot();
	albaVMEIterator *iter = root->NewIterator();
	albaVME *volume = NULL;

	for (albaVME *iNode = iter->GetFirstNode(); iNode; iNode = iter->GetNextNode())
	{
		if (iNode->IsA("albaVMEVolumeGray"))
		{
			volume = iNode;
		}
	}
	
	if (volume)
	{
		VmeShow(volume, true); // Show Last Volume
	}


	for (albaVME *iNode = iter->GetFirstNode(); iNode; iNode = iter->GetNextNode())
	{
		if (!iNode->IsA("albaVMEVolumeGray"))
		{
			bool vme_in_tree = !iNode->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") ||
				(iNode->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") &&
					iNode->GetTagArray()->GetTag("VISIBLE_IN_THE_TREE")->GetValueAsDouble() != 0);

			VmeShow(iNode, vme_in_tree); // Show all VMEs
		}
	}

	iter->Delete();
}

//----------------------------------------------------------------------------
void appLogic::VmeAdded(albaVME *vme)
{
	albaLogicWithManagers::VmeAdded(vme);

	//VmeShow(vme, true);
}

//----------------------------------------------------------------------------
void appLogic::CreateMenu()
{
	// Include Icons
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

	// File Menu //////////////////////////////////////////////////////////////////////////
	wxMenu *file_menu = new wxMenu;
	albaGUI::AddMenuItem(file_menu, MENU_FILE_NEW, _("&New  \tCtrl+N"), FILE_NEW_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_OPEN, _("&Open   \tCtrl+O"), FILE_OPEN_xpm);
	file_menu->AppendSeparator();

	// Import menu item
	m_ImportMenu = new wxMenu; // NOT USED

	// Export menu item
	m_ExportMenu = new wxMenu;  // NOT USED

	// Print menu item
	albaGUI::AddMenuItem(file_menu, MENU_FILE_PRINT, _("&Print  \tCtrl+P"), FILE_PRINT_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_PRINT_PREVIEW, _("Print Preview"), FILE_PRINT_PREVIEW_xpm);
	file_menu->Append(MENU_FILE_PRINT_SETUP, _("Printer Setup"));
	file_menu->Append(MENU_FILE_PRINT_PAGE_SETUP, _("Page Setup"));

	// Recent file item
	m_RecentFileMenu = new wxMenu;
	file_menu->AppendSeparator();
	file_menu->Append(0, _("Recent Files"), m_RecentFileMenu);
	file_menu->AppendSeparator();
	albaGUI::AddMenuItem(file_menu, MENU_FILE_QUIT, _("&Quit  \tCtrl+Q"), FILE_EXIT_xpm);
	
	m_MenuBar->Append(file_menu, _("&File"));

	// Edit Menu //////////////////////////////////////////////////////////////////////////
	wxMenu *edit_menu = new wxMenu;
	albaGUI::AddMenuItem(edit_menu, OP_UNDO, _("Undo  \tCtrl+Z"), EDIT_UNDO_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_REDO, _("Redo  \tCtrl+Shift+Z"), EDIT_REDO_xpm);
	edit_menu->AppendSeparator();

	albaGUI::AddMenuItem(edit_menu, ID_APP_SETTINGS, _("Settings..."), EDIT_SETTINGS_xpm);
	m_MenuBar->Append(edit_menu, _("&Edit"));

	// View Menu //////////////////////////////////////////////////////////////////////////
	m_ViewMenu = new wxMenu;
	m_MenuBar->Append(m_ViewMenu, _("&View"));

	// Operations Menu ////////////////////////////////////////////////////////////////////
	m_OpMenu = new wxMenu; 
	m_MenuBar->Append(m_OpMenu, _("&Operations"));

	// Wizard Menu ////////////////////////////////////////////////////////////////////
	if (m_UseWizardManager)
	{
		m_WizardMenu = new wxMenu;
		m_MenuBar->Append(m_WizardMenu, _("&Wizard"));
	}

	// Help Menu //////////////////////////////////////////////////////////////////////////
	wxMenu *help_menu = new wxMenu;
	albaGUI::AddMenuItem(help_menu, ABOUT_APPLICATION, _("About"), HELP_ABOUT_xpm);
	albaGUI::AddMenuItem(help_menu, HELP_HOME, _("Help"), HELP_HELP_xpm);
	m_MenuBar->Append(help_menu, _("&Help"));

	m_Win->SetMenuBar(m_MenuBar);

	EnableItem(MENU_FILE_PRINT, false);
	EnableItem(MENU_FILE_PRINT_PREVIEW, false);
	EnableItem(MENU_FILE_PRINT_SETUP, false);
	EnableItem(MENU_FILE_PRINT_PAGE_SETUP, false);
}
//----------------------------------------------------------------------------
void appLogic::CreateToolbar()
{
	m_ToolBar = new wxToolBar(m_Win, MENU_VIEW_TOOLBAR, wxPoint(0, 0), wxSize(-1, -1), wxTB_FLAT | wxTB_NODIVIDER);
	m_ToolBar->SetMargins(0, 0);
	m_ToolBar->SetToolSeparation(2);
	m_ToolBar->SetToolBitmapSize(wxSize(20, 20));

	// File
	m_ToolBar->AddTool(MENU_FILE_NEW, _("New project"), albaPictureFactory::GetPictureFactory()->GetBmp("FILE_NEW"));
	m_ToolBar->AddTool(MENU_FILE_OPEN, _("Open project"), albaPictureFactory::GetPictureFactory()->GetBmp("FILE_OPEN"));
	m_ToolBar->AddSeparator();

	// Views	
	m_ToolBar->AddTool(ID_SHOW_VIEW_VTK_SURFACE, _("View Surface"), albaPictureFactory::GetPictureFactory()->GetBmp("VIEW_SURFACE_ICON"));
	m_ToolBar->AddTool(ID_SHOW_ORTHOSLICE_VIEW, _("View OrthoSlice"), albaPictureFactory::GetPictureFactory()->GetBmp("VIEW_ORTHO"));
	m_ToolBar->AddSeparator();

	// Camera	
	m_ToolBar->AddTool(CAMERA_RESET, _("Reset camera to fit all"), albaPictureFactory::GetPictureFactory()->GetBmp("ZOOM_ALL"));
	m_ToolBar->AddTool(CAMERA_FIT, _("Reset camera to fit selected object"), albaPictureFactory::GetPictureFactory()->GetBmp("ZOOM_SEL"));
	m_ToolBar->AddTool(CAMERA_FLYTO, _("Fly to object under mouse"), albaPictureFactory::GetPictureFactory()->GetBmp("FLYTO"));

	if (m_UseSnapshotManager)
	{
		// Snapshot tool
		m_ToolBar->AddSeparator();
		m_ToolBar->AddTool(MENU_FILE_SNAPSHOT, _("Create Snapshot"), albaPictureFactory::GetPictureFactory()->GetBmp("CAMERA"));
		m_ToolBar->AddTool(MENU_FILE_MANAGE_SNAPSHOT, _("Manage Snapshots"), albaPictureFactory::GetPictureFactory()->GetBmp("IMAGE_PREVIEW"));
	}
}
//----------------------------------------------------------------------------
void appLogic::EnableMenuAndToolbar()
{
	bool enable = !(m_RunningOperation || m_WizardRunning);

	albaLogicWithManagers::EnableMenuAndToolbar();

	albaVME *node = m_OpManager->GetSelectedVme();

	if (m_UseSnapshotManager && m_SnapshotManager)
	{
		EnableItem(MENU_FILE_MANAGE_SNAPSHOT, enable && m_SnapshotManager->HasSnapshots(m_VMEManager->GetRoot()));
		EnableItem(MENU_FILE_SNAPSHOT, enable && m_ViewManager->GetSelectedView() != NULL);
	}
}

