/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolSegmentation
 Authors: Gianluigi Crimi, from albaInteractorSegmentationPicker

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaToolSegmentation_h
#define __albaToolSegmentation_h

#include "albaInteractionTool.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaView;

/** Base class of the tools of the segmentation operation.
  The operation makes the tool work only on its view with OperateOnlyOn; SetRendererAndView gives the view for the picking.
  - left button down: VME_PICKED to the listener, with the picked position (GetPointer) and the modifier (GetArg: ALBA_CTRL_KEY, ALBA_ALT_KEY or 0)
  - drag after a left button down that picked something: VME_PICKING with the picked position
  - left button up: MOUSE_UP
  - the buttons are used by the tool, the camera moves with the drags
  - keys: KEY_PRESSED / KEY_RELEASED to the listener, see below; the tool keeps the state of CTRL, ALT and SHIFT */
class ALBA_EXPORT albaToolSegmentation : public albaInteractionTool
{
public:
  /** @ingroup Events
  /** @{ */
  /** A key has been pressed or released: the event is an albaEvent with the key code (GetArg);
    the state of the modifier keys is read from the tool (the sender). The listener sets the event bool (SetBool) to true if it used the key. */
  ALBA_ID_DEC(KEY_PRESSED);
  ALBA_ID_DEC(KEY_RELEASED);
  /** @} */

  albaAbstractTypeMacro(albaToolSegmentation, albaInteractionTool);

  bool OnLeftButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;
  bool OnLeftButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;
  bool OnMiddleButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;
  bool OnMiddleButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override {return true;}
  bool OnRightButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;
  bool OnRightButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override {return true;}

  /** Send VME_PICKING while picking; the drag is left to the camera */
  bool OnDragImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;

  /** Forward the key to the listener, which reads the modifier keys with GetCtrlDown, GetAltDown and GetShiftDown */
  bool OnKey(int keyCode, bool pressed) override;

protected:
  albaToolSegmentation();
  virtual ~albaToolSegmentation() {}

  /** Send to the listener the event eventId with the picked position (GetPointer) and the modifier (GetArg); return true if a VME was picked */
  bool SendPickingInformation(double pos[2], long modifier, int eventId);

  bool m_IsPicking;          ///< true while the left button, pressed on a VME, is down
  double m_PickPosition[3];  ///< the last picked position

private:
  albaToolSegmentation(const albaToolSegmentation&);  // Not implemented.
  void operator=(const albaToolSegmentation&);  // Not implemented.
};

#endif
