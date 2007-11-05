/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpMove.h,v $
  Language:  C++
  Date:      $Date: 2007-11-05 10:31:24 $
  Version:   $Revision: 1.3 $
  Authors:   Stefano Perticoni        
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpMove_H__
#define __medOpMove_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mmoTransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoTranslate;
class mafGizmoRotate;
class mafGizmoScale;
class mafGuiTransformMouse;
class mafGuiSaveRestorePose;
class mafGuiTransformTextEntries;
class mafNode;

//----------------------------------------------------------------------------
// medOpMove :
//----------------------------------------------------------------------------
/** 
Transform a vme using constrains 
*/

class medOpMove : public mmoTransformInterface
{
public:
  medOpMove(const wxString &label = "Move\tCtrl+T");
 ~medOpMove(); 
  virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(medOpMove, mafOp);

  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme);

  /** Builds operation's interface. */
  void OpRun();
  
  /** Execute the operation. */
  void OpDo();
  
  /** Makes the undo for the operation. */
  void OpUndo();
	
  void Reset();

protected:
  /** Create the gui */
  virtual void CreateGui();
  
  void OnEventThis(mafEventBase *maf_event);  
  void OnEventGizmoTranslate(mafEventBase *maf_event);
  void OnEventGizmoRotate(mafEventBase *maf_event);
  void OnEventGuiTransformMouse(mafEventBase *maf_event);
  void OnEventGuiSaveRestorePose(mafEventBase *maf_event);
  void OnEventGuiTransformTextEntries(mafEventBase *maf_event);

  void OpStop(int result);

  /** 
  plugged objects */
  mafGizmoTranslate           *m_GizmoTranslate;  
  mafGizmoRotate              *m_GizmoRotate;
  mafGuiTransformMouse        *m_GuiTransformMouse;
  mafGuiSaveRestorePose       *m_GuiSaveRestorePose;
  mafGuiTransformTextEntries  *m_GuiTransformTextEntries;

  double m_RotationStep;
  double m_TranslationStep;
  int    m_EnableStep;

  /** 
  Override superclass*/
  void RefSysVmeChanged();
};
#endif
  


