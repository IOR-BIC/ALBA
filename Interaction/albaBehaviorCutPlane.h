/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaBehaviorCutPlane
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaBehaviorCutPlane_h
#define __albaBehaviorCutPlane_h

#include "albaBehaviorRotateTranslate.h"

/** Behavior of the cut plane of the surface operations (clip, split, boolean, labelize):
  the left button rotates the plane and the middle button translates it, in the view plane, without roll.
  With the clicks with modifier enabled, the left button with SHIFT or CTRL sends ID_CLICK_ON_VME to the
  listener, with ALBA_SHIFT_KEY or ALBA_CTRL_KEY as arg (SHIFT wins). With the transform disabled,
  e.g. when a gizmo moves the plane, the behavior only sends the clicks with modifier. */
class ALBA_EXPORT albaBehaviorCutPlane : public albaBehaviorRotateTranslate
{
public:
  albaTypeMacro(albaBehaviorCutPlane, albaBehaviorRotateTranslate);

  /** Enable/disable the clicks with modifier (default disabled) */
  void EnableModifierClicks(bool enable) {m_ModifierClicks = enable;}

  /** Enable/disable the rotation and translation of the plane (default enabled) */
  void EnableTransform(bool enable) {m_TransformEnabled = enable;}

  /** The left button with SHIFT or CTRL sends ID_CLICK_ON_VME, if the clicks with modifier are enabled */
  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2]);

protected:
  albaBehaviorCutPlane();
  virtual ~albaBehaviorCutPlane();

  /** No transform when disabled */
  virtual bool PrepareTransform(int button);

  bool m_ModifierClicks;   ///< true if the left button with SHIFT or CTRL sends ID_CLICK_ON_VME
  bool m_TransformEnabled; ///< true if the buttons rotate and translate the plane

private:
  albaBehaviorCutPlane(const albaBehaviorCutPlane&);  // Not implemented.
  void operator=(const albaBehaviorCutPlane&);  // Not implemented.
};
#endif
