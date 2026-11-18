/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRotateTranslateGUI
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaRotateTranslateGUI_H__
#define __albaRotateTranslateGUI_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaTransformInterfaceGUI.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaInteractionBehavior;
class albaRotateTranslateBehavior;
class albaVME;

/** GUI of the transform mouse behavior: it creates an albaRotateTranslateBehavior for the VME, assigns it to
  the VME while the widgets are enabled, and lets the user choose the rotation and translation constraints.
  The transforms of the behavior are forwarded to the listener (ID_TRANSFORM, sent by this object). */
class ALBA_EXPORT albaRotateTranslateGUI : public albaTransformInterfaceGUI
{
public:
  albaRotateTranslateGUI(albaVME *input, albaObserver *listener = NULL, bool testMode = false);
	~albaRotateTranslateGUI();

  void OnEvent(albaEventBase *alba_event);

  /** Override superclass */
	void EnableWidgets(bool enable);

  //----------------------------------------------------------------------------
  //gui constants:
  //----------------------------------------------------------------------------
  // this constants must be visible from the owner object
  enum GUI_TRANSFORM_MOUSE_ID
  {
      ID_ROTATION_AXES = MINID,
      ID_TRASLATION_AXES,
  };

  /** Assign the behavior to the VME, or give the VME its previous behavior again */
  void AttachInteractorToVme();
  void DetachInteractorFromVme();

  /** Set the constraints, as albaRotateTranslateBehavior::TRANSFORM_CONSTRAINT */
  void SetRotationConstraintId(int value){m_RotationConstraintId = value;};
  void SetTranslationConstraintId(int value){m_TranslationConstraintId = value;};

  /** Return the behavior moving the VME */
  albaRotateTranslateBehavior *GetBehavior() {return m_Behavior;}

protected:
  /** Override superclass */
  void CreateGui();

  /** Override superclass */
  void RefSysVmeChanged();

  albaRotateTranslateBehavior *m_Behavior;  ///< the behavior moving the VME
  albaInteractionBehavior *m_OldBehavior;  ///< the behavior of the VME before this one

  int m_RotationConstraintId;
	int m_TranslationConstraintId;

  friend class albaRotateTranslateGUITest;
};
#endif
