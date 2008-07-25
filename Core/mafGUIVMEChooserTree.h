/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIVMEChooserTree.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:54 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
                 mafGUIVMEChooserTree (wxWindow *parent, mafGUICheckTree *tree, ValidateCallBackType vme_accept_function = 0, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false, long style = REPRESENTATION_AS_TREE, bool multiSelect = false); 
  virtual       ~mafGUIVMEChooserTree();

  /** Return the choosed node*/
  std::vector<mafNode*> GetChoosedNode();

  /** Called by the Custom-Tree-Event-Handler - via OnMouseDown*/
  virtual void OnIconClick(wxTreeItemId item);

protected:
  /** Return the status of the node according to the vme visibility. */
  int GetVmeStatus(mafNode *node);

  void InitializeImageList();

  /** Initialize images for multi selection dialog. */
  void InitializeImageListMulti();

  /** Update the vme nodes icon. */
  void VmeUpdateIcon(mafNode *n);

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
