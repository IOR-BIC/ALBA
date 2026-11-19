/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaFlyToTool
 Authors: Gianluigi Crimi, from albaPositionalEventRouter::FlyTo

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaFlyToTool_h
#define __albaFlyToTool_h

#include "albaInteractionTool.h"
#include "albaServiceClient.h"

/** Tool moving the camera to the point clicked on a VME, pushed by the logic when the fly-to mode starts.
  The left button flies toward the point, the right button flies to it moving the camera away;
  after the fly, or a click not on a VME, the tool pops itself from the stack of the PER. */
class ALBA_EXPORT albaFlyToTool : public albaInteractionTool, public albaServiceClient
{
public:
  albaTypeMacro(albaFlyToTool, albaInteractionTool);

  /** Fly toward the clicked point */
  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2]);

  /** Fly to the clicked point moving the camera away */
  virtual bool OnRightButtonDown(albaView *view, vtkRenderer *ren, double pos[2]);

  /** Move the camera of ren in numSteps steps to the point picked at pos: the focal point goes to
    the point and the camera approaches it by zoom (from -1 to 1, negative moves the camera away).
    Return false if no point is picked. */
  static bool FlyTo(albaView *view, vtkRenderer *ren, double pos[2], double zoom, int numSteps = 20);

protected:
  albaFlyToTool() {}
  virtual ~albaFlyToTool() {}

  /** Fly and pop the tool */
  bool FlyAndPop(albaView *view, vtkRenderer *ren, double pos[2], double zoom);

private:
  albaFlyToTool(const albaFlyToTool&);  // Not implemented.
  void operator=(const albaFlyToTool&);  // Not implemented.
};
#endif
