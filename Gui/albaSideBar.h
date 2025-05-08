/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSideBar
 Authors: Silvano Imboden, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaSideBar_H__
#define __albaSideBar_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/utils.h>

#include "albaEvent.h"
#include "albaObserver.h"
#include "albaGUICheckTree.h"
#include "albaGUIVMEChooserTree.h"
#include "albaAbsSideBar.h"
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class albaGUIHolder;
class albaGUI;
class albaGUIPanelStack;
class albaGUIPanel;
class albaVME;
class albaView;
class albaGUINamedPanel;
class albaGUISplittedPanel;
class albaGUIMDIFrame;
//----------------------------------------------------------------------------
// albaSideBar :
//----------------------------------------------------------------------------
/**
*/
class ALBA_EXPORT albaSideBar: public albaAbsSideBar
{
public:
	albaSideBar(albaGUIMDIFrame* parent, int id, albaObserver *Listener, long style = DOUBLE_NOTEBOOK);
	~albaSideBar(); 

  enum SIDEBAR_STYLE
  {
    SINGLE_NOTEBOOK,
    DOUBLE_NOTEBOOK
  };

	/** Add a new vme into the tree. */
	virtual void VmeAdd(albaVME *vme);

	/** Remove a vme from the tree. */
	virtual void VmeRemove(albaVME *vme);

	/** Notify to the tree that a vme is modified. */
	virtual void VmeModified(albaVME *vme);

	/** Notify to the tree the visibility of the vme. */
	virtual void VmeShow(albaVME *vme, bool visibility);

	/** Notify to the tree that the vme has been selected. */
	virtual void VmeSelected(albaVME *vme);

	/** Return True if the VME is Expanded on Tree */
	virtual bool IsVMEExpanded(albaVME *vme);

	/** Collapse the VME on the Tree */
	virtual void CollapseVME(albaVME *vme);

	/** Expand the VME on the Tree  */
	virtual void ExpandVME(albaVME *vme);

	/** Show the operation's parameters gui on the tabbed panel. */
	virtual void OpShowGui(bool push_gui, albaGUIPanel *panel);

	/** Hide the view/operation's gui from the tabbed panel. */
	virtual void OpHideGui(bool view_closed);

	/** Plug the view settings on the tabbed panel. */
	virtual void ViewSelect(albaView *view);

	/** Notify to the tree that a view has been deleted. */
	virtual void ViewDeleted(albaView *view);

	/** Enable/Disable the vme selection when an operation ends/start. */
	virtual void EnableSelect(bool enable);

	/** Switch the visibility of the panel. */
	virtual void Show();

	/** Return true if the panel is showed. */
	virtual bool IsShown();

	/** Sets the visibility of the panel. */
	virtual void Show(bool show);

	/** * Respond to a VME_CHOOSE evt. Build a dialog containing the vme tree and return the vme choosed from the user. */
	virtual 	std::vector<albaVME*> VmeChoose(void *vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, albaString title = "Choose Node", bool multiSelect = false, albaVME *vme = NULL);

	/** Open a Find VME dialog. */
	virtual void FindVME();

protected:
  void UpdateVmePanel();  

	wxNotebook        *m_Notebook;
	wxNotebook        *m_VmeNotebook;
	wxSplitterWindow *m_SideSplittedPanel;

  albaGUIPanelStack	*m_OpPanel;
	albaGUIHolder	*m_ViewPropertyPanel;
  albaGUICheckTree  *m_Tree;

  albaGUIHolder  *m_VmePanel;
  albaGUIHolder  *m_VmePipePanel;

  albaVME     *m_SelectedVme;
  albaView     *m_SelectedView;
  albaObserver *m_Listener;

  albaGUI *m_AppendingGUI;
  
	albaGUI *m_CurrentVmeGui;
	albaGUI *m_CurrentVmeOutputGui;
  albaGUI *m_CurrentPipeGui;

  int m_Style;
};
#endif
