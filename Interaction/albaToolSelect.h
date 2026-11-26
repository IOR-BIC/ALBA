/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolSelect
 Authors: Gianluigi Crimi, from albaPositionalEventRouter

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaToolSelect_h
#define __albaToolSelect_h

#include "albaInteractionTool.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;

/** Default actions of the clicks on the views, owned by the PER like the camera tool.
  The PER calls it on each button down not used by the tools, before the behavior of the picked
  VME and the camera, which receive the same click; before the call it sets the VME picked by the
  click (NULL if none) and the picked position. Gizmos are never selected.
  - a button down on a VME asks the logic manager to select it (for a landmark cloud, its landmark
    nearest to the picked position), if the selection is enabled;
  - the right button released without dragging asks the logic manager to show the context menu of
    the view, with the VME items if the picked VME can be selected. */
class ALBA_EXPORT albaToolSelect : public albaInteractionTool
{
public:
  albaTypeMacro(albaToolSelect, albaInteractionTool);

  /** Set the VME picked by the button down (NULL if none) and the picked position */
  void SetPickedVME(albaVME *vme, double pos[3]);

  /** Return the VME picked by the last button down, NULL if none */
  albaVME *GetPickedVME() {return m_PickedVME;}

  /** Enable/disable the selection (default enabled) */
  void EnableSelect(bool enable) {m_CanSelect = enable;}
  bool IsSelectEnabled() {return m_CanSelect;}

  /** The button down selects the picked VME; the right button prepares the context menu.
    Return true if the picked VME has been selected. */
  virtual bool OnLeftButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return Select();}
  virtual bool OnMiddleButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return Select();}
  virtual bool OnRightButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);

  /** A drag cancels the context menu */
  virtual bool OnDrag(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);

  /** The right button released without dragging shows the context menu of the view; return true if shown */
  virtual bool OnRightButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);

protected:
  albaToolSelect();
  virtual ~albaToolSelect() {}

  /** Select the picked VME through the logic manager, if the selection is enabled; return true if selected */
  bool Select();

  /** Return true if the picked VME can be selected: not NULL and not a gizmo */
  bool IsPickedVmeSelectable();

  albaVME *m_PickedVME;          ///< the VME picked by the last button down, NULL if none
  double   m_PickedPosition[3];  ///< the position picked by the last button down
  bool     m_CanSelect;          ///< true if the button down selects the picked VME
  bool     m_ShowContextMenu;    ///< true from the right button down to a drag or to the release

private:
  albaToolSelect(const albaToolSelect&);  // Not implemented.
  void operator=(const albaToolSelect&);  // Not implemented.
};
#endif
