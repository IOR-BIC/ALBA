/*=========================================================================

 Program: MAF2
 Module: mafGUIVMEChooser
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class mafVME;

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
           mafGUIVMEChooser(mafGUICheckTree *tree, wxString dialog_title="Vme Chooser", long vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, bool multiSelect = false, mafVME *subTree=NULL);
          ~mafGUIVMEChooser();
	std::vector<mafVME*> ShowChooserDialog();
  void  OnEvent(mafEventBase *maf_event);

protected:
  mafGUIVMEChooserTree *m_ChooserTree;
  ValidateCallBackType m_ValidateCallback; // pointer to the callback
};
#endif
