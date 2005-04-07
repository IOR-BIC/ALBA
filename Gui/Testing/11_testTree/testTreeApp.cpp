/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTreeApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-07 18:37:22 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "testTreeApp.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testTreeApp)

//--------------------------------------------------------------------------------
bool testTreeApp::OnInit()
//--------------------------------------------------------------------------------
{
	m_logic = new testTreeLogic();
  SetTopWindow(m_logic->GetTopWin());  
  m_logic->Show();
  return TRUE;
}
//--------------------------------------------------------------------------------
int testTreeApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}





