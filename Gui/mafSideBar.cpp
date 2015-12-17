/*=========================================================================

 Program: MAF2
 Module: mafSideBar
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

#include "mafSideBar.h"
#include "mafView.h"
#include "mafViewVTK.h"
#include "mafPipe.h"
#include "mafGUI.h"
#include "mafGUIHolder.h"
#include "mafGUIPanelStack.h"
#include "mafGUIPanel.h"
#include "mafGUISplittedPanel.h"
#include "mafGUITree.h"

#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafVMERoot.h"

//----------------------------------------------------------------------------
mafSideBar::mafSideBar(wxWindow* parent, int id, mafObserver *Listener, long style)
//----------------------------------------------------------------------------
{
  m_SelectedVme  = NULL;
  m_SelectedView = NULL;
  m_CurrentVmeGui = NULL;
  m_CurrentPipeGui = NULL;
	m_AppendingGUI = NULL;
  m_Listener = Listener;
  m_Style = style;

  //splitted panel  
  m_Notebook = new wxNotebook(parent,id);
  m_Notebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));
	if(style == DOUBLE_NOTEBOOK)
	{
		m_SideSplittedPanel = new wxSplitterWindow(m_Notebook, -1, wxDefaultPosition, wxSize(-1,-1),/*wxSP_3DSASH |*/ wxSP_FULLSASH);
		m_Tree = new mafGUICheckTree(m_SideSplittedPanel,-1,false,true);
	}
	else
	{
  	m_Tree = new mafGUICheckTree(m_Notebook,-1,false,true);
	}

  m_Tree->SetListener(Listener);
  m_Tree->SetSize(-1,300);
  m_Tree->SetTitle(" vme hierarchy: ");
	if(style == DOUBLE_NOTEBOOK)
		m_Notebook->AddPage(m_SideSplittedPanel,_("Data tree"),true);
	else 
		m_Notebook->AddPage(m_Tree,_("Data tree"),true);

  //view property panel
  m_ViewPropertyPanel = new mafGUIHolder(m_Notebook,-1,false,true);
  m_ViewPropertyPanel->SetTitle(_("No view selected:"));
  m_Notebook->AddPage(m_ViewPropertyPanel,_("View settings"));

  //op_panel ----------------------------
  m_OpPanel  = new mafGUIPanelStack(m_Notebook ,-1);
  mafGUINamedPanel *empty_op = new mafGUINamedPanel(m_OpPanel ,-1,false,true);
  empty_op->SetTitle(_(" No operation running:"));
  m_OpPanel->Push(empty_op);
  m_Notebook->AddPage(m_OpPanel ,_("Operation"));

  if (style == DOUBLE_NOTEBOOK)
  {
    m_VmeNotebook = new wxNotebook(m_SideSplittedPanel,-1);
    m_VmeNotebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));

    m_VmePipePanel = new mafGUIHolder(m_VmeNotebook,-1,false,true);
    m_VmeNotebook->AddPage(m_VmePipePanel,_(" Visual props "));
    m_VmePanel = new mafGUIHolder(m_VmeNotebook,-1,false,true);
    m_VmeNotebook->AddPage(m_VmePanel,_("VME"));

    m_SideSplittedPanel->SetMinimumPaneSize(50);
    m_SideSplittedPanel->SplitHorizontally(m_Tree,m_VmeNotebook);
  }
  else
  {
    m_VmePanel = new mafGUIHolder(m_Notebook,-1,false,true);
    m_Notebook->AddPage(m_VmePanel ,_("VME"));
  }
}
//----------------------------------------------------------------------------
mafSideBar::~mafSideBar() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafSideBar::OpShowGui(bool push_gui, mafGUIPanel *panel)
//----------------------------------------------------------------------------
{
	m_Notebook->Show(true);
	if(push_gui)
	{
		m_Notebook->SetSelection(2);
		m_OpPanel->Push(panel);
	}
}
//----------------------------------------------------------------------------
void mafSideBar::OpHideGui(bool view_closed)
//----------------------------------------------------------------------------
{
	if(view_closed)
	{
		this->m_Notebook->SetSelection(0);
		this->m_ViewPropertyPanel->RemoveCurrentGui();
		this->m_ViewPropertyPanel->SetTitle("");
	}
	else
	{
		m_OpPanel->Pop();
		m_Notebook->SetSelection(0);
	}
}
//----------------------------------------------------------------------------
void mafSideBar::ViewSelect(mafView *view)
//----------------------------------------------------------------------------
{
	m_Tree->ViewSelected(view);
	if(view)
	{
		wxString s = " ";
		s += wxStripMenuCodes(view->GetLabel());
		s += " props:";
		m_ViewPropertyPanel->SetTitle(s);
		mafGUI *gui = view->GetGui();
		if(gui) 
			m_ViewPropertyPanel->Put(gui);
		else
			m_ViewPropertyPanel->RemoveCurrentGui();
	}
	else
	{
    m_ViewPropertyPanel->SetTitle(_("No view selected:"));
		m_ViewPropertyPanel->RemoveCurrentGui();
	}
  m_SelectedView = view;
  UpdateVmePanel();
}
//----------------------------------------------------------------------------
void mafSideBar::ViewDeleted(mafView *view)
//----------------------------------------------------------------------------
{
	m_Tree->ViewDeleted(view);
  ViewSelect(NULL);
}
//----------------------------------------------------------------------------
void mafSideBar::EnableSelect(bool enable)
//----------------------------------------------------------------------------
{
	m_Tree->EnableSelect(enable);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
	m_Tree->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
	m_Tree->VmeRemove(vme);
  if (vme == m_SelectedVme)
  {
    m_SelectedVme = NULL;
    UpdateVmePanel();
  }  
}
//----------------------------------------------------------------------------
void mafSideBar::VmeModified(mafNode *vme)
//----------------------------------------------------------------------------
{
	m_Tree->VmeModified(vme);
  UpdateVmePanel();
}
//----------------------------------------------------------------------------
void mafSideBar::VmeShow(mafNode *vme, bool visibility)
//----------------------------------------------------------------------------
{
	m_Tree->VmeShow(vme,visibility);
  UpdateVmePanel();
}
//----------------------------------------------------------------------------
void mafSideBar::VmeSelected(mafNode *vme)
//----------------------------------------------------------------------------
{
  m_SelectedVme = vme;
  UpdateVmePanel();
  m_Tree->VmeSelected(vme);
}
//----------------------------------------------------------------------------
void mafSideBar::VmePropertyRemove(mafGUI *gui)
//----------------------------------------------------------------------------
{
	m_VmePanel->Remove(gui);
}
//----------------------------------------------------------------------------
void mafSideBar::Show()
//----------------------------------------------------------------------------
{
	m_Notebook->Show(!m_Notebook->IsShown());
}
//----------------------------------------------------------------------------
void mafSideBar::UpdateVmePanel()
//----------------------------------------------------------------------------
{
  mafVMEOutput *vme_out = NULL;
  mafPipe      *vme_pipe = NULL;
  mafGUI       *vme_gui = NULL;
  mafGUI       *vme_out_gui = NULL;
  mafGUI       *vme_pipe_gui = NULL;
	if(m_AppendingGUI && m_CurrentVmeGui)
		m_AppendingGUI->Remove(m_CurrentVmeGui);
	m_AppendingGUI = NULL;
  
	if(m_SelectedVme)
	{
	  vme_gui = m_SelectedVme->GetGui();
	
	  if(m_SelectedVme->IsMAFType(mafVME))
	  {
	    mafVME *v = (mafVME*) m_SelectedVme;
	    vme_out = v->GetOutput();
	    if(!vme_out->IsA("mafVMEOutputNULL")) // Paolo 2005-05-05
	    {
	      vme_out_gui = vme_out->GetGui();
	      if (!v->IsDataAvailable())
	        vme_out->Update();
	    }
	    else
	      vme_out = NULL;
	  }
	
	
	  if(m_SelectedView)
	  {
	    vme_pipe = m_SelectedView->GetNodePipe(m_SelectedVme);
	
	    if(vme_pipe)
	      vme_pipe_gui = vme_pipe->GetGui();
	  }
	
	  m_CurrentPipeGui = vme_pipe_gui;
	  m_CurrentVmeGui = vme_gui;
	
	  
	  m_AppendingGUI = new mafGUI(NULL);
	
	  if (vme_gui)
	  {
			m_AppendingGUI->Label("test");
	    m_AppendingGUI->AddGui(vme_gui);
	  }

	  if (m_Style== SINGLE_NOTEBOOK && vme_pipe_gui)
	  {
	    m_AppendingGUI->Label(_("GUI Visual Pipes"),true);
	    m_AppendingGUI->AddGui(vme_pipe_gui);
	  }
		else 

		if (vme_out_gui)
		{
			m_AppendingGUI->Label(_("Output"),true);
			m_AppendingGUI->AddGui(vme_out_gui);
		}

		m_AppendingGUI->FitGui();
		m_AppendingGUI->Update();
  }

	if (m_Style== DOUBLE_NOTEBOOK)
	{
		m_VmePipePanel->Put(vme_pipe_gui);
	}
	m_VmePanel->Put(m_AppendingGUI);
}
