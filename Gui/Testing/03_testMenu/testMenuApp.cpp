/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testMenuApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:46:53 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "testMenuApp.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testMenuApp)

//--------------------------------------------------------------------------------
bool testMenuApp::OnInit()
//--------------------------------------------------------------------------------
{
	m_logic = new testMenuLogic();
  SetTopWindow(m_logic->GetTopWin());  
  m_logic->Show();
  return TRUE;
}
//--------------------------------------------------------------------------------
int testMenuApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}

