/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgVMEChooserTree.h,v $
  Language:  C++
  Date:      $Date: 2005-05-18 16:15:55 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgVMEChooserTree_H__
#define __mmgVMEChooserTree_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mmgCheckTree.h"

//----------------------------------------------------------------------------
// Forward refs:
//----------------------------------------------------------------------------
class mafNode;
class mmgVmeChooserAccept;

//----------------------------------------------------------------------------
// mmgVMEChooserTree :
//----------------------------------------------------------------------------
/** 
*/
class mmgVMEChooserTree: public mmgCheckTree
{
public:
                 mmgVMEChooserTree (wxWindow *parent, mmgCheckTree *tree, long vme_accept_function = 0, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false); 
  virtual       ~mmgVMEChooserTree();

  mafNode *GetChoosedNode() {return m_ChoosedNode;};

protected:
  /** Return the status of the node according to the vme visibility. */
  int GetVmeStatus(mafNode *node);

  void InitializeImageList();

  /** Called by the Custom-Tree-Event-Handler - via OnMouseDown*/
  void OnIconClick(wxTreeItemId item);

  /** Called by the Custom-Tree-Event-Handler */
  void ShowContextualMenu(wxMouseEvent& event);

  /** respond to Selection Changed */
  virtual void OnSelectionChanged(wxTreeEvent& event);

  mmgVmeChooserAccept *m_AcceptFunction;
  mafNode   *m_ChoosedNode;

  DECLARE_EVENT_TABLE()
};
#endif