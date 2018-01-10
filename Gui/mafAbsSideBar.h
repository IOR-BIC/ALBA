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
#ifndef __mafAbsSideBar_H__
#define __mafAbsSideBar_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/utils.h>

#include "mafEvent.h"
#include "mafObserver.h"
#include "mafGUICheckTree.h"
#include "mafGUIVMEChooserTree.h"
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
class mafGUIMDIFrame;
//----------------------------------------------------------------------------
// mafSideBar :
//----------------------------------------------------------------------------
/**
*/
class MAF_EXPORT mafAbsSideBar
{
public:
	mafAbsSideBar() {};
	virtual ~mafAbsSideBar() {};

  // Description:
	// Add a new vme into the tree.
	virtual void VmeAdd(mafVME *vme) = 0;

	// Description:
	// Remove a vme from the tree.
	virtual void VmeRemove(mafVME *vme) = 0;

	// Description:
	// Notify to the tree that a vme is modified.
	virtual void VmeModified(mafVME *vme) = 0;

	// Description:
	// Notify to the tree the visibility of the vme.
	virtual void VmeShow(mafVME *vme, bool visibility) = 0;

	// Description:
	// Notify to the tree that the vme has been selected.
	virtual void VmeSelected(mafVME *vme) = 0;

	// Description:
	// Show the operation's parameters gui on the tabbed panel.
	virtual void OpShowGui(bool push_gui, mafGUIPanel *panel) = 0;

	// Description:
	// Hide the view/operation's gui from the tabbed panel.
	virtual void OpHideGui(bool view_closed) = 0;

	// Description:
	// Plug the view settings on the tabbed panel.
	virtual void ViewSelect(mafView *view) = 0;

	// Description:
	// Notify to the tree that a view has been deleted.
	virtual void ViewDeleted(mafView *view) = 0;

	// Description:
	// Enable/Disable the vme selection when an operation ends/start.
	virtual void EnableSelect(bool enable) = 0;

	// Description:
	// Switch the visibility of the panel.
	virtual void Show() = 0;

	virtual bool IsShown() = 0;
  virtual void Show(bool show) = 0;

	/** Respond to a VME_CHOOSE evt. Build a dialog containing the vme tree and return the vme choosed from the user. */
	virtual std::vector<mafVME*> VmeChoose(long vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, mafString title = "Choose Node", bool multiSelect = false, mafVME *vme = NULL) = 0;

	/** Open a Find VME dialog.*/
	virtual void FindVME() = 0;
};
#endif
