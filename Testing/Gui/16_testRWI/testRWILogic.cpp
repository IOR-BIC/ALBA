/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testRWILogic
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


#include "testRWILogic.h"
#include "albaDecl.h"
#include "albaGUI.h"
#include "testRWIBaseDlg.h"
#include "testRWIDlg.h"

//--------------------------------------------------------------------------------
//const:
//--------------------------------------------------------------------------------
enum 
{
  ID_D1 = MINID,
  ID_D2,
  ID_D3,
};

//--------------------------------------------------------------------------------
testRWILogic::testRWILogic()
//--------------------------------------------------------------------------------
{
  /**todo: PAOLO please read here */

  // RESULT OF RWI TEST :
  // RWIBASE produce a memory leak 5600 byte long, as follow -- by me (Silvano) this is not considered an error but a feature :-)
  // C:\Program Files\VisualStudio7\Vc7\include\crtdbg.h(689) : {2804} normal block at 0x099C00A8, 5600 bytes long.
  // Data: <            Buil> 00 00 00 00 00 00 00 00 00 00 00 00 42 75 69 6C 
  // Object dump complete.

  // the leaks is always 5600 bytes long, doesn't matter how many instances of RWI you have created,
  // so maybe it is related to something concerned with the initialization of the OpenGL context,
  // and the real problem could be in my NVidia OpenGL Driver


  m_win = new wxFrame(NULL,-1,"TestRWI",wxDefaultPosition,wxDefaultSize,
    wxMINIMIZE_BOX | wxMAXIMIZE_BOX | /*wxRESIZE_BORDER |*/ wxSYSTEM_MENU | wxCAPTION );
  albaSetFrame(m_win);

  albaGUI *gui = new albaGUI(this);
  gui->Divider();
  gui->Label("Examples of VTK RenderWindow"); 
  gui->Button(ID_D1,"test RWIBase");
  gui->Button(ID_D2,"test RWI");
  gui->Label("");
  gui->Label("");
  gui->Button(ID_D3,"quit");
  gui->Reparent(m_win);
  m_win->Fit(); // resize m_win to fit it's content
  
}
//--------------------------------------------------------------------------------
testRWILogic::~testRWILogic()
//--------------------------------------------------------------------------------
{
}
//--------------------------------------------------------------------------------
void testRWILogic::OnEvent(albaEventBase *alba_event)
//--------------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event)) 
  {
    switch(e->GetId())
    {
    case ID_D1:
      {
        testRWIBaseDlg d("test RWIBase");
        d.ShowModal();
      }
      break;
    case ID_D2: 
      {
        testRWIDlg d("test RWI");
        d.ShowModal();
      }
      break;
    case ID_D3: 
      m_win->Destroy();
      break;
    }
  }
}
//--------------------------------------------------------------------------------
void testRWILogic::Show()
//--------------------------------------------------------------------------------
{
  m_win->Show(true);
}
