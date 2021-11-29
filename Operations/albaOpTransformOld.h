/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTransformOld
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpTransformOld_H__
#define __albaOpTransformOld_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
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
// albaOpTransformOld :
//----------------------------------------------------------------------------
/** 
Transform a vme using constrains 
*/

class ALBA_EXPORT albaOpTransformOld : public albaOpTransformInterface
{
public:
  albaOpTransformOld(const wxString &label = "Transform  \tCtrl+T");
 ~albaOpTransformOld(); 
  virtual void OnEvent(albaEventBase *alba_event);
  
  albaTypeMacro(albaOpTransformOld, albaOp);

  albaOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(albaVME* vme);

  /** Builds operation's interface. */
  void OpRun();
  
  /** Execute the operation. */
  void OpDo();
  
  /** Makes the undo for the operation. */
  void OpUndo();
	
  void Reset();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:
  /** Create the gui */
  virtual void CreateGui();
  
  void OnEventThis(albaEventBase *alba_event);  
  void OnEventGizmoTranslate(albaEventBase *alba_event);
  void OnEventGizmoRotate(albaEventBase *alba_event);
  void OnEventGizmoScale(albaEventBase *alba_event);
  void OnEventGuiTransform(albaEventBase *alba_event);
  void OnEventGuiSaveRestorePose(albaEventBase *alba_event);
  void OnEventGuiTransformTextEntries(albaEventBase *alba_event);

  void OpStop(int result);

  /** 
  plugged objects */
  albaGizmoTranslate           *m_GizmoTranslate;  
  albaGizmoRotate              *m_GizmoRotate;
  albaGizmoScale               *m_GizmoScale;
  albaGUITransformMouse        *m_GuiTransform;
  albaGUISaveRestorePose       *m_GuiSaveRestorePose;
  albaGUITransformTextEntries  *m_GuiTransformTextEntries;

  double m_RotationStep;
  double m_TranslationStep;
  int    m_EnableStep;

  /** 
  Override superclass*/
  void RefSysVmeChanged();

  /** test friend */
  friend class albaOpTransformOldTest;
};
#endif
  


