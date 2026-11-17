/*=========================================================================

 Program: ALBA
 Module: albaCompositorMouseFreeTransformBehavior
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaCompositorMouseFreeTransformBehavior_h
#define __albaCompositorMouseFreeTransformBehavior_h

#include "albaCompositorMouseBehavior.h"

//----------------------------------------------------------------------------
/** Mouse compositor of the free transform of a VME (albaGUITransformMouseFree, used by the prosthesis):
  it is a albaCompositorMouseBehavior creating albaGenericMouseFreeTransformBehavior, which rotates
  the VME freely around its pivot, instead of albaGenericMouseBehavior. */
class ALBA_EXPORT albaCompositorMouseFreeTransformBehavior : public albaCompositorMouseBehavior
{
public:
  albaTypeMacro(albaCompositorMouseFreeTransformBehavior, albaCompositorMouseBehavior);

protected:
  /** Create the free transform generic mouse */
  albaGenericMouseBehavior *NewGenericMouse() override;
};
#endif

