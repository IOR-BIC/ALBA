/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgCheckTree.h,v $
  Language:  C++
  Date:      $Date: 2005-04-07 18:35:37 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmgCheckTree_H__
#define __mmgCheckTree_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h" //important: mafDefines should always be included as first
#include "mmgTree.h"
#include "mafPics.h"
#include <wx/imaglist.h>
#include <wx/treectrl.h>
//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
class mafNode;
class mafView;

//----------------------------------------------------------------------------
// mmgCheckTree :
//----------------------------------------------------------------------------
/** */
class mmgCheckTree: public mmgTree
{
public:
                 mmgCheckTree (wxWindow* parent, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false); 
  virtual       ~mmgCheckTree();

  /** Add the vme to the checked tree and create the related icon. */
	void VmeAdd(mafNode *n);

  /** Remove the vme from the checked tree. */
  void VmeRemove(mafNode *n);

  /** Select the vme node of the checked tree. */
  void VmeSelected(mafNode *n);

  /** Update the vme node icon into the checked tree. */
  void VmeShow(mafNode *n, bool show);

  /** Update the vme node label with the new vme name. */
  void VmeModified(mafNode *n);

  /** Update the vme tree nodes with the new vme visibility for the selected view. */
  void ViewSelected(mafView *view);

  /** Update the vme tree nodes when a view is deleted. */
  void ViewDeleted(mafView *view);

  /**todo: 
  setting m_enableSelect to false
  doesnt prevent to changhe the selection using the keyboard !!
  //SIL. 7-4-2005: 
  */
  void EnableSelect(bool enable);

protected:
  /** Update the vme nodes icon. */
  void VmeUpdateIcon(mafNode *n);
  
  /** Return the status of the node according to the vme visibility. */
  int GetVmeStatus(mafNode *vme);

  /** Enable/disable crypto for a subtree. */
  void CryptSubTree(bool crypt);

public:
  /** Intercept the mouse button down and check if the click is on the icon. */
	void OnMouseDown(wxMouseEvent& event);

  /** Intercept the mouse button down and check if the click is on the icon. */
  void OnMouseUp(wxMouseEvent& event);

  /** Intercept the mouse button down and check if the click is on the icon. */
  void OnMouseEvent(wxMouseEvent& event);

protected:
  mafView  *m_view;
  mafNode  *m_clicked_vme;
  bool      m_check_crypto;
  bool      m_canSelect;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::
// RMENU
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::
public:

  /** Show the contextual menù over the selected node. */
  void ShowContextualMenu(wxMouseEvent& event);

protected:

  /** Manage the contextual menù. */
  void OnContextualMenu(wxCommandEvent& event);

  wxMenu *m_RMenu;	

  DECLARE_EVENT_TABLE()
}; // end of mmgCheckTree
#endif

