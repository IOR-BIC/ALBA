/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIViewFrame
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



#include "albaGUIViewFrame.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaView.h"
//#include "albaSceneGraph.h"
//#include "albaSceneNode.h"
//#include "vtkVersion.h"

//----------------------------------------------------------------------------
// albaGUIViewFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIViewFrame, wxFrame)
    EVT_CLOSE(albaGUIViewFrame::OnCloseWindow)
    EVT_SIZE(albaGUIViewFrame::OnSize)
    EVT_BUTTON(VIEW_CLICKED, albaGUIViewFrame::OnSelect)
	  EVT_ACTIVATE  (albaGUIViewFrame::OnActivate)
END_EVENT_TABLE()

bool albaGUIViewFrame::m_Quitting = false;
//----------------------------------------------------------------------------
albaGUIViewFrame::albaGUIViewFrame( wxFrame* parent, 
                            wxWindowID id, 
                            const wxString& title,
                            const wxPoint& pos, 
                            const wxSize& size, 
                            long style 
                            )
//----------------------------------------------------------------------------
: wxFrame(parent, id, title, pos, size, style)
{
  m_Listener = NULL;
  m_ClientWin = NULL;
}
//----------------------------------------------------------------------------
albaGUIViewFrame::~albaGUIViewFrame( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIViewFrame::OnCloseWindow(wxCloseEvent &event)
//----------------------------------------------------------------------------
{ 
	albaEventMacro(albaEvent(this,VIEW_DELETE,m_View));
	Destroy();
}
//----------------------------------------------------------------------------
void albaGUIViewFrame::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{ 
	Refresh();
  if (m_ClientWin)
  {
    wxLayoutAlgorithm layout;
    layout.LayoutWindow(this,m_ClientWin);
  }

#ifndef WIN32
  int w,h;
  //don't initialize w & h using the event - use GetClientSize instead
  this->GetClientSize(&w,&h); 
  
  if(m_View)
  {
    m_View->SetWindowSize(w,h);
  }
#endif

}
//----------------------------------------------------------------------------
void albaGUIViewFrame::OnSelect(wxCommandEvent &event) 
//----------------------------------------------------------------------------
{
  wxWindow *rwi = (wxWindow*)event.GetEventObject();
  albaEventMacro(albaEvent(this,VIEW_SELECT,m_View,rwi));
}
//----------------------------------------------------------------------------
void albaGUIViewFrame::OnActivate(wxActivateEvent& event)
//----------------------------------------------------------------------------
{ 
	if( event.GetActive() && !m_Quitting )
  {
		albaEventMacro(albaEvent(this,VIEW_SELECT,m_View,(wxWindow*)NULL));
	  Layout();
	}
}
//----------------------------------------------------------------------------
void albaGUIViewFrame::SetView(albaView *view)
//----------------------------------------------------------------------------
{
   m_View = view;
   m_ClientWin = m_View->GetWindow();
   m_ClientWin->Reparent(this);
   m_ClientWin->Show(true);

   SetTitle(wxStripMenuCodes(m_View->GetLabel()));
}
//----------------------------------------------------------------------------
void albaGUIViewFrame::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case VIEW_QUIT:
      albaEventMacro(albaEvent(this,VIEW_DELETE,m_View));
      Destroy();
      break;
    default:
      // forward to ViewManager
      albaEventMacro(*e);
      break;
    }
  }
}
