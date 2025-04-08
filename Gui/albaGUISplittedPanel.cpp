/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISplittedPanel
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


#include "albaGUISplittedPanel.h" 
//----------------------------------------------------------------------------
// costants :
//----------------------------------------------------------------------------
#define PAN_BOTTOM         200

//----------------------------------------------------------------------------
// albaGUISplittedPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUISplittedPanel,albaGUIPanel)
    EVT_SIZE(albaGUISplittedPanel::OnSize)
    EVT_SASH_DRAGGED(PAN_BOTTOM, albaGUISplittedPanel::OnSashDrag)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
albaGUISplittedPanel::albaGUISplittedPanel(wxWindow* parent, wxWindowID id, int size) 
:albaGUIPanel(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL | wxCLIP_CHILDREN) 
//----------------------------------------------------------------------------
{
  m_Bottom = new wxSashLayoutWindow(this, PAN_BOTTOM, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
  m_Bottom->SetDefaultSize(wxSize(1000, size));
  m_Bottom->SetOrientation(wxLAYOUT_HORIZONTAL);
  m_Bottom->SetAlignment(wxLAYOUT_BOTTOM);
  m_Bottom->SetSashVisible(wxSASH_TOP, true);
  m_Bottom->SetExtraBorderSize(0);

  m_BottomPanel    = NULL;
  m_TopPanel = NULL;
}
//----------------------------------------------------------------------------
albaGUISplittedPanel::~albaGUISplittedPanel( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUISplittedPanel::OnSize(wxSizeEvent& WXUNUSED(event))
//----------------------------------------------------------------------------
{
  DoLayout();
}
//----------------------------------------------------------------------------
void albaGUISplittedPanel::OnSashDrag(wxSashEvent& event)
//----------------------------------------------------------------------------
{
  if(event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE) 
    return;
  if(event.GetId()==PAN_BOTTOM) 
    m_Bottom->SetDefaultSize(wxSize(1000,event.GetDragRect().height));
  DoLayout();
}
//----------------------------------------------------------------------------
void albaGUISplittedPanel::DoLayout()
//----------------------------------------------------------------------------
{
  wxLayoutAlgorithm layout;
  layout.LayoutWindow(this,m_TopPanel);
}
//----------------------------------------------------------------------------
void albaGUISplittedPanel::PutOnTop(wxWindow *w)
//----------------------------------------------------------------------------
{
  if (m_TopPanel) m_BottomPanel->Show(false);
  w->Reparent(this);
  m_TopPanel = w;
  w->Show(true);
}
//----------------------------------------------------------------------------
void albaGUISplittedPanel::PutOnBottom(wxWindow *w)
//----------------------------------------------------------------------------
{
  if (m_BottomPanel) m_BottomPanel->Show(false);
  w->Reparent(m_Bottom);
  m_BottomPanel = w;
  w->Show(true);
}
