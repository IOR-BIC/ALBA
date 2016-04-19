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
#include "mafGUICheckTree.h"
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafGUIHolder;
class mafGUI;
class mafGUIPanelStack;
class mafGUIPanel;
class mafVME;
class mafView;
class mafGUINamedPanel;
class mafGUISplittedPanel;
//----------------------------------------------------------------------------
// mafSideBar :
//----------------------------------------------------------------------------
/**
*/
class MAF_EXPORT mafSideBar
{
public:
	mafSideBar(wxWindow* parent, int id, mafObserver *Listener, long style = DOUBLE_NOTEBOOK);
	~mafSideBar(); 

  enum SIDEBAR_STYLE
  {
    SINGLE_NOTEBOOK,
    DOUBLE_NOTEBOOK
  };

  // Description:
	// Add a new vme into the tree.
	void VmeAdd(mafVME *vme);

	// Description:
	// Remove a vme from the tree.
	void VmeRemove(mafVME *vme);

	// Description:
	// Notify to the tree that a vme is modified.
	void VmeModified(mafVME *vme);

	// Description:
	// Notify to the tree the visibility of the vme.
	void VmeShow(mafVME *vme, bool visibility);

	// Description:
	// Notify to the tree that the vme has been selected.
	void VmeSelected(mafVME *vme);

	// Description:
	// Remove the vme property gui from the panel.
	void VmePropertyRemove(mafGUI *gui);

	// Description:
	// Show the operation's parameters gui on the tabbed panel.
	void OpShowGui(bool push_gui, mafGUIPanel *panel);

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
	mafGUICheckTree *GetTree() {return m_Tree;};

  bool IsShown() {return m_Notebook->IsShown();};
  void Show( bool show) {m_Notebook->Show(show);};

  wxNotebook        *m_Notebook;
  wxNotebook        *m_VmeNotebook;
  //mafGUINamedPanel    *m_SideSplittedPanel;
  wxSplitterWindow *m_SideSplittedPanel;
protected:
  void UpdateVmePanel();  

  mafGUIPanelStack	*m_OpPanel;
	mafGUIHolder	*m_ViewPropertyPanel;
  mafGUICheckTree  *m_Tree;

  mafGUIHolder  *m_VmePanel;
  mafGUIHolder  *m_VmePipePanel;

  mafVME     *m_SelectedVme;
  mafView     *m_SelectedView;
  mafObserver *m_Listener;

  mafGUI *m_AppendingGUI;
  
	mafGUI *m_CurrentVmeGui;
	mafGUI *m_CurrentVmeOutputGui;
  mafGUI *m_CurrentPipeGui;

  int m_Style;
};
#endif
