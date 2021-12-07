/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMove
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpMove_H__
#define __albaOpMove_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOpTransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGizmoTranslate;
class albaGizmoRotate;
class albaGizmoScale;
class albaGUITransformMouse;
class albaGUISaveRestorePose;
class albaGUITransformTextEntries;
class albaVME;

//----------------------------------------------------------------------------
// albaOpMove :
//----------------------------------------------------------------------------
/** 
Transform a vme using constrains 
*/

class ALBA_EXPORT albaOpMove : public albaOpTransformInterface
{
public:
  albaOpMove(const wxString &label = "Move\tCtrl+T");
 ~albaOpMove(); 
  virtual void OnEvent(albaEventBase *alba_event);
  
  albaTypeMacro(albaOpMove, albaOp);

  albaOp* Copy();

  /** Builds operation's interface. */
  void OpRun();
  
  /** Execute the operation. */
  void OpDo();
  
  /** Makes the undo for the operation. */
  void OpUndo();
	
  void Reset();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** Create the gui */
  virtual void CreateGui();
  
  void OnEventThis(albaEventBase *alba_event);  
  void OnEventGizmoTranslate(albaEventBase *alba_event);
  void OnEventGizmoRotate(albaEventBase *alba_event);
  void OnEventGuiTransformMouse(albaEventBase *alba_event);
  void OnEventGuiSaveRestorePose(albaEventBase *alba_event);
  void OnEventGuiTransformTextEntries(albaEventBase *alba_event);

  void OpStop(int result);

  /** 
  plugged objects */
  albaGizmoTranslate           *m_GizmoTranslate;  
  albaGizmoRotate              *m_GizmoRotate;
  albaGUITransformMouse        *m_GuiTransformMouse;
  albaGUISaveRestorePose       *m_GuiSaveRestorePose;
  albaGUITransformTextEntries  *m_GuiTransformTextEntries;

  double m_RotationStep;
  double m_TranslationStep;
  int    m_EnableStep;

  /** 
  Override superclass*/
  void RefSysVmeChanged();
};
#endif
  


