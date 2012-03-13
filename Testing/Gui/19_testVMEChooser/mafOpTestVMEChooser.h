/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpTestVMEChooser.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 12:00:48 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafOpTestVMEChooser_H__
#define __mafOpTestVMEChooser_H__
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
class mafOpTestVMEChooser : public mafOp
// ============================================================================
{
public:
                mafOpTestVMEChooser(wxString label);
               ~mafOpTestVMEChooser();
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
#endif // __mafOpTestVMEChooser_H__
