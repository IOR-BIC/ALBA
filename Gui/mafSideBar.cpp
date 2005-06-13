/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSideBar.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-13 12:31:20 $
  Version:   $Revision: 1.15 $
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
#include "mafVMEOutput.h"

//----------------------------------------------------------------------------
mafSideBar::mafSideBar(wxWindow* parent, int id, mafObserver *Listener)
//----------------------------------------------------------------------------
{
  m_SelectedVme  = NULL;
  m_SelectedView = NULL;
  
  //splitted panel  
  m_SideSplittedPanel = new mmgSplittedPanel(parent,-1,200); //SIL. 29-4-2003 - 200 is the height of the vme_property panel
  m_Notebook = new wxNotebook(m_SideSplittedPanel,-1);

  //tree ----------------------------
  m_Tree = new mmgCheckTree(m_Notebook,-1,true);
  m_Tree->SetListener(Listener);
  m_Tree->SetTitle(" vme hierarchy: ");
  m_Notebook->AddPage(m_Tree,"vme manager",true);
  m_SideSplittedPanel->PutOnTop(m_Notebook);

  //view property panel
	m_ViewPropertyPanel = new mmgGuiHolder(m_Notebook,-1,true);
  m_ViewPropertyPanel->SetTitle(" no view selected:");
	m_Notebook->AddPage(m_ViewPropertyPanel,"view settings");
	
	//op_panel ----------------------------
  m_OpPanel  = new mmgPanelStack(m_Notebook ,-1);
	mmgNamedPanel *empty_op = new mmgNamedPanel(m_OpPanel ,-1,true);
  empty_op->SetTitle(" no operation running:");
  m_OpPanel->Push(empty_op);
	m_Notebook->AddPage(m_OpPanel ," op. parameters");
  
  
  // wxFrame is needed to avoid endless loop in wxNotebook when
  // a button inside it is pressed and a different notebook tab is pressed suddenly after!! 
  // (mmgNamedPanel doesn't block the problem)
  wxFrame *vme_property_frame = new wxFrame(m_SideSplittedPanel,-1,"Vme Property",wxDefaultPosition,wxDefaultSize,wxNO_BORDER);
  wxNotebook *vme_notebook = new wxNotebook(vme_property_frame,-1);

  m_VmeOutputPanel = new mmgGuiHolder(vme_notebook,-1,false,true);
  vme_notebook->AddPage(m_VmeOutputPanel," vme output ");
  m_VmePipePanel = new mmgGuiHolder(vme_notebook,-1,false,true);
  vme_notebook->AddPage(m_VmePipePanel," vme pipe ");
  m_VmePanel = new mmgGuiHolder(vme_notebook,-1,false,true);
  vme_notebook->AddPage(m_VmePanel," vme object ");

  m_SideSplittedPanel->PutOnBottom(vme_property_frame);

  //m_side_bar->Put(m_Notebook);
}
//----------------------------------------------------------------------------
mafSideBar::~mafSideBar() 
//----------------------------------------------------------------------------
{
	cppDEL(m_Notebook);
}
//----------------------------------------------------------------------------
void mafSideBar::OpShowGui(bool push_gui, mmgPanel *panel)
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
		s += " view:";
		m_ViewPropertyPanel->SetTitle(s);
		mmgGui *gui = view->GetGui();
		if(gui) 
			m_ViewPropertyPanel->Put(gui);
		else
			m_ViewPropertyPanel->RemoveCurrentGui();
	}
	else
	{
		m_ViewPropertyPanel->SetTitle("");
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
  m_SelectedView = NULL; // Paolo 2005-04-23 m_SelectedView is initialized in ViewSelect
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
    UpdateVmePanel();
    m_SelectedVme = NULL;
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
  m_Tree->VmeSelected(vme);
  UpdateVmePanel();
}
//----------------------------------------------------------------------------
void mafSideBar::VmePropertyRemove(mmgGui *gui)
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
  mmgGui       *vme_gui = NULL;
  mmgGui       *vme_out_gui = NULL;
  mmgGui       *vme_pipe_gui = NULL;

  if(m_SelectedVme)
  {
    vme_gui = m_SelectedVme->GetGui();

    if(m_SelectedVme->IsMAFType(mafVME))
    {
      mafVME *v = (mafVME*) m_SelectedVme;
      vme_out = v->GetOutput();
      if(!vme_out->IsA("mafVMEOutputNULL")) // Paolo 2005-05-05
        vme_out_gui = vme_out->GetGui();
      else
        vme_out = NULL;
    }

    if(m_SelectedView)
    {
      vme_pipe = m_SelectedView->GetNodePipe(m_SelectedVme);
      if(vme_pipe)
        vme_pipe_gui = vme_pipe->GetGui();
    }
  }

  m_VmePanel->Put(vme_gui);
  m_VmeOutputPanel->Put(vme_out_gui);
  m_VmePipePanel->Put(vme_pipe_gui);

  /* code stub to future support of dynamic creation/destruction of GUI
  // vme_gui changed 
  if( vme_gui != m_VmePanel->GetCurrentGui() ) 
  {
    m_VmePanel->Put(vme_gui);
    
    //if(last_vme_gui && last_vme)
       //last_vme->DeleteGui(); // - what if last_vme was destroyed ?
  }
  
  // vme_out changed 
  if( vme_out_gui != m_VmeOutputPanel->GetCurrentGui() ) 
  {
    m_VmeOutputPanel->Put(vme_out_gui);

    //if(last_vme_out_gui && last_vme_out)
      // last_vme_out->DeleteGui(); // - what if last_vme_out was destroyed ?
  }

  // vme_pipe changed 
  if( vme_pipe_gui != m_VmePipePanel->GetCurrentGui() ) 
  {
    m_VmePipePanel->Put(vme_pipe_gui);

    //if(last_vme_pipe_gui && last_vme_pipe)
      // last_vme_pipe->DeleteGui(); // - what if last_vme_pipe was destroyed ?
  }*/
}
