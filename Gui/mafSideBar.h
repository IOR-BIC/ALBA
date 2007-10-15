/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSideBar.h,v $
  Language:  C++
  Date:      $Date: 2007-10-15 10:12:54 $
  Version:   $Revision: 1.12 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafSideBar_H__
#define __mafSideBar_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/utils.h>

#include "mafEvent.h"
#include "mafObserver.h"
#include "mmgCheckTree.h"
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgGuiHolder;
class mmgGui;
class mmgPanelStack;
class mmgPanel;
class mafNode;
class mafView;
class mmgNamedPanel;
class mmgSplittedPanel;
//----------------------------------------------------------------------------
// mafSideBar :
//----------------------------------------------------------------------------
/**
*/
class mafSideBar : mafObserver
{
public:
	mafSideBar(wxWindow* parent, int id, mafObserver *Listener, long style = DOUBLE_NOTEBOOK);
	~mafSideBar(); 

  enum SIDEBAR_STYLE
  {
    SINGLE_NOTEBOOK,
    DOUBLE_NOTEBOOK
  };

	void OnEvent(mafEventBase *ev);

  // Description:
	// Add a new vme into the tree.
	void VmeAdd(mafNode *vme);

	// Description:
	// Remove a vme from the tree.
	void VmeRemove(mafNode *vme);

	// Description:
	// Notify to the tree that a vme is modified.
	void VmeModified(mafNode *vme);

	// Description:
	// Notify to the tree the visibility of the vme.
	void VmeShow(mafNode *vme, bool visibility);

	// Description:
	// Notify to the tree that the vme has been selected.
	void VmeSelected(mafNode *vme);

	// Description:
	// Remove the vme property gui from the panel.
	void VmePropertyRemove(mmgGui *gui);

	// Description:
	// Show the operation's parameters gui on the tabbed panel.
	void OpShowGui(bool push_gui, mmgPanel *panel);

	// Description:
	// Hide the view/operation's gui from the tabbed panel.
	void OpHideGui(bool view_closed);

	// Description:
	// Plug the view settings on the tabbed panel.
	void ViewSelect(mafView *view);

	// Description:
	// Notify to the tree that a view has been deleted.
	void ViewDeleted(mafView *view);

	// Description:
	// Enable/Disable the vme selection when an operation ends/start.
	void EnableSelect(bool enable);

	// Description:
	// Switch the visibility of the panel.
	void Show();

	// Description:
	// Return the pointer to the tree.
	mmgCheckTree *GetTree() {return m_Tree;};

  bool IsShown() {return m_Notebook->IsShown();};
  void Show( bool show) {m_Notebook->Show(show);};

  wxNotebook        *m_Notebook;
  //mmgNamedPanel    *m_SideSplittedPanel;
  wxSplitterWindow *m_SideSplittedPanel;
protected:
  void UpdateVmePanel();  

  mmgPanelStack	*m_OpPanel;
	mmgGuiHolder	*m_ViewPropertyPanel;
  mmgCheckTree  *m_Tree;
  mafString      m_SearchVmeEntry;

  mmgGuiHolder  *m_VmePanel;
  mmgGuiHolder  *m_VmeOutputPanel;
  mmgGuiHolder  *m_VmePipePanel;

  mafNode     *m_SelectedVme;
  mafView     *m_SelectedView;
  mafObserver *m_Listener;
};
#endif
