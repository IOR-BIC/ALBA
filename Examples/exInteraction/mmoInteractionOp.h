/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoInteractionOp.h,v $
  Language:  C++
  Date:      $Date: 2006-07-07 09:18:05 $
  Version:   $Revision: 1.3 $
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
                mmoInteractionOp(wxString label);
               ~mmoInteractionOp();
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
