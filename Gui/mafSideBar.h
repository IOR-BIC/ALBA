/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSideBar.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:01:22 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafSideBar_H__
#define __mafSideBar_H__

#include "mafDefines.h" //important: mafDefines should always be included as first
#include <wx/notebook.h>
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgSashPanel;
class mmgGuiHolder;
class mmgGui;
class mmgPanelStack;
class mmgPanel;
class mmgTree;
class mafNode;
class mafView;
//----------------------------------------------------------------------------
// mafSideBar :
//----------------------------------------------------------------------------
/**
*/
class mafSideBar
{
public:
	mafSideBar(wxWindow* parent, int id, mafEventListener *Listener);
	~mafSideBar(); 

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
	// Enable/Disable the vme selectione when an operation ends/start.
	void EnableSelect(bool enable);

	// Description:
	// Switch the visibility of the panel.
	void Show();

	// Description:
	// Return the pointer to the tree.
	mmgTree *GetTree() {return m_tree;};

protected:
  wxNotebook        *m_notebook;
  mmgSashPanel      *m_side_bar;
  mmgPanelStack			*m_op_panel;
	mmgGuiHolder			*m_view_property_panel;
  mmgTree           *m_tree;
  mmgGuiHolder      *m_vme_panel;
};
#endif
