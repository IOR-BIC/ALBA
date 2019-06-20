/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testPicFactoryApp
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


#include "testPicFactoryApp.h"
#include "albaPics.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testPicFactoryApp)

//--------------------------------------------------------------------------------
bool testPicFactoryApp::OnInit()
//--------------------------------------------------------------------------------
{
  albaPics.Initialize();	

  // this defines a char** variable called ico_xpm
  #include "ico.xpm"
  
  // this add the icon to the PicFactory as "ico"
  albaADDPIC(ico);

  // these replace the File_Open and File_Save icon with ico
  albaPics.Add("FILE_OPEN",ico_xpm);
  albaPics.Add("FILE_SAVE",ico_xpm);

  m_logic = new testPicFactoryLogic();
  SetTopWindow(m_logic->GetTopWin());  
  m_logic->Show();
  return TRUE;
}
//--------------------------------------------------------------------------------
int testPicFactoryApp::OnExit()
//--------------------------------------------------------------------------------
{
  cppDEL(m_logic);
  return 0;
}






