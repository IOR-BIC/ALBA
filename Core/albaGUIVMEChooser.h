/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIVMEChooser
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIVmeChooser_H__
#define __albaGUIVmeChooser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIDialog.h"
#include "albaGUIVMEChooserTree.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUICheckTree;
class albaVME;

//----------------------------------------------------------------------------
// albaGUIVMEChooser :
//----------------------------------------------------------------------------
/**
Display a modal dialog filled with a vme tree and
return the choosed vme. The tree is filled from
the vme passed in the constructor with its children.
*/
class albaGUIVMEChooser : public albaGUIDialog
{

public:
	albaGUIVMEChooser(albaGUICheckTree *tree, wxString dialog_title = "Vme Chooser", void *vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, bool multiSelect = false, albaVME *subTree = NULL);

	~albaGUIVMEChooser();

	std::vector<albaVME*> ShowChooserDialog();
	void  OnEvent(albaEventBase *alba_event);

protected:
	albaGUIVMEChooserTree *m_ChooserTree;
	ValidateCallBackType m_ValidateCallback; // pointer to the callback
	
	void LoadLayout();
	void SaveLayout();
};
#endif
