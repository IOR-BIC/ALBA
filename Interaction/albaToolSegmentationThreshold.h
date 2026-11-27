/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolSegmentationThreshold
 Authors: Gianluigi Crimi, from albaInteractorPERScalarInformation

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaToolSegmentationThreshold_h
#define __albaToolSegmentationThreshold_h

#include "albaToolSegmentation.h"

/** Tool of the threshold initialization of the segmentation.
  Moving the mouse without buttons it sends MOUSE_MOVE with the picked position, the point id (GetArg)
  and the scalar value (GetDouble) under the mouse; the buttons and the keys are handled by albaToolSegmentation. */
class ALBA_EXPORT albaToolSegmentationThreshold : public albaToolSegmentation
{
public:
  albaTypeMacro(albaToolSegmentationThreshold, albaToolSegmentation);

  /** Send the scalar value under the mouse */
  bool OnMoveImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) override;

protected:
  albaToolSegmentationThreshold() {}
  virtual ~albaToolSegmentationThreshold() {}

private:
  albaToolSegmentationThreshold(const albaToolSegmentationThreshold&);  // Not implemented.
  void operator=(const albaToolSegmentationThreshold&);  // Not implemented.
};

#endif
