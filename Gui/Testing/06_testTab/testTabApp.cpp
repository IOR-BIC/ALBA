/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTabApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:48:09 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "testTabApp.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testTabApp)

//--------------------------------------------------------------------------------
bool testTabApp::OnInit()
//--------------------------------------------------------------------------------
{
	m_logic = new testTabLogic();
  SetTopWindow(m_logic->GetTopWin());  
  m_logic->Show();
  return TRUE;
}
//--------------------------------------------------------------------------------
int testTabApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}



