/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUICheckTree.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 10:05:18 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafGUICheckTree_H__
#define __mafGUICheckTree_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGUITree.h"
#include "mafPics.h"
#include <wx/imaglist.h>
#include <wx/treectrl.h>
#include <map>
#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_MAP(MAF_EXPORT,wxString,int);
#endif
//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
class mafNode;
class mafView;

//----------------------------------------------------------------------------
// mafGUICheckTree :
//----------------------------------------------------------------------------
/** 
  class Name : mafGUICheckTree.
  Class that represents the widget component put inside side bar and containing the vme tree with the possibility 
  to check vme visibility and/or  vme selection. It is possible only one selection at time.
*/
class MAF_EXPORT mafGUICheckTree: public mafGUITree
{
public:
  /** constructor .*/
  mafGUICheckTree (wxWindow* parent, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false); 
  /** destructor .*/
  virtual       ~mafGUICheckTree();

  /** Add the vme to the checked tree and create the related icon. */
	void VmeAdd(mafNode *n);

  /** Remove the vme from the checked tree. */
  void VmeRemove(mafNode *n);

  /** Select the vme node of the checked tree. */
  void VmeSelected(mafNode *n);

  /** Return the current selected node.*/
  mafNode *GetSelectedNode() {return m_SelectedNode;};

  /** Return true if the icon associated to the tree item is checked.*/
  bool IsIconChecked(wxTreeItemId item);

  /** Update the vme node icon into the checked tree. */
  void VmeShow(mafNode *n, bool show);

  /** Update the vme node label with the new vme name. */
  void VmeModified(mafNode *n);

  /** Update the vme tree nodes with the new vme visibility for the selected view. */
  void ViewSelected(mafView *view);

  /** Update the vme tree nodes when a view is deleted. */
  void ViewDeleted(mafView *view);

// todo: 
// setting m_enableSelect to false
// doesn't prevent to change the selection using the keyboard !!
//SIL. 7-4-2005: 

  /** Enable the selection of a node tree.*/
  void EnableSelect(bool enable);

  /** Retrieve wxwindows widget pointer of the tree.*/
  wxTreeCtrl *GetTree() {return m_NodeTree;};
  
protected:
  /** Update the vme nodes icon. */
  void VmeUpdateIcon(mafNode *n);

  /** Update all the vme nodes icon. */
  void TreeUpdateIcon();
  
  /** Return the status of the node according to the vme visibility. */
  virtual int GetVmeStatus(mafNode *vme);

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
  mafView *m_View;
  mafNode *m_SelectedNode;
  //bool      m_CheckCrypto;
  bool     m_CanSelect;
  wxMenu  *m_RMenu;	

  typedef std::map<wxString,int> MapClassNameToIcon;
  MapClassNameToIcon m_MapClassNameToIcon;

  /** event table declaration */
  DECLARE_EVENT_TABLE()
}; // end of mafGUICheckTree
#endif
