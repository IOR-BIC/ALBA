/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSelectPointTool
 Authors: Gianluigi Crimi, from albaInteractorSelectPoint by Matteo Giacomoni, Stefano Perticoni

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaSelectPointTool_h
#define __albaSelectPointTool_h

#include "albaInteractionTool.h"

/** Tool picking the points of the render window of its renderer.
  Left button picks a point, with CTRL if the CTRL modifier is on (default), and sends
  albaEvent(this, VME_PICKED) with the picked point (GetVtkObj, a vtkPoints) and the point id (GetArg).
  While picking the camera does not move; otherwise the tool does not use the button and the camera moves. */
class ALBA_EXPORT albaSelectPointTool : public albaInteractionTool
{
public:
  albaTypeMacro(albaSelectPointTool, albaInteractionTool);

  /** Pick a point */
  bool OnLeftButtonDown(albaEventInteraction *e) override;

  /** End picking */
  bool OnLeftButtonUp(albaEventInteraction *e) override;

  /** While picking the camera does not move */
  bool OnDrag(albaEventInteraction *e) override;

  /** Set the CTRL modifier to On => picking performed on LeftButtonDown + CTRL */
  void SetCtrlModifierOn() {m_UseCtrlModifier = true;}

  /** Set the CTRL modifier to Off => pick performed on Left button down*/
  void SetCtrlModifierOff() {m_UseCtrlModifier = false;}

  /** Set the CTRL modifier: true => pick on Ctrl + LeftButtonDown, false => pick on LeftButtonDown only */
  void SetCtrlModifier(bool useCtrlModifier) {m_UseCtrlModifier = useCtrlModifier;}

  /** Get the CTRL modifier */
  bool GetCtrlModifier() {return m_UseCtrlModifier;}

protected:
  albaSelectPointTool();
  virtual ~albaSelectPointTool() {}

  /** Pick the point under the mouse and send it to the listener */
  void PickPoint(albaEventInteraction *e);

  bool m_IsPicking;       ///< true while the picking button is down
  bool m_UseCtrlModifier; ///< true if the picking needs CTRL

private:
  albaSelectPointTool(const albaSelectPointTool&);  // Not implemented.
  void operator=(const albaSelectPointTool&);  // Not implemented.
};

#endif
