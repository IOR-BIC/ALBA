/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpMAFTransform.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:51 $
  Version:   $Revision: 1.2 $
  Authors:   Stefano Perticoni        
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpMAFTransform_H__
#define __mafOpMAFTransform_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafOpTransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoTranslate;
class mafGizmoRotate;
class mafGizmoScale;
class mafGUITransformMouse;
class mafGUISaveRestorePose;
class mafGUITransformTextEntries;
class mafNode;

//----------------------------------------------------------------------------
// mafOpMAFTransform :
//----------------------------------------------------------------------------
/** 
Transform a vme using constrains 
*/

class mafOpMAFTransform : public mafOpTransformInterface
{
public:
  mafOpMAFTransform(const wxString &label = "Transform  \tCtrl+T");
 ~mafOpMAFTransform(); 
  virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(mafOpMAFTransform, mafOp);

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
  void OnEventGizmoScale(mafEventBase *maf_event);
  void OnEventGuiTransform(mafEventBase *maf_event);
  void OnEventGuiSaveRestorePose(mafEventBase *maf_event);
  void OnEventGuiTransformTextEntries(mafEventBase *maf_event);

  void OpStop(int result);

  /** 
  plugged objects */
  mafGizmoTranslate           *m_GizmoTranslate;  
  mafGizmoRotate              *m_GizmoRotate;
  mafGizmoScale               *m_GizmoScale;
  mafGUITransformMouse        *m_GuiTransform;
  mafGUISaveRestorePose       *m_GuiSaveRestorePose;
  mafGUITransformTextEntries  *m_GuiTransformTextEntries;

  double m_RotationStep;
  double m_TranslationStep;
  int    m_EnableStep;

  /** 
  Override superclass*/
  void RefSysVmeChanged();
};
#endif
  


