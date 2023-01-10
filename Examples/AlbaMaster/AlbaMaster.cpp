/*=========================================================================
Program:   AlbaMaster
Module:    AlbaMaster.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

//Enable This for memory leak detection
//#include <vld.h>

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in ALBA must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "AlbaMaster.h"

#include "albaDecl.h"
#include "albaGUIMDIFrame.h"
#include "albaPics.h"
#include "albaPipeFactoryVME.h"
#include "albaServiceLocator.h"
#include "albaVMEFactory.h"
#include "albaVMERoot.h"
#include "albaVMESurface.h"



//WIZARD
// Comment this line to disable wizard sample
//#define USE_WIZARD

#ifdef USE_WIZARD
#include "albaWizard.h"
#include "exWizardSample.h"
#endif

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(AlbaMaster)

//--------------------------------------------------------------------------------
bool AlbaMaster::OnInit()
{
  albaPictureFactory::GetPictureFactory()->Initialize();	

#include "Examples/AlbaMaster/FRAME_ICON16x16.xpm"
	albaADDPIC(FRAME_ICON16x16);
#include "Examples/AlbaMaster/FRAME_ICON32x32.xpm"
	albaADDPIC(FRAME_ICON32x32);
#include "Examples/AlbaMaster/FRAME_ICON64x64.xpm"
	albaADDPIC(FRAME_ICON64x64);
#include "Examples/AlbaMaster/FRAME_ICON128x128.xpm"
	albaADDPIC(FRAME_ICON128x128);
#include "Examples/AlbaMaster/FRAME_ICON256x256.xpm"
	albaADDPIC(FRAME_ICON256x256);
#include "Examples/AlbaMaster/MDICHILD_ICON.xpm"
	albaADDPIC(MDICHILD_ICON);

#include "pic/GIZMO_TRANSLATE_ICON.xpm"
	albaADDPIC(GIZMO_TRANSLATE_ICON);
#include "pic/GIZMO_ROTATE_ICON.xpm"
	albaADDPIC(GIZMO_ROTATE_ICON);

	int result;
	result = albaVMEFactory::Initialize();
	assert(result==ALBA_OK);

	// Initialize and Fill of PipeFactory -- could be a SideEffect of the node plug
	result = albaPipeFactoryVME::Initialize();
	assert(result==ALBA_OK);

	m_LogicInitialized = false;
	m_Logic = new albaLogicWithManagers();
	albaServiceLocator::SetLogicManager(m_Logic);
  
	m_Logic->GetTopWin()->SetTitle("AlbaMaster");

	//m_Logic->PlugTimebar(false);
	//m_Logic->PlugMenu(false);
	//m_Logic->PlugToolbar(false);
	//m_Logic->PlugLogbar(false);
	//m_Logic->PlugSidebar(true, albaSideBar::SINGLE_NOTEBOOK);
	//m_Logic->PlugOpManager(false);
	//m_Logic->PlugViewManager(false);
	//m_Logic->PlugVMEManager(false);  // the VmeManager at the moment cause 4 leaks of 200+32+24+56 bytes  //SIL. 20-4-2005:

#ifdef USE_WIZARD
  m_Logic->PlugWizardManager(true);
#endif
	m_Logic->PlugSnapshotManager(true);
	m_Logic->Configure();

	SetTopWindow(albaGetFrame());  

	m_Logic->PlugStandardOperations();
	
#ifdef USE_WIZARD
  //------------------------------------------------------------
  // Wizard Menu:
  //------------------------------------------------------------

  //A simple wizard sample
  m_Logic->Plug(new exWizardSample("Import loop"),"");
#endif

	m_Logic->PlugStandardOperations();

	//////////////////////////////////////////////////////////////////////////
	// Splash Screen
	albaString splashImageName = "AlbaMasterSplash.bmp";

	wxString splashDir = albaGetApplicationDirectory().c_str();
	wxBitmap splashBitmap;
	splashBitmap.LoadFile(splashDir + "\\Config\\" + splashImageName, wxBITMAP_TYPE_BMP);
	m_Logic->ShowSplashScreen(splashBitmap);

	//////////////////////////////////////////////////////////////////////////
	wxHandleFatalExceptions();

	//m_Logic->ShowSplashScreen();
	m_Logic->Show();
	m_Logic->Init(argc, argv); // calls FileNew - which create the root

	m_LogicInitialized = true;

	return TRUE;
}
//--------------------------------------------------------------------------------
int AlbaMaster::OnExit()
{
	m_LogicInitialized = false;

	cppDEL(m_Logic);
	return 0;
}

//--------------------------------------------------------------------------------
void AlbaMaster::OnFatalException()
{
	m_Logic->HandleException();
}

//--------------------------------------------------------------------------------
int AlbaMaster::FilterEvent(wxEvent& event)
{
	if (m_LogicInitialized && m_Logic)
		return m_Logic->AppEventFilter(event);

	return -1;
}
