/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSideBar.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:01:22 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafSideBar.h"

#include "mafView.h"
#include "mmgSashPanel.h"
#include "mmgGui.h"
#include "mmgGuiHolder.h"
#include "mmgPanelStack.h"
#include "mmgPanel.h"
#include "mmgSplittedPanel.h"
#include "mmgTree.h"

//----------------------------------------------------------------------------
mafSideBar::mafSideBar(wxWindow* parent, int id, mafEventListener *Listener)
//----------------------------------------------------------------------------
{
	m_side_bar = new mmgSashPanel(parent, id, wxRIGHT,245, "Side Bar \tCtrl+S"); // 245 is the width of the sideBar
  
	m_notebook = new wxNotebook(m_side_bar,-1);

  //splitted panel  
  mmgSplittedPanel *sp = new mmgSplittedPanel(m_notebook,-1,140);  //SIL. 29-4-2003 - 140 is the height of the vme_property panel
  m_notebook->AddPage(sp,"vme manager",true);

  //tree ----------------------------
  m_tree = new mmgTree(sp,-1,true);
  m_tree->SetListener(Listener);
  m_tree->SetTitle(" vme hierarchy: ");
  sp->PutOnTop(m_tree);
  
  //vme_panel
  m_vme_panel = new mmgGuiHolder(sp,-1);
  m_vme_panel->SetTitle(" vme property:");
  sp->PutOnBottom(m_vme_panel);

	//view property panel
	m_view_property_panel = new mmgGuiHolder(m_notebook,-1,true);
  m_view_property_panel->SetTitle(" no view selected:");
	m_notebook->AddPage(m_view_property_panel,"view settings");
	
	//op_panel ----------------------------
  m_op_panel  = new mmgPanelStack(m_notebook ,-1);
	mmgNamedPanel *empty_op = new mmgNamedPanel(m_op_panel ,-1,true);
  empty_op->SetTitle(" no operation running:");
  m_op_panel->Push(empty_op);
	m_notebook->AddPage(m_op_panel ," op. parameters");

  m_side_bar->Put(m_notebook);
}
//----------------------------------------------------------------------------
mafSideBar::~mafSideBar() 
//----------------------------------------------------------------------------
{
	cppDEL(m_side_bar);
/* //SIL. 13-11-2003: - objects die by themself because are children of m_side_bar
	wxDEL(m_notebook);
	wxDEL(m_vme_panel);
	wxDEL(m_view_property_panel);
	wxDEL(m_op_panel);
*/
}
//----------------------------------------------------------------------------
void mafSideBar::OpShowGui(bool push_gui, mmgPanel *panel)
//----------------------------------------------------------------------------
{
	m_side_bar->Show(true);
	if(push_gui)
	{
		m_notebook->SetSelection(2);
		m_op_panel->Push(panel);
	}
}
//----------------------------------------------------------------------------
void mafSideBar::OpHideGui(bool view_closed)
//----------------------------------------------------------------------------
{
	if(view_closed)
	{
		this->m_notebook->SetSelection(0);
		this->m_view_property_panel->Remove();
		this->m_view_property_panel->SetTitle("");
	}
	else
	{
		m_op_panel->Pop();
		m_notebook->SetSelection(0);
	}
}
//----------------------------------------------------------------------------
void mafSideBar::ViewSelect(mafView *view)
//----------------------------------------------------------------------------
{
	//tree->ViewSelected(view);
	if(view)
	{
		wxString s = " ";
		s += wxStripMenuCodes(view->m_label);
		s += " view:";
		m_view_property_panel->SetTitle(s);
		mmgGui *gui = (mmgGui*)view->m_gui;
		if(gui) 
			m_view_property_panel->Put(gui);
		else
			m_view_property_panel->Remove();
	}
	else
	{
		m_view_property_panel->SetTitle("");
		m_view_property_panel->Remove();
	}
}
//----------------------------------------------------------------------------
void mafSideBar::ViewDeleted(mafView *view)
//----------------------------------------------------------------------------
{
	//m_tree->ViewDeleted(view);
}
//----------------------------------------------------------------------------
void mafSideBar::EnableSelect(bool enable)
//----------------------------------------------------------------------------
{
	//m_tree->EnableSelect(enable);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
	//m_tree->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
	//m_tree->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeModified(mafNode *vme)
//----------------------------------------------------------------------------
{
	//m_tree->VmeModified(vme);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeShow(mafNode *vme, bool visibility)
//----------------------------------------------------------------------------
{
	//m_tree->VmeShow(vme,visibility);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeSelected(mafNode *vme)
//----------------------------------------------------------------------------
{
  //m_tree->VmeSelected(vme);
  //mafVmeData* vd = (mafVmeData*)vme->GetClientData();
	//vd->UpdateMaterialIcon();
	//m_vme_panel->Put(vd->m_gui);
}
//----------------------------------------------------------------------------
void mafSideBar::VmePropertyRemove(mmgGui *gui)
//----------------------------------------------------------------------------
{
	m_vme_panel->Remove(gui);
}
//----------------------------------------------------------------------------
void mafSideBar::Show()
//----------------------------------------------------------------------------
{
	m_side_bar->Show(!m_side_bar->IsShown());
}
