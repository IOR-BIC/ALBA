/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSegmentationEditTool
 Authors: Gianluigi Crimi, from albaInteractorPERBrushFeedback

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaSegmentationEditTool_h
#define __albaSegmentationEditTool_h

#include "albaSegmentationTool.h"

/** Tool of the editing of the segmentation.
  - moving the mouse without buttons: MOUSE_MOVE with the picked position (GetPointer), true if something
    has been picked (GetArg) and the CTRL modifier (GetBool)
  - wheel with SHIFT: MOUSE_WHEEL with the rotation (GetArg) and the last picked position (GetPointer);
    without SHIFT the wheel moves the camera
  The buttons and the keys are handled by albaSegmentationTool. */
class ALBA_EXPORT albaSegmentationEditTool : public albaSegmentationTool
{
public:
  albaTypeMacro(albaSegmentationEditTool, albaSegmentationTool);

  /** Send the position under the mouse, for the brush feedback */
  bool OnMove(albaEventInteraction *e) override;

  /** With SHIFT send the wheel rotation */
  bool OnWheel(albaEventInteraction *e) override;

protected:
  albaSegmentationEditTool() {}
  virtual ~albaSegmentationEditTool() {}

private:
  albaSegmentationEditTool(const albaSegmentationEditTool&);  // Not implemented.
  void operator=(const albaSegmentationEditTool&);  // Not implemented.
};

#endif
