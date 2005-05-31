/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoTestVMEChooser.h,v $
  Language:  C++
  Date:      $Date: 2005-05-31 23:58:53 $
  Version:   $Revision: 1.3 $
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
#include "mmgVMEChooserAccept.h"
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

  class mafNodeAccept : public mmgVMEChooserAccept
  {
  public:

    mafNodeAccept() {};
    ~mafNodeAccept() {};

    bool Validate(mafNode *node) {return(node != NULL && strcmp(node->GetName(),"vme generic 1") == 0);};
  };
  mafNodeAccept *m_NodeAccept;

protected:
  void OpStop(int result);
  mafNode *m_ChoosedNode;
};
#endif // __mmoTestVMEChooser_H__