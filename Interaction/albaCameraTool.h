/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCameraTool
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaCameraTool_h
#define __albaCameraTool_h

#include "albaInteractionTool.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaDevice;
class albaCameraMouseTool;
class albaCameraTrackerTool;

/** Tool moving the camera, always at the bottom of the tool stack of the PER.
  It is made of the mouse camera tool and of the tracker camera tool: the mouse
  methods go to the mouse tool, the tracker methods to the tracker tool. Removing the tracker support means removing
  this class and the tracker tool and renaming the mouse tool as albaCameraTool. */
class ALBA_EXPORT albaCameraTool : public albaInteractionTool
{
public:
  albaTypeMacro(albaCameraTool, albaInteractionTool);

  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2]);
  virtual bool OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2]);
  virtual bool OnMiddleButtonDown(albaView *view, vtkRenderer *ren, double pos[2]);
  virtual bool OnMiddleButtonUp(albaView *view, vtkRenderer *ren, double pos[2]);
  virtual bool OnRightButtonDown(albaView *view, vtkRenderer *ren, double pos[2]);
  virtual bool OnRightButtonUp(albaView *view, vtkRenderer *ren, double pos[2]);
  virtual bool OnDrag(albaView *view, vtkRenderer *ren, double pos[2]);
  virtual bool OnWheel(albaView *view, vtkRenderer *ren, double pos[2], double rotation);
  virtual bool OnTrackerButtonDown(albaDeviceButtonsPadTracker *tracker, int button, albaMatrix *pose);
  virtual bool OnTrackerButtonUp(albaDeviceButtonsPadTracker *tracker, int button);
  virtual bool OnTrackerDrag(albaDeviceButtonsPadTracker *tracker, albaMatrix *pose);

  /** Set the state of the modifier keys, also on the mouse and tracker tools */
  virtual void SetModifierKeys(bool ctrlDown, bool altDown, bool shiftDown);

  /** Return the tool moving the camera because of the device, NULL if none */
  albaInteractionTool *GetInteractingTool(albaDevice *device);

  albaCameraMouseTool *GetMouseTool() {return m_MouseTool;}
  albaCameraTrackerTool *GetTrackerTool() {return m_TrackerTool;}

protected:
  albaCameraTool();
  virtual ~albaCameraTool();

  albaCameraMouseTool   *m_MouseTool;
  albaCameraTrackerTool *m_TrackerTool;

private:
  albaCameraTool(const albaCameraTool&);  // Not implemented.
  void operator=(const albaCameraTool&);  // Not implemented.
};

#endif
