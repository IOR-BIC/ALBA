/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testGuiLogic.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 11:09:27 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
//----------------------------------------------------------------------------
// Include:
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


