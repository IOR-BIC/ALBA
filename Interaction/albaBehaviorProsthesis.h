/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaBehaviorProsthesis
 Authors: Gianluigi Crimi, from albaGUITransformMouseFloatVME by Nicola Vanella

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaBehaviorProsthesis_h
#define __albaBehaviorProsthesis_h

#include "albaBehaviorRotateTranslate.h"

/** Behavior moving the prosthesis: the left button rotates freely around the pivot (SetRefSysVME),
  in the slice views and in 2D modality around the view normal; the left button + CTRL rolls,
  the left button + SHIFT and the middle button translate in the view plane. */
class ALBA_EXPORT albaBehaviorProsthesis : public albaBehaviorRotateTranslate
{
public:
  albaTypeMacro(albaBehaviorProsthesis, albaBehaviorRotateTranslate);

  /** In 2D modality the left button rotates around the view normal */
  void Set2DModality(bool mod2d) {m_Modality2D = mod2d;}

protected:
  albaBehaviorProsthesis();
  virtual ~albaBehaviorProsthesis();

  /** The left button + SHIFT translates; the rotation constraint of the left button depends on the view */
  bool PrepareTransform(int button) override;

  /** Free rotation around the pivot, or roll in 2D modality */
  void TrackballRotate() override;

  /** Rotation around the view normal */
  void TrackballRoll() override;

  bool m_Modality2D; ///< true if the left button rotates around the view normal

private:
  albaBehaviorProsthesis(const albaBehaviorProsthesis&);  // Not implemented.
  void operator=(const albaBehaviorProsthesis&);  // Not implemented.
};
#endif
