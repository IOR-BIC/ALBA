/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpReparentTo
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpReparentTo_H__
#define __albaOpReparentTo_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVME;
class albaEvent;
class albaGUI;

//----------------------------------------------------------------------------
// albaOpReparentTo :
//----------------------------------------------------------------------------
/** */

static albaVME *glo_VmeForReparent;

class ALBA_EXPORT albaOpReparentTo: public albaOp
{
public:
  albaOpReparentTo(const wxString &label = "ReparentTo");
 ~albaOpReparentTo(); 
  
  albaTypeMacro(albaOpReparentTo, albaOp);

  albaOp* Copy();

	/** Perform validation on the vme chooser */
	static bool VMEAcceptForReparent(albaVME *vme);

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	static int ReparentTo(albaVME * input, albaVME * targetVme, albaVME * oldParent);

	/** Makes the undo for the operation. */
  void OpUndo();

  /** Set the target vme (used to be called without using interface)*/
  void SetTargetVme(albaVME *target);

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	albaVME *m_TargetVme;
  albaVME *m_OldParent;
};
#endif
