/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testFrameApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:46:28 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "testFrameApp.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testFrameApp)

//--------------------------------------------------------------------------------
bool testFrameApp::OnInit()
//--------------------------------------------------------------------------------
{
	m_logic = new testFrameLogic();
  SetTopWindow(m_logic->GetTopWin());  
  m_logic->Show();
	m_logic->Init( argc, argv );
  return TRUE;
}
//--------------------------------------------------------------------------------
int testFrameApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}

