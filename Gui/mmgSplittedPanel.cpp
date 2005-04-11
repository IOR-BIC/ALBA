/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgSplittedPanel.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:28 $
  Version:   $Revision: 1.2 $
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


#include "mmgSplittedPanel.h" 
//----------------------------------------------------------------------------
// costants :
//----------------------------------------------------------------------------
#define PAN_BOTTOM         200

//----------------------------------------------------------------------------
// mmgSplittedPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgSplittedPanel,mmgPanel)
    EVT_SIZE(mmgSplittedPanel::OnSize)
    EVT_SASH_DRAGGED(PAN_BOTTOM, mmgSplittedPanel::OnSashDrag)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
mmgSplittedPanel::mmgSplittedPanel(wxWindow* parent, wxWindowID id, int size) 
:mmgPanel(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL | wxCLIP_CHILDREN) 
//----------------------------------------------------------------------------
{
  m_Bottom = new wxSashLayoutWindow(this, PAN_BOTTOM, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
  m_Bottom->SetDefaultSize(wxSize(1000, size));
  m_Bottom->SetOrientation(wxLAYOUT_HORIZONTAL);
  m_Bottom->SetAlignment(wxLAYOUT_BOTTOM);
  m_Bottom->SetSashVisible(wxSASH_TOP, TRUE);
  m_Bottom->SetExtraBorderSize(0);

  m_bottomw    = NULL;
  m_topw = NULL;
}
//----------------------------------------------------------------------------
mmgSplittedPanel::~mmgSplittedPanel( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgSplittedPanel::OnSize(wxSizeEvent& WXUNUSED(event))
//----------------------------------------------------------------------------
{
  DoLayout();
}
//----------------------------------------------------------------------------
void mmgSplittedPanel::OnSashDrag(wxSashEvent& event)
//----------------------------------------------------------------------------
{
  if(event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE) 
    return;
  if(event.GetId()==PAN_BOTTOM) 
    m_Bottom->SetDefaultSize(wxSize(1000,event.GetDragRect().height));
  DoLayout();
}
//----------------------------------------------------------------------------
void mmgSplittedPanel::DoLayout()
//----------------------------------------------------------------------------
{
  wxLayoutAlgorithm layout;
  layout.LayoutWindow(this,m_topw);
}
//----------------------------------------------------------------------------
void mmgSplittedPanel::PutOnTop(wxWindow *w)
//----------------------------------------------------------------------------
{
  if (m_topw) m_bottomw->Show(false);
  w->Reparent(this);
  m_topw = w;
  w->Show(true);
}
//----------------------------------------------------------------------------
void mmgSplittedPanel::PutOnBottom(wxWindow *w)
//----------------------------------------------------------------------------
{
  if (m_bottomw) m_bottomw->Show(false);
  w->Reparent(m_Bottom);
  m_bottomw = w;
  w->Show(true);
}
