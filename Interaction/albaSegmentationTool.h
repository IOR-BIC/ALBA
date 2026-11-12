/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSegmentationTool
 Authors: Gianluigi Crimi, from albaInteractorSegmentationPicker

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaSegmentationTool_h
#define __albaSegmentationTool_h

#include "albaInteractionTool.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaView;

/** Base class of the tools of the segmentation operation.
  The tool works only on the view set with SetRendererAndView: the events coming from other views are ignored.
  - left button down: VME_PICKED to the listener, with the picked position (GetPointer) and the modifier (GetArg: ALBA_CTRL_KEY, ALBA_ALT_KEY or 0)
  - drag after a left button down that picked something: VME_PICKING with the picked position
  - left button up: MOUSE_UP
  - the buttons are used by the tool, the camera moves with the drags
  - keys: KEY_PRESSED / KEY_RELEASED to the listener, see below; the tool keeps the state of CTRL, ALT and SHIFT */
class ALBA_EXPORT albaSegmentationTool : public albaInteractionTool
{
public:
  /** @ingroup Events
  /** @{ */
  /** A key has been pressed or released: the event is an albaEvent with the key code (GetArg);
    the state of the modifier keys is read from the tool (the sender). The listener sets the event bool (SetBool) to true if it used the key. */
  ALBA_ID_DEC(KEY_PRESSED);
  ALBA_ID_DEC(KEY_RELEASED);
  /** @} */

  albaAbstractTypeMacro(albaSegmentationTool, albaInteractionTool);

  bool OnLeftButtonDown(albaEventInteraction *e) override;
  bool OnLeftButtonUp(albaEventInteraction *e) override;
  bool OnMiddleButtonDown(albaEventInteraction *e) override;
  bool OnMiddleButtonUp(albaEventInteraction *e) override {return true;}
  bool OnRightButtonDown(albaEventInteraction *e) override;
  bool OnRightButtonUp(albaEventInteraction *e) override {return true;}

  /** Send VME_PICKING while picking; the drag is left to the camera */
  bool OnDrag(albaEventInteraction *e) override;

  /** Update the state of the modifier keys and forward the key to the listener */
  bool OnKey(int keyCode, bool pressed) override;

  /** Return true if CTRL is down */
  bool GetCtrlDown() {return m_CtrlDown;}

  /** Return true if ALT is down */
  bool GetAltDown() {return m_AltDown;}

  /** Return true if SHIFT is down */
  bool GetShiftDown() {return m_ShiftDown;}

protected:
  albaSegmentationTool();
  virtual ~albaSegmentationTool() {}

  /** Return true if the event comes from a mouse working on the view of the tool */
  bool IsOnToolView(albaEventInteraction *e);

  /** Send to the listener the event eventId with the picked position (GetPointer) and the modifier (GetArg); return true if a VME was picked */
  bool SendPickingInformation(albaEventInteraction *e, long modifier, int eventId);

  bool m_IsPicking;          ///< true while the left button, pressed on a VME, is down
  double m_PickPosition[3];  ///< the last picked position
  bool m_CtrlDown;           ///< true while CTRL is down
  bool m_AltDown;            ///< true while ALT is down
  bool m_ShiftDown;          ///< true while SHIFT is down

private:
  albaSegmentationTool(const albaSegmentationTool&);  // Not implemented.
  void operator=(const albaSegmentationTool&);  // Not implemented.
};

#endif
