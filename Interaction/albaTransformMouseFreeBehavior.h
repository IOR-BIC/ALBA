/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformMouseFreeBehavior
 Authors: Gianluigi Crimi, from albaGUITransformMouseFloatVME by Nicola Vanella

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaTransformMouseFreeBehavior_h
#define __albaTransformMouseFreeBehavior_h

#include "albaTransformMouseBehavior.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaView;

/** Transform mouse behavior moving a VME freely (used by the prosthesis): it creates
  albaGenericMouseFreeTransformBehavior, which rotates the VME freely around its pivot, and adapts the
  rotation constraint to the view: in the slice views the rotation is around the view normal. */
class ALBA_EXPORT albaTransformMouseFreeBehavior : public albaTransformMouseBehavior
{
public:
  albaTypeMacro(albaTransformMouseFreeBehavior, albaTransformMouseBehavior);

protected:
  albaTransformMouseFreeBehavior() {}
  virtual ~albaTransformMouseFreeBehavior() {}

  /** Create the free transform generic mouse */
  albaGenericMouseBehavior *NewGenericMouse() override;

  /** Adapt the rotation constraint to the view */
  void OnTransform() override;

  /** Return true for the slice views */
  bool IsSliceView(albaView *v);

private:
  albaTransformMouseFreeBehavior(const albaTransformMouseFreeBehavior&);  // Not implemented.
  void operator=(const albaTransformMouseFreeBehavior&);  // Not implemented.
};

#endif
