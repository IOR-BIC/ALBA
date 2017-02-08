/*=========================================================================

 Program: MAF2
 Module: mafInteractorCameraPicker
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medInteractorPicker_h
#define __medInteractorPicker_h

#include "mafDefines.h"
#include "mafInteractorCameraMove.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** 
Class mafInteractorCameraPicker:

When attached to a vme this behavior performs the following actions:

Left Mouse button => camera rotate
Middle Mouse button => camera pan
Right Mouse button  => camera zoom

CTRL + Left Mouse button => send picked vme abs coordinates to the listener

@sa mafViewArbitraryOrthoSlice as an example on how to use this picker
*/
class MAF_EXPORT mafInteractorCameraPicker : public mafInteractorCameraMove
{
public:
  mafTypeMacro(mafInteractorCameraPicker,mafInteractorCameraMove);

  /** Redefined to send picking events if continuous picking is enabled */
  virtual void OnEvent(mafEventBase *event);

   /** Enable/disable continuous picking in OnEvent. */
  void EnableContinuousPicking(bool enable);

protected:
  virtual void OnButtonDown   (mafEventInteraction *e);
  virtual void OnButtonUp     (mafEventInteraction *e);

  /** 
  Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position. */
  void SendPickingInformation(mafView *v, double *mouse_pos = NULL, int msg_id = VME_PICKED, mafMatrix *tracker_pos = NULL, bool mouse_flag = true);

  bool m_ContinuousPickingFlag;

  mafInteractorCameraPicker();
  virtual ~mafInteractorCameraPicker();

private:
  mafInteractorCameraPicker(const mafInteractorCameraPicker&);  // Not implemented.
  void operator=(const mafInteractorCameraPicker&);  // Not implemented.
};
#endif 
