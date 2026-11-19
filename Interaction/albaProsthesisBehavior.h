/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaProsthesisBehavior
 Authors: Gianluigi Crimi, from albaGUITransformMouseFloatVME by Nicola Vanella

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaProsthesisBehavior_h
#define __albaProsthesisBehavior_h

#include "albaRotateTranslateBehavior.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaView;

/** Behavior moving the prosthesis: the left button rotates freely around the pivot (SetRefSysVME),
  in the slice views and in 2D modality around the view normal; the left button + CTRL rolls,
  the left button + SHIFT and the middle button translate in the view plane. */
class ALBA_EXPORT albaProsthesisBehavior : public albaRotateTranslateBehavior
{
public:
  albaTypeMacro(albaProsthesisBehavior, albaRotateTranslateBehavior);

  /** In 2D modality the left button rotates around the view normal */
  void Set2DModality(bool mod2d) {m_Modality2D = mod2d;}

protected:
  albaProsthesisBehavior();
  virtual ~albaProsthesisBehavior();

  /** The left button + SHIFT translates; the rotation constraint of the left button depends on the view */
  bool PrepareTransform(int button) override;

  /** Free rotation around the pivot, or roll in 2D modality */
  void TrackballRotate() override;

  /** Rotation around the view normal */
  void TrackballRoll() override;

  /** Return true for the slice views */
  bool IsSliceView(albaView *v);

  bool m_Modality2D; ///< true if the left button rotates around the view normal

private:
  albaProsthesisBehavior(const albaProsthesisBehavior&);  // Not implemented.
  void operator=(const albaProsthesisBehavior&);  // Not implemented.
};
#endif
