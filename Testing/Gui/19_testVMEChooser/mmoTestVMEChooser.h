/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmoTestVMEChooser
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mmoTestVMEChooser_H__
#define __mmoTestVMEChooser_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaNode.h"

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaNodeGeneric;
class albaNode;

// ============================================================================
class mmoTestVMEChooser : public albaOp
// ============================================================================
{
public:
                mmoTestVMEChooser(wxString label);
               ~mmoTestVMEChooser();
	virtual void OnEvent(albaEventBase *event);
  albaOp* Copy();

  bool Accept(albaNode* vme) {return true;};
  void OpRun();
  void OpDo();
  void OpUndo();

  static bool Validate(albaNode *node) {return(node != NULL && strcmp(node->GetName(),"vme generic 1") == 0);};

protected:
  void OpStop(int result);
  albaNode *m_ChoosedNode;
};
#endif // __mmoTestVMEChooser_H__
