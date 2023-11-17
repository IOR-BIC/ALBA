/*=========================================================================
Program:   AlbaViewer
Module:    appSideBar.cpp
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appSideBar.h"

#include "albaGUI.h"
#include "albaGUIDialogFindVme.h"
#include "albaGUIDockManager.h"
#include "albaGUIHolder.h"
#include "albaGUIMDIFrame.h"
#include "albaGUIPanel.h"
#include "albaGUIPanelStack.h"
#include "albaGUISplittedPanel.h"
#include "albaGUITree.h"
#include "albaGUIVMEChooser.h"
#include "albaPipe.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaVMEOutput.h"
#include "albaVMERoot.h"
#include "albaView.h"
#include "albaViewVTK.h"

//----------------------------------------------------------------------------
appSideBar::appSideBar(albaGUIMDIFrame* parent, int id, albaObserver *Listener)
{
  m_SelectedVme  = NULL;
	m_OldSelectedVme = NULL;
  m_SelectedView = NULL;
  m_CurrentVmeGui = NULL;
	m_AppendingGUI = NULL;

  m_Listener = Listener;

  //Notebook
  m_Notebook = new wxNotebook(parent,id);
  m_Notebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));
	
	//Splitted panel  
	m_SideSplittedPanel = new wxSplitterWindow(m_Notebook, -1, wxDefaultPosition, wxSize(-1, -1),/*wxSP_3DSASH |*/ wxSP_3D | wxSP_LIVE_UPDATE);

	// Tree
	m_Tree = new albaGUICheckTree(m_SideSplittedPanel, -1, false, true);
	m_Tree->SetListener(Listener);
	m_Tree->SetSize(-1, 300);
	m_Tree->SetTitle(" vme hierarchy: ");
	m_Notebook->AddPage(m_SideSplittedPanel, _("Data tree"), true);

	//
	m_VmeNotebook = new wxNotebook(m_SideSplittedPanel, -1);
	m_VmeNotebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));
	
  //View property panel
  m_ViewPropertyPanel = new albaGUIHolder(m_VmeNotebook,-1,false,true);
  m_ViewPropertyPanel->SetTitle(_("No view selected:"));
	m_VmeNotebook->AddPage(m_ViewPropertyPanel,_("View settings"));

	//VME panel
	//m_VmePanel = new albaGUIHolder(m_VmeNotebook, -1, false, true);
	//m_VmeNotebook->AddPage(m_VmePanel, _("VME"));

	m_SideSplittedPanel->SetMinimumPaneSize(50);
	m_SideSplittedPanel->SplitHorizontally(m_Tree, m_VmeNotebook);

	parent->AddDockPane(m_Notebook, wxPaneInfo()
		.Name("sidebar")
		.Caption(wxT("Control Panel"))
		.Right()
		.Layer(2)
		.MinSize(240, 450)
		.TopDockable(false)
		.BottomDockable(false)
	);
}
//----------------------------------------------------------------------------
appSideBar::~appSideBar()
{
}

//----------------------------------------------------------------------------
void appSideBar::OpShowGui(bool push_gui, albaGUIPanel *panel)
{
	m_Notebook->Show(true);
// 	if(push_gui)
// 	{
// 		m_Notebook->SetSelection(2);
// 		m_OpPanel->Push(panel);
// 	}
}
//----------------------------------------------------------------------------
void appSideBar::OpHideGui(bool view_closed)
{
	if(view_closed)
	{
		m_Notebook->SetSelection(0);
		m_ViewPropertyPanel->RemoveCurrentGui();
		m_ViewPropertyPanel->SetTitle("");
	}
	else
	{
		//m_OpPanel->Pop();
		m_Notebook->SetSelection(0);
	}
}
//----------------------------------------------------------------------------
void appSideBar::ViewSelect(albaView *view)
{
	m_Tree->ViewSelected(view);
	if (view)
	{
		wxString s = " ";
		s += wxStripMenuCodes(view->GetLabel());
		s += " props:";
		m_ViewPropertyPanel->SetTitle(s);
		albaGUI *gui = view->GetGui();
		if (gui)
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
	//UpdateVmePanel();
	//m_Notebook->SetSelection(1);
}
//----------------------------------------------------------------------------
void appSideBar::ViewDeleted(albaView *view)
{
	m_Tree->ViewDeleted(view);
  ViewSelect(NULL);
}
//----------------------------------------------------------------------------
void appSideBar::EnableSelect(bool enable)
{
	m_Tree->EnableSelect(enable);
}
//----------------------------------------------------------------------------
void appSideBar::VmeAdd(albaVME *vme)
{
	m_Tree->VmeAdd(vme);
	//UpdateVmePanel();
}
//----------------------------------------------------------------------------
void appSideBar::VmeRemove(albaVME *vme)
{
	m_Tree->VmeRemove(vme);
	//UpdateVmePanel();
}
//----------------------------------------------------------------------------
void appSideBar::VmeModified(albaVME *vme)
{
	m_Tree->VmeModified(vme);
	//UpdateVmePanel();
}
//----------------------------------------------------------------------------
void appSideBar::VmeShow(albaVME *vme, bool visibility)
{
	m_Tree->VmeShow(vme, visibility);
	//UpdateVmePanel();
}
//----------------------------------------------------------------------------
void appSideBar::VmeSelected(albaVME *vme)
{
   m_SelectedVme = vme;
//   UpdateVmePanel();

	m_Tree->VmeSelected(vme);
	m_Tree->SetFocus();
}

//----------------------------------------------------------------------------
std::vector<albaVME*> appSideBar::VmeChoose(void *vme_accept_function, long style, albaString title, bool multiSelect, albaVME *vme)
{
	albaGUIVMEChooser vc(m_Tree, title.GetCStr(), vme_accept_function, style, multiSelect, vme);
	return vc.ShowChooserDialog();

// 	albaErrorMessage("Error on choose VME");
// 	std::vector<albaVME*> emptyList;
// 	return emptyList;
}

//----------------------------------------------------------------------------
void appSideBar::FindVME()
{
	albaGUICheckTree *tree = m_Tree;
	albaGUIDialogFindVme fd(_("Find VME"));
	fd.SetTree(tree);
	fd.ShowModal();

	//albaErrorMessage("Error on find VME");
}

//----------------------------------------------------------------------------
void appSideBar::Show()
{
	m_Notebook->Show(!m_Notebook->IsShown());
}

//----------------------------------------------------------------------------
void appSideBar::UpdateVmePanel()
{ }