/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPERPicker
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractorPERPicker_h
#define __albaInteractorPERPicker_h

#include "albaInteractionTool.h"

/** Tool picking a point with a click of the left button.
  A click without dragging sends VME_PICKED to the listener with the picked point
  (gizmos are not picked); the drags are not used, so they move the camera.
  The button down with SHIFT is not used. A VME with a behavior, e.g. a gizmo,
  comes before the tools, so it keeps working while the picker is active. */
class ALBA_EXPORT albaInteractorPERPicker : public albaInteractionTool
{
public: 
  albaTypeMacro(albaInteractorPERPicker, albaInteractionTool);

  virtual bool OnLeftButtonDown(albaEventInteraction *e);

  /** The drags move the camera, and the button up after a drag does not pick */
  virtual bool OnDrag(albaEventInteraction *e) {m_Dragged = true; return false;}

  /** A click without dragging picks the point */
  virtual bool OnLeftButtonUp(albaEventInteraction *e);

protected:
  albaInteractorPERPicker() {m_Dragged = false;}
  virtual ~albaInteractorPERPicker() {}

  bool m_Dragged; ///< true if the mouse moved after the button down

private:
  albaInteractorPERPicker(const albaInteractorPERPicker&);  // Not implemented.
  void operator=(const albaInteractorPERPicker&);  // Not implemented.
};

#endif 
