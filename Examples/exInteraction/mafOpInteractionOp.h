/*=========================================================================

 Program: MAF2
 Module: mafOpInteractionOp
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafOpInteraction_H__
#define __mafOpInteraction_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafOp.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafVME;
class mafVMEGizmo;
class mafInteractorCompositorMouse;

// ============================================================================
class __declspec( dllexport ) mafOpInteractionOp : public mafOp
// ============================================================================
{
public:
  mafOpInteractionOp(wxString label="InteractionOp");
  ~mafOpInteractionOp();
  
  mafTypeMacro(mafOpInteractionOp, mafOp);

  virtual void OnEvent(mafEventBase *e);
  mafOp* Copy();

  bool Accept(mafVME* vme) {return true;};
  void OpRun();
  void OpDo();
  void OpUndo();

/******************************************************************************/
/** UNCOMMENT HERE TO DECLARE THE ACTIONS                  (PASS 1)           */
/* */
  static const char *m_Actions[];
  virtual const char ** GetActions();
/******************************************************************************/

protected:
  void OpStop(int result);
  
  mafVMEGizmo         *m_Gizmo;
  mafInteractorCompositorMouse  *m_MouseInteractor;

};
#endif // __mafOpInteraction_H__
