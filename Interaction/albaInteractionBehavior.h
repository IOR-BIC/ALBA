/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionBehavior
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractionBehavior_h
#define __albaInteractionBehavior_h

#include "albaInteractionTool.h"
#include "vtkALBASmartPointer.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;
class vtkProp3D;

/** Base class of the behaviors: a behavior is the interaction tool of a VME.
  The PER calls its button, drag and wheel methods (see albaInteractionTool) when a button
  is pressed on the VME, and keeps sending the drags and the button up to it until that button
  is released. A method returning false leaves the button to the camera.
  Before the button down the PER sets the prop and the picked position; the VME of the behavior
  is set by its owner, e.g. a gizmo behavior transforms the input VME of the gizmo.
  Behaviors send their output events to the listener. */
class ALBA_EXPORT albaInteractionBehavior : public albaInteractionTool
{
public:
  albaInteractionBehavior();
  virtual ~albaInteractionBehavior();

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  ALBA_ID_DEC(BUTTON_DOWN);         ///< Sent by albaTransformBehavior at the start of a transform
  ALBA_ID_DEC(BUTTON_UP);           ///< Sent by albaTransformBehavior at the end of a transform
  ALBA_ID_DEC(MODIFIER_CLICK);      ///< Left button with SHIFT or CTRL, when enabled; arg: ALBA_SHIFT_KEY or ALBA_CTRL_KEY (SHIFT wins)

  albaAbstractTypeMacro(albaInteractionBehavior, albaInteractionTool);

  /** Set the VME moved by the behavior */
  virtual void SetVME(albaVME *vme);

  /** Set the picked prop (optional and to be removed!!!)*/
  void SetProp(vtkProp3D *prop);

  /** Set the position picked on the VME by the button down */
  void SetPickedPosition(double pos[3]) {m_PickedPosition[0] = pos[0]; m_PickedPosition[1] = pos[1]; m_PickedPosition[2] = pos[2];}

  /** Enable/disable the clicks with modifier: the left button with SHIFT or CTRL sends MODIFIER_CLICK
    to the listener instead of being handled by the behavior (default disabled) */
  void EnableModifierClicks(bool enable) {m_ModifierClicks = enable;}

  /** Set the button and the modifiers reported by the BUTTON_DOWN and BUTTON_UP events of albaTransformBehavior */
  void SetStartButton(int btn) {m_StartButton = btn;}
  int GetStartButton() {return m_StartButton;}
  void SetModifiers(int mod) {m_Modifiers = mod;}
  int GetModifiers() {return m_Modifiers;}

protected:
  /** Send MODIFIER_CLICK if the clicks with modifier are enabled and SHIFT or CTRL is down; return true if sent */
  bool SendModifierClick();

  int           m_StartButton;        ///< the button reported by BUTTON_DOWN and BUTTON_UP
  unsigned long m_Modifiers;          ///< the modifiers reported by BUTTON_DOWN and BUTTON_UP
  albaVME       *m_VME;               ///< the VME moved by the behavior (optional)
  vtkALBASmartPointer<vtkProp3D> m_Prop; ///< the picked prop (optional)
  double m_PickedPosition[3];         ///< the position picked on the VME by the button down
  bool m_ModifierClicks;              ///< true if the left button with SHIFT or CTRL sends MODIFIER_CLICK

private:
  albaInteractionBehavior(const albaInteractionBehavior&);  // Not implemented.
  void operator=(const albaInteractionBehavior&);  // Not implemented.
};
#endif
