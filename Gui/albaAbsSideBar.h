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

  // Description:
	// Add a new vme into the tree.
	virtual void VmeAdd(albaVME *vme) = 0;

	// Description:
	// Remove a vme from the tree.
	virtual void VmeRemove(albaVME *vme) = 0;

	// Description:
	// Notify to the tree that a vme is modified.
	virtual void VmeModified(albaVME *vme) = 0;

	// Description:
	// Notify to the tree the visibility of the vme.
	virtual void VmeShow(albaVME *vme, bool visibility) = 0;

	// Description:
	// Notify to the tree that the vme has been selected.
	virtual void VmeSelected(albaVME *vme) = 0;

	// Description:
	// Show the operation's parameters gui on the tabbed panel.
	virtual void OpShowGui(bool push_gui, albaGUIPanel *panel) = 0;

	// Description:
	// Hide the view/operation's gui from the tabbed panel.
	virtual void OpHideGui(bool view_closed) = 0;

	// Description:
	// Plug the view settings on the tabbed panel.
	virtual void ViewSelect(albaView *view) = 0;

	// Description:
	// Notify to the tree that a view has been deleted.
	virtual void ViewDeleted(albaView *view) = 0;

	// Description:
	// Enable/Disable the vme selection when an operation ends/start.
	virtual void EnableSelect(bool enable) = 0;

	// Description:
	// Switch the visibility of the panel.
	virtual void Show() = 0;

	virtual bool IsShown() = 0;
  virtual void Show(bool show) = 0;

	/** Respond to a VME_CHOOSE evt. Build a dialog containing the vme tree and return the vme choosed from the user. */
	virtual std::vector<albaVME*> VmeChoose(void *vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, albaString title = "Choose Node", bool multiSelect = false, albaVME *vme = NULL) = 0;

	/** Open a Find VME dialog.*/
	virtual void FindVME() = 0;
};
#endif
