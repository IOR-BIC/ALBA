/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaLogicWithManagers
 Authors: Silvano Imboden, Paolo Quadrani
 
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
  
  #include "albaOpImporterVTK.h"
  #include "albaOpImporterSTL.h"
  #include "albaInteractionManager.h"
  #include "albaInteractionFactory.h"
  #include "albaInteractor.h"
  #include "albaDeviceManager.h"
  #include "albaAction.h"
  #include "albaDeviceButtonsPadMouse.h"
  #include "albaInteractorPER.h"
  #include "albaGUITreeContextualMenu.h"
  #include "albaGUIContextualMenu.h"
  #include "vtkCamera.h"
#endif

#include "albaSideBar.h"
#include "albaUser.h"
#include "albaGUIMDIFrame.h"
#include "albaGUIMDIChild.h"
#include "albaGUICheckTree.h"
#include "albaGUITimeBar.h"
#include "albaGUIMaterialChooser.h"
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

//IMPORTERS
#include "albaOpImporterASCII.h"
#include "albaOpImporterAnalogWS.h"
#include "albaOpImporterAnsysCDBFile.h"
#include "albaOpImporterAnsysInputFile.h"
#include "albaOpImporterC3D.h"
#ifdef ALBA_USE_GDCM
#include "albaGUIDicomSettings.h"
#include "albaOpImporterDicom.h"
#endif
#include "albaOpImporterGRFWS.h"
#include "albaOpImporterImage.h"
#include "albaOpImporterLandmark.h"
#include "albaOpImporterLandmarkTXT.h"
#include "albaOpImporterLandmarkWS.h"
#include "albaOpImporterMSF.h"
#include "albaOpImporterMSF1x.h"
#include "albaOpImporterMesh.h"
#include "albaOpImporterRAWImages.h"
#include "albaOpImporterRAWVolume.h"
#include "albaOpImporterSTL.h"
#include "albaOpImporterPLY.h"
#include "albaOpImporterVRML.h"
#include "albaOpImporterVTK.h"
#include "albaOpImporterMetaImage.h"
#include "albaOpImporterVTKXML.h"
#include "albaOpImporterAbaqusFile.h"
#include "albaOpMeshScarlarsImporter.h"
#include "albaOpImporterDicFile.h"
#include "albaOpImporterPointCloud.h"
#include "albaOpImporterScancoImageIO.h"

//EXPORTERS
#include "albaOpExporterAnsysCDBFile.h"
#include "albaOpExporterAnsysInputFile.h"
#include "albaOpExporterBmp.h"
#ifdef ALBA_USE_GDCM
#include "albaOpExporterDicom.h"
#endif 
#include "albaOpExporterGRFWS.h"
#include "albaOpExporterLandmark.h"
#include "albaOpExporterMSF.h"
#include "albaOpExporterMesh.h"
#include "albaOpExporterMeters.h"
#include "albaOpExporterRAW.h"
#include "albaOpExporterSTL.h"
#include "albaOpExporterPLY.h"
#include "albaOpExporterVTK.h"
#include "albaOpExporterVTKXML.h"
#include "albaOpExporterMetaImage.h"
#include "albaOpExporterWrappedMeter.h"
#include "albaOpExporterAbaqusFile.h"

// OPERATIONS
#include "albaOp2DMeasure.h"
#include "albaOpMeasure2D.h"
#include "albaOpComputeInertialTensor.h"
#include "albaOpTransformOld.h"
#include "albaOpAddLandmark.h"
#include "albaOpBooleanSurface.h"
#include "albaOpClassicICPRegistration.h"
#include "albaOpCleanSurface.h"
#include "albaOpClipSurface.h"
#include "albaOpComputeWrapping.h"
#include "albaOpConnectivitySurface.h"
#include "albaOpCreateAverageLandmark.h"
#include "albaOpCreateAveragePlane.h"
#include "albaOpCreateEditSkeleton.h"
#include "albaOpCreateGroup.h"
#include "albaOpCreateLabeledVolume.h"
#include "albaOpCreateMeter.h"
#include "albaOpCreateProber.h"
#include "albaOpCreateRefSys.h"
#include "albaOpCreateRefSysFromViewSlice.h"
#include "albaOpCreateSlicer.h"
#include "albaOpCreatePolyline.h"
#include "albaOpCreateSpline.h"
#include "albaOpCreateSurface.h"
#include "albaOpCreateSurfaceParametric.h"
#include "albaOpCreateVolume.h"
#include "albaOpCreateInfoText.h"
#include "albaOpCreateWrappedMeter.h"
#include "albaOpCrop.h"
#include "albaOpCropDeformableROI.h"
#include "albaOpDecimateSurface.h"
#include "albaOpEditMetadata.h"
#include "albaOpEditNormals.h"
#include "albaOpExtractIsosurface.h"
#include "albaOpExtractImageFromArbitraryView.h"
#include "albaOpExtrusionHoles.h"
#include "albaOpEqualizeHistogram.h"
#include "albaOpFillHoles.h"
#include "albaOpFilterSurface.h"
#include "albaOpFilterVolume.h"
#include "albaOpFilterImage.h"
#include "albaOpFlipNormals.h"
#include "albaOpFreezeVME.h"
#include "albaOpInteractiveClipSurface.h"
#include "albaOpIterativeRegistration.h"
#include "albaOpLabelizeSurface.h"
#include "albaOpMML.h"
#include "albaOpMML3.h"
#include "albaOpMakeVMETimevarying.h"
#include "albaOpMeshDeformation.h"
#include "albaOpMeshQuality.h"
#include "albaOpMove.h"
#include "albaOpRegisterClusters.h"
#include "albaOpRemoveCells.h"
#include "albaOpScaleDataset.h"
#include "albaOpSegmentation.h"
#include "albaOpSegmentationRegionGrowingConnectedThreshold.h"
#include "albaOpSegmentationRegionGrowingLocalAndGlobalThreshold.h"
#include "albaOpSmoothSurface.h"
#include "albaOpSmoothSurfaceCells.h"
#include "albaOpSplitSurface.h"
#include "albaOpSubdivide.h"
#include "albaOpSurfaceMirror.h"
#include "albaOpTransform.h"
#include "albaOpTriangulateSurface.h"
#include "albaOpVOIDensity.h"
#include "albaOpVolumeMeasure.h"
#include "albaOpVolumeMirror.h"
#include "albaOpVolumeResample.h"
#include "albaOpESPCalibration.h"
#include "albaOpComputeHausdorffDistance.h"
#include "albaOpCreateCentroid.h"

//TMP
#include "albaOpImporterEmgWS.h"
#include "albaOpImporterExternalFile.h"
#include "albaOpImporterMotionData.h"
#include "albaOpImporterRawMotionData.h"
#include "albaOpImporterVMEDataSetAttributes.h"
#include "albaVMERawMotionData.h"
#include "albaOpExporterAnalogWS.h"
#include "albaOpExporterLandmarkWS.h"
#include "albaOpApplyTrajectory.h"
#include "albaOpCreateProsthesis.h"
#include "albaOpDecomposeTimeVarVME.h"
#include "albaOpExtractGeometry.h"
#include "albaOpGarbageCollectMSFDir.h"
#include "albaOpLabelExtractor.h"
#include "albaOpInteractionDebugger.h"
#include "albaOpMML3ContourWidget.h"
#include "albaOpMML3GlobalRegistration.h"
#include "albaOpMML3ModelView.h"
#include "albaOpMML3ModelView2DPipe.h"
#include "albaOpMML3ModelView3DPipe.h"
#include "albaOpMML3NonUniformSlicePipe.h"
#include "albaOpMML3ParameterView.h"
#include "albaOpMMLContourWidget.h"
#include "albaOpMMLModelView.h"
#include "albaOpMMLParameterView.h"
#include "albaOpMatrixVectorMath.h"
#include "albaOpOpenExternalFile.h"
#include "albaOpScalarToSurface.h"
#include "albaOpVOIDensityEditor.h"
#include "albaOpValidateTree.h"
#include "albaOpVolumeUnion.h"
#include "albaOpInteractorSliderSample.h"


// VIEWS
#include "albaView3D.h"
#include "albaViewArbitraryOrthoSlice.h"
#include "albaViewArbitrarySlice.h"
#include "albaViewCT.h"
#include "albaViewCompound.h"
#include "albaViewCompoundWindowing.h"
#include "albaViewGlobalSlice.h"
#include "albaViewGlobalSliceCompound.h"
#include "albaViewHTML.h"
#include "albaViewImage.h"
#include "albaViewImageCompound.h"
#include "albaViewIsosurfaceCompound.h"
#include "albaViewOrthoSlice.h"
#include "albaViewPlot.h"
#include "albaViewRX.h"
#include "albaViewRXCT.h"
#include "albaViewRXCompound.h"
#include "albaViewSlice.h"
#include "albaViewSliceBlend.h"
#include "albaViewSliceBlendRX.h"
#include "albaViewSliceGlobal.h"
#include "albaViewSliceOnCurve.h"
#include "albaViewSliceOnCurveCompound.h"
#include "albaViewSlicer.h"
#include "albaViewVTK.h"
#include "albaViewVTKCompound.h"
#include "albaViewVirtualRX.h"
#include "albaOpNearestLandmark.h"


#define IDM_WINDOWNEXT 4004
#define IDM_WINDOWPREV 4006



//----------------------------------------------------------------------------
albaLogicWithManagers::albaLogicWithManagers(albaGUIMDIFrame *mdiFrame/*=NULL*/)
{
	if (mdiFrame == NULL)
		m_Win = new albaGUIMDIFrame("alba", wxDefaultPosition, wxSize(800, 600), wxFRAME_NO_WINDOW_MENU);
	else
		m_Win = mdiFrame;

	m_Win->SetListener(this);

	m_ChildFrameStyle = 0; 
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
	m_RecentFileMenu = NULL;

	m_MaterialChooser = NULL;

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
	m_ShowInteractionSettings = false;
	m_SelectedLandmark = NULL;
	m_FatalExptionOccurred = false;

	m_EventFilterFunc = NULL;

	m_SkipCameraUpdate = false;

	m_OpeningMSF = false;

	m_AppLayout = NULL;

	m_AboutImage = "AlbaMasterAbout.bmp";
}
//----------------------------------------------------------------------------
albaLogicWithManagers::~albaLogicWithManagers()
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
		m_VMEManager->SetListener(this);
		m_VMEManager->SetMsfFileExtension(m_Extension);
		//m_VMEManager->SetSingleBinaryFile(m_StorageSettings->GetSingleFileStatus()!= 0);
	}

	if (m_UseInteractionManager)
	{
		m_InteractionManager = new albaInteractionManager();
		m_InteractionManager->SetListener(this);

		m_Mouse = m_InteractionManager->GetMouseDevice();
		//SIL m_InteractionManager->GetClientDevice()->AddObserver(this, MCH_INPUT);

		if (m_ShowInteractionSettings)
			m_SettingsDialog->AddPage(m_InteractionManager->GetGui(), _("Interaction Manager"));
	}
	
	if (m_UseViewManager)
	{
		m_ViewManager = new albaViewManager();
		m_ViewManager->SetListener(this);
		m_ViewManager->SetMouse(m_Mouse);

		m_SettingsDialog->AddPage(m_ViewManager->GetSettingsGui(), "View");
	}

	if (m_UseOpManager)
	{
		m_OpManager = new albaOpManager();
		m_OpManager->SetListener(this);
		m_OpManager->SetMouse(m_Mouse);
		m_OpManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
	}

	if (m_UseHelpManager)
	{
		m_HelpManager = new albaHelpManager();
		//m_HelpManager->SetListener(this);
	}

	if (m_UseSnapshotManager)
	{
		m_SnapshotManager = new albaSnapshotManager();
		m_SnapshotManager->SetMouse(m_Mouse);

		m_SettingsDialog->AddPage(m_SnapshotManager->GetSettingsGui(), "Snapshot");
	}

	ConfigureWizardManager();
}

// Window
//----------------------------------------------------------------------------
void albaLogicWithManagers::UpdateFrameTitle()
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

	m_AppTitle = m_Win->GetTitle().ToAscii();

	RestoreLayout();

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
    
// currently albaInteraction is strictly dependent on VTK
#ifdef ALBA_USE_VTK    
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
	albaGUIMDIChild::OnQuit();
	m_Win->OnQuit();

	if (m_SnapshotManager)
		cppDEL(m_SnapshotManager);

	cppDEL(m_VMEManager);
	cppDEL(m_MaterialChooser);
	cppDEL(m_HelpManager);

	cppDEL(m_ViewManager);

#ifdef ALBA_USE_VTK
	cppDEL(m_InteractionManager);
#endif
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
		/*if (e->GetId() == albaDataVector::SINGLE_FILE_DATA)
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
#ifdef ALBA_USE_VTK
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
			albaString *filename = e->GetString();
			if (filename)
				OnFileOpen((*filename).GetCStr());
			else
				OnFileOpen();
			UpdateFrameTitle();
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
			albaString *s = e->GetString();
			albaString str;

			if (s != NULL)
				str = *s;
			else
				str = "Choose Node";

			std::vector<albaVME*> nodeVector = m_SideBar->VmeChoose(e->GetPointer(), REPRESENTATION_AS_TREE, str, e->GetBool(), e->GetVme());
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
			albaString *s = e->GetString();
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
			albaGUIMDIChild *c = (albaGUIMDIChild *)m_Win->GetActiveChild();
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
				if (!albaOpSelect::SafeDownCast(m_OpManager->GetRunningOperation()))
					m_WizardManager->WizardContinue(e->GetArg());
			}
			//else we manage the operation end by unlock the close button and so on
			else
			{
				albaGUIMDIChild *c = (albaGUIMDIChild *)m_Win->GetActiveChild();
				if (c != NULL)
					c->SetAllowCloseWindow(true);
				OpRunTerminated(e->GetArg());
			}
		}
		break;
		case OP_SHOW_GUI:
			OpShowGui(!e->GetBool(), (albaGUIPanel*)e->GetWin());
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
			albaView *view = NULL;
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

#ifdef ALBA_USE_VTK
			// currently albaInteraction is strictly dependent on VTK (marco)
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
				albaGUIMDIChild *c = (albaGUIMDIChild *)m_Win->GetActiveChild();
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
			albaViewCompound *v = albaViewCompound::SafeDownCast(m_ViewManager->GetSelectedView());
			if (v && e->GetBool())
			{
				v->GetRWI()->SaveAllImages(v->GetLabel(), v);
			}
			else
			{
				albaView *v = m_ViewManager->GetSelectedView();
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
			// currently albaInteraction is strictly dependent on VTK (marco)
#ifdef ALBA_USE_VTK
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
			// currently albaInteraction is strictly dependent on VTK (marco)
#ifdef ALBA_USE_VTK
			if (m_InteractionManager)
			{
				vtkRenderer *ren = (vtkRenderer*)e->GetVtkObj();
				//assert(ren);
				m_InteractionManager->PreResetCamera(ren);
				//albaLogMessage("CAMERA_PRE_RESET");
			}
#endif
			break;
		case CAMERA_POST_RESET:
			// currently albaInteraction is strictly dependent on VTK (marco)
#ifdef ALBA_USE_VTK
			if (m_InteractionManager)
			{
				vtkRenderer *ren = (vtkRenderer*)e->GetVtkObj();
				//assert(ren); //modified by Marco. 2-11-2004 Commented out to allow reset camera of all cameras.
				m_InteractionManager->PostResetCamera(ren);
				//albaLogMessage("CAMERA_POST_RESET");
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
			// currently albaInteraction is strictly dependent on VTK (marco)
#ifdef ALBA_USE_VTK
			if (m_InteractionManager)
			{
				albaInteractor *interactor = albaInteractor::SafeDownCast(e->GetMafObject());
				assert(interactor);
				albaString *action_name = e->GetString();
				m_InteractionManager->BindAction(*action_name, interactor);
			}
#endif
			break;
		case INTERACTOR_REMOVE:
			// currently albaInteraction is strictly dependent on VTK (marco)
#ifdef ALBA_USE_VTK
			if (m_InteractionManager)
			{
				albaInteractor *interactor = albaInteractor::SafeDownCast(e->GetMafObject());
				assert(interactor);
				albaString *action_name = e->GetString();
				m_InteractionManager->UnBindAction(*action_name, interactor);
			}
#endif
			break;
		case PER_PUSH:
			// currently albaInteraction is strictly dependent on VTK (marco)
#ifdef ALBA_USE_VTK
			if (m_InteractionManager)
			{
				albaInteractorPER *per = albaInteractorPER::SafeDownCast(e->GetMafObject());
				assert(per);
				m_InteractionManager->PushPER(per);
			}
#endif
			break;
		case PER_POP:
			// currently albaInteraction is strictly dependent on VTK (marco)
#ifdef ALBA_USE_VTK
			if (m_InteractionManager) m_InteractionManager->PopPER();
#endif
			break;
		case GET_CURRENT_PER:
			if (m_InteractionManager)
				e->SetMafObject(m_InteractionManager->GetPER());
			break;
		case DEVICE_ADD:
			m_InteractionManager->AddDeviceToTree((albaDevice *)e->GetMafObject());
			break;
		case DEVICE_REMOVE:
			m_InteractionManager->RemoveDeviceFromTree((albaDevice *)e->GetMafObject());
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
				albaString typeName = e->GetString()->GetCStr();
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
				OnEvent(&albaEvent(this, WIZARD_RUN_CONTINUE, true));
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
			albaGUIMDIChild *c = (albaGUIMDIChild *)m_Win->GetActiveChild();
			if (c != NULL)
				c->SetAllowCloseWindow(false);
			WizardRunStarting();
		}
		break;
		case WIZARD_RUN_TERMINATED:
		{
			//Manage end event from the wizard unlock window close button
			//and enabling toolbar
			albaGUIMDIChild *c = (albaGUIMDIChild *)m_Win->GetActiveChild();
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
			albaView *view;
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
			albaView *view;
			const char *viewStr = e->GetString()->GetCStr();

			view = m_ViewManager->GetFromList(viewStr);
			if (view)
			{
				albaGUIMDIChild *c = (albaGUIMDIChild *)view->GetFrame();
				m_ViewManager->ViewDelete(view);
				if (c != NULL)
					c->Destroy();
			}
		}
		break;
		case WIZARD_RUN_OP:
		{
			//Running an op required from the wizard
			albaString *tmp = e->GetString();
			albaLogMessage("wiz starting :%s", tmp->GetCStr());
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
				albaLogMessage("Reload requested whitout opened MSF");
				//continue wizard with error
				m_WizardManager->WizardContinue(false);
			}
			else
			{
				int opened = m_VMEManager->MSFOpen(file);
				//continue wizard after open operation
				m_WizardManager->WizardContinue(opened != ALBA_ERROR);
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
#ifdef _DEBUG
			e->Log();
#endif 
			break;
		} // end switch case
	} // end if SafeDowncast
}

// FILE //////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaLogicWithManagers::OnFileNew()
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
void albaLogicWithManagers::OnFileOpen(const char *file_to_open)
{
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
			
			if (file.IsEmpty() && m_WizardManager && m_WizardRunning)
				m_WizardManager->WizardContinue(false);
			else if (file.IsEmpty())
				return;

			int opened = m_VMEManager->MSFOpen(file);
			//If there is a wizzard running we need to continue it after open operation
			if (m_WizardManager && m_WizardRunning)
				m_WizardManager->WizardContinue(opened != ALBA_ERROR);
		}
		m_OpeningMSF = false;
	}
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
void albaLogicWithManagers::OnFileSave()
{
  if(m_VMEManager)
  {
		albaString save_default_folder = albaGetLastUserFolder();
	  save_default_folder.ParsePathName();
	  m_VMEManager->SetDirName(save_default_folder);
	  int saved=m_VMEManager->MSFSave();
	  //If there is a wizard running we need to continue it after save operation
	  if (m_WizardManager && m_WizardRunning)
		  m_WizardManager->WizardContinue(saved!=ALBA_ERROR);
	  UpdateFrameTitle();
  }
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::OnFileSaveAs()
{
  if(m_VMEManager) 
  {
	  albaString save_default_folder = albaGetLastUserFolder();
	  save_default_folder.ParsePathName();
	  m_VMEManager->SetDirName(save_default_folder);
	  int saved=m_VMEManager->MSFSaveAs();
	  //If there is a wizard running we need to continue it after save operation
	  if (m_WizardManager && m_WizardRunning)
		  m_WizardManager->WizardContinue(saved!=ALBA_ERROR);
	  UpdateFrameTitle();
  }
}

// VME ///////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeShow(albaVME *vme, bool visibility)
{
	if (!vme)
		return;

	EnableCameraUpdateSkip();

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

	DisableCameraUpdateSkip();
	
	albaYield();

	CameraUpdate();
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
void albaLogicWithManagers::VmeDoubleClicked(albaEvent &e)
{
	albaVME *node = e.GetVme();
	if (node)
	{
		albaLogMessage("Double click on %s", node->GetName());
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeSelect(albaVME *vme)
{

	if (vme != NULL && m_OpManager && m_OpManager->GetSelectedVme()!=vme)
	{
		m_OpManager->OpSelect(vme);

		// currently albaInteraction is strictly dependent on VTK (marco)
#ifdef ALBA_USE_VTK
		if (m_InteractionManager)
			m_InteractionManager->VmeSelected(vme);
#endif

		EnableMenuAndToolbar();
	}
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
void albaLogicWithManagers::VmeChooseMaterial(albaVME *vme, bool updateProperty)
{
	if (m_MaterialChooser == NULL)
	{
		m_MaterialChooser = new albaGUIMaterialChooser();
	}
	if (m_MaterialChooser->ShowChooserDialog(vme))
	{
		this->m_ViewManager->PropertyUpdate(updateProperty);
		this->m_ViewManager->CameraUpdate();
		this->m_VMEManager->MSFModified(true);
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::VmeUpdateProperties(albaVME *vme, bool updatePropertyFromTag)
{
	this->m_ViewManager->PropertyUpdate(updatePropertyFromTag);
	this->m_ViewManager->CameraUpdate();
	this->m_VMEManager->MSFModified(true);
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
	m_RunningOperation = true;
  EnableMenuAndToolbar();

  if(m_InteractionManager) 
		m_InteractionManager->EnableSelect(false);

  if(m_SideBar)    
		m_SideBar->EnableSelect(false);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::OpRunTerminated(int runOk)
{
	m_RunningOperation = false;
  EnableMenuAndToolbar();

  if(m_InteractionManager) 
    m_InteractionManager->EnableSelect(true);

  if(m_SideBar)
    m_SideBar->EnableSelect(true);

	if(m_ApplicationSettings->IsAutoSaveOn() && !m_OpeningMSF)
		OnFileSave();
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::OpShowGui(bool push_gui, albaGUIPanel *panel)
{
	if(m_SideBar) m_SideBar->OpShowGui(push_gui, panel);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::OpHideGui(bool view_closed)
{
	if(m_SideBar) m_SideBar->OpHideGui(view_closed);
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
void albaLogicWithManagers::ViewCreated(albaView *v)
{
	// removed temporarily support for external Views
	if (v)
	{
		// child views
		albaGUIMDIChild *c = new albaGUIMDIChild(m_Win, v);
		c->SetWindowStyleFlag(m_ChildFrameStyle);
		c->SetListener((albaObserver*)m_ViewManager);
		v->SetFrame(c);
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::ViewSelect()
{
	if (m_ViewManager)
	{
		albaView *view = m_ViewManager->GetSelectedView();
		if (m_SideBar)	m_SideBar->ViewSelect(view);

		// currently albaInteraction is strictly dependent on VTK (marco)
#ifdef ALBA_USE_VTK
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
void albaLogicWithManagers::ViewContextualMenu(bool vme_menu)
{
	albaGUIContextualMenu *contextMenu = new albaGUIContextualMenu();
	contextMenu->SetListener(this);
	albaView *v = m_ViewManager->GetSelectedView();
	albaGUIMDIChild *c = (albaGUIMDIChild *)m_Win->GetActiveChild();
	if (c != NULL)
		contextMenu->ShowContextualMenu(c, v, vme_menu);
	cppDEL(contextMenu);
}

// CAMERA ////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void albaLogicWithManagers::CameraUpdate()
{
	if (m_ViewManager && !m_SkipCameraUpdate)
		m_ViewManager->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::CameraReset()
{
	if (m_ViewManager) 
		m_ViewManager->CameraReset();
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::EnableCameraUpdateSkip()
{
	m_SkipCameraUpdate++;
}

//----------------------------------------------------------------------------
void albaLogicWithManagers::DisableCameraUpdateSkip()
{
	assert(m_SkipCameraUpdate);
	m_SkipCameraUpdate--;
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
	//------------------------------------------------------------
	// Importer Menu:
	//------------------------------------------------------------

	Plug(new albaOpImporterImage("Images"));
	Plug(new albaOpImporterRAWVolume("RAW Volume"));
	Plug(new albaOpImporterRAWImages("RAW Images"));
	Plug(new albaOpImporterSTL("STL"));
	Plug(new albaOpImporterPLY("PLY"));
	Plug(new albaOpImporterVRML("VRML"));
	Plug(new albaOpImporterVTK("VTK"));
	Plug(new albaOpImporterVTKXML("VTK xml"));
	Plug(new albaOpImporterMetaImage("ITK MetaImage"));
	Plug(new albaOpImporterMSF("ALBA"));
	Plug(new albaOpImporterMSF1x("MSF 1.x"));
	Plug(new albaOpImporterAnsysCDBFile("Ansys CDB File"), "Finite Element");
	Plug(new albaOpImporterAnsysInputFile("Ansys Input File"), "Finite Element");
	Plug(new albaOpImporterAbaqusFile("Abaqus File"), "Finite Element");
	Plug(new albaOpImporterMesh("Generic Mesh"), "Finite Element");
	Plug(new albaOpMeshScarlarsImporter("Load Scalars from Lis files"), ("Finite Element"));
	Plug(new albaOpImporterASCII("ASCII"));

#ifdef ALBA_USE_GDCM
	albaGUIDicomSettings *dicomSettings = new albaGUIDicomSettings(NULL, "DICOM");
	Plug(new albaOpImporterDicom("DICOM"), "", true, dicomSettings);
#endif
	Plug(new albaOpImporterLandmark("Landmark"), "Landmark Suite");
	Plug(new albaOpImporterLandmarkTXT("Landmark TXT"), "Landmark Suite");
	Plug(new albaOpImporterLandmarkWS("Landmark WS"), "Landmark Suite");
	Plug(new albaOpImporterC3D("C3D"), "Landmark Suite");
	Plug(new albaOpImporterAnalogWS("EMG"));
	Plug(new albaOpImporterGRFWS("GRF"));
	Plug(new albaOpImporterDicFile("Digital Image Correlation (DIC)"));
	Plug(new albaOpImporterPointCloud("Generic Point Cloud"));
	Plug(new albaOpImporterScancoImageIO("Scanco IO"));
	Plug(new albaOpImporterScancoImageIO("Scanco IO")); 

	//------------------------------------------------------------
	// Exporter Menu:
	//------------------------------------------------------------

	Plug(new albaOpExporterMSF("ALBA"));
	Plug(new albaOpExporterSTL("STL"));
	Plug(new albaOpExporterPLY("PLY"));
	Plug(new albaOpExporterVTK("VTK"));
	Plug(new albaOpExporterVTKXML("VTK xml"));
	Plug(new albaOpExporterMetaImage("ITK MetaImage"));
#ifdef ALBA_USE_GDCM
	Plug(new albaOpExporterDicom("Dicom"), "", true, dicomSettings);
#endif
	Plug(new albaOpExporterRAW("Raw"));
	Plug(new albaOpExporterBmp("Bmp"));
	Plug(new albaOpExporterLandmark("Landmark"));
	Plug(new albaOpExporterWrappedMeter());
	Plug(new albaOpExporterGRFWS());
	Plug(new albaOpExporterMeters());
	Plug(new albaOpExporterAnsysCDBFile("Ansys CDB File"), "Finite Element");
	Plug(new albaOpExporterAnsysInputFile("Ansys Input File"), "Finite Element");
	Plug(new albaOpExporterAbaqusFile("Abaqus File"), "Finite Element");
	Plug(new albaOpExporterMesh("Generic Mesh"), "Finite Element");

	Plug(new albaOpImporterEmgWS("EmgWS"), "TMP to test");
	Plug(new albaOpImporterExternalFile(), "TMP to test");
	Plug(new albaOpImporterRawMotionData("Raw motion data"), "TMP to test");
	Plug(new albaOpImporterMotionData<albaVMERawMotionData>("Raw Motion Data (template)", "RAW Motion Data (*.MAN)|*.MAN", "Dictionary (*.txt)|*.txt"), "TMP to test");
	Plug(new albaOpImporterVMEDataSetAttributes(), "TMP to test");
	Plug(new albaOpExporterAnalogWS(), "TMP to test");
	Plug(new albaOpExporterLandmarkWS(), "TMP to test");


	//------------------------------------------------------------
	// Operation Menu:
	//------------------------------------------------------------

	Plug(new albaOpCreateVolume("Create Volume"), _("Create"));
	Plug(new albaOpAddLandmark("Add Landmark  \tCtrl+A"), _("Create"));
	Plug(new albaOpCreateAverageLandmark("Create Average Landmark"), _("Create"));
	Plug(new albaOpCreateAveragePlane("Create Average Plane"), _("Create"));
	Plug(new albaOpCreateGroup("Group"), _("Create"));
	Plug(new albaOpCreateMeter("Meter"), _("Create"));
	Plug(new albaOpCreateRefSys("RefSys"), _("Create"));
	Plug(new albaOpCreateInfoText("Info Text"), _("Create"));
	Plug(new albaOpCreateRefSysFromViewSlice("RefSys from View Slice"), _("Create"));
	Plug(new albaOpCreateProber("Prober"), _("Create"));
	Plug(new albaOpCreateSlicer("Slicer"), _("Create"));
	Plug(new albaOpExtractIsosurface("Extract Isosurface"), _("Create"));
	Plug(new albaOpExtractImageFromArbitraryView("Extract Image from Arbitrary View"), _("Create"));
	Plug(new albaOpCreateSurfaceParametric("Parametric Surface"), _("Create"));
	Plug(new albaOpCreatePolyline("Polyline"), _("Create"));
	Plug(new albaOpCreateSpline("Spline"), _("Create"));
	Plug(new albaOpCreateLabeledVolume(), _("Create"));
	Plug(new albaOpCreateEditSkeleton(), _("Create"));
	Plug(new albaOpCreateSurface(), _("Create"));
	Plug(new albaOpCreateWrappedMeter(), _("Create"));
	Plug(new albaOpComputeWrapping("Computing Wrapping"), _("Create"));
	Plug(new albaOpFreezeVME(), _("Create"));
	Plug(new albaOpSegmentationRegionGrowingConnectedThreshold(), _("Create"));
	Plug(new albaOpSegmentationRegionGrowingLocalAndGlobalThreshold(), _("Create"));
	Plug(new albaOpSegmentation(), _("Create"));
	Plug(new albaOpNearestLandmark(), _("Create"));
	Plug(new albaOpCreateCentroid(), _("Create"));


	Plug(new albaOpClipSurface("Clip Surface"), _("Modify"));
	Plug(new albaOpFilterSurface("Filter Surface"), _("Modify"));
	Plug(new albaOpFilterVolume("Filter Volume"), _("Modify"));
	Plug(new albaOpFilterImage("Filter Image"), _("Modify"));
	Plug(new albaOpDecimateSurface("Decimate Surface"), _("Modify"));
	Plug(new albaOpConnectivitySurface("Connectivity Surface"), _("Modify"));
	Plug(new albaOpEditNormals("Edit Normals"), _("Modify"));
	Plug(new albaOpEditMetadata("Metadata Editor"), _("Modify"));
	Plug(new albaOpTransformOld("Transform"), _("Modify"));
	Plug(new albaOpVolumeResample("Resample Volume"), _("Modify"));
	Plug(new albaOpCrop("Crop Volume"), _("Modify"));
	Plug(new albaOpBooleanSurface("Boolean Surface"), _("Modify"));
	Plug(new albaOpMML("MML"), _("Modify"));
	Plug(new albaOpMML3("MML 3"), _("Modify"));
	Plug(new albaOpCropDeformableROI("Crop ROI"), _("Modify"));
	Plug(new albaOpFlipNormals("Flip Normals"), _("Modify"));
	Plug(new albaOpRemoveCells("Remove Cells"), _("Modify"));
	Plug(new albaOpExtrusionHoles(), _("Modify"));
	Plug(new albaOpTransform(), _("Modify"));
	Plug(new albaOpScaleDataset("Scale Dataset"), _("Modify"));
	Plug(new albaOpMove("Move"), _("Modify"));
	Plug(new albaOpSplitSurface(), _("Modify"));
	Plug(new albaOpInteractiveClipSurface(), _("Modify"));
	Plug(new albaOpLabelizeSurface(), _("Modify"));
	Plug(new albaOpSmoothSurface(), _("Modify"));
	Plug(new albaOpCleanSurface(), _("Modify"));
	Plug(new albaOpTriangulateSurface(), _("Modify"));
	Plug(new albaOpSurfaceMirror(), _("Modify"));
	Plug(new albaOpVolumeMirror(), _("Modify"));
	Plug(new albaOpSubdivide(), _("Modify"));
	Plug(new albaOpFillHoles(), _("Modify"));
	Plug(new albaOpMeshDeformation(), _("Modify"));
	Plug(new albaOpMakeVMETimevarying(), _("Modify"));
	Plug(new albaOpEqualizeHistogram(), _("Modify"));
	Plug(new albaOpSmoothSurfaceCells(), _("Modify"));

	Plug(new albaOpMeasure2D("2D Measure"), "Measure");
	Plug(new albaOp2DMeasure("2D Measure (Old)"), "Measure");
	Plug(new albaOpVOIDensity("VOI Density"), "Measure");
	Plug(new albaOpVolumeMeasure("Volume"), "Measure");
	Plug(new albaOpMeshQuality("Mesh Quality"), "Measure");
	Plug(new albaOpComputeInertialTensor(), "Measure");
	Plug(new albaOpComputeHausdorffDistance(), "Measure");

	Plug(new albaOpIterativeRegistration(), _("Register"));
	Plug(new albaOpRegisterClusters("Clusters"), _("Register"));
	Plug(new albaOpClassicICPRegistration("Surface"), _("Register"));

	Plug(new albaOpESPCalibration("Calibrate ESP Phantom"), _("Calibration"));


	Plug(new albaOpApplyTrajectory(), "TMP to test");
	Plug(new albaOpCreateProsthesis(), "TMP to test");
	Plug(new albaOpDecomposeTimeVarVME(), "TMP to test");
	Plug(new albaOpExtractGeometry(), "TMP to test");
	Plug(new albaOpGarbageCollectMSFDir(), "TMP to test");
	Plug(new albaOpLabelExtractor(), "TMP to test");
	Plug(new albaOpInteractionDebugger(), "TMP to test");
	Plug(new albaOpOpenExternalFile(), "TMP to test");
	Plug(new albaOpScalarToSurface(), "TMP to test");
	Plug(new albaOpVOIDensityEditor(), "TMP to test");
	Plug(new albaOpValidateTree(), "TMP to test");
	Plug(new albaOpVolumeUnion(), "TMP to test");
	Plug(new albaOpInteractorSliderSample(), "TMP to test");

}

//----------------------------------------------------------------------------
void albaLogicWithManagers::PlugStandardViews()
{
	//------------------------------------------------------------
	// View Menu:
	//------------------------------------------------------------

	//View VTK
	Plug(new albaViewVTK("Surface"));

	//View Arbitrary Slice
	albaViewArbitrarySlice *varbitrary = new albaViewArbitrarySlice("Arbitrary Slice");
	varbitrary->PackageView();
	Plug(varbitrary);

	//View Orthoslice
	albaViewOrthoSlice *vortho = new albaViewOrthoSlice("Orthoslice");
	vortho->PackageView();
	Plug(vortho);

	//View RX Compound
	albaViewRXCompound *vrx = new albaViewRXCompound("RX");
	vrx->PackageView();
	Plug(vrx);

	//View RX Compound
	albaViewVirtualRX *vvrx = new albaViewVirtualRX("Virtual RX");
	vvrx->PackageView();
	Plug(vvrx);

	//View Isosurface
	albaViewVTK *viso = new albaViewVTK("Isosurface");
	viso->PlugVisualPipe("albaVMEVolumeGray", "albaPipeIsosurface", MUTEX);
	Plug(viso);

	//View Isosurface Compound
	albaViewIsosurfaceCompound *visoc = new albaViewIsosurfaceCompound("Isosurface Compound");
	visoc->PackageView();
	Plug(visoc);

	//View RXCT
	albaViewRXCT *vrxct = new albaViewRXCT("RX-CT");
	vrxct->PackageView();
	Plug(vrxct);

	//View Global Slice Compound
	albaViewGlobalSliceCompound *vglobal = new albaViewGlobalSliceCompound("Global Slice");
	vglobal->PackageView();
	Plug(vglobal);

	//View Image Compound
	albaViewImageCompound *vimageC = new albaViewImageCompound("Image");
	vimageC->PackageView();
	Plug(vimageC);

	//View 3D
	Plug(new albaView3D("3D"));

	//View Arbitrary Orthoslice
	albaViewArbitraryOrthoSlice *varbitraryortho = new albaViewArbitraryOrthoSlice("Arbitrary Orthoslice", albaAxes::BODY);
	varbitraryortho->PackageView();
	Plug(varbitraryortho);

	//View Slice On Curve Compound
	albaViewSliceOnCurveCompound *vsliceoncurveC = new albaViewSliceOnCurveCompound("Slice On Curve");
	vsliceoncurveC->PackageView();
	Plug(vsliceoncurveC);

	//View Image
	albaViewImage *vimage = new albaViewImage("Image");
	//vimage->PackageView();
	Plug(vimage);

	// 	//View Compound Windowing 
	// 	albaViewCompoundWindowing *vCompWind = new albaViewCompoundWindowing("Compound Windowing (albaViewCompoundWindowing)");
	// 	vCompWind->PackageView();
	// 	Plug(vCompWind);

	//View CT 
	albaViewCT *vCT = new albaViewCT("CT");
	vCT->PackageView();
	Plug(vCT);

	//View Global Slice 
	albaViewGlobalSlice *vGlobalS = new albaViewGlobalSlice("Global Slice");
	//vGlobalS->PackageView();
	Plug(vGlobalS);

	//View RX 
	albaViewRX *vRX = new albaViewRX("RX");
	//vRX->PackageView();
	Plug(vRX);

	//View Slice
	albaViewSlice *vslice = new albaViewSlice("Slice");
	//vslice->PackageView();
	Plug(vslice);

	//View Slice Blend
	albaViewSliceBlend *vSliceBlend = new albaViewSliceBlend("Slice Blend");
	//vSliceBlend->PackageView();
	Plug(vSliceBlend);

	//View Slice Blend RX
	albaViewSliceBlendRX *vSliceBlendRX = new albaViewSliceBlendRX("Slice Blend RX");
	vSliceBlendRX->PackageView();
	Plug(vSliceBlendRX);

	//View Slice Global
	albaViewSliceGlobal *vSliceGlobal = new albaViewSliceGlobal("Slice Slice Global");
	//vSliceGlobal->PackageView();
	Plug(vSliceGlobal);

	//View Slice On Curve
	albaViewSliceOnCurve *vsliceoncurve = new albaViewSliceOnCurve("Slice On Curve");
	vsliceoncurve->PackageView();
	Plug(vsliceoncurve);

	//View Slicer
	albaViewSlicer *vslicer = new albaViewSlicer("Slicer");
	vslicer->PackageView();
	Plug(vslicer);

	//View RayCast
	albaViewVTK *vRayCast = new albaViewVTK("3D Volume Rendering");
	vRayCast->PlugVisualPipe("albaVMEVolumeGray", "albaPipeRayCast", MUTEX);
	Plug(vRayCast);

	//View HTML
	albaViewHTML *vHtml = new albaViewHTML("HTML");
	//vHtml->PackageView();
	Plug(vHtml);

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
void albaLogicWithManagers::ConfigureWizardManager()
{
	//Setting wizard specific data
	if(m_UseWizardManager)
	{
		m_WizardManager = new albaWizardManager();
		m_WizardManager->SetListener(this);
		m_WizardManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
	}
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::WizardRunStarting()
{
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
#include "pic/menu/WINDOW_CASCADE.xpm"
#include "pic/menu/WINDOW_HORIZONTALLY.xpm"
#include "pic/menu/WINDOW_PREV.xpm"
#include "pic/menu/WINDOW_NEXT.xpm"
#include "pic/menu/WINDOW_VERTICALLY.xpm"

	m_MenuBar = new wxMenuBar;
	wxMenu *file_menu = new wxMenu;
	albaGUI::AddMenuItem(file_menu, MENU_FILE_NEW, _("&New  \tCtrl+N"), FILE_NEW_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_OPEN, _("&Open   \tCtrl+O"), FILE_OPEN_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_SAVE, _("&Save  \tCtrl+S"), FILE_SAVE_xpm);
	file_menu->Append(MENU_FILE_SAVEAS, _("Save &As  \tCtrl+Shift+S"));

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
	m_MenuBar->Append(m_ViewMenu, _("&View"));

	m_OpMenu = new wxMenu;
	m_MenuBar->Append(m_OpMenu, _("&Operations"));

	wxMenu    *windowMenu = new wxMenu;
	m_MenuBar->Append(windowMenu, _("Window"));
	albaGUI::AddMenuItem(windowMenu, TILE_WINDOW_CASCADE, _("&Cascade"), WINDOW_CASCADE_xpm);
	albaGUI::AddMenuItem(windowMenu, TILE_WINDOW_HORIZONTALLY, _("Tile &Horizontally"), WINDOW_HORIZONTALLY_xpm);
	albaGUI::AddMenuItem(windowMenu, TILE_WINDOW_VERTICALLY, _("Tile &Vertically"), WINDOW_VERTICALLY_xpm);

	windowMenu->AppendSeparator();
	albaGUI::AddMenuItem(windowMenu, IDM_WINDOWNEXT, _("&Next"), WINDOW_NEXT_xpm);
	albaGUI::AddMenuItem(windowMenu, IDM_WINDOWPREV, _("&Previous"), WINDOW_PREV_xpm);

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
void albaLogicWithManagers::TreeContextualMenu(albaEvent &e)
{
	albaGUITreeContextualMenu *contextMenu = new albaGUITreeContextualMenu();
	contextMenu->SetListener(this);
	contextMenu->SetOpManager(m_OpManager);

	albaView *v = m_ViewManager->GetSelectedView();
	albaVME  *vme = e.GetVme();
	bool vme_menu = e.GetBool();
	bool autosort = e.GetArg() != 0;

	contextMenu->CreateContextualMenu((albaGUICheckTree *)e.GetSender(), v, vme, vme_menu);
	contextMenu->ShowContextualMenu();

	cppDEL(contextMenu);
}

// Splash Screen
//----------------------------------------------------------------------------
void albaLogicWithManagers::ShowSplashScreen()
{
	wxBitmap splashImage = albaPictureFactory::GetPictureFactory()->GetBmp("SPLASH_SCREEN");
	ShowSplashScreen(splashImage);
}
//----------------------------------------------------------------------------
void albaLogicWithManagers::ShowSplashScreen(wxBitmap &splashImage, wxString message, int x, int y, wxColour color)
{
	m_SplashScreen = new albaGUISplashScreen(splashImage, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 2000, NULL);

	if (message.IsEmpty()) message = m_AppBuildNum;

	m_SplashScreen->SetText(message, x, y, color);
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
	command = command + "\"" + url + "/\"";
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
	wxString path, name, ext;
	wxFileName::SplitPath(filename.GetCStr(), &path, &name, &ext);
	ext.MakeLower();
	if (ext == "vtk")
	{
		albaOpImporterVTK *vtkImporter = new albaOpImporterVTK("importer");
		vtkImporter->SetInput(m_VMEManager->GetRoot());
		vtkImporter->SetListener(m_OpManager);
		vtkImporter->SetFileName(filename.GetCStr());
		vtkImporter->ImportVTK();
		vtkImporter->OpDo();
		cppDEL(vtkImporter);
	}
	else if (ext == "stl")
	{
		albaOpImporterSTL *stlImporter = new albaOpImporterSTL("importer");
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
void albaLogicWithManagers::CreateStorage(albaEvent *e)
{
	albaVMEStorage *oldStorage, *newStorage;
	oldStorage = (albaVMEStorage *)e->GetMafObject();
	if (oldStorage)
	{
		m_VMEManager->NotifyRemove(oldStorage->GetRoot());
		m_OpManager->VmeSelected(NULL);		
	}
	newStorage = albaVMEStorage::New();
	newStorage->SetListener(m_VMEManager);
	newStorage->GetRoot()->Initialize();
	e->SetMafObject(newStorage);

	albaDEL(oldStorage);
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
			OnFileSaveAs();

			if (m_OpManager->Running())
				m_OpManager->StopCurrentOperation();
		}
	}
	OnQuit();
}

//----------------------------------------------------------------------------
int albaLogicWithManagers::AppEventFilter(wxEvent& event)
{
	if (m_EventFilterFunc)
		return (*m_EventFilterFunc)(event);
		
	else return -1;
}
