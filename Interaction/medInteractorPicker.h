/*=========================================================================

 Program: MAF2Medical
 Module: medInteractorPicker
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

#include "medInteractionDefines.h"
#include "mafInteractorCameraMove.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** 
Class medInteractorPicker:

When attached to a vme this behavior performs the following actions:

Left Mouse button => camera rotate
Middle Mouse button => camera pan
Right Mouse button  => camera zoom

CTRL + Left Mouse button => send picked vme abs coordinates to the listener

@sa medViewArbitraryOrthoSlice as an example on how to use this picker
*/
class MED_INTERACTION_EXPORT medInteractorPicker : public mafInteractorCameraMove
{
public:
  mafTypeMacro(medInteractorPicker,mafInteractorCameraMove);

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

  medInteractorPicker();
  virtual ~medInteractorPicker();

private:
  medInteractorPicker(const medInteractorPicker&);  // Not implemented.
  void operator=(const medInteractorPicker&);  // Not implemented.
};
#endif 
