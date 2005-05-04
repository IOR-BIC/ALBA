/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgViewFrame.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-04 11:44:06 $
  Version:   $Revision: 1.5 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "mmgViewFrame.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafView.h"
//#include "mafSceneGraph.h"
//#include "mafSceneNode.h"
//#include "vtkVersion.h"

//----------------------------------------------------------------------------
// mmgViewFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgViewFrame, wxFrame)
    EVT_CLOSE(mmgViewFrame::OnCloseWindow)
    EVT_SIZE(mmgViewFrame::OnSize)
    EVT_BUTTON(VIEW_CLICKED, mmgViewFrame::OnSelect)
	  EVT_ACTIVATE  (mmgViewFrame::OnActivate)
END_EVENT_TABLE()

bool mmgViewFrame::m_quitting = false;
//----------------------------------------------------------------------------
mmgViewFrame::mmgViewFrame( wxFrame* parent, 
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
  m_clientwin = NULL;
}
//----------------------------------------------------------------------------
mmgViewFrame::~mmgViewFrame( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgViewFrame::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{ 
	mafEventMacro(mafEvent(this,VIEW_DELETE,m_view));
	Destroy();
}
//----------------------------------------------------------------------------
void mmgViewFrame::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{ 
   Refresh();
   if (m_clientwin)
   {
     wxLayoutAlgorithm layout;
     layout.LayoutWindow(this,m_clientwin);
   }
}
//----------------------------------------------------------------------------
void mmgViewFrame::OnSelect(wxCommandEvent &event) 
//----------------------------------------------------------------------------
{
  wxWindow *rwi = (wxWindow*)event.GetEventObject();
  mafEventMacro(mafEvent(this,VIEW_SELECT,m_view,rwi));
}
//----------------------------------------------------------------------------
void mmgViewFrame::OnActivate(wxActivateEvent& event)
//----------------------------------------------------------------------------
{ 
	if( event.GetActive() && !m_quitting )
  {
		mafEventMacro(mafEvent(this,VIEW_SELECT,m_view,(wxWindow*)NULL));
	  Layout();
	}
}
//----------------------------------------------------------------------------
void mmgViewFrame::SetView(mafView *view)
//----------------------------------------------------------------------------
{
   m_view = view;
   m_clientwin = m_view->GetWindow();
   m_clientwin->Reparent(this);
   m_clientwin->Show(true);

   SetTitle(wxStripMenuCodes(m_view->GetLabel()));
}
//----------------------------------------------------------------------------
void mmgViewFrame::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(event))
  {
    switch(e->GetId())
    {
    case VIEW_QUIT:
      mafEventMacro(mafEvent(this,VIEW_DELETE,m_view));
      Destroy();
      break;
    default:
      // forward to ViewManager
      mafEventMacro(*e);
      break;
    }
  }
}