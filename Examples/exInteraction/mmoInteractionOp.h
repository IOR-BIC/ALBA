/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoInteractionOp.h,v $
  Language:  C++
  Date:      $Date: 2007-01-30 14:56:34 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmoInteraction_H__
#define __mmoInteraction_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafOp.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafVME;
class mafVMEGizmo;
class mmiCompositorMouse;

// ============================================================================
class mmoInteractionOp : public mafOp
// ============================================================================
{
public:
  mmoInteractionOp(wxString label="InteractionOp");
  ~mmoInteractionOp();
  
  mafTypeMacro(mmoInteractionOp, mafOp);

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
  mmiCompositorMouse  *m_MouseInteractor;

};
#endif // __mmoInteraction_H__
