/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPickerBehavior
 Authors: Marco Petrone, originally by Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaPickerBehavior_h
#define __albaPickerBehavior_h

#include "albaInteractionBehavior.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaObserver;

/** interactor implementing a picking operation */
class ALBA_EXPORT albaPickerBehavior : public albaInteractionBehavior
{
public:
  albaTypeMacro(albaPickerBehavior,albaInteractionBehavior);

  /** The left button picks and sends VME_PICKED */
  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2]);

  /** With continuous picking the release of the left button picks and sends VME_PICKED */
  virtual bool OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2]);

  /** With continuous picking the drags pick and send VME_PICKING */
  virtual bool OnDrag(albaView *view, vtkRenderer *ren, double pos[2]);

  /** The left button of a tracker picks along the tracker pose and sends VME_PICKED */
  virtual bool OnTrackerButtonDown(albaDeviceButtonsPadTracker *tracker, int button, albaMatrix *pose);
  virtual bool OnTrackerButtonUp(albaDeviceButtonsPadTracker *tracker, int button) {return button == ALBA_LEFT_BUTTON;}

  /** Enable/disable continuous picking in OnEvent. */
  void EnableContinuousPicking(bool enable) {m_ContinuousPickingFlag = enable;};

  bool IsContinuousPicking(){return m_ContinuousPickingFlag;};

protected:
  albaPickerBehavior();
  virtual ~albaPickerBehavior();
  

  bool m_ContinuousPickingFlag;

  /** 
  Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position. */
  void SendPickingInformation(albaView *v, double *mouse_pos = NULL, int msg_id = VME_PICKED, albaMatrix *tracker_pos = NULL, bool mouse_flag = true);
  
private:
  albaPickerBehavior(const albaPickerBehavior&);  // Not implemented.
  void operator=(const albaPickerBehavior&);  // Not implemented.
};

#endif 
