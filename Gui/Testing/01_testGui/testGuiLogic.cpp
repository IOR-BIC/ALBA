/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testGuiLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:23:49 $
  Version:   $Revision: 1.3 $
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


#include "testGuiLogic.h" 
//#include "mmgMDIFrame.h"
#include "mmgFrame.h"
#include "testGui.h" 
#include "mmgPanel.h" 

//----------------------------------------------------------------------------
testGuiLogic::testGuiLogic()
//----------------------------------------------------------------------------
{
  //m_win = new mmgMDIFrame("testFrame", wxDefaultPosition, wxSize(800, 600));
  m_win = new mmgFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);
  //mmgPanel *p = new mmgPanel(m_win,-1);
  //p->Show(true);
  //p->SetSize(100,100);
  m_testGui = new testGui(m_win);
  //p->Add(m_testGui->GetGui());
}
//----------------------------------------------------------------------------
testGuiLogic::~testGuiLogic()
//----------------------------------------------------------------------------
{
  cppDEL(m_testGui);
}
//----------------------------------------------------------------------------
void testGuiLogic::Show()
//----------------------------------------------------------------------------
{
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void testGuiLogic::OnQuit()
//----------------------------------------------------------------------------
{
	m_win->Destroy();
}
//----------------------------------------------------------------------------
wxWindow* testGuiLogic::GetTopWin()
//----------------------------------------------------------------------------
{
  return m_win;
}
//----------------------------------------------------------------------------
void testGuiLogic::OnEvent(mafEvent& e)
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


