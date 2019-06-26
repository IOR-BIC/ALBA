/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testOpManApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "testOpManApp.h"
#include "albaNodeFactory.h"
#include "albaPics.h"
#include "albaGUIMDIFrame.h"

#include "albaNodeFactory.h"
#include "albaNodeGeneric.h"
#include "albaNodeRoot.h"

#include "albaOpCreateGenericVme.h"
#include "albaOpTest.h"
//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testOpManApp)

//--------------------------------------------------------------------------------
bool testOpManApp::OnInit()
//--------------------------------------------------------------------------------
{
  albaPics.Initialize();	

  albaNodeFactory *node_factory  = albaNodeFactory::GetInstance();
  assert(node_factory!=NULL);
  int result = albaNodeFactory::Initialize();
  assert(result==ALBA_OK);
  albaPlugNode<albaNodeRoot>("albaNodeRoot");
  albaPlugNode<albaNodeGeneric>("albaNodeGeneric");

  m_logic = new albaLogicWithManagers();
  m_logic->Configure();

  m_logic->GetTopWin()->SetTitle("testOpMan");
  m_logic->Plug(new albaOpCreateGenericVme("Add Vme Generic \tCtrl+A"));
  m_logic->Plug(new albaOpTest("Test \tCtrl+B"));

  SetTopWindow(albaGetFrame());  
  m_logic->Show();
  m_logic->Init(0,NULL);
  return TRUE;
}
//--------------------------------------------------------------------------------
int testOpManApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}








