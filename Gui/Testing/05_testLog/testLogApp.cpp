/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testLogApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:47:37 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "testLogApp.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testLogApp)

//--------------------------------------------------------------------------------
bool testLogApp::OnInit()
//--------------------------------------------------------------------------------
{
	m_logic = new testLogLogic();
  SetTopWindow(m_logic->GetTopWin());  
  m_logic->Show();
  return TRUE;
}
//--------------------------------------------------------------------------------
int testLogApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}

