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
class albaBehaviorRotateTranslate;

/** GUI of the transform mouse behavior: it lets the user choose the rotation and translation constraints
  and the reference system of an albaBehaviorRotateTranslate created by the operation.
  The operation assigns the behavior to the VME and receives its transforms. */
class ALBA_EXPORT albaRotateTranslateGUI : public albaTransformInterfaceGUI
{
public:
  albaRotateTranslateGUI(albaBehaviorRotateTranslate *behavior, albaObserver *listener = NULL, bool testMode = false);
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

  /** Set the constraints, as albaBehaviorRotateTranslate::TRANSFORM_CONSTRAINT */
  void SetRotationConstraintId(int value){m_RotationConstraintId = value;};
  void SetTranslationConstraintId(int value){m_TranslationConstraintId = value;};

  /** Return the behavior configured by this GUI */
  albaBehaviorRotateTranslate *GetBehavior() {return m_Behavior;}

protected:
  /** Override superclass */
  void CreateGui();

  /** Override superclass */
  void RefSysVmeChanged();

  albaBehaviorRotateTranslate *m_Behavior;  ///< the behavior configured by this GUI, owned by the operation

  int m_RotationConstraintId;
	int m_TranslationConstraintId;

  friend class albaRotateTranslateGUITest;
};
#endif
