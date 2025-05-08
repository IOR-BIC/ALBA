/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSideBar
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaAbsSideBar_H__
#define __albaAbsSideBar_H__
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
class ALBA_EXPORT albaAbsSideBar
{
public:
	albaAbsSideBar() {};
	virtual ~albaAbsSideBar() {};

	/** Add a new vme into the tree. */
	virtual void VmeAdd(albaVME *vme) = 0;

	/** Remove a vme from the tree. */
	virtual void VmeRemove(albaVME *vme) = 0;

	/** Notify to the tree that a vme is modified. */
	virtual void VmeModified(albaVME *vme) = 0;

	/** Notify to the tree the visibility of the vme. */
	virtual void VmeShow(albaVME *vme, bool visibility) = 0;

	/** Notify to the tree that the vme has been selected. */
	virtual void VmeSelected(albaVME *vme) = 0;

	/** Return True if the VME is Expanded on Tree */
	virtual bool IsVMEExpanded(albaVME *vme) = 0;

	/** Collapse the VME on the Tree */
	virtual void CollapseVME(albaVME *vme) = 0;

	/** Expand the VME on the Tree  */
	virtual void ExpandVME(albaVME *vme) = 0;

	/** Show the operation's parameters gui on the tabbed panel. */
	virtual void OpShowGui(bool push_gui, albaGUIPanel *panel) = 0;

	/** Hide the view/operation's gui from the tabbed panel. */
	virtual void OpHideGui(bool view_closed) = 0;

	/** Plug the view settings on the tabbed panel. */
	virtual void ViewSelect(albaView *view) = 0;

	/** Notify to the tree that a view has been deleted. */
	virtual void ViewDeleted(albaView *view) = 0;
	
	/** Enable/Disable the vme selection when an operation ends/start. */
	virtual void EnableSelect(bool enable) = 0;
	
	/** Switch the visibility of the panel. */
	virtual void Show() = 0;

	/** Return true if the panel is showed. */
	virtual bool IsShown() = 0;

	/** Sets the visibility of the panel. */
  virtual void Show(bool show) = 0;

	/** * Respond to a VME_CHOOSE evt. Build a dialog containing the vme tree and return the vme choosed from the user. */
	virtual std::vector<albaVME*> VmeChoose(void *vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, albaString title = "Choose Node", bool multiSelect = false, albaVME *vme = NULL) = 0;

	/** Open a Find VME dialog. */
	virtual void FindVME() = 0;


};
#endif
