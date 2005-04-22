/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testSideBarApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-22 13:18:03 $
  Version:   $Revision: 1.3 $
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


#include "testSideBarApp.h"
#include "mafPics.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testSideBarApp)

//--------------------------------------------------------------------------------
bool testSideBarApp::OnInit()
//--------------------------------------------------------------------------------
{
  mafPics.Initialize();	

	m_logic = new testSideBarLogic();
  SetTopWindow(m_logic->GetTopWin());  
  m_logic->Show();
  return TRUE;
}
//--------------------------------------------------------------------------------
int testSideBarApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}




