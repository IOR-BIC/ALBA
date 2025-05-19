/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUICheckTree
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUICheckTree_H__
#define __albaGUICheckTree_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUITree.h"
#include "albaPics.h"
#include <wx/imaglist.h>
#include <wx/treectrl.h>
#include <map>
#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_MAP(ALBA_EXPORT,wxString,int);
#endif
//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
class albaVME;
class albaView;

//----------------------------------------------------------------------------
// albaGUICheckTree :
//----------------------------------------------------------------------------
/** 
  class Name : albaGUICheckTree.
  Class that represents the widget component put inside side bar and containing the vme tree with the possibility 
  to check vme visibility and/or  vme selection. It is possible only one selection at time.
*/
class ALBA_EXPORT albaGUICheckTree: public albaGUITree
{
public:
  /** constructor .*/
  albaGUICheckTree (wxWindow* parent, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false); 
  /** destructor .*/
  virtual       ~albaGUICheckTree();

  /** Add the vme to the checked tree and create the related icon. */
	void VmeAdd(albaVME *vme);

  /** Remove the vme from the checked tree. */
  void VmeRemove(albaVME *vme);

  /** Select the vme node of the checked tree. */
  void VmeSelected(albaVME *vme);

  /** Return the current selected node.*/
  albaVME *GetSelectedNode() {return m_SelectedVME;};

  /** Return true if the icon associated to the tree item is checked.*/
  bool IsIconChecked(wxTreeItemId item);

  /** Update the vme node icon into the checked tree. */
  void VmeShow(albaVME *vme, bool show);

  /** Update the vme node label with the new vme name. */
  void VmeModified(albaVME *vme);

  /** Update the vme tree nodes with the new vme visibility for the selected view. */
  void ViewSelected(albaView *view);

  /** Update the vme tree nodes when a view is deleted. */
  void ViewDeleted(albaView *view);

// todo: 
// setting m_enableSelect to false
// doesn't prevent to change the selection using the keyboard !!
//SIL. 7-4-2005: 

  /** Enable the selection of a node tree.*/
  void EnableSelect(bool enable);

  /** Retrieve wxwindows widget pointer of the tree.*/
  wxTreeCtrl *GetTree() {return m_NodeTree;};
  
	/** Return True if the VME is Expanded on Tree */
	virtual bool IsVMEExpanded(albaVME *vme);

	/** Collapse the VME on the Tree */
	virtual void CollapseVME(albaVME *vme);

	/** Expand the VME on the Tree  */
	virtual void ExpandVME(albaVME *vme);
protected:
  /** Update the vme nodes icon. */
  void VmeUpdateIcon(albaVME *vme);

  /** Update all the vme nodes icon. */
  void TreeUpdateIcon();
  
  /** Return the status of the node according to the vme visibility. */
  virtual int GetVmeStatus(albaVME *vme);

  /** retrieve the icon-index for a vme given the classname */
  int ClassNameToIcon(wxString classname);

  /** Fill the image list considering the visibility vme type, and vme availability.
           The result is a list of images, and one image can be associated to the label that represent the node of the tree in the wxwidget.*/
  virtual void InitializeImageList();

  /** Given two bitmaps, it creates a third bitmap.
           The  result is a merge of the others with the right side of the first image that is the left side of the second one.*/
  wxBitmap MergeIcons(wxBitmap state, wxBitmap vme);

public: 
  /** Called by the Custom-Tree-Event-Handler */
	void OnMouseDown(wxMouseEvent& event);

  /** Called by the Custom-Tree-Event-Handler */
  void OnMouseUp(wxMouseEvent& event);

  /** Called by the Custom-Tree-Event-Handler */
  void OnMouseEvent(wxMouseEvent& event);

  /** Called by the Custom-Tree-Event-Handler - via OnMouseDown*/
  virtual void OnIconClick(wxTreeItemId item);

  /** Called by the Custom-Tree-Event-Handler */
  virtual void ShowContextualMenu(wxMouseEvent& event);

  /** respond to Selection Changed */
  virtual void OnSelectionChanged(wxTreeEvent& event);

protected:
  albaView *m_View;
  albaVME *m_SelectedVME;
  //bool      m_CheckCrypto;
  bool     m_CanSelect;
  wxMenu  *m_RMenu;	

  typedef std::map<wxString,int> MapClassNameToIcon;
  MapClassNameToIcon m_MapClassNameToIcon;

  /** event table declaration */
  DECLARE_EVENT_TABLE()
}; // end of albaGUICheckTree
#endif
