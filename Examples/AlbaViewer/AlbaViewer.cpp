/*=========================================================================
Program:   AlbaViewer
Module:    AlbaViewer.cpp
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

//Enable This for memory leak detection
//#include <vld.h>

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "AlbaViewer.h"
#include "appDecl.h"
#include "appUtils.h"

#include "albaPipeFactoryVME.h"
#include "albaPipeVolumeSliceBlend.h"
#include "albaServiceLocator.h"
#include "albaVMEFactory.h" 
#include "albaViewOrthoSlice.h"
#include "albaViewVTK.h"

#include <vtkTimerLog.h>

//Comment this line to disable wizard sample
//#define USE_WIZARD

#ifdef USE_WIZARD
#include "albaWizard.h"
#include "appWizardSample.h"
#endif
#include "albaOpTransform.h"
#include "albaOpImporterImage.h"
#include "albaOpImporterRAWVolume.h"
#include "albaOpImporterRAWImages.h"
#include "albaOpImporterSTL.h"
#include "albaOpImporterPLY.h"
#include "albaOpImporterVRML.h"
#include "albaOpImporterVTK.h"
#include "albaOpImporterVTKXML.h"
#include "albaOpImporterMetaImage.h"
#include "albaOpImporterMSF.h"
#include "albaOpImporterMSF1x.h"
#include "albaOpImporterAnsysCDBFile.h"
#include "albaOpImporterAnsysInputFile.h"
#include "albaOpImporterAbaqusFile.h"
#include "albaOpImporterMesh.h"
#include "albaOpImporterASCII.h"
#include "albaGUIDicomSettings.h"
#include "albaOpImporterDicom.h"
#include "albaOpImporterLandmark.h"
#include "albaOpImporterLandmarkTXT.h"
#include "albaOpImporterLandmarkWS.h"
#include "albaOpImporterC3D.h"
#include "albaOpImporterAnalogWS.h"
#include "albaOpImporterGRFWS.h"
#include "albaOpImporterDicFile.h"
#include "albaOpImporterPointCloud.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------

IMPLEMENT_APP(AlbaViewer)

//--------------------------------------------------------------------------------
bool AlbaViewer::OnInit()
{
	InitializeIcons();
	
	int result;
  result = albaVMEFactory::Initialize();
	assert(result == ALBA_OK);

	result = albaPipeFactoryVME::Initialize();
	assert(result == ALBA_OK);

	// Create Logic Manager
	m_Logic = new appLogic();
	albaServiceLocator::SetLogicManager(m_Logic);
	m_Logic->GetTopWin()->SetTitle("AlbaViewer");

#ifdef USE_WIZARD
	m_Logic->PlugWizardManager(true);
#endif

	// Plug and Enable Snapshot Manager
	m_Logic->PlugSnapshotManager(true);

	m_Logic->Configure();

	SetTopWindow(albaGetFrame());

	//////////////////////////////////////////////////////////////////////////
	//Views

	m_Logic->PlugStandardViews();


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


	//////////////////////////////////////////////////////////////////////////

	// Splash Screen
	albaString splashImageName = "AlbaViewerSplash.bmp";

	wxString splashDir = appUtils::GetConfigDirectory().ToAscii();
	wxBitmap splashBitmap;
	splashBitmap.LoadFile(splashDir + "\\" + splashImageName, wxBITMAP_TYPE_BMP);
	// m_Logic->ShowSplashScreen(splashBitmap); // Uncomment to show SplashScreen

	//////////////////////////////////////////////////////////////////////////
	wxHandleFatalExceptions();

	m_Logic->Show();
	m_Logic->Init(argc, argv); // Calls FileNew - which create the root

	return true;
}

//--------------------------------------------------------------------------------
int AlbaViewer::OnExit()
{
  cppDEL(m_Logic);

  // This hack is fixing VTK internal memory leak
  vtkTimerLog::CleanupLog();
  return 0;
}

//--------------------------------------------------------------------------------
void AlbaViewer::InitializeIcons()
{
	albaPictureFactory::GetPictureFactory()->Initialize();

#include "pic/FRAME_ICON16x16.xpm"
	albaADDPIC(FRAME_ICON16x16);
#include "pic/FRAME_ICON32x32.xpm"
	albaADDPIC(FRAME_ICON32x32);
#include "pic/FRAME_ICON64x64.xpm"
	albaADDPIC(FRAME_ICON64x64);
#include "pic/FRAME_ICON128x128.xpm"
	albaADDPIC(FRAME_ICON128x128);
#include "pic/FRAME_ICON256x256.xpm"
	albaADDPIC(FRAME_ICON256x256);
#include "pic/MDICHILD_ICON.xpm"
	albaADDPIC(MDICHILD_ICON);

#include "pic/CAMERA.xpm"
	albaADDPIC(CAMERA);
#include "pic/VIEW_SURFACE_ICON.xpm"
	albaADDPIC(VIEW_SURFACE_ICON);
#include "pic/VIEW_ORTHO.xpm"
	albaADDPIC(VIEW_ORTHO);
}

//----------------------------------------------------------------------------
void AlbaViewer::OnFatalException()
{
	m_Logic->HandleException();
}

//--------------------------------------------------------------------------------
int AlbaViewer::FilterEvent(wxEvent& event) 
{
	int keyCode = ((wxKeyEvent&)event).GetKeyCode();
	bool controlDown = ((wxKeyEvent&)event).ControlDown();

	// Press Button
	if (event.GetEventType() == wxEVT_KEY_DOWN)
	{
// 		if (keyCode == 'A' && controlDown) //Check CTRL+A Key  
// 		{
// 			//
// 			return true;
// 		}
// 		else if (keyCode == 'O' && !m_Logic->IsOperationRunning()) //Check O Key  
// 		{
// 			//
// 			return true;
// 		}
 	}

	// Release Button
	if (event.GetEventType() == wxEVT_KEY_UP && (keyCode == 'O') && !m_Logic->IsOperationRunning())
	{
		//	
	}

	return -1;
}
