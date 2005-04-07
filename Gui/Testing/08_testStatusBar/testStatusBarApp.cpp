/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testStatusBarApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-07 10:09:26 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "testStatusBarApp.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testStatusBarApp)

//--------------------------------------------------------------------------------
bool testStatusBarApp::OnInit()
//--------------------------------------------------------------------------------
{
	m_logic = new testStatusBarLogic();
  SetTopWindow(m_logic->GetTopWin());  
  m_logic->Show();
  return TRUE;
}
//--------------------------------------------------------------------------------
int testStatusBarApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}





