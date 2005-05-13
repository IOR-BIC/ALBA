/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgVMEChooserTree.h,v $
  Language:  C++
  Date:      $Date: 2005-05-13 16:15:39 $
  Version:   $Revision: 1.2 $
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
class mmgVMEChooserAccept;

//----------------------------------------------------------------------------
// mmgVMEChooserTree :
//----------------------------------------------------------------------------
/** 
*/
class mmgVMEChooserTree: public mmgCheckTree
{
public:
                 mmgVMEChooserTree (wxWindow* parent, wxWindowID id=-1, bool CloseButton = false, bool HideTitle = false, long vme_accept_function = 0); 
  virtual       ~mmgVMEChooserTree();

  /** Fill the VMEChooserTree by taking in account the accept function.*/
  void FillTree(mmgCheckTree *tree);

protected:
  /** Return the status of the node according to the vme visibility. */
  int GetVmeStatus(mafNode *node);

  void InitializeImageList();

public: 
  /** Called by the Custom-Tree-Event-Handler - via OnMouseDown*/
  void OnIconClick(wxTreeItemId item);

  /** Called by the Custom-Tree-Event-Handler */
  void ShowContextualMenu(wxMouseEvent& event);

  /** respond to Selection Changed */
  virtual void OnSelectionChanged(wxTreeEvent& event);

  mmgVMEChooserAccept *m_AcceptFunction;
};
#endif
