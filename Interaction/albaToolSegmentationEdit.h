/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolSegmentationEdit
 Authors: Gianluigi Crimi, from albaInteractorPERBrushFeedback

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaToolSegmentationEdit_h
#define __albaToolSegmentationEdit_h

#include "albaToolSegmentation.h"

/** Tool of the editing of the segmentation.
  - moving the mouse without buttons: MOUSE_MOVE with the picked position (GetPointer), true if something
    has been picked (GetArg) and the CTRL modifier (GetBool)
  - wheel with SHIFT: MOUSE_WHEEL with the rotation (GetArg) and the last picked position (GetPointer);
    without SHIFT the wheel moves the camera
  The buttons and the keys are handled by albaToolSegmentation. */
class ALBA_EXPORT albaToolSegmentationEdit : public albaToolSegmentation
{
public:
  albaTypeMacro(albaToolSegmentationEdit, albaToolSegmentation);

  /** Send the position under the mouse, for the brush feedback */
  bool OnMove(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;

  /** With SHIFT send the wheel rotation */
  bool OnWheel(albaRWIBase *rwi, vtkRenderer *ren, double pos[2], double rotation) override;

protected:
  albaToolSegmentationEdit() {}
  virtual ~albaToolSegmentationEdit() {}

private:
  albaToolSegmentationEdit(const albaToolSegmentationEdit&);  // Not implemented.
  void operator=(const albaToolSegmentationEdit&);  // Not implemented.
};

#endif
