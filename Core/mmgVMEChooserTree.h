/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgVMEChooserTree.h,v $
  Language:  C++
  Date:      $Date: 2007-06-15 14:15:18 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgVMEChooserTree_H__
#define __mmgVMEChooserTree_H__

enum VME_CHOOSER_STYLES
{
  REPRESENTATION_AS_TREE,
  REPRESENTATION_AS_ACCEPTABLE_NODE_LIST
};

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mmgCheckTree.h"

//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
class mafNode;

typedef bool (*ValidateCallBackType) (mafNode *);

//----------------------------------------------------------------------------
// mmgVMEChooserTree :
//----------------------------------------------------------------------------
/** 
*/
class mmgVMEChooserTree: public mmgCheckTree
{
public:
                 mmgVMEChooserTree (wxWindow *parent, mmgCheckTree *tree, ValidateCallBackType vme_accept_function = 0, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false, long style = REPRESENTATION_AS_TREE); 
  virtual       ~mmgVMEChooserTree();

  /** Return the choosed node*/
  mafNode *GetChoosedNode() {return m_ChoosedNode;};

protected:
  /** Return the status of the node according to the vme visibility. */
  int GetVmeStatus(mafNode *node);

  void InitializeImageList();

  /** clone in tree a subtree of source_item */
  void CloneSubTree(mmgCheckTree *tree, wxTreeItemId *source_item, wxTreeItemId *dest_parent_item);

  /** Called by the Custom-Tree-Event-Handler - via OnMouseDown*/
  void OnIconClick(wxTreeItemId item);

  /** Called by the Custom-Tree-Event-Handler */
  void ShowContextualMenu(wxMouseEvent& event);

  /** respond to Selection Changed */
  virtual void OnSelectionChanged(wxTreeEvent& event);

  ValidateCallBackType ValidateFunction;

  mafNode   *m_ChoosedNode;
  long       m_ChooserTreeStyle;

  DECLARE_EVENT_TABLE()
};
#endif
