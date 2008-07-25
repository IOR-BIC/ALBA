/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIVMEChooser.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:54 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUIVmeChooser_H__
#define __mafGUIVmeChooser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGUIDialog.h"
#include "mafGUIVMEChooserTree.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUICheckTree;
class mafNode;

//----------------------------------------------------------------------------
// mafGUIVMEChooser :
//----------------------------------------------------------------------------
/**
Display a modal dialog filled with a vme tree and
return the choosed vme. The tree is filled from
the vme passed in the constructor with its children.
*/
class mafGUIVMEChooser : public mafGUIDialog
{

public:
           mafGUIVMEChooser(mafGUICheckTree *tree, wxString dialog_title="Vme Chooser", long vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, bool multiSelect = false);
          ~mafGUIVMEChooser();
	std::vector<mafNode*> ShowChooserDialog();
  void  OnEvent(mafEventBase *maf_event);

protected:
  mafGUIVMEChooserTree *m_ChooserTree;
  ValidateCallBackType m_ValidateCallback; // pointer to the callback
};
#endif
