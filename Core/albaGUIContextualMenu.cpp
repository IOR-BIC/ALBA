/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIContextualMenu
 Authors: Paolo Quadrani
 
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


#include "albaGUIContextualMenu.h"
#include "wx/utils.h"
#include <wx/laywin.h>
#include <wx/mdi.h>

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaSceneNode.h"
#include "albaSceneGraph.h"
#include "albaView.h"
#include "albaViewVTK.h"
#include "albaViewCompound.h"
#include "albaGUIMDIChild.h"
#include "albaGUIDialog.h"
#include "albaGUI.h"
#include "albaPipe.h"

#include "albaPipe.h"

#include "albaVME.h"
#include "albaVMELandmark.h"

#include "vtkALBASmartPointer.h"
#include "vtkVRMLExporter.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "albaViewSlice.h"
#include "albaViewArbitraryOrthoSlice.h"
#include "albaViewArbitrarySlice.h"
#include "albaVMEImage.h"
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
    CONTEXTUAL_MENU_EXTRACT_SLICE,
		CONTEXTUAL_MENU_SAVE_AS_IMAGE,
    CONTEXTUAL_MENU_SAVE_ALL_AS_IMAGE,
    CONTEXTUAL_MENU_EXPORT_AS_VRML,
	CONTEXTUAL_VIEW_MENU_STOP
};

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIContextualMenu,wxMenu)
  EVT_MENU_RANGE(CONTEXTUAL_VIEW_MENU_START, CONTEXTUAL_VIEW_MENU_STOP, albaGUIContextualMenu::OnContextualViewMenu)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUIContextualMenu::albaGUIContextualMenu()
:wxMenu()
//----------------------------------------------------------------------------
{
	m_ChildViewActive = NULL;
	m_ViewActive      = NULL;
  m_Listener        = NULL;
}
//----------------------------------------------------------------------------
albaGUIContextualMenu::~albaGUIContextualMenu()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIContextualMenu::ShowContextualMenu(wxFrame *child, albaView *view, bool vme_menu)
//----------------------------------------------------------------------------
{
	m_ChildViewActive = child;
	m_ViewActive      = view;

	albaViewCompound *viewCopound = albaViewCompound::SafeDownCast(m_ViewActive);
  albaViewSlice *viewSlice = albaViewSlice::SafeDownCast(viewCopound ? viewCopound->GetSubView() : m_ViewActive);

  bool opRunning = GetLogicManager()->IsOperationRunning();

	m_ChildViewActive->SetFocus();

	if(vme_menu)
	{
		this->Append(CONTEXTUAL_MENU_HIDE_VME, "Hide");
		this->Append(CONTEXTUAL_MENU_DELETE_VME, "Delete");
		this->Append(CONTEXTUAL_MENU_TRANSFORM, "Transform  \tCtrl+T");
    this->AppendSeparator();
    this->Append(CONTEXTUAL_MENU_VME_PIPE,"Visual Props");
    this->AppendSeparator();
  }
	this->Append(CONTEXTUAL_MENU_RENAME_VIEW, "Rename View");
	this->AppendSeparator();
  if (m_ChildViewActive->IsMaximized())
  {
    this->Append(CONTEXTUAL_MENU_NORMAL_SIZE_CHILD_VIEW, "Normal Size");
  }
  else
  {
    this->Append(CONTEXTUAL_MENU_MAXIMIZE_CHILD_VIEW, "Maximize");
  }
  if (viewCopound)
  {
    if (viewCopound->IsSubViewMaximized())
    {
      this->Append(CONTEXTUAL_MENU_NORMAL_SIZE_CHILD_SUB_VIEW, "Normal Size SubView");
    }
    else
    {
      this->Append(CONTEXTUAL_MENU_MAXIMIZE_CHILD_SUB_VIEW, "Maximize SubView");
    }
  }

  if (viewSlice && !opRunning && viewSlice->IsSliceExtractionEnabled() && viewSlice->IsVolumeVisible())
  {
    this->AppendSeparator();
    this->Append(CONTEXTUAL_MENU_EXTRACT_SLICE, "Extract Slice");
  }

  this->AppendSeparator();
	this->Append(CONTEXTUAL_MENU_SAVE_AS_IMAGE, _("Save as Image"));

  if (viewCopound)
  {
    this->Append(CONTEXTUAL_MENU_SAVE_ALL_AS_IMAGE, _("Save All as Image"));
  }  
  
  
  this->Append(CONTEXTUAL_MENU_EXPORT_AS_VRML, _("Export as VRML"));

	int x,y;
	::wxGetMousePosition(&x, &y);
	m_ChildViewActive->ScreenToClient(&x, &y);

	m_ViewActive->GetWindow()->PopupMenu(this, wxPoint(x, y));
		
	m_ChildViewActive->Refresh();
}
//----------------------------------------------------------------------------
void albaGUIContextualMenu::OnContextualViewMenu(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
	switch(event.GetId())
	{
		case CONTEXTUAL_MENU_HIDE_VME:
		{
      if (albaViewVTK::SafeDownCast(m_ViewActive))
      {
        bool pipe_created = false;
        bool mutex = false;
			  albaVME *vme = ((albaViewVTK *)m_ViewActive)->GetSceneGraph()->GetSelectedVme();
			  if(vme->IsALBAType(albaVMELandmark))
				  vme = vme->GetParent();

        albaSceneGraph *sg = ((albaViewVTK *)m_ViewActive)->GetSceneGraph();
        if(sg)
        {
          albaSceneNode *sn = sg->Vme2Node(vme);
          if(sn)
          {
            pipe_created = (sn->IsVisible());
            mutex = sn->GetMutex();
          }
        } 
        if(pipe_created && mutex) 
          m_ViewActive->VmeDeletePipe(vme);
        GetLogicManager()->VmeShow(vme, false);
			}
		}
		break;
    case CONTEXTUAL_MENU_VME_PIPE:
    {
      albaSceneGraph *sg = m_ViewActive->GetSceneGraph();
      albaVME *vme = NULL;
      if(sg)
      {
        vme = sg->GetSelectedVme();
        albaSceneNode *sn = sg->Vme2Node(vme);
        if (sn)
        {
          albaPipe *p = sn->GetPipe();
          if (p)
          {
            wxString t = vme->GetName();
            t += " pipe gui";
            albaGUIDialog dlg(t,albaCLOSEWINDOW);
            albaGUI *gui = p->GetGui();
            wxWindow *old_parent = gui->GetParent();
            dlg.Add(gui,1,wxEXPAND);
            dlg.ShowModal();
            gui->Reparent(old_parent);
            if (old_parent == albaGetFrame())
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
			albaEventMacro(albaEvent(this, MENU_OP, (long)OP_CUT));
		break;
		case CONTEXTUAL_MENU_TRANSFORM:
		{
			albaString s = "Transform  \tCtrl+T";
			albaEventMacro(albaEvent(this, PARSE_STRING, &s));
		}
		break;
		case CONTEXTUAL_MENU_MAXIMIZE_CHILD_VIEW:
			m_ChildViewActive->Maximize(true);
		break;
    case CONTEXTUAL_MENU_MAXIMIZE_CHILD_SUB_VIEW:
    {
      albaViewCompound *vc = albaViewCompound::SafeDownCast(m_ViewActive);
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
      albaViewCompound *vc = albaViewCompound::SafeDownCast(m_ViewActive);
      if (vc)
      {
        vc->MaximizeSubView(0,false);
      }
    }
    break;
    case CONTEXTUAL_MENU_EXTRACT_SLICE:
    {
      albaViewCompound *viewCopound = albaViewCompound::SafeDownCast(m_ViewActive);
			albaViewSlice *viewSlice = albaViewSlice::SafeDownCast(viewCopound ? viewCopound->GetSubView() : m_ViewActive);
			albaVMEImage *image = viewSlice->ExtractSliceImage();

			if (viewCopound->IsA("albaViewArbitrarySlice"))
			{
				((albaViewArbitrarySlice *)viewCopound)->StoreViewParamsToVMETags(image);
			}
			else if (viewCopound->IsA("albaViewArbitraryOrthoSlice"))
			{
				((albaViewArbitraryOrthoSlice *)viewCopound)->StoreViewParamsToVMETags(image);
			}
		}
    break;
    case CONTEXTUAL_MENU_SAVE_AS_IMAGE:
    {
      albaView *view = m_ViewActive->IsA("albaViewCompound") ? ((albaViewCompound *)m_ViewActive)->GetSubView() : m_ViewActive;
      view->GetRWI()->SaveImage(view->GetLabel());
    }
		break;
    case CONTEXTUAL_MENU_SAVE_ALL_AS_IMAGE:
      m_ViewActive->GetRWI()->SaveAllImages(m_ViewActive->GetLabel(),albaViewCompound::SafeDownCast(m_ViewActive));
			break;
    case CONTEXTUAL_MENU_EXPORT_AS_VRML:
    {
			albaString file_dir  = albaGetLastUserFolder();
			albaString wildc     = "VRML (*.wrl)|*.wrl";
			albaString file      = albaGetSaveFile(file_dir,wildc);
			if (!file.IsEmpty())
			{
				vtkRenderWindow *renwin = m_ViewActive->GetRWI()->GetRenderWindow();
				albaSceneGraph * sceneGraph = 	m_ViewActive->GetSceneGraph();

				if(sceneGraph->m_RenBack)
				{
					renwin->RemoveRenderer(sceneGraph->m_AlwaysVisibleRenderer);
				}

				renwin->RemoveRenderer(sceneGraph->m_AlwaysVisibleRenderer);
				
				vtkALBASmartPointer<vtkVRMLExporter> vrml_exporter;
				vrml_exporter->SetFileName(file);
				vrml_exporter->SetInput(renwin);
				vrml_exporter->Update();
				vrml_exporter->Write();


				if(sceneGraph->m_RenBack)
				{
					renwin->AddRenderer(sceneGraph->m_RenBack);
				}

				renwin->AddRenderer(sceneGraph->m_AlwaysVisibleRenderer);
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
	GetLogicManager()->CameraUpdate();
}
