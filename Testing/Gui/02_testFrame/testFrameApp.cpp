/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testFrameApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/



#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "testFrameApp.h"
#include "albaPics.h"
//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testFrameApp)

//--------------------------------------------------------------------------------
bool testFrameApp::OnInit()
//--------------------------------------------------------------------------------
{
  albaPics.Initialize();	
        
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

