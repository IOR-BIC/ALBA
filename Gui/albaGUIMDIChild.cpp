/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIMDIChild
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


#include "albaGUIMDIChild.h"
#include "albaGUIFrame.h"
#include "albaDecl.h"
#include "albaView.h"
#include "albaGUILab.h"
#include "albaPics.h" // frame icon
//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------
static const int PH = 13; //panel height
// the ID from 0 to 1138 are reserved for the DisplayList men

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIMDIChild,wxMDIChildFrame)
        EVT_CLOSE     (									 albaGUIMDIChild::OnCloseWindow)
				EVT_SIZE      (                  albaGUIMDIChild::OnSize)
        EVT_BUTTON    (VIEW_CLICKED,     albaGUIMDIChild::OnSelect)
				EVT_ACTIVATE  (albaGUIMDIChild::OnActivate)
        EVT_MAXIMIZE  (albaGUIMDIChild::OnMaximize)
END_EVENT_TABLE()

bool albaGUIMDIChild::m_Quitting = false;
//----------------------------------------------------------------------------
albaGUIMDIChild::albaGUIMDIChild(wxMDIParentFrame* parent,albaView *view)
:wxMDIChildFrame(parent,-1, "child",wxDefaultPosition, wxDefaultSize/*, 0*/)
//----------------------------------------------------------------------------
{
  assert(view);
  m_View     = view;
  m_Listener = NULL;
  m_AllowCloseFlag  = true;
  m_EnableResize    = true;

  this->Show(false);
  m_Win = m_View->GetWindow();
  m_Win->Reparent(this);
  m_Win->Show(true);

  SetIcon(albaPictureFactory::GetPictureFactory()->GetIcon("MDICHILD_ICON"));
	SetTitle(wxStripMenuCodes(m_View->GetLabel()));
}
//----------------------------------------------------------------------------
albaGUIMDIChild::~albaGUIMDIChild()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIMDIChild::OnSelect(wxCommandEvent &event) 
//----------------------------------------------------------------------------
{
  Activate(); // allow activation with the RMouse too
	wxWindow *rwi = (wxWindow*)event.GetEventObject();
  albaEventMacro(albaEvent(this,VIEW_SELECT,m_View,rwi));
}
//----------------------------------------------------------------------------
void albaGUIMDIChild::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  if (!m_EnableResize)
  {
    return;
  }

  int w,h;
  //don't initialize w & h using the event - use GetClientSize instead
  this->GetClientSize(&w,&h); 

  if (h < PH || w < PH) return;

	m_Win->Move(0,0);
	m_Win->SetSize(w,h);
	m_Win->Layout();
#ifndef WIN32
  if(m_View)
  {
    m_View->SetWindowSize(w,h);
  }
#endif
  m_View->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaGUIMDIChild::EnableResize(bool enable)
//----------------------------------------------------------------------------
{
  m_EnableResize = enable;
}
//----------------------------------------------------------------------------
void albaGUIMDIChild::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{
  if (!m_AllowCloseFlag) {return;}

  // VIEW_DELETE must be sent from here and not from the destructor
	// otherwise VIEW_DELETE is sent also on the closing of the application
	// when the listener (the ViewManager) has been already destroyed

	albaEventMacro(albaEvent(this,VIEW_DELETE,m_View));
	Destroy();
  m_View = NULL;
}
//----------------------------------------------------------------------------
void albaGUIMDIChild::OnActivate(wxActivateEvent& event)
//----------------------------------------------------------------------------
{ 
  if( event.GetActive() && m_View && !m_Quitting )
  {
    albaEventMacro(albaEvent(this,VIEW_SELECT,m_View,(wxWindow*)NULL));
    Layout();
  }
}
//----------------------------------------------------------------------------
void albaGUIMDIChild::OnMaximize(wxMaximizeEvent &event)
//----------------------------------------------------------------------------
{
  if (m_View)
  {
    albaString msg = "MaximizeSelectedView";
    albaEventMacro(albaEvent(this,VIEW_MAXIMIZE,&msg));
  }
}
