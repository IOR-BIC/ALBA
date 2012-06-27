/*=========================================================================

 Program: MAF2
 Module: mmoTestVMEChooser
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mmoTestVMEChooser_H__
#define __mmoTestVMEChooser_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafNode.h"

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafNodeGeneric;
class mafNode;

// ============================================================================
class mmoTestVMEChooser : public mafOp
// ============================================================================
{
public:
                mmoTestVMEChooser(wxString label);
               ~mmoTestVMEChooser();
	virtual void OnEvent(mafEventBase *event);
  mafOp* Copy();

  bool Accept(mafNode* vme) {return true;};
  void OpRun();
  void OpDo();
  void OpUndo();

  static bool Validate(mafNode *node) {return(node != NULL && strcmp(node->GetName(),"vme generic 1") == 0);};

protected:
  void OpStop(int result);
  mafNode *m_ChoosedNode;
};
#endif // __mmoTestVMEChooser_H__
