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

/** Behavior moving the prosthesis freely: it creates albaProsthesisTransformBehavior, which rotates
  the VME freely around its pivot, and adapts the rotation constraint to the view: in the slice views
  the rotation is around the view normal. The left button with SHIFT uses a fourth transform behavior
  with the default constraints. */
class ALBA_EXPORT albaProsthesisBehavior : public albaRotateTranslateBehavior
{
public:
  albaTypeMacro(albaProsthesisBehavior, albaRotateTranslateBehavior);

  /** Create also the transform behavior of the left button with SHIFT */
  void CreateTransformBehaviors(albaVME *vme) override;

  albaTransformBehavior *GetShiftTransformBehavior() {return m_ShiftTransform;}

protected:
  albaProsthesisBehavior();
  virtual ~albaProsthesisBehavior();

  /** The left button with SHIFT uses the SHIFT transform behavior */
  albaTransformBehavior *GetLeftButtonTransform() override;

  /** Create the free transform behavior */
  albaTransformBehavior *NewTransformBehavior() override;

  /** Adapt the rotation constraint to the view */
  void OnTransform() override;

  /** Return true for the slice views */
  bool IsSliceView(albaView *v);

  albaTransformBehavior *m_ShiftTransform; ///< left button + SHIFT

private:
  albaProsthesisBehavior(const albaProsthesisBehavior&);  // Not implemented.
  void operator=(const albaProsthesisBehavior&);  // Not implemented.
};

#endif
