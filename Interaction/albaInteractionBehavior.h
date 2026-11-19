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

  albaAbstractTypeMacro(albaInteractionBehavior, albaInteractionTool);

  /** Set the VME moved by the behavior */
  virtual void SetVME(albaVME *vme);

  /** Set the picked prop (optional and to be removed!!!)*/
  void SetProp(vtkProp3D *prop);

  /** Set the position picked on the VME by the button down */
  void SetPickedPosition(double pos[3]) {m_PickedPosition[0] = pos[0]; m_PickedPosition[1] = pos[1]; m_PickedPosition[2] = pos[2];}

  /** Enable/disable the clicks with modifier: the left button with SHIFT or CTRL sends ID_CLICK_ON_VME
    to the listener instead of being handled by the behavior, with ALBA_SHIFT_KEY or ALBA_CTRL_KEY
    as arg (SHIFT wins); default disabled */
  void EnableModifierClicks(bool enable) {m_ModifierClicks = enable;}

protected:
  /** Send ID_CLICK_ON_VME if the clicks with modifier are enabled and SHIFT or CTRL is down; return true if sent */
  bool SendModifierClick();

  albaVME       *m_VME;               ///< the VME moved by the behavior (optional)
  vtkALBASmartPointer<vtkProp3D> m_Prop; ///< the picked prop (optional)
  double m_PickedPosition[3];         ///< the position picked on the VME by the button down
  bool m_ModifierClicks;              ///< true if the left button with SHIFT or CTRL sends ID_CLICK_ON_VME

private:
  albaInteractionBehavior(const albaInteractionBehavior&);  // Not implemented.
  void operator=(const albaInteractionBehavior&);  // Not implemented.
};
#endif
