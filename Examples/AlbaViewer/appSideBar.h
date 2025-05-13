/*=========================================================================
Program:   AlbaViewer
Module:    appSideBar.h
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

#ifndef __appSideBar_H__
#define __appSideBar_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaAbsSideBar.h"
#include "albaEvent.h"
#include "albaGUICheckTree.h"
#include "albaGUIVMEChooserTree.h"
#include "albaObserver.h"

#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/utils.h>

//----------------------------------------------------------------------------
// Forward reference
//----------------------------------------------------------------------------
class albaGUI;
class albaGUIHolder;
class albaGUIMDIFrame;
class albaGUINamedPanel;
class albaGUIPanel;
class albaGUIPanelStack;
class albaGUISplittedPanel;
class albaVME;
class albaView;
class wxListCtrl;

//----------------------------------------------------------------------------
// Class Name: appSideBar
//----------------------------------------------------------------------------
class appSideBar: public albaAbsSideBar
{
public:

	appSideBar(albaGUIMDIFrame* parent, int id, albaObserver *Listener);
	~appSideBar(); 
	
	// Add a new vme into the tree
	void VmeAdd(albaVME *vme);

	// Remove a vme from the tree
	void VmeRemove(albaVME *vme);

	// Notify to the tree that a vme is modified.
	void VmeModified(albaVME *vme);

	// Notify to the tree the visibility of the vme.
	void VmeShow(albaVME *vme, bool visibility);

	// Notify to the tree that the vme has been selected.
	void VmeSelected(albaVME *vme);

	/** Return True if the VME is Expanded on Tree */
	virtual bool IsVMEExpanded(albaVME *vme);

	/** Collapse the VME on the Tree */
	virtual void CollapseVME(albaVME *vme);

	/** Expand the VME on the Tree  */
	virtual void ExpandVME(albaVME *vme);
		
	// Show the operation's parameters gui on the tabbed panel.
	void OpShowGui(bool push_gui, albaGUIPanel *panel);

	// Hide the view/operation's gui from the tabbed panel.
	void OpHideGui(bool view_closed);

	// Plug the view settings on the tabbed panel.
	void ViewSelect(albaView *view);

	// Notify to the tree that a view has been deleted.
	void ViewDeleted(albaView *view);

	// Enable/Disable the vme selection when an operation ends/start.
	void EnableSelect(bool enable);

	// Switch the visibility of the panel.
	void Show();

	bool IsShown() {return m_Notebook->IsShown();};
  void Show(bool show) {m_Notebook->Show(show);};

	// Respond to a VME_CHOOSE event. Build a dialog containing the vme tree and return the vme choosed from the user.
	std::vector<albaVME*> VmeChoose(void *vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, albaString title = "Choose Node", bool multiSelect = false, albaVME *vme = NULL);

	// Open a Find VME dialog.
	void FindVME();

protected:

  void UpdateVmePanel();  

	wxNotebook        *m_Notebook;
	wxNotebook        *m_VmeNotebook;
	wxSplitterWindow	*m_SideSplittedPanel;

  albaGUIPanelStack	*m_OpPanel;
	albaGUIHolder			*m_ViewPropertyPanel;
	albaGUICheckTree  *m_Tree;

	albaGUIHolder  *m_VmePanel;
  albaGUIHolder  *m_VmePipePanel;

  albaVME			*m_SelectedVme;
	albaVME			*m_OldSelectedVme;
  albaView     *m_SelectedView;
  albaObserver *m_Listener;

	albaGUI *m_AppendingGUI;
	albaGUI *m_CurrentVmeGui;
};
#endif
