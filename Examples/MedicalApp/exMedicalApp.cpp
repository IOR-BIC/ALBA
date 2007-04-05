/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: exMedicalApp.cpp,v $
  Language:  C++
  Date:      $Date: 2007-04-05 10:03:53 $
  Version:   $Revision: 1.3 $
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


#include "exMedicalApp.h"
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

#include "mmoCreateGroup.h"
#include "mmoCreateMeter.h"
#include "mmoCreateSlicer.h"
#include "mmoMAFTransform.h"
#include "mmoReparentTo.h"
#include "mmoSTLExporter.h"
#include "mmoSTLImporter.h"
#include "mmoVTKExporter.h"
#include "mmoVTKImporter.h"
#include "mmoMSF1xImporter.h"
#include "mmoInteractionOp.h"

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
IMPLEMENT_APP(exMedicalApp)

//--------------------------------------------------------------------------------
bool exMedicalApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafPics.Initialize();	

  int result = mafVMEFactory::Initialize();
  assert(result==MAF_OK);
  
  // Initialize and Fill of PipeFactory -- could be a SideEffect of the node plug
  result = mafPipeFactoryVME::Initialize();
  assert(result==MAF_OK);

  // Initialize Interaction factory
  result = mafPipeFactoryVME::Initialize();
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

  m_logic->GetTopWin()->SetTitle("Medical example");
  SetTopWindow(mafGetFrame());  

  //------------------------------------------------------------
  // Importer Menu':
  m_logic->Plug(new mmoSTLImporter("STL Importer"));
  m_logic->Plug(new mmoVTKImporter("VTK Importer"));
  //m_logic->Plug(new mmoMSF1xImporter("MSF 1.x Importer"));
  //------------------------------------------------------------

  //------------------------------------------------------------
  // Exporter Menu':
  m_logic->Plug(new mmoSTLExporter("STL Exporter"));
  m_logic->Plug(new mmoVTKExporter("VTK Exporter"));
  //------------------------------------------------------------

  //------------------------------------------------------------
  // Operation Menu':
  m_logic->Plug(new mmoCreateGroup("Create Group"));
  m_logic->Plug(new mmoCreateMeter("Create Meter"));
  m_logic->Plug(new mmoCreateSlicer("Create Slicer"));
  m_logic->Plug(new mmoMAFTransform("MAF Transform"));
  m_logic->Plug(new mmoReparentTo("Reparent to...  \tCtrl+R"));
  m_logic->Plug(new mmoInteractionOp("InteractionOp  \tCtrl+R"));  
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
int exMedicalApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}

