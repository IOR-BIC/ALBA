/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoReparentTo.h,v $
  Language:  C++
  Date:      $Date: 2006-09-22 10:11:57 $
  Version:   $Revision: 1.2 $
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
  mmoReparentTo(wxString label = "ReparentTo");
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
