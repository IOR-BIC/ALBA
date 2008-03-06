/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: exInteractionApp.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 12:02:19 $
  Version:   $Revision: 1.5 $
  Authors:   Paolo Quadrani
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


#include "exInteractionApp.h"
#include "mafDecl.h"
#include "mafVMEFactory.h"
#include "mafPics.h"
#include "mmgMDIFrame.h"

#include "mafNodeFactory.h"
#include "mafNodeGeneric.h"
#include "mafNodeRoot.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafPipeFactoryVME.h"

#include "mafOpCreateGroup.h"
#include "mafOpCreateMeter.h"
#include "mafOpCreateSlicer.h"
#include "mafOpMAFTransform.h"
#include "mafOpReparentTo.h"
#include "mafOpExporterSTL.h"
#include "mafOpImporterSTL.h"
#include "mafOpExporterVTK.h"
#include "mafOpImporterVTK.h"
#include "mafOpImporterMSF1x.h"
#include "mafOpInteractionOp.h"

#include "mafViewVTK.h"
#include "mafViewCompound.h"

#include "mafInteractionFactory.h"

#include "mafAvatar3DCone.h"
//#include "mafAvatar3D2DPicker.h"

//#include "mmdIntersense.h"
#ifdef MAF_USE_P5_GLOVE
  #include "mmdP5Glove.h"
#endif

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(exInteractionApp)

//--------------------------------------------------------------------------------
bool exInteractionApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafPics.Initialize();	

  int result = mafVMEFactory::Initialize();
  assert(result==MAF_OK);
  
  // Initialize and Fill of PipeFactory -- could be a SideEffect of the node plug
  result = mafPipeFactoryVME::Initialize();
  assert(result==MAF_OK);

  // Initialize Interaction factory
  result = mafInteractionFactory::Initialize();
  assert(result==MAF_OK);



  //---------- devices --------
//  mafPlugDevice<mmdVirtualTracker>("Virtual Tracker");
//  mafPlugDevice<mmdVicon>("Vicon");
//  mafPlugObject<mmdTracker>("TrackerTool"); // used to allow Intersense to create its tools
//  mafPlugObject<mmdViconTracker>("ViconTrackerTool"); // used to allow Intersense to create its tools  
//  mafPlugDevice<mmdSUHapticDevice>("SU Haptic");

  //mafPlugDevice<mmdIntersense>("Intersense");
#ifdef MAF_USE_P5_GLOVE
  mafPlugDevice<mmdP5Glove>("P5 Glove");
#endif
  
  //---------- avatars --------
  mafPlugAvatar<mafAvatar3DCone>("Cone");
//  mafPlugAvatar<mafAvatar3D2DPicker>("2DCone");

  m_logic = new mafLogicWithManagers();
  //m_logic->PlugTimebar(false);
  //m_logic->PlugMenu(false);
  //m_logic->PlugToolbar(false);
  //m_logic->PlugLogbar(false);
  //m_logic->PlugSidebar(false);
  //m_logic->PlugOpManager(false);
  //m_logic->PlugViewManager(false);
  //m_logic->PlugVMEManager(false);  // the VmeManager at the moment cause 4 leaks of 200+32+24+56 bytes  //SIL. 20-4-2005: 
  m_logic->Configure();

  m_logic->GetTopWin()->SetTitle("Interaction example");
  SetTopWindow(mafGetFrame());  

  //------------------------------------------------------------
  // Importer Menu':
  m_logic->Plug(new mafOpImporterSTL("STL Importer"));
  m_logic->Plug(new mafOpImporterVTK("VTK Importer"));
  //m_logic->Plug(new mafOpImporterMSF1x("MSF 1.x Importer"));
  //------------------------------------------------------------

  //------------------------------------------------------------
  // Exporter Menu':
  m_logic->Plug(new mafOpExporterSTL("STL Exporter"));
  m_logic->Plug(new mafOpExporterVTK("VTK Exporter"));
  //------------------------------------------------------------

  //------------------------------------------------------------
  // Operation Menu':
  m_logic->Plug(new mafOpCreateGroup("Create Group"));
  m_logic->Plug(new mafOpCreateMeter("Create Meter"));
  m_logic->Plug(new mafOpCreateSlicer("Create Slicer"));
  m_logic->Plug(new mafOpMAFTransform("MAF Transform"));
  m_logic->Plug(new mafOpReparentTo("Reparent to...  \tCtrl+R"));
  m_logic->Plug(new mafOpInteractionOp("InteractionOp  \tCtrl+R"));  
  //------------------------------------------------------------
  
  //------------------------------------------------------------
  // View Menu':
  mafViewVTK *v = new mafViewVTK("Slice view", CAMERA_CT);
  v->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
  m_logic->Plug(v);
  m_logic->Plug(new mafViewVTK("VTK view"));

  mafViewCompound *vc = new mafViewCompound("view compound",3);
  mafViewVTK *v2 = new mafViewVTK("Slice view", CAMERA_CT);
  v2->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
  vc->PlugChildView(v2);
  m_logic->Plug(vc);
  //------------------------------------------------------------

  m_logic->Show();
  m_logic->Init(0,NULL); // calls FileNew - which create the root
  return TRUE;
}
//--------------------------------------------------------------------------------
int exInteractionApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}

