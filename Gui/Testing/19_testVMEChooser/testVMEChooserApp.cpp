/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testVMEChooserApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-18 15:27:21 $
  Version:   $Revision: 1.1 $
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


#include "testVMEChooserApp.h"
#include "mafVMEFactory.h"
#include "mafPics.h"
#include "mmgMDIFrame.h"

#include "mafNodeGeneric.h"
#include "mafNodeRoot.h"

#include "mmoCreateGenericVme.h"
#include "mmoTestVMEChooser.h"
//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testVMEChooserApp)

//--------------------------------------------------------------------------------
bool testVMEChooserApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafPics.Initialize();	

  int result = mafVMEFactory::Initialize();
  assert(result==MAF_OK);

  m_logic = new mafLogicWithManagers();
  m_logic->Configure();

  m_logic->GetTopWin()->SetTitle("testOpMan");
  m_logic->Plug(new mmoCreateGenericVme("Add VME Generic \tCtrl+A"));
  m_logic->Plug(new mmoTestVMEChooser("Test VME Chooser\tCtrl+B"));

  SetTopWindow(mafGetFrame());  
  m_logic->Show();
  m_logic->Init(0,NULL);
  return TRUE;
}
//--------------------------------------------------------------------------------
int testVMEChooserApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}