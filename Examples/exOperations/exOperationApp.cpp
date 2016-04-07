/*=========================================================================

 Program: MAF2
 Module: exOperationApp
 Authors: Paolo Quadrani
 
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

#include "exOperationApp.h"
#include "mafDecl.h"

#include "mafVMEFactory.h"
#include "mafPics.h"
#include "mafGUIMDIFrame.h"

#include "mafNodeGeneric.h"
#include "mafNodeRoot.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafPipeFactoryVME.h"
#include "mafInteractionFactory.h"

#include "mafOp2DMeasure.h"
#include "mafOpAddLandmark.h"
#include "mafOpClipSurface.h"
#include "mafOpCreateGroup.h"
#include "mafOpCreateMeter.h"
#include "mafOpCreateProber.h"
#include "mafOpCreateRefSys.h"
#include "mafOpCreateSlicer.h"
#include "mafOpCreateVolume.h"
#include "mafOpCrop.h"
#include "mafOpEditMetadata.h"
#include "mafOpExtractIsosurface.h"
#include "mafOpFilterSurface.h"
#include "mafOpFilterVolume.h"
#include "mafOpImporterImage.h"
#include "mafOpMAFTransform.h"
#include "mafOpExporterMSF.h"
#include "mafOpImporterMSF.h"
#include "mafOpImporterMSF1x.h"
#include "mafOpImporterRAWVolume.h"
#include "mafOpReparentTo.h"
#include "mafOpExporterSTL.h"
#include "mafOpImporterSTL.h"
#include "mafOpVolumeResample.h"
#include "mafOpImporterVRML.h"
#include "mafOpExporterVTK.h"
#include "mafOpImporterVTK.h"
#include "mafOpValidateTree.h"
#include "mafOpRemoveCells.h"
#include "mafOpEditNormals.h"
#include "mafOpVOIDensityEditor.h"
#include "mafOpExporterRaw.h"

#ifdef MAF_USE_ITK
  #include "mafOpImporterASCII.h"
#endif

#include "mafViewVTK.h"
#include "mafViewCompound.h"
#include "mafOpCreateSurfaceParametric.h"

//#include "mafUser.h"
//#include "mafCrypt.h"
//#include "mmoSRBUpload.h"
//#include "mafViewPlot.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(exOperationApp)

//--------------------------------------------------------------------------------
bool exOperationApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafPictureFactory::GetPictureFactory()->Initialize();	

#include "Examples/exOperations/MAFIcons/FRAME_ICON16x16.xpm"
  mafADDPIC(FRAME_ICON16x16);
#include "Examples/exOperations/MAFIcons/FRAME_ICON32x32.xpm"
  mafADDPIC(FRAME_ICON32x32);
#include "Examples/exOperations/MAFIcons/MDICHILD_ICON.xpm"
  mafADDPIC(MDICHILD_ICON);

  int result = mafVMEFactory::Initialize();
  assert(result==MAF_OK);
  
  // Initialize and Fill of PipeFactory -- could be a SideEffect of the node plug
  result = mafPipeFactoryVME::Initialize();
  assert(result==MAF_OK);

  result = mafInteractionFactory::Initialize();
  assert(result==MAF_OK);

  m_Logic = new mafLogicWithManagers();
  m_Logic->GetTopWin()->SetTitle("Operations example");

  //m_Logic->PlugTimebar(false);
  //m_Logic->PlugMenu(false);
  //m_Logic->PlugToolbar(false);
  //m_Logic->PlugLogbar(false);
  //m_Logic->PlugSidebar(true, mafSideBar::SINGLE_NOTEBOOK);
  //m_Logic->PlugOpManager(false);
  //m_Logic->PlugViewManager(false);
  //m_Logic->PlugVMEManager(false);  // the VmeManager at the moment cause 4 leaks of 200+32+24+56 bytes  //SIL. 20-4-2005: 
  m_Logic->Configure();

  SetTopWindow(mafGetFrame());

  //------------------------------------------------------------
  // Importer Menu':
  //------------------------------------------------------------
  m_Logic->Plug(new mafOpImporterImage("Images"));
  m_Logic->Plug(new mafOpImporterRAWVolume("RAW Volume"));
  m_Logic->Plug(new mafOpImporterSTL("STL"));
  m_Logic->Plug(new mafOpImporterVRML("VRML"));
  m_Logic->Plug(new mafOpImporterVTK("VTK"));
  m_Logic->Plug(new mafOpImporterMSF("MSF"));
  m_Logic->Plug(new mafOpImporterMSF1x("MSF 1.x"));
#ifdef MAF_USE_ITK
  m_Logic->Plug(new mafOpImporterASCII("ASCII"));
#endif
  //------------------------------------------------------------

  //------------------------------------------------------------
  // Exporter Menu':
  //------------------------------------------------------------
  m_Logic->Plug(new mafOpExporterMSF("MSF"));
  m_Logic->Plug(new mafOpExporterSTL("STL"));
  m_Logic->Plug(new mafOpExporterVTK("VTK"));
  m_Logic->Plug(new mafOpExporterRAW("RAW"));
  //------------------------------------------------------------

  //------------------------------------------------------------
  // Operation Menu':
  //------------------------------------------------------------
  //m_Logic->Plug(new mmoSRBUpload());
  m_Logic->Plug(new mafOpValidateTree());
  m_Logic->Plug(new mafOpRemoveCells());
  m_Logic->Plug(new mafOpEditNormals());
  m_Logic->Plug(new mafOp2DMeasure("2D Measure"));
  m_Logic->Plug(new mafOpAddLandmark("Add Landmark"));
  m_Logic->Plug(new mafOpClipSurface("Clip Surface"));
  m_Logic->Plug(new mafOpCreateSurfaceParametric("Surface Parametric"),"Create");
  m_Logic->Plug(new mafOpCreateGroup("Group"),"Create");
  m_Logic->Plug(new mafOpCreateMeter("Meter"),"Create");
  m_Logic->Plug(new mafOpCreateRefSys("RefSys"),"Create");
  m_Logic->Plug(new mafOpCreateProber("Prober"),"Create");
  m_Logic->Plug(new mafOpCreateSlicer("Slicer"),"Create");
  m_Logic->Plug(new mafOpCreateVolume("Constant Volume"),"Create");
  m_Logic->Plug(new mafOpEditMetadata("Metadata Editor"));
  m_Logic->Plug(new mafOpExtractIsosurface("Extract Isosurface"));
  m_Logic->Plug(new mafOpFilterSurface("Surface"),"Filter");
  m_Logic->Plug(new mafOpFilterVolume("Volume"),"Filter/Volume");
  m_Logic->Plug(new mafOpVOIDensityEditor(), "Filter");
  m_Logic->Plug(new mafOpMAFTransform("Transform  \tCtrl+T"));
  m_Logic->Plug(new mafOpReparentTo("Reparent to...  \tCtrl+R"));
  m_Logic->Plug(new mafOpVolumeResample("Resample Volume"));
  m_Logic->Plug(new mafOpCrop("Crop Volume"));
  //------------------------------------------------------------
  
  //------------------------------------------------------------
  // View Menu':
  //------------------------------------------------------------
  m_Logic->Plug(new mafViewVTK("VTK view"));
  m_Logic->Plug(new mafViewVTK("VTK stereo view",CAMERA_PERSPECTIVE,true,false,1));
  mafViewVTK *v = new mafViewVTK("Slice view", CAMERA_CT);
  v->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice",MUTEX);
  m_Logic->Plug(v);
  mafViewVTK *rxv = new mafViewVTK("RX view", CAMERA_RX_FRONT);
  rxv->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeProjected",MUTEX);
  m_Logic->Plug(rxv);
  mafViewVTK *viso = new mafViewVTK("Isosurface view");
  viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface",MUTEX);
  m_Logic->Plug(viso);
  mafViewVTK *visoGPU = new mafViewVTK("Isosurface View (GPU)");
  visoGPU->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurfaceGPU",MUTEX);
  m_Logic->Plug(visoGPU);
  //m_Logic->Plug(new mafViewPlot("Plot view"));

  mafViewCompound *vc = new mafViewCompound("view compound",3);
  mafViewVTK *v2 = new mafViewVTK("Slice view", CAMERA_CT);
  v2->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice", MUTEX);
  vc->PlugChildView(v2);
  m_Logic->Plug(vc);
  //------------------------------------------------------------

  //wxHandleFatalExceptions();

  //m_Logic->ShowSplashScreen();
  m_Logic->Show();
 

  m_Logic->Init(argc,argv); // calls FileNew - which create the root
  return TRUE;
}
//--------------------------------------------------------------------------------
int exOperationApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_Logic);
  return 0;
}
//--------------------------------------------------------------------------------
void exOperationApp::OnFatalException()
//--------------------------------------------------------------------------------
{
  m_Logic->HandleException();
}
