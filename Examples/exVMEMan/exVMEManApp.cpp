/*=========================================================================

 Program: MAF2
 Module: exVMEManApp
 Authors: Silvano Imboden
 
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


#include "exVMEManApp.h"
#include "mafPics.h"
#include "mafGUIMDIFrame.h"
#include "mafPipeFactoryVME.h"
#include "mafVMEFactory.h"

#include "mafOpCreateGenericVme.h"
#include "mafOpTest.h"
//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(exVMEManApp)

//--------------------------------------------------------------------------------
bool exVMEManApp::OnInit()
//--------------------------------------------------------------------------------
{
  // this must be initialized before of Node/VME factories to allow
  // plugging of Node/VME icons
  mafPictureFactory::GetPictureFactory()->Initialize();  

  int result = mafVMEFactory::Initialize();
  assert(result==MAF_OK);

  result = mafPipeFactoryVME::Initialize();
  assert(result==MAF_OK);
  
  m_logic = new mafLogicWithManagers();
  m_logic->GetTopWin()->SetTitle("testOpMan");
  m_logic->Configure();
  SetTopWindow(mafGetFrame()); 
  
  m_logic->Plug(new mafOpCreateGenericVme("Add Vme Generic \tCtrl+A"));
  m_logic->Plug(new mafOpTest("Test \tCtrl+B"));

  m_logic->Show();
  m_logic->Init(0,NULL);
  return TRUE;
}
//--------------------------------------------------------------------------------
int exVMEManApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}








