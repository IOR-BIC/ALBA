/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: exOperationApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-10-11 17:49:08 $
  Version:   $Revision: 1.26 $
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

#include "mmoCreateGroup.h"
#include "mmoCreateMeter.h"
#include "mmoCreateProber.h"
#include "mmoCreateSlicer.h"
#include "mmoExplodeCollapse.h"
#include "mmoExtractIsosurface.h"
#include "mmoFilterSurface.h"
#include "mmoImageImporter.h"
#include "mmoMAFTransform.h"
#include "mmoMSFExporter.h"
#include "mmoMSF1xImporter.h"
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

  m_logic->GetTopWin()->SetTitle("Operations example");
  SetTopWindow(mafGetFrame());  

  //------------------------------------------------------------
  // Importer Menu':
  m_logic->Plug(new mmoImageImporter("Images"));
  m_logic->Plug(new mmoSTLImporter("STL"));
  m_logic->Plug(new mmoVRMLImporter("VRML"));
  m_logic->Plug(new mmoVTKImporter("VTK"));
  m_logic->Plug(new mmoMSF1xImporter("MSF 1.x"));
  //------------------------------------------------------------

  //------------------------------------------------------------
  // Exporter Menu':
  m_logic->Plug(new mmoMSFExporter("MSF"));
  m_logic->Plug(new mmoSTLExporter("STL"));
  m_logic->Plug(new mmoVTKExporter("VTK"));
  //------------------------------------------------------------

  //------------------------------------------------------------
  // Operation Menu':
  m_logic->Plug(new mmoCreateGroup("Create Group"));
  m_logic->Plug(new mmoCreateMeter("Create Meter"));
  m_logic->Plug(new mmoCreateProber("Create Prober"));
  m_logic->Plug(new mmoCreateSlicer("Create Slicer"));
  m_logic->Plug(new mmoExplodeCollapse("Explode/Collapse cloud"));
  m_logic->Plug(new mmoExtractIsosurface("Extract Isosurface"));
  m_logic->Plug(new mmoFilterSurface("Filter Surface"));
  m_logic->Plug(new mmoMAFTransform("MAF Transform"));
  m_logic->Plug(new mmoReparentTo("Reparent to...  \tCtrl+R"));
  //------------------------------------------------------------
  
  //------------------------------------------------------------
  // View Menu':
  m_logic->Plug(new mafViewVTK("VTK view"));
  mafViewVTK *v = new mafViewVTK("Slice view", CAMERA_CT);
  v->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
  m_logic->Plug(v);
  mafViewVTK *viso = new mafViewVTK("Isosurface view");
  viso->PlugVisualPipe("mafVMEVolumeGray", "mafPipeIsosurface");
  m_logic->Plug(viso);

  mafViewCompound *vc = new mafViewCompound("view compound",3);
  mafViewVTK *v2 = new mafViewVTK("Slice view", CAMERA_CT);
  v2->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice");
  vc->PlugChildView(v2);
  m_logic->Plug(vc);
  //------------------------------------------------------------

  wxHandleFatalExceptions();

  m_logic->Show();
  mafString app_stamp;
  app_stamp = "OPEN_ALL_DATA";
  m_logic->SetApplicationStamp(app_stamp);
  m_logic->Init(0,NULL); // calls FileNew - which create the root
  return TRUE;
}
//--------------------------------------------------------------------------------
int exOperationApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}
//--------------------------------------------------------------------------------
void exOperationApp::OnFatalException()
//--------------------------------------------------------------------------------
{
  m_logic->HandleException();
}
