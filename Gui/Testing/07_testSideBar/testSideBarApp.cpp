/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testSideBarApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:48:39 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "testSideBarApp.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testSideBarApp)

//--------------------------------------------------------------------------------
bool testSideBarApp::OnInit()
//--------------------------------------------------------------------------------
{
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




