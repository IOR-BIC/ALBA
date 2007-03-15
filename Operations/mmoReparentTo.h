/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoReparentTo.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoReparentTo_H__
#define __mmoReparentTo_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVME;
class mafEvent;
class mmgGui;

//----------------------------------------------------------------------------
// mmoReparentTo :
//----------------------------------------------------------------------------
/** */
class mmoReparentTo: public mafOp
{
public:
  mmoReparentTo(const wxString &label = "ReparentTo");
 ~mmoReparentTo(); 
  
  mafTypeMacro(mmoReparentTo, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

  /** Set the target vme (used to be called without using interface)*/
  void SetTargetVme(mafVME *target);

protected:
	mafVME *m_TargetVme;
  mafVME *m_OldParent;
};
#endif
