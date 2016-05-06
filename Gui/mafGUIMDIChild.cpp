/*=========================================================================

 Program: MAF2
 Module: mafGUIMDIChild
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGUIMDIChild.h"
#include "mafGUIFrame.h"
#include "mafDecl.h"
#include "mafView.h"
//#include "mflAssembly.h"
//#include "mafSceneNode.h"
//#include "mafSceneGraph.h"
#include "mafGUILab.h"
//#include "mafGUIPicButton.h"  //SIL. 7-4-2005: -- seems not required
//#include "mafPipe.h"

//#include "mafVME.h"
#include "mafPics.h" // frame icon
//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------
static const int PH = 13; //panel height
// the ID from 0 to 1138 are reserved for the DisplayList men

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIMDIChild,wxMDIChildFrame)
        EVT_CLOSE     (									 mafGUIMDIChild::OnCloseWindow)
				EVT_SIZE      (                  mafGUIMDIChild::OnSize)
        EVT_BUTTON    (VIEW_CLICKED,     mafGUIMDIChild::OnSelect)
				EVT_ACTIVATE  (mafGUIMDIChild::OnActivate)
        EVT_MAXIMIZE  (mafGUIMDIChild::OnMaximize)
END_EVENT_TABLE()

bool mafGUIMDIChild::m_Quitting = false;
//----------------------------------------------------------------------------
mafGUIMDIChild::mafGUIMDIChild(wxMDIParentFrame* parent,mafView *view)
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

  SetIcon(mafPictureFactory::GetPictureFactory()->GetIcon("MDICHILD_ICON"));
	SetTitle(wxStripMenuCodes(m_View->GetLabel()));
}
//----------------------------------------------------------------------------
mafGUIMDIChild::~mafGUIMDIChild()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIMDIChild::OnSelect(wxCommandEvent &event) 
//----------------------------------------------------------------------------
{
  Activate(); // allow activation with the RMouse too
	wxWindow *rwi = (wxWindow*)event.GetEventObject();
  mafEventMacro(mafEvent(this,VIEW_SELECT,m_View,rwi));
}
//----------------------------------------------------------------------------
void mafGUIMDIChild::OnSize(wxSizeEvent &event)
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
void mafGUIMDIChild::EnableResize(bool enable)
//----------------------------------------------------------------------------
{
  m_EnableResize = enable;
}
//----------------------------------------------------------------------------
void mafGUIMDIChild::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{
  if (!m_AllowCloseFlag) {return;}

  // VIEW_DELETE must be sent from here and not from the destructor
	// otherwise VIEW_DELETE is sent also on the closing of the application
	// when the listener (the ViewManager) has been already destroyed

	mafEventMacro(mafEvent(this,VIEW_DELETE,m_View));
	Destroy();
  m_View = NULL;
}
//----------------------------------------------------------------------------
void mafGUIMDIChild::OnActivate(wxActivateEvent& event)
//----------------------------------------------------------------------------
{ 
  if( event.GetActive() && m_View && !m_Quitting )
  {
    mafEventMacro(mafEvent(this,VIEW_SELECT,m_View,(wxWindow*)NULL));
    Layout();
  }
}
//----------------------------------------------------------------------------
void mafGUIMDIChild::OnMaximize(wxMaximizeEvent &event)
//----------------------------------------------------------------------------
{
  if (m_View)
  {
    mafString msg = "MaximizeSelectedView";
    mafEventMacro(mafEvent(this,VIEW_MAXIMIZE,&msg));
  }
}
