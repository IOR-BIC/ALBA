/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testGuiApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:45:59 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "testGuiApp.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testGuiApp)

//--------------------------------------------------------------------------------
bool testGuiApp::OnInit()
//--------------------------------------------------------------------------------
{
	m_logic = new testGuiLogic();
  SetTopWindow(m_logic->GetTopWin());  
  m_logic->Show();
  return TRUE;
}
//--------------------------------------------------------------------------------
int testGuiApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}


