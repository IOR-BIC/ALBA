/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testPicFactoryApp.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-07 08:42:26 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "testPicFactoryApp.h"
#include "mafPics.h"

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
IMPLEMENT_APP(testPicFactoryApp)

//--------------------------------------------------------------------------------
bool testPicFactoryApp::OnInit()
//--------------------------------------------------------------------------------
{
  // this defines a char** variable called ico_xpm
  #include "ico.xpm"
  
  // this add the icon to the PicFactory as "ico"
  mafADDPIC(ico);

  // these replace the File_Open and File_Save icon with ico
  mafAddPic("FILE_OPEN",ico_xpm);
  mafAddPic("FILE_SAVE",ico_xpm);

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






