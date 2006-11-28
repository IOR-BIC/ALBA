/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgApplicationLayoutSettings.cpp,v $
Language:  C++
Date:      $Date: 2006-11-28 12:18:23 $
Version:   $Revision: 1.2 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmgApplicationLayoutSettings.h"
#include <wx/config.h>
#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgMDIFrame.h"
#include "mafViewManager.h"
#include "mafView.h"

#include "mmaApplicationLayout.h"
#include "mafNode.h"

//----------------------------------------------------------------------------
mmgApplicationLayoutSettings::mmgApplicationLayoutSettings(mafObserver *listener)
//----------------------------------------------------------------------------
{
	m_Listener      = listener;
  m_DefaultLayout = "Default Layout";
  m_ViewManager   = NULL;
  m_Layout        = NULL;
  m_Win           = NULL;

  InitializeLayout();

  m_Gui = new mmgGui(this);
  m_Gui->Label(_("Application layout"));
  m_Gui->Button(SAVE_LAYOUT_ID,_("Save in root"));
  m_Gui->String(LAYOUT_NAME_ID,_("name"),&m_DefaultLayout);
  m_Gui->Label("");
}
//----------------------------------------------------------------------------
mmgApplicationLayoutSettings::~mmgApplicationLayoutSettings() 
//----------------------------------------------------------------------------
{
  mafDEL(m_Layout);
  m_Gui = NULL; // gui is destroyed by the dialog.
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
  {
    case SAVE_LAYOUT_ID:
    {
      SaveLayout();
    }
    break;
    case LAYOUT_NAME_ID:
    break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
  /*wxConfig *config = new wxConfig(wxEmptyString);
  config->Write("DefaultLayout", m_DefaultLayout.GetCStr());
  cppDEL(config);*/
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::SaveLayout()
//----------------------------------------------------------------------------
{
  if (m_ViewManager)
  {
    wxFrame *frame = (wxFrame *)mafGetFrame();
    int pos[2], size[2];
    wxRect rect;
    rect = frame->GetRect();
    pos[0] = rect.GetPosition().x;
    pos[1] = rect.GetPosition().y;
    size[0] = rect.GetSize().GetWidth();
    size[1] = rect.GetSize().GetHeight();

    mafNode *root = m_ViewManager->GetCurrentRoot();
    m_Layout = mmaApplicationLayout::SafeDownCast(root->GetAttribute("ApplicationLayout"));
    if (m_Layout == NULL)
    {
      mafNEW(m_Layout);
      root->SetAttribute(m_Layout->GetName(), m_Layout);
    }
    else
    {
    }
    m_Layout->SetApplicationInfo(frame->IsMaximized(), pos, size);
    wxPaneInfo toolbar = m_Win->GetDockManager().GetPane("toolbar");
    bool toolbar_vis = toolbar.IsShown();
    m_Layout->SetInterfaceElementVisibility("toolbar", toolbar_vis);
    wxPaneInfo sidebar = m_Win->GetDockManager().GetPane("sidebar");
    bool sidebar_vis = sidebar.IsShown();
    m_Layout->SetInterfaceElementVisibility("sidebar", sidebar_vis);
    wxPaneInfo logbar = m_Win->GetDockManager().GetPane("logbar");
    bool logbar_vis = logbar.IsShown();
    m_Layout->SetInterfaceElementVisibility("logbar", logbar_vis);
    m_Layout->SetLayoutName(m_DefaultLayout.GetCStr());
    mafView *v = m_ViewManager->GetList();
    for(; v; v = v->m_Next)
    {
      m_Layout->AddView(v);
    }
  }
}
//----------------------------------------------------------------------------
void mmgApplicationLayoutSettings::InitializeLayout()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  wxString layout_name;
  if(config->Read("DefaultLayout", &layout_name))
  {
    m_DefaultLayout = layout_name.c_str();
  }
  cppDEL(config);
}
