/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgContextualMenu.cpp,v $
  Language:  C++
  Date:      $Date: 2006-11-16 13:46:21 $
  Version:   $Revision: 1.9 $
  Authors:   Paolo Quadrani    
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


#include "mmgContextualMenu.h"
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
#include "mmgMDIChild.h"
#include "mmgDialog.h"
#include "mmgGui.h"
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
    CONTEXTUAL_MENU_EXPORT_AS_VRML,
    //CONTEXTUAL_MENU_EXTERNAL_INTERNAL_VIEW,
	CONTEXTUAL_VIEW_MENU_STOP
};

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgContextualMenu,wxMenu)
  EVT_MENU_RANGE(CONTEXTUAL_VIEW_MENU_START, CONTEXTUAL_VIEW_MENU_STOP, mmgContextualMenu::OnContextualViewMenu)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgContextualMenu::mmgContextualMenu()
:wxMenu()
//----------------------------------------------------------------------------
{
	m_ChildViewActive = NULL;
	m_ViewActive      = NULL;
  m_Listener        = NULL;
}
//----------------------------------------------------------------------------
mmgContextualMenu::~mmgContextualMenu()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgContextualMenu::ShowContextualMenu(wxFrame *child, mafView *view, bool vme_menu)
//----------------------------------------------------------------------------
{
	m_ChildViewActive = child;
	m_ViewActive      = view;

	m_ChildViewActive->SetFocus();

	if(vme_menu)
	{
		this->Append(CONTEXTUAL_MENU_HIDE_VME, "Hide");
		this->Append(CONTEXTUAL_MENU_DELETE_VME, "Delete");
		this->Append(CONTEXTUAL_MENU_TRANSFORM, "Transform  \tCtrl+T");
   // this->AppendSeparator();
    //this->Append(CONTEXTUAL_MENU_VME_PIPE,"visual props");
    this->AppendSeparator();
  }
	this->Append(CONTEXTUAL_MENU_RENAME_VIEW, "Rename View");
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
  mafViewCompound *vc = mafViewCompound::SafeDownCast(m_ViewActive);
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
	this->Append(CONTEXTUAL_MENU_SAVE_AS_IMAGE, "Save as Image");
  this->Append(CONTEXTUAL_MENU_EXPORT_AS_VRML, "Export as VRML");

	int x,y;
	::wxGetMousePosition(&x, &y);
	m_ChildViewActive->ScreenToClient(&x, &y);

	m_ViewActive->GetWindow()->PopupMenu(this, wxPoint(x, y));
		
	m_ChildViewActive->Refresh();
}
//----------------------------------------------------------------------------
void mmgContextualMenu::OnContextualViewMenu(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
	switch(event.GetId())
	{
		case CONTEXTUAL_MENU_HIDE_VME:
		{
      if (mafViewVTK::SafeDownCast(m_ViewActive))
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
            mmgDialog dlg(t,mafCLOSEWINDOW);
            mmgGui *gui = p->GetGui();
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
			mafString s = "Transform  \tCtrl+T";
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
			mafEventMacro(mafEvent(this, VIEW_SAVE_IMAGE));
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
