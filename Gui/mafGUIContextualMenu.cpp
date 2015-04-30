/*=========================================================================

 Program: MAF2
 Module: mafGUIContextualMenu
 Authors: Daniele Giunchi
 
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


#include "mafGUIContextualMenu.h"
#include "wx/utils.h"
#include <wx/laywin.h>
#include <wx/mdi.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafSceneNode.h"
#include "mafSceneGraph.h"
#include "mafView.h"
#include "mafViewVTK.h"
#include "mafViewCompound.h"
#include "mafGUIMDIChild.h"
#include "mafGUIDialog.h"
#include "mafGUI.h"
#include "mafPipe.h"

#include "mafPipe.h"

#include "mafVME.h"
#include "mafVMELandmark.h"

#include "vtkMAFSmartPointer.h"
#include "vtkVRMLExporter.h"

//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------
enum VIEW_CONTEXTUAL_MENU_ID
{
  CONTEXTUAL_VIEW_MENU_START = MINID,	
		CONTEXTUAL_MENU_RENAME_VIEW,				
		CONTEXTUAL_MENU_HIDE_VME,				
		CONTEXTUAL_MENU_DELETE_VME,
		CONTEXTUAL_MENU_TRANSFORM,
    CONTEXTUAL_MENU_VME_PIPE,
		CONTEXTUAL_MENU_QUIT_CHILD_VIEW,
		CONTEXTUAL_MENU_MAXIMIZE_CHILD_VIEW,
    CONTEXTUAL_MENU_MAXIMIZE_CHILD_SUB_VIEW,
		CONTEXTUAL_MENU_NORMAL_SIZE_CHILD_VIEW,
    CONTEXTUAL_MENU_NORMAL_SIZE_CHILD_SUB_VIEW,
		CONTEXTUAL_MENU_SAVE_AS_IMAGE,
    CONTEXTUAL_MENU_SAVE_ALL_AS_IMAGE,
    CONTEXTUAL_MENU_EXPORT_AS_VRML,
    //CONTEXTUAL_MENU_EXTERNAL_INTERNAL_VIEW,
	CONTEXTUAL_VIEW_MENU_STOP
};

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIContextualMenu,wxMenu)
  EVT_MENU_RANGE(CONTEXTUAL_VIEW_MENU_START, CONTEXTUAL_VIEW_MENU_STOP, mafGUIContextualMenu::OnContextualViewMenu)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUIContextualMenu::mafGUIContextualMenu()
:wxMenu()
//----------------------------------------------------------------------------
{
	m_ChildViewActive = NULL;
	m_ViewActive      = NULL;
  m_Listener        = NULL;
}
//----------------------------------------------------------------------------
mafGUIContextualMenu::~mafGUIContextualMenu()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIContextualMenu::ShowContextualMenu(wxFrame *child, mafView *view, bool vme_menu)
//----------------------------------------------------------------------------
{
	m_ChildViewActive = child;
	m_ViewActive      = view;

	m_ChildViewActive->SetFocus();

	if(vme_menu)
	{
		this->Append(CONTEXTUAL_MENU_HIDE_VME, "Hide");
		this->Append(CONTEXTUAL_MENU_DELETE_VME, "Delete");
		this->Append(CONTEXTUAL_MENU_TRANSFORM, "Move\tCtrl+T");
    this->AppendSeparator();
    this->Append(CONTEXTUAL_MENU_VME_PIPE,"visual props");
    this->AppendSeparator();
  }
	this->Append(CONTEXTUAL_MENU_RENAME_VIEW, "Rename View");
  mafViewCompound *vc = mafViewCompound::SafeDownCast(m_ViewActive);
#ifdef WIN32  
	this->AppendSeparator();
  if (m_ChildViewActive->IsMaximized())
  {
    this->Append(CONTEXTUAL_MENU_NORMAL_SIZE_CHILD_VIEW, "Normal Size");
  }
  else
  {
    this->Append(CONTEXTUAL_MENU_MAXIMIZE_CHILD_VIEW, "Maximize");
  }
  if (vc)
  {
    if (vc->IsSubViewMaximized())
    {
      this->Append(CONTEXTUAL_MENU_NORMAL_SIZE_CHILD_SUB_VIEW, "Normal Size SubView");
    }
    else
    {
      this->Append(CONTEXTUAL_MENU_MAXIMIZE_CHILD_SUB_VIEW, "Maximize SubView");
    }
  }
#endif
  //this->Append(CONTEXTUAL_MENU_EXTERNAL_INTERNAL_VIEW, "External", "Switch view visualization between external/internal", TRUE);
  //this->FindItem(CONTEXTUAL_MENU_EXTERNAL_INTERNAL_VIEW)->Check(m_ViewActive->IsExternal());
	this->AppendSeparator();
	this->Append(CONTEXTUAL_MENU_SAVE_AS_IMAGE, _("Save as Image"));

  if (vc)
  {
    this->Append(CONTEXTUAL_MENU_SAVE_ALL_AS_IMAGE, _("Save All as Image"));
  }  
  
  
  this->Append(CONTEXTUAL_MENU_EXPORT_AS_VRML, _("Export Scene (VRML)"));

	int x,y;
	::wxGetMousePosition(&x, &y);
	m_ChildViewActive->ScreenToClient(&x, &y);

	m_ViewActive->GetWindow()->PopupMenu(this, wxPoint(x, y));
		
	m_ChildViewActive->Refresh();
}
//----------------------------------------------------------------------------
void mafGUIContextualMenu::OnContextualViewMenu(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
	switch(event.GetId())
	{
		case CONTEXTUAL_MENU_HIDE_VME:
		{
      if (mafViewVTK::SafeDownCast(m_ViewActive) || m_ViewActive->IsA("mafViewVTKCompound")) // added  || mafViewVTKCompound::SafeDownCast(m_ViewActive) by Losi 07/07/2010 fix bug #2190
      {
        bool pipe_created = false;
        bool mutex = false;
			  mafVME *vme = (mafVME *)((mafViewVTK *)m_ViewActive)->GetSceneGraph()->GetSelectedVme();
			  if(vme->IsMAFType(mafVMELandmark))
				  vme = vme->GetParent();

        mafSceneGraph *sg = ((mafViewVTK *)m_ViewActive)->GetSceneGraph();
        if(sg)
        {
          mafSceneNode *sn = sg->Vme2Node(vme);
          if(sn)
          {
            pipe_created = (sn->m_Pipe != NULL);
            mutex = sn->m_Mutex;
          }
        } 
        if(pipe_created && mutex) 
          m_ViewActive->VmeDeletePipe(vme);
        mafEventMacro(mafEvent(this, VME_SHOW, vme, false));
			}
		}
		break;
    case CONTEXTUAL_MENU_VME_PIPE:
    {
      mafSceneGraph *sg = m_ViewActive->GetSceneGraph();
      mafVME *vme = NULL;
      if(sg)
      {
        vme = (mafVME *)sg->GetSelectedVme();
        mafSceneNode *sn = sg->Vme2Node(vme);
        if (sn)
        {
          mafPipe *p = sn->m_Pipe;
          if (p)
          {
            wxString t = vme->GetName();
            t += " pipe gui";
            mafGUIDialog dlg(t,mafCLOSEWINDOW);
            mafGUI *gui = p->GetGui();
            wxWindow *old_parent = gui->GetParent();
            dlg.Add(gui,1,wxEXPAND);
            dlg.ShowModal();
            gui->Reparent(old_parent);
            if (old_parent == mafGetFrame())
            {
              gui->Show(false);
            }
          }
          else
          {
            wxMessageBox("Visual pipe has no gui!!", _("Warning"));
          }
        }
      }
    }
    break;
		case CONTEXTUAL_MENU_DELETE_VME:
			//send the event to simulate the cut started by the menu button.
			mafEventMacro(mafEvent(this, MENU_OP, (long)OP_CUT));
		break;
		case CONTEXTUAL_MENU_TRANSFORM:
		{
			mafString s = "Move\tCtrl+T";
			mafEventMacro(mafEvent(this, PARSE_STRING, &s));
		}
		break;
		case CONTEXTUAL_MENU_MAXIMIZE_CHILD_VIEW:
			m_ChildViewActive->Maximize(true);
		break;
    case CONTEXTUAL_MENU_MAXIMIZE_CHILD_SUB_VIEW:
    {
      mafViewCompound *vc = mafViewCompound::SafeDownCast(m_ViewActive);
      if (vc)
      {
        int subview_idx = vc->GetSubViewIndex();
        vc->MaximizeSubView(subview_idx);
      }
    }
    break;
		case CONTEXTUAL_MENU_NORMAL_SIZE_CHILD_VIEW:
			m_ChildViewActive->Maximize(false);
		break;
    case CONTEXTUAL_MENU_NORMAL_SIZE_CHILD_SUB_VIEW:
    {
      mafViewCompound *vc = mafViewCompound::SafeDownCast(m_ViewActive);
      if (vc)
      {
        vc->MaximizeSubView(0,false);
      }
    }
    break;
/*    case CONTEXTUAL_MENU_EXTERNAL_INTERNAL_VIEW:
    {
      bool ext = !this->FindItem(CONTEXTUAL_MENU_EXTERNAL_INTERNAL_VIEW)->IsChecked();
      m_ViewActive->ViewExternal(ext);
      //m_ViewActive->HideGui();
      mafEventMacro(mafEvent(this,VIEW_CREATED,m_ViewActive));
      m_ChildViewActive->Destroy();
      m_ViewActive->m_frame->Show(TRUE);
      //m_ViewActive->ShowGui();
      return;
    }
    break;*/
		case CONTEXTUAL_MENU_SAVE_AS_IMAGE:
			mafEventMacro(mafEvent(this, VIEW_SAVE_IMAGE,false));
		break;
    case CONTEXTUAL_MENU_SAVE_ALL_AS_IMAGE:
			mafEventMacro(mafEvent(this, VIEW_SAVE_IMAGE,true));
		break;
    case CONTEXTUAL_MENU_EXPORT_AS_VRML:
    {
      mafString file_dir  = mafGetApplicationDirectory().c_str();
      mafString wildc     = "VRML (*.wrl)|*.wrl";
      mafString file      = mafGetSaveFile(file_dir,wildc).c_str();
      if (!file.IsEmpty())
      {
        vtkRenderWindow *renwin = m_ViewActive->GetRWI()->GetRenderWindow();
        vtkMAFSmartPointer<vtkVRMLExporter> vrml_exporter;
        vrml_exporter->SetFileName(file);
        vrml_exporter->SetInput(renwin);
        vrml_exporter->Update();
        vrml_exporter->Write();
      }
    }
    break;
		case CONTEXTUAL_MENU_RENAME_VIEW:
		{
			wxTextEntryDialog *dlg = new wxTextEntryDialog(m_ChildViewActive,"please enter a name", "VIEW NAME", m_ViewActive->GetName());
			int result = dlg->ShowModal(); 
			wxString name = dlg->GetValue();
			cppDEL(dlg);
			if(result != wxID_OK) return;
			m_ViewActive->SetName(name);
			if(name == "")
        m_ChildViewActive->SetTitle(wxStripMenuCodes(m_ViewActive->GetLabel()));
			else
				m_ChildViewActive->SetTitle(name);
		}
		break;
	}
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
