/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaBehaviorRotateTranslate
 Authors: Gianluigi Crimi, from albaGUITransformMouse by Stefano Perticoni

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaBehaviorRotateTranslate_h
#define __albaBehaviorRotateTranslate_h

#include "albaBehaviorTransform.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;

/** Behavior moving a VME with the mouse: left button rotates, middle button translates,
  left button + CTRL rotates around the view normal (roll); SHIFT and the middle button + CTRL are not used.
  SetVME sets the default transforms: rotation and translation in the view plane, around the origin of the VME.
  The constraints of rotation and translation are changed with SetRotationConstraint and SetTranslationConstraint,
  in the reference system of SetRefSysVME (default: the moved VME). */
class ALBA_EXPORT albaBehaviorRotateTranslate : public albaBehaviorTransform
{
public:
  albaTypeMacro(albaBehaviorRotateTranslate, albaBehaviorTransform);

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

  /** Set the moved VME and the default transforms */
  virtual void SetVME(albaVME *vme);

  /** Enable/disable the roll with the left button + CTRL (default enabled) */
  void EnableRoll(bool enable) {m_RollEnabled = enable;}

  /** Set the VME giving the reference system of the constraints and the pivot */
  void SetRefSysVME(albaVME *refSysVME);

  /** Set the rotation constraint, one of TRANSFORM_CONSTRAINT from X_AXIS to NORMAL_VIEW_PLANE */
  void SetRotationConstraint(int constraint);

  /** Set the translation constraint, one of TRANSFORM_CONSTRAINT */
  void SetTranslationConstraint(int constraint);

  /** Redefined to set the renderer also in the roll constraint */
  virtual void SetRendererAndView(vtkRenderer *ren, albaView *view);

protected:
  albaBehaviorRotateTranslate();
  virtual ~albaBehaviorRotateTranslate();

  /** Left button: rotation, with CTRL roll; middle button: translation */
  virtual bool PrepareTransform(int button);

  albaInteractorConstraint *m_RollConstraint; ///< rotation constraint of the left button + CTRL: around the view normal
  bool m_RollEnabled;                         ///< true if the left button + CTRL rolls
  albaVME *m_RefSysVME;                       ///< VME giving the reference system of the constraints and the pivot

private:
  albaBehaviorRotateTranslate(const albaBehaviorRotateTranslate&);  // Not implemented.
  void operator=(const albaBehaviorRotateTranslate&);  // Not implemented.
};
#endif
