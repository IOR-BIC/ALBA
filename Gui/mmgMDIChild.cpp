/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMDIChild.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 14:24:53 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

//----------------------------------------------------------------------------
// Include: - include the class being defined first
//----------------------------------------------------------------------------
#include "mmgMDIChild.h"

#include "mmgFrame.h"
#include "mafDecl.h"
#include "mafView.h"
//#include "mflAssembly.h"
//#include "mafSceneNode.h"
//#include "mafSceneGraph.h"
#include "mmgBitmaps.h"
#include "mmgLab.h"
#include "mmgPicButton.h"
//#include "mafPipe.h"

//#include "mafNode.h"
//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------
static const int PH = 13; //panel height
// the ID from 0 to 1138 are reserved for the DisplayList men

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgMDIChild,wxMDIChildFrame)
        EVT_CLOSE     (									 mmgMDIChild::OnCloseWindow)
				EVT_SIZE      (                  mmgMDIChild::OnSize)
        EVT_BUTTON    (VIEW_CLICKED,     mmgMDIChild::OnSelect)
				EVT_ACTIVATE  (mmgMDIChild::OnActivate)
END_EVENT_TABLE()

bool mmgMDIChild::m_quitting = false;
//----------------------------------------------------------------------------
mmgMDIChild::mmgMDIChild(wxMDIParentFrame* parent,mafView *view)
:wxMDIChildFrame(parent,-1, "child",wxDefaultPosition, wxDefaultSize, 0)
//----------------------------------------------------------------------------
{
  this->Show(false);

  assert(view);
  m_view     = view;
  m_Listener = NULL;

  m_win = m_view->m_win;
  m_win->Reparent(this);
  m_win->Show(true);

	SetTitle(wxStripMenuCodes(m_view->m_label));
}
//----------------------------------------------------------------------------
mmgMDIChild::~mmgMDIChild( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgMDIChild::OnSelect(wxCommandEvent &event) 
//----------------------------------------------------------------------------
{
  Activate(); // allow activation with the RMouse too
	wxWindow *rwi = (wxWindow*)event.GetEventObject();
  mafEventMacro(mafEvent(this,VIEW_SELECT,m_view,rwi));
}
//----------------------------------------------------------------------------
void mmgMDIChild::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  int w,h;
  //dont initialize w & h using the event - use GetClientSize instead
  this->GetClientSize(&w,&h); 

  if (h < PH || w < PH) return;

	m_win->Move(0,0);
	m_win->SetSize(w,h);
	m_win->Layout();
}
//----------------------------------------------------------------------------
void mmgMDIChild::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{ 
  // VIEW_DELETE must be sent from here and not from the destructor
	// otherwise VIEW_DELETE is sent olso on the closing of the application
	// when the listenere (the ViewManager) has been yet destroy

	mafEventMacro(mafEvent(this,VIEW_DELETE,m_view));
	Destroy();
}
//----------------------------------------------------------------------------
void mmgMDIChild::OnActivate(wxActivateEvent& event)
//----------------------------------------------------------------------------
{ 
  if( event.GetActive() && !m_quitting )
  {
    mafEventMacro(mafEvent(this,VIEW_SELECT,m_view,(wxWindow*)NULL));
    Layout();
  }
}
