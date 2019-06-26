/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformMouse
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUITransformMouse_H__
#define __albaGUITransformMouse_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaObserver.h"
#include "albaGUITransformInterface.h"
#include "albaInteractorCompositorMouse.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUI;
class albaGUIButton;
class albaInteractorGenericMouse;
class albaInteractor;
class albaVME;

//----------------------------------------------------------------------------
/** transform gui

  @sa
 
  @todo

*/
class ALBA_EXPORT albaGUITransformMouse : public albaGUITransformInterface
{
public:
  albaGUITransformMouse(albaVME *input, albaObserver *listener = NULL, bool testMode = false);
	~albaGUITransformMouse(); 

  // constraints enum
  enum TRANSFORM_MOUSE_WIDGET_ID
  {
    X_AXIS = 0,
    Y_AXIS, 
    Z_AXIS,
    VIEW_PLANE, 
    NORMAL_VIEW_PLANE,
    XY_PLANE, 
    XZ_PLANE, 
    YZ_PLANE,
    SURFACE_SNAP,
    NORMAL_SURFACE,
  };

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

  /** Start and stop interaction through this object isa */
  void AttachInteractorToVme();
  void DetachInteractorFromVme();

  void SetRotationConstraintId(int value){m_RotationConstraintId = value;};
  void SetTranslationConstraintId(int value){m_TranslationConstraintId = value;};

  albaInteractorGenericMouse* CreateBehavior(MMI_ACTIVATOR activator);

  albaInteractorGenericMouse* GetIsaRotate();

protected:  
  albaInteractor* m_OldInteractor;
   
  /** Create interactors */
  void CreateISA();

  /** Override superclass */
  void CreateGui();

  albaInteractorCompositorMouse *m_IsaCompositor;

  albaInteractorGenericMouse *m_IsaRotate;
  albaInteractorGenericMouse *m_IsaTranslate;
  albaInteractorGenericMouse *m_IsaRoll;

  int m_RotationConstraintId;
	int m_TranslationConstraintId;
  
  /** Override superclass */;
  void RefSysVmeChanged();

  friend class albaGUITransformMouseTest;
  
};
#endif
