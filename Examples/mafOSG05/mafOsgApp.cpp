/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOsgApp.cpp,v $
  Language:  C++
  Date:      $Date: 2006-02-21 16:14:10 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafTestLogic.h"
#include "mafPics.h"

//--------------------------------------------------------------------------------
class mafOsgApp : public wxApp
//--------------------------------------------------------------------------------
{
public:

  bool OnInit()
  {
    mafPics.Initialize();	

    m_logic = new mafTestLogic();
    SetTopWindow(m_logic->GetTopWin());
    m_logic->Show();
    m_logic->Init(argc,argv);
    return true;
  };

  int  OnExit() 
  {
    cppDEL(m_logic);
    return 0;
  };

  mafTestLogic  *m_logic;
};
//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------
DECLARE_APP(mafOsgApp)
IMPLEMENT_APP(mafOsgApp)

