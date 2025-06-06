/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testTimeBarApp
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


#include "testTimeBarApp.h"
#include "albaPics.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testTimeBarApp)

//--------------------------------------------------------------------------------
bool testTimeBarApp::OnInit()
//--------------------------------------------------------------------------------
{
  albaPics.Initialize();	

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





