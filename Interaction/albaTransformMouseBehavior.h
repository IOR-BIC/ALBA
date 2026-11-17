/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformMouseBehavior
 Authors: Gianluigi Crimi, from albaGUITransformMouse by Stefano Perticoni

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaTransformMouseBehavior_h
#define __albaTransformMouseBehavior_h

#include "albaCompositorMouseBehavior.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;

/** Behavior moving a VME with the mouse: left button rotates, middle button translates,
  left button + CTRL rotates around the view normal.
  CreateTransformBehaviors creates the three generic mouse behaviors; their transforms (ID_TRANSFORM)
  and the other events they send go to the listener of this behavior. The constraints of rotation and
  translation are changed with SetRotationConstraint and SetTranslationConstraint, in the reference
  system of SetRefSysVME (default: the moved VME). */
class ALBA_EXPORT albaTransformMouseBehavior : public albaCompositorMouseBehavior
{
public:
  albaTypeMacro(albaTransformMouseBehavior, albaCompositorMouseBehavior);

  /** Constraints of rotation and translation */
  enum TRANSFORM_CONSTRAINT
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

  /** Create the rotate, translate and roll behaviors moving the VME */
  void CreateTransformBehaviors(albaVME *vme);

  /** Set the VME giving the reference system of the constraints */
  void SetRefSysVME(albaVME *refSysVME);

  /** Set the rotation constraint, one of TRANSFORM_CONSTRAINT from X_AXIS to NORMAL_VIEW_PLANE */
  void SetRotationConstraint(int constraint);

  /** Set the translation constraint, one of TRANSFORM_CONSTRAINT */
  void SetTranslationConstraint(int constraint);

  albaGenericMouseBehavior *GetRotateBehavior() {return m_Rotate;}
  albaGenericMouseBehavior *GetTranslateBehavior() {return m_Translate;}
  albaGenericMouseBehavior *GetRollBehavior() {return m_Roll;}

  /** Redefined to forward the events of the generic mouse behaviors to the listener */
  virtual void OnEvent(albaEventBase *event);

protected:
  albaTransformMouseBehavior();
  virtual ~albaTransformMouseBehavior() {}

  /** Called on each transform of the generic mouse behaviors, before forwarding it to the listener */
  virtual void OnTransform() {}

  albaGenericMouseBehavior *m_Rotate;    ///< left button
  albaGenericMouseBehavior *m_Translate; ///< middle button
  albaGenericMouseBehavior *m_Roll;      ///< left button + CTRL
  albaVME *m_RefSysVME;                  ///< VME giving the reference system of the constraints

private:
  albaTransformMouseBehavior(const albaTransformMouseBehavior&);  // Not implemented.
  void operator=(const albaTransformMouseBehavior&);  // Not implemented.
};

#endif
