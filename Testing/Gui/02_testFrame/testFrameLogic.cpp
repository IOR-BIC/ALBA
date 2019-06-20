/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testFrameLogic
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


#include "testFrameLogic.h" 

//----------------------------------------------------------------------------
testFrameLogic::testFrameLogic()
//----------------------------------------------------------------------------
{
  m_win = new albaGUIMDIFrame("testFrame", wxDefaultPosition, wxSize(800, 600));
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
	//albaGUIMDIChild::OnQuit(); //prevent all the MDIChild from Notify the Activation (when a Child is deleted, one of the remaining is Activated)
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
void testFrameLogic::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event)) 
  {
    switch(e->GetId())
    {
    case MENU_FILE_QUIT:
      OnQuit();		
      break; 
    default:
      e->Log();
      break; 
    }
  }
}
