/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpInteractionOp.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 09:55:16 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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

  bool Accept(mafNode* vme) {return true;};
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
