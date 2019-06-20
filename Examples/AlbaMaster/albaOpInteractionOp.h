/*=========================================================================
Program:   AlbaMaster
Module:    albaOpInteractionOp.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpInteraction_H__
#define __albaOpInteraction_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaVME;
class albaVMEGizmo;
class mmiCompositorMouse;

// ============================================================================
class albaOpInteractionOp : public albaOp
{
public:
	albaOpInteractionOp(wxString label);
	~albaOpInteractionOp();
	virtual void OnEvent(albaEventBase *e);
  albaOp* Copy();

  bool Accept(albaVME* vme) {return true;};
  void OpRun();
  void OpDo();
  void OpUndo();

/******************************************************************************/
/** UNCOMMENT HERE TO DECLARE THE ACTIONS                  (PASS 1)           */
/* *
  static const char *m_Actions[];
  virtual const char ** GetActions();
/******************************************************************************/

protected:
  void OpStop(int result);
  
  albaVMEGizmo         *m_Gizmo;
  mmiCompositorMouse  *m_MouseInteractor;

};
#endif // __albaOpInteraction_H__








