/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testSashApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:25:05 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "testSashApp.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testSashApp)

//--------------------------------------------------------------------------------
bool testSashApp::OnInit()
//--------------------------------------------------------------------------------
{
	m_logic = new testSashLogic();
  SetTopWindow(m_logic->GetTopWin());  
  m_logic->Show();
  return TRUE;
}
//--------------------------------------------------------------------------------
int testSashApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}

