/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIVMEChooserTree
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIVMEChooserTree_H__
#define __albaGUIVMEChooserTree_H__

enum VME_CHOOSER_STYLES
{
  REPRESENTATION_AS_TREE,
  REPRESENTATION_AS_ACCEPTABLE_NODE_LIST
};

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUICheckTree.h"

//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
class albaVME;

typedef bool (*ValidateCallBackType) (albaVME *);

//----------------------------------------------------------------------------
// albaGUIVMEChooserTree :
//----------------------------------------------------------------------------
/** Widget representing the tree that allow the user to choose a VME. 
This widget has the possibility to manage validation callback passed through a constructor that allows to say if a VME can be choose or not.
Another feature of the widget is to show also a checkbox near the VME's icon to allow a multiple selection.
*/
class albaGUIVMEChooserTree: public albaGUICheckTree
{
public:
                 albaGUIVMEChooserTree (wxWindow *parent, albaGUICheckTree *tree, ValidateCallBackType vme_accept_function = 0, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false, long style = REPRESENTATION_AS_TREE, bool multiSelect = false, albaVME *subTree=NULL); 
  virtual       ~albaGUIVMEChooserTree();

  /** Return the choosed node*/
  std::vector<albaVME*> GetChoosedNode();

  /** Called by the Custom-Tree-Event-Handler - via OnMouseDown*/
  virtual void OnIconClick(wxTreeItemId item);

protected:
  /** Return the status of the node according to the vme visibility. */
  int GetVmeStatus(albaVME *node);

	void VmeUpdateIcon(albaVME *vme);

  void InitializeImageList();

  /** clone in tree a subtree of source_item */
  void CloneSubTree(albaGUICheckTree *tree, wxTreeItemId *source_item, wxTreeItemId *dest_parent_item);

  /** Called by the Custom-Tree-Event-Handler */
  void ShowContextualMenu(wxMouseEvent& event);

  /** respond to Selection Changed */
  virtual void OnSelectionChanged(wxTreeEvent& event);

  ValidateCallBackType m_ValidateFunction;

  albaVME   *m_ChoosedNode; ///< Pointer to the selected node in single selection.
  long       m_ChooserTreeStyle;
  std::vector<albaVME*> m_CheckedNode; ///< Vector of checked node (used in multi-selection)
  bool m_MultipleSelection; ///< Flag that manage the single or multi selection of the nodes inside the tree.

  DECLARE_EVENT_TABLE()
};
#endif
