/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSideBar.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-22 20:02:38 $
  Version:   $Revision: 1.6 $
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


#include "mafSideBar.h"
#include "mafView.h"
#include "mmgSashPanel.h"
#include "mmgGui.h"
#include "mmgGuiHolder.h"
#include "mmgPanelStack.h"
#include "mmgPanel.h"
#include "mmgSplittedPanel.h"
#include "mmgTree.h"
#include "mafVME.h"
//----------------------------------------------------------------------------
mafSideBar::mafSideBar(wxWindow* parent, int id, mafEventListener *Listener)
//----------------------------------------------------------------------------
{
  m_selected_vme =NULL;
  m_selected_view =NULL;
  
	m_notebook = new wxNotebook(parent,-1);
  //m_notebook->SetSize();

  //splitted panel  
  mmgSplittedPanel *sp = new mmgSplittedPanel(m_notebook,-1,200);  //SIL. 29-4-2003 - 200 is the height of the vme_property panel
  m_notebook->AddPage(sp,"vme manager",true);

  //tree ----------------------------
  m_tree = new mmgCheckTree(sp,-1,true);
  m_tree->SetListener(Listener);
  m_tree->SetTitle(" vme hierarchy: ");
  sp->PutOnTop(m_tree);
  
  //vme_panel  //SIL. 22-4-2005: refactored
  mmgNamedPanel *vme_np = new mmgNamedPanel(sp,-1);
  vme_np->SetTitle("Selected Vme Property");
  wxNotebook *vme_notebook = new wxNotebook(vme_np,-1);
  m_vme_panel = new mmgGuiHolder(vme_notebook,-1,false,true);
  vme_notebook->AddPage(m_vme_panel," vme object ");
  m_vme_output_panel = new mmgGuiHolder(vme_notebook,-1,false,true);
  vme_notebook->AddPage(m_vme_output_panel," vme output ");
  m_vme_pipe_panel = new mmgGuiHolder(vme_notebook,-1,false,true);
  vme_notebook->AddPage(m_vme_pipe_panel," vme pipe ");
  vme_np->Add(vme_notebook,1,wxEXPAND);
  sp->PutOnBottom(vme_np);

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

  //m_side_bar->Put(m_notebook);
}
//----------------------------------------------------------------------------
mafSideBar::~mafSideBar() 
//----------------------------------------------------------------------------
{
	cppDEL(m_notebook);
}
//----------------------------------------------------------------------------
void mafSideBar::OpShowGui(bool push_gui, mmgPanel *panel)
//----------------------------------------------------------------------------
{
	m_notebook->Show(true);
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
	m_tree->ViewSelected(view);
	if(view)
	{
		wxString s = " ";
		s += wxStripMenuCodes(view->GetLabel());
		s += " view:";
		m_view_property_panel->SetTitle(s);
		mmgGui *gui = view->GetGui();
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
  m_selected_view = view;
  UpdateVmePanel();
}
//----------------------------------------------------------------------------
void mafSideBar::ViewDeleted(mafView *view)
//----------------------------------------------------------------------------
{
	m_tree->ViewDeleted(view);
}
//----------------------------------------------------------------------------
void mafSideBar::EnableSelect(bool enable)
//----------------------------------------------------------------------------
{
	m_tree->EnableSelect(enable);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
	m_tree->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
	m_tree->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeModified(mafNode *vme)
//----------------------------------------------------------------------------
{
	m_tree->VmeModified(vme);
}
//----------------------------------------------------------------------------
void mafSideBar::VmeShow(mafNode *vme, bool visibility)
//----------------------------------------------------------------------------
{
	m_tree->VmeShow(vme,visibility);
  UpdateVmePanel();
}
//----------------------------------------------------------------------------
void mafSideBar::VmeSelected(mafNode *vme)
//----------------------------------------------------------------------------
{
  m_selected_vme = vme;
  m_tree->VmeSelected(vme);
  UpdateVmePanel();
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
	m_notebook->Show(!m_notebook->IsShown());
}
//----------------------------------------------------------------------------
void mafSideBar::UpdateVmePanel()
//----------------------------------------------------------------------------
{
  //static mafNode       *last_vme = NULL;
  //static mafVMEOutput  *last_vme_out = NULL;
  //static mafPipe       *last_vme_pipe = NULL;
  static mmgGui *last_vme_gui = NULL;
  static mmgGui *last_vme_out_gui = NULL;
  static mmgGui *last_vme_pipe_gui = NULL;

  mafVMEOutput *vme_out = NULL;
  mafPipe      *vme_pipe = NULL;
  mmgGui       *vme_gui = NULL;
  mmgGui       *vme_out_gui = NULL;
  mmgGui       *vme_pipe_gui = NULL;

  if(m_selected_vme)
  {
    vme_gui = m_selected_vme->GetGui();

    if(m_selected_vme->IsA("mafVME"))
    {
      mafVME *v = (mafVME*) m_selected_vme;
      vme_out = v->GetOutput();
      if(vme_out)
        vme_out_gui = vme_out->GetGui();
    }

    if(m_selected_view)
    {
      vme_pipe = m_selected_view->GetNodePipe(m_selected_vme);
      if(vme_pipe)
        vme_pipe_gui = vme_pipe->GetGui();
    }
  }

  // vme_gui changed 
  if( vme_gui != last_vme_gui ) 
  {
    m_vme_panel->Put(vme_gui);
    
    //if(last_vme_gui && last_vme)
       //last_vme->DeleteGui(); // - what if last_vme was destroyed ?
  }
  
  // vme_out changed 
  if( vme_out_gui != last_vme_out_gui ) 
  {
    m_vme_output_panel->Put(vme_out_gui);

    //if(last_vme_out_gui && last_vme_out)
      // last_vme_out->DeleteGui(); // - what if last_vme_out was destroyed ?
  }

  // vme_pipe changed 
  if( vme_pipe_gui != last_vme_pipe_gui ) 
  {
    m_vme_pipe_panel->Put(vme_pipe_gui);

    //if(last_vme_pipe_gui && last_vme_pipe)
      // last_vme_pipe->DeleteGui(); // - what if last_vme_pipe was destroyed ?
  }

  //last_vme = m_selected_vme;
  //last_vme_out = vme_out;
  //last_vme_pipe = vme_pipe;
  last_vme_gui = vme_gui;
  last_vme_out_gui = vme_out_gui;
  last_vme_pipe_gui = vme_pipe_gui;
}
