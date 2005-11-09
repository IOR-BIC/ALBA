/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: exOperationApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-09 11:26:54 $
  Version:   $Revision: 1.34 $
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


#include "exOperationApp.h"
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

#include "mmo2DMeasure.h"
#include "mmoAddLandmark.h"
#include "mmoClipSurface.h"
#include "mmoCreateGroup.h"
#include "mmoCreateMeter.h"
#include "mmoCreateProber.h"
#include "mmoCreateRefSys.h"
#include "mmoCreateSlicer.h"
#include "mmoExplodeCollapse.h"
#include "mmoExtractIsosurface.h"
#include "mmoFilterSurface.h"
#include "mmoImageImporter.h"
#include "mmoMAFTransform.h"
#include "mmoMSFExporter.h"
#include "mmoMSF1xImporter.h"
#include "mmoRAWImporterVolume.h"
#include "mmoReparentTo.h"
#include "mmoSTLExporter.h"
#include "mmoSTLImporter.h"
#include "mmoVRMLImporter.h"
#include "mmoVTKExporter.h"
#include "mmoVTKImporter.h"

#include "mafViewVTK.h"
#include "mafViewCompound.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(exOperationApp)

//--------------------------------------------------------------------------------
bool exOperationApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafPics.Initialize();	

  int result = mafVMEFactory::Initialize();
  assert(result==MAF_OK);
  
  // Initialize and Fill of PipeFactory -- could be a SideEffect of the node plug
  result = mafPipeFactoryVME::Initialize();
  assert(result==MAF_OK);

  m_Logic = new mafLogicWithManagers();
  //m_Logic->PlugTimebar(false);
  //m_Logic->PlugMenu(false);
  //m_Logic->PlugToolbar(false);
  //m_Logic->PlugLogbar(false);
  //m_Logic->PlugSidebar(false);
  //m_Logic->PlugOpManager(false);
  //m_Logic->PlugViewManager(false);
  //m_Logic->PlugVMEManager(false);  // the VmeManager at the moment cause 4 leaks of 200+32+24+56 bytes  //SIL. 20-4-2005: 
  m_Logic->Configure();

  m_Logic->GetTopWin()->SetTitle("Operations example");
  SetTopWindow(mafGetFrame());  

  //------------------------------------------------------------
  // Importer Menu':
  m_Logic->Plug(new mmoImageImporter("Images"));
  m_Logic->Plug(new mmoRAWImporterVolume("RAW Volume"));
  m_Logic->Plug(new mmoSTLImporter("STL"));
  m_Logic->Plug(new mmoVRMLImporter("VRML"));
  m_Logic->Plug(new mmoVTKImporter("VTK"));
  m_Logic->Plug(new mmoMSF1xImporter("MSF 1.x"));
  //------------------------------------------------------------

  //------------------------------------------------------------
  // Exporter Menu':
  m_Logic->Plug(new mmoMSFExporter("MSF"));
  m_Logic->Plug(new mmoSTLExporter("STL"));
  m_Logic->Plug(new mmoVTKExporter("VTK"));
  //------------------------------------------------------------

  //------------------------------------------------------------
  // Operation Menu':
  m_Logic->Plug(new mmo2DMeasure("2D Measure"));
  m_Logic->Plug(new mmoAddLandmark("Add Landmark"));
  m_Logic->Plug(new mmoClipSurface("Clip Surface"));
  m_Logic->Plug(new mmoCreateGroup("Create Group"));
  m_Logic->Plug(new mmoCreateMeter("Create Meter"));
  m_Logic->Plug(new mmoCreateRefSys("Create RefSys"));
  m_Logic->Plug(new mmoCreateProber("Create Prober"));
  m_Logic->Plug(new mmoCreateSlicer("Create Slicer"));
  m_Logic->Plug(new mmoExplodeCollapse("Explode/Collapse cloud"));
  m_Logic->Plug(new mmoExtractIsosurface("Extract Isosurface"));
  m_Logic->Plug(new mmoFilterSurface("Filter Surface"));
  m_Logic->Plug(new mmoMAFTransform("MAF Transform  \tCtrl+T"));
  m_Logic->Plug(new mmoReparentTo("Reparent to...  \tCtrl+R"));
  //------------------------------------------------------------
  
  //------------------------------------------------------------
  // View Menu':
  m_Logic->Plug(new mafViewVTK("VTK view"));
  mafViewVTK *v = new mafViewVTK("Slice view", CAMERA_CT);
  v->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
  m_Logic->Plug(v);
  mafViewVTK *viso = new mafViewVTK("Isosurface view");
  viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface");
  m_Logic->Plug(viso);

  mafViewCompound *vc = new mafViewCompound("view compound",3);
  mafViewVTK *v2 = new mafViewVTK("Slice view", CAMERA_CT);
  v2->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
  vc->PlugChildView(v2);
  m_Logic->Plug(vc);
  //------------------------------------------------------------

  //wxHandleFatalExceptions();

  //m_Logic->ShowSplashScreen();
  m_Logic->Show();
  mafString app_stamp;
  app_stamp = "OPEN_ALL_DATA";
  m_Logic->SetApplicationStamp(app_stamp);
  m_Logic->Init(0,NULL); // calls FileNew - which create the root
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
