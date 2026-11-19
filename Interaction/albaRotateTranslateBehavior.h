/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRotateTranslateBehavior
 Authors: Gianluigi Crimi, from albaGUITransformMouse by Stefano Perticoni

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaRotateTranslateBehavior_h
#define __albaRotateTranslateBehavior_h

#include "albaInteractionBehavior.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;
class albaTransformBehavior;

/** Behavior moving a VME with the mouse: left button rotates, middle button translates,
  left button + CTRL rotates around the view normal.
  CreateTransformBehaviors creates the three transform behaviors (rotate, translate and roll), and the
  button methods choose which one moves the VME; their transforms (ID_TRANSFORM)
  and the other events they send go to the listener of this behavior. The constraints of rotation and
  translation are changed with SetRotationConstraint and SetTranslationConstraint, in the reference
  system of SetRefSysVME (default: the moved VME). */
class ALBA_EXPORT albaRotateTranslateBehavior : public albaInteractionBehavior
{
public:
  albaTypeMacro(albaRotateTranslateBehavior, albaInteractionBehavior);

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
  virtual void CreateTransformBehaviors(albaVME *vme);

  /** Enable/disable the roll with the left button + CTRL (default enabled) */
  void EnableRoll(bool enable) {m_RollEnabled = enable;}

  /** Set the VME giving the reference system of the constraints */
  void SetRefSysVME(albaVME *refSysVME);

  /** Set the rotation constraint, one of TRANSFORM_CONSTRAINT from X_AXIS to NORMAL_VIEW_PLANE */
  void SetRotationConstraint(int constraint);

  /** Set the translation constraint, one of TRANSFORM_CONSTRAINT */
  void SetTranslationConstraint(int constraint);

  albaTransformBehavior *GetRotateBehavior() {return m_Rotate;}
  albaTransformBehavior *GetTranslateBehavior() {return m_Translate;}
  albaTransformBehavior *GetRollBehavior() {return m_Roll;}

  /** The left button rotates, with CTRL it rolls; with SHIFT it is not used.
    With the clicks with modifier enabled, SHIFT and CTRL send ID_CLICK_ON_VME instead.
    Without CreateTransformBehaviors the behavior only sends the clicks with modifier. */
  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2]);

  /** The middle button translates, without modifier keys */
  virtual bool OnMiddleButtonDown(albaView *view, vtkRenderer *ren, double pos[2]);

  /** The release of the button ends the motion */
  virtual bool OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2]) {return Stop(view, ren, pos);}
  virtual bool OnMiddleButtonUp(albaView *view, vtkRenderer *ren, double pos[2]) {return Stop(view, ren, pos);}

  /** The drags go to the transform behavior moving the VME */
  virtual bool OnDrag(albaView *view, vtkRenderer *ren, double pos[2]);

  /** Redefined to forward the events of the transform behaviors to the listener */
  virtual void OnEvent(albaEventBase *event);

protected:
  albaRotateTranslateBehavior();
  virtual ~albaRotateTranslateBehavior();

  /** Create a transform behavior: subclasses can create a specialized one */
  virtual albaTransformBehavior *NewTransformBehavior();

  /** Return the transform behavior of the left button for the current modifier keys, NULL if none */
  virtual albaTransformBehavior *GetLeftButtonTransform();

  /** Start the motion of the transform behavior */
  bool Start(albaTransformBehavior *transform, albaView *view, vtkRenderer *ren, double pos[2]);

  /** Stop the motion, if any */
  bool Stop(albaView *view, vtkRenderer *ren, double pos[2]);

  /** Called on each transform of the transform behaviors, before forwarding it to the listener */
  virtual void OnTransform() {}

  albaTransformBehavior *m_Rotate;    ///< left button
  albaTransformBehavior *m_Translate; ///< middle button
  albaTransformBehavior *m_Roll;      ///< left button + CTRL
  albaTransformBehavior *m_Active;    ///< the transform behavior moving the VME, NULL if none
  bool m_RollEnabled;                 ///< true if the left button + CTRL rolls
  albaVME *m_RefSysVME;                  ///< VME giving the reference system of the constraints

private:
  albaRotateTranslateBehavior(const albaRotateTranslateBehavior&);  // Not implemented.
  void operator=(const albaRotateTranslateBehavior&);  // Not implemented.
};

#endif
