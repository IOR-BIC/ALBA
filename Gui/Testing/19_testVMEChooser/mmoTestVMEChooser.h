/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoTestVMEChooser.h,v $
  Language:  C++
  Date:      $Date: 2005-05-18 15:27:20 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmoTestVMEChooser_H__
#define __mmoTestVMEChooser_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafOp.h"
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

protected:
  void OpStop(int result);
  mafNode *m_ChoosedNode;
};
#endif // __mmoTestVMEChooser_H__