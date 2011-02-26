/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractorPicker.h,v $
Language:  C++
Date:      $Date: 2011-02-26 17:36:30 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2010 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __medInteractorPicker_h
#define __medInteractorPicker_h

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
class medInteractorPicker : public mafInteractorCameraMove
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
