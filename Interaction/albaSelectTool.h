/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSelectTool
 Authors: Gianluigi Crimi, from albaPositionalEventRouter

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaSelectTool_h
#define __albaSelectTool_h

#include "albaInteractionTool.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;

/** Default actions of the clicks on the views, owned by the PER like the camera tool.
  The PER calls it on each button down not used by the tools, before the behavior of the picked
  VME and the camera, which receive the same click; before the call it sets the VME picked by the
  click (NULL if none) and the picked position. Gizmos are never selected.
  - a button down on a VME sends VME_SELECT (for a landmark cloud, its landmark nearest to the
    picked position), if the selection is enabled;
  - the right button released without dragging sends SHOW_VIEW_CONTEXTUAL_MENU with the picked VME.
  The events go to the listener on MCH_UP with the VME as data. */
class ALBA_EXPORT albaSelectTool : public albaInteractionTool
{
public:
  albaTypeMacro(albaSelectTool, albaInteractionTool);

  /** Set the VME picked by the button down (NULL if none) and the picked position */
  void SetPickedVME(albaVME *vme, double pos[3]);

  /** Return the VME picked by the last button down, NULL if none */
  albaVME *GetPickedVME() {return m_PickedVME;}

  /** Enable/disable the selection (default enabled) */
  void EnableSelect(bool enable) {m_CanSelect = enable;}
  bool IsSelectEnabled() {return m_CanSelect;}

  /** The button down selects the picked VME; the right button prepares the context menu.
    Return true if VME_SELECT has been sent. */
  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2]) {return Select();}
  virtual bool OnMiddleButtonDown(albaView *view, vtkRenderer *ren, double pos[2]) {return Select();}
  virtual bool OnRightButtonDown(albaView *view, vtkRenderer *ren, double pos[2]);
  virtual bool OnTrackerButtonDown(albaDeviceButtonsPadTracker *tracker, int button, albaMatrix *pose) {return Select();}

  /** A drag cancels the context menu */
  virtual bool OnDrag(albaView *view, vtkRenderer *ren, double pos[2]);

  /** The right button released without dragging sends SHOW_VIEW_CONTEXTUAL_MENU; return true if sent */
  virtual bool OnRightButtonUp(albaView *view, vtkRenderer *ren, double pos[2]);

protected:
  albaSelectTool();
  virtual ~albaSelectTool() {}

  /** Send VME_SELECT for the picked VME, if the selection is enabled; return true if sent */
  bool Select();

  /** Return true if the picked VME can be selected: not NULL and not a gizmo */
  bool IsPickedVmeSelectable();

  albaVME *m_PickedVME;          ///< the VME picked by the last button down, NULL if none
  double   m_PickedPosition[3];  ///< the position picked by the last button down
  bool     m_CanSelect;          ///< true if the button down selects the picked VME
  bool     m_ShowContextMenu;    ///< true from the right button down to a drag or to the release

private:
  albaSelectTool(const albaSelectTool&);  // Not implemented.
  void operator=(const albaSelectTool&);  // Not implemented.
};
#endif
