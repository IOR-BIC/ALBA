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

	// VTK View (Surface)
	m_Logic->Plug(new albaViewVTK("Surface"));	

	// View Orthoslice
	albaViewOrthoSlice *vortho = new albaViewOrthoSlice("Orthoslice");
	vortho->PackageView();
	m_Logic->Plug(vortho);

	//////////////////////////////////////////////////////////////////////////
	//Operations
	m_Logic->Plug(new albaOpTransform("Transform \tCtrl+t"));

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

	return TRUE;
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
