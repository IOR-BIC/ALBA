/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTimeBarApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-04 15:01:48 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "testTimeBarApp.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testTimeBarApp)

//--------------------------------------------------------------------------------
bool testTimeBarApp::OnInit()
//--------------------------------------------------------------------------------
{
	m_logic = new testTimeBarLogic();
  SetTopWindow(m_logic->GetTopWin());  
  m_logic->Show();
  return TRUE;
}
//--------------------------------------------------------------------------------
int testTimeBarApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}





