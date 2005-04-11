/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testFrameLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:23:58 $
  Version:   $Revision: 1.2 $
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


#include "testFrameLogic.h" 

//----------------------------------------------------------------------------
testFrameLogic::testFrameLogic()
//----------------------------------------------------------------------------
{
  m_win = new mmgMDIFrame("testFrame", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);
}
//----------------------------------------------------------------------------
testFrameLogic::~testFrameLogic()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void testFrameLogic::Show()
//----------------------------------------------------------------------------
{
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void testFrameLogic::Init(int argc, char** argv)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void testFrameLogic::OnQuit()
//----------------------------------------------------------------------------
{
	//mmgMDIChild::OnQuit(); //prevent all the MDIChild from Notify the Activation (when a Child is deleted, one of the remaining is Activated)
  /*
		cppDEL(m_SideBarTabbed); //must be after deleting the vme_manager
		if(m_PlugLogbar) 
		{
			delete wxLog::SetActiveTarget(NULL); 
		}
  }
  */
	m_win->Destroy();
}
//----------------------------------------------------------------------------
wxWindow* testFrameLogic::GetTopWin()
//----------------------------------------------------------------------------
{
  return m_win;
}
//----------------------------------------------------------------------------
void testFrameLogic::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
  switch(e.GetId())
  {
  case MENU_FILE_QUIT:
			OnQuit();		
  break; 
  default:
    e.Log();
  break; 
  }
}

