/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testOpManApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-12 15:42:10 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
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


#include "testOpManApp.h"
#include "mafNodeFactory.h"
#include "mafPics.h"
#include "mmgMDIFrame.h"

#include "mafNodeFactory.h"
#include "mafNodeGeneric.h"
#include "mafNodeRoot.h"

#include "mmoCreateGenericVme.h"
#include "mmoTest.h"
//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testOpManApp)

//--------------------------------------------------------------------------------
bool testOpManApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafNodeFactory *node_factory  = mafNodeFactory::GetInstance();
  assert(node_factory!=NULL);
  int result = mafNodeFactory::Initialize();
  assert(result==MAF_OK);
  mafPlugNode<mafNodeRoot>("mafNodeRoot");
  mafPlugNode<mafNodeGeneric>("mafNodeGeneric");

  m_logic = new mafLogicWithManagers();
  m_logic->Configure();

  m_logic->GetTopWin()->SetTitle("testOpMan");
  m_logic->Plug(new mmoCreateGenericVme("Add Vme Generic \tCtrl+A"));
  m_logic->Plug(new mmoTest("Test \tCtrl+B"));

  SetTopWindow(mafGetFrame());  
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








