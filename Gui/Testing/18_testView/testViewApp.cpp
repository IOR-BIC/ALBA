/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testViewApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:20:04 $
  Version:   $Revision: 1.1 $
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


#include "testViewApp.h"
#include "mafNodeFactory.h"
#include "mafPics.h"
#include "mmgMDIFrame.h"

#include "mafNodeFactory.h"
#include "mafNodeGeneric.h"
#include "mafNodeRoot.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"

#include "mmoCreateVmeSurface.h"
#include "mafViewVTK.h"
//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testViewApp)

//--------------------------------------------------------------------------------
bool testViewApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafNodeFactory *node_factory  = mafNodeFactory::GetInstance();
  assert(node_factory!=NULL);
  int result = mafNodeFactory::Initialize();
  assert(result==MAF_OK);
  mafPlugNode<mafNodeRoot>("mafNodeRoot");
  mafPlugNode<mafNodeGeneric>("mafNodeGeneric");
  mafPlugNode<mafVMERoot>("mafVMERoot");
  mafPlugNode<mafVMESurface>("mafVMESurface");

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

  m_logic->GetTopWin()->SetTitle("testView");
  SetTopWindow(mafGetFrame());  

  m_logic->Plug(new mmoCreateVmeSurface("Add Vme Surface \tCtrl+A"));
  m_logic->Plug(new mafViewVTK("mafViewVTK"));

  m_logic->Show();
  m_logic->Init(0,NULL);
  return TRUE;
}
//--------------------------------------------------------------------------------
int testViewApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}










