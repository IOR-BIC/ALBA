/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: exOperationApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-07 14:44:48 $
  Version:   $Revision: 1.6 $
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
#include "mafVMEFactory.h"
#include "mafPics.h"
#include "mmgMDIFrame.h"

#include "mafNodeFactory.h"
#include "mafNodeGeneric.h"
#include "mafNodeRoot.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafPipeFactory.h"

#include "mmoReparentTo.h"
#include "mmoVTKExporter.h"
#include "mmoVTKImporter.h"
#include "mmoCreateMeter.h"
#include "mmoCreateSlicer.h"
#include "mafViewVTK.h"

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
  result = mafPipeFactory::Initialize();
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

  m_logic->Plug(new mmoVTKImporter("VTK Importer"));
  m_logic->Plug(new mmoVTKExporter("VTK Exporter"));
  m_logic->Plug(new mmoCreateMeter("Create Meter"));
  m_logic->Plug(new mmoCreateSlicer("Create Slicer"));
  m_logic->Plug(new mmoReparentTo("Reparent to...  \tCtrl+R"));
  
  m_logic->Plug(new mafViewVTK("VTK view"));

  m_logic->Show();
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
