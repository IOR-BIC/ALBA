/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolPicker
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaToolPicker_h
#define __albaToolPicker_h

#include "albaInteractionTool.h"
#include <vector>

class albaVME;

/** Tool picking a point with the left button.
  A click without dragging sends VME_PICKED to the listener with the picked point and VME;
  the drags are not used, so they move the camera.
  With continuous picking the button down picks and sends VME_PICKED, the drags send VME_PICKING
  and do not move the camera, and the button up sends VME_PICKED.
  With no target VME every VME except the gizmos is picked; with target VMEs only those are picked,
  and the button down on another VME or on the background is left to the PER.
  The button down with SHIFT is not used. A VME with a behavior, e.g. a gizmo,
  comes before the tools, so it keeps working while the picker is active. */
class ALBA_EXPORT albaToolPicker : public albaInteractionTool
{
public: 
  albaTypeMacro(albaToolPicker, albaInteractionTool);

  /** Add a VME to the picked ones */
  void AddTargetVME(albaVME *vme);

  /** Remove a VME from the picked ones */
  void RemoveTargetVME(albaVME *vme);

  /** Remove all the target VMEs: every VME except the gizmos is picked */
  void RemoveAllTargetVMEs() {m_TargetVMEs.clear();}

  /** Enable/disable continuous picking: pick at the button down, during the drags and at the button up */
  void EnableContinuousPicking(bool enable) {m_ContinuousPicking = enable;}
  bool IsContinuousPicking() {return m_ContinuousPicking;}

protected:
  albaToolPicker();
  virtual ~albaToolPicker() {}

  virtual bool OnLeftButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);

  /** The drags move the camera, and the button up after a drag does not pick; with continuous picking they pick */
  virtual bool OnDragImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);

  /** A click without dragging picks the point; with continuous picking the button up picks */
  virtual bool OnLeftButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);

  /** Return true if the VME is a target, or with no target VME if it is not a gizmo */
  bool IsPickable(albaVME *vme);

  std::vector<albaVME *> m_TargetVMEs; ///< the picked VMEs, all the non-gizmo VMEs if empty
  bool m_ContinuousPicking; ///< pick at the button down, during the drags and at the button up
  bool m_Picking; ///< a continuous picking started on a pickable VME
  bool m_Dragged; ///< true if the mouse moved after the button down

private:
  albaToolPicker(const albaToolPicker&);  // Not implemented.
  void operator=(const albaToolPicker&);  // Not implemented.
};

#endif 
