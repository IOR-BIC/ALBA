/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMDIChild.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-21 12:03:29 $
  Version:   $Revision: 1.15 $
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


#include "mmgMDIChild.h"
#include "mmgFrame.h"
#include "mafDecl.h"
#include "mafView.h"
#include "mafViewVTK.h"
//#include "mflAssembly.h"
//#include "mafSceneNode.h"
//#include "mafSceneGraph.h"
#include "mmgLab.h"
//#include "mmgPicButton.h"  //SIL. 7-4-2005: -- seems not required
//#include "mafPipe.h"

//#include "mafNode.h"
#include "mafPics.h" // frame icon
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

bool mmgMDIChild::m_Quitting = false;
//----------------------------------------------------------------------------
mmgMDIChild::mmgMDIChild(wxMDIParentFrame* parent,mafView *view)
:wxMDIChildFrame(parent,-1, "child",wxDefaultPosition, wxDefaultSize/*, 0*/)
//----------------------------------------------------------------------------
{
  this->Show(false);

  assert(view);
  m_View     = view;
  m_Listener = NULL;

  m_Win = m_View->GetWindow();
  m_Win->Reparent(this);
  m_Win->Show(true);

  SetIcon(mafPics.GetIcon("MDICHILD_ICON"));
	SetTitle(wxStripMenuCodes(m_View->GetLabel()));
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
  mafEventMacro(mafEvent(this,VIEW_SELECT,m_View,rwi));
}
//----------------------------------------------------------------------------
void mmgMDIChild::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  int w,h;
  //don't initialize w & h using the event - use GetClientSize instead
  this->GetClientSize(&w,&h); 

  if (h < PH || w < PH) return;

	m_Win->Move(0,0);
	m_Win->SetSize(w,h);
	m_Win->Layout();
#ifndef WIN32
  if(mafViewVTK::SafeDownCast(m_View))
  {
    ((mafViewVTK *)m_View)->SetWindowSize(w,h);
  }
#endif
}
//----------------------------------------------------------------------------
void mmgMDIChild::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{ 
  // VIEW_DELETE must be sent from here and not from the destructor
	// otherwise VIEW_DELETE is sent also on the closing of the application
	// when the listener (the ViewManager) has been already destroied

	mafEventMacro(mafEvent(this,VIEW_DELETE,m_View));
	Destroy();
}
//----------------------------------------------------------------------------
void mmgMDIChild::OnActivate(wxActivateEvent& event)
//----------------------------------------------------------------------------
{ 
  if( event.GetActive() && !m_Quitting )
  {
    mafEventMacro(mafEvent(this,VIEW_SELECT,m_View,(wxWindow*)NULL));
    Layout();
  }
}
