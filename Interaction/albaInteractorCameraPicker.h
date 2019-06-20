/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorCameraPicker
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medInteractorPicker_h
#define __medInteractorPicker_h

#include "albaDefines.h"
#include "albaInteractorCameraMove.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** 
Class albaInteractorCameraPicker:

When attached to a vme this behavior performs the following actions:

Left Mouse button => camera rotate
Middle Mouse button => camera pan
Right Mouse button  => camera zoom

CTRL + Left Mouse button => send picked vme abs coordinates to the listener

@sa albaViewArbitraryOrthoSlice as an example on how to use this picker
*/
class ALBA_EXPORT albaInteractorCameraPicker : public albaInteractorCameraMove
{
public:
  albaTypeMacro(albaInteractorCameraPicker,albaInteractorCameraMove);

  /** Redefined to send picking events if continuous picking is enabled */
  virtual void OnEvent(albaEventBase *event);

   /** Enable/disable continuous picking in OnEvent. */
  void EnableContinuousPicking(bool enable);

protected:
  virtual void OnButtonDown   (albaEventInteraction *e);
  virtual void OnButtonUp     (albaEventInteraction *e);

  /** 
  Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position. */
  void SendPickingInformation(albaView *v, double *mouse_pos = NULL, int msg_id = VME_PICKED, albaMatrix *tracker_pos = NULL, bool mouse_flag = true);

  bool m_ContinuousPickingFlag;

  albaInteractorCameraPicker();
  virtual ~albaInteractorCameraPicker();

private:
  albaInteractorCameraPicker(const albaInteractorCameraPicker&);  // Not implemented.
  void operator=(const albaInteractorCameraPicker&);  // Not implemented.
};
#endif 
