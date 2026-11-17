/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSegmentationThresholdTool
 Authors: Gianluigi Crimi, from albaInteractorPERScalarInformation

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaSegmentationThresholdTool_h
#define __albaSegmentationThresholdTool_h

#include "albaSegmentationTool.h"

/** Tool of the threshold initialization of the segmentation.
  Moving the mouse without buttons it sends MOUSE_MOVE with the picked position, the point id (GetArg)
  and the scalar value (GetDouble) under the mouse; the buttons and the keys are handled by albaSegmentationTool. */
class ALBA_EXPORT albaSegmentationThresholdTool : public albaSegmentationTool
{
public:
  albaTypeMacro(albaSegmentationThresholdTool, albaSegmentationTool);

  /** Send the scalar value under the mouse */
  bool OnMove(albaEventInteraction *e) override;

protected:
  albaSegmentationThresholdTool() {}
  virtual ~albaSegmentationThresholdTool() {}

private:
  albaSegmentationThresholdTool(const albaSegmentationThresholdTool&);  // Not implemented.
  void operator=(const albaSegmentationThresholdTool&);  // Not implemented.
};

#endif
