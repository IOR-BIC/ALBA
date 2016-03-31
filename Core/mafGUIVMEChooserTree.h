/*=========================================================================

 Program: MAF2
 Module: mafGUIVMEChooserTree
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIVMEChooserTree_H__
#define __mafGUIVMEChooserTree_H__

enum VME_CHOOSER_STYLES
{
  REPRESENTATION_AS_TREE,
  REPRESENTATION_AS_ACCEPTABLE_NODE_LIST
};

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGUICheckTree.h"

//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
class mafNode;

typedef bool (*ValidateCallBackType) (mafNode *);

//----------------------------------------------------------------------------
// mafGUIVMEChooserTree :
//----------------------------------------------------------------------------
/** Widget representing the tree that allow the user to choose a VME. 
This widget has the possibility to manage validation callback passed through a constructor that allows to say if a VME can be choose or not.
Another feature of the widget is to show also a checkbox near the VME's icon to allow a multiple selection.
*/
class mafGUIVMEChooserTree: public mafGUICheckTree
{
public:
                 mafGUIVMEChooserTree (wxWindow *parent, mafGUICheckTree *tree, ValidateCallBackType vme_accept_function = 0, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false, long style = REPRESENTATION_AS_TREE, bool multiSelect = false, mafNode *subTree=NULL); 
  virtual       ~mafGUIVMEChooserTree();

  /** Return the choosed node*/
  std::vector<mafNode*> GetChoosedNode();

  /** Called by the Custom-Tree-Event-Handler - via OnMouseDown*/
  virtual void OnIconClick(wxTreeItemId item);

protected:
  /** Return the status of the node according to the vme visibility. */
  int GetVmeStatus(mafNode *node);

  void InitializeImageList();

  /** clone in tree a subtree of source_item */
  void CloneSubTree(mafGUICheckTree *tree, wxTreeItemId *source_item, wxTreeItemId *dest_parent_item);

  /** Called by the Custom-Tree-Event-Handler */
  void ShowContextualMenu(wxMouseEvent& event);

  /** respond to Selection Changed */
  virtual void OnSelectionChanged(wxTreeEvent& event);

  ValidateCallBackType m_ValidateFunction;

  mafNode   *m_ChoosedNode; ///< Pointer to the selected node in single selection.
  long       m_ChooserTreeStyle;
  std::vector<mafNode*> m_CheckedNode; ///< Vector of checked node (used in multi-selection)
  bool m_MultipleSelection; ///< Flag that manage the single or multi selection of the nodes inside the tree.

  DECLARE_EVENT_TABLE()
};
#endif
