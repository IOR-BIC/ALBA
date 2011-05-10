/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractorSegmentationPicker.h,v $
Language:  C++
Date:      $Date: 2011-05-10 15:08:19 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni, Gianluigi Crimi
==========================================================================
Copyright (c) 2010 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __medInteractorSegmentationPicker_h
#define __medInteractorSegmentationPicker_h

#include "mafInteractorCameraMove.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

class medInteractorSegmentationPicker : public mafInteractorCameraMove
{
public:
  mafTypeMacro(medInteractorSegmentationPicker,mafInteractorCameraMove);

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

  medInteractorSegmentationPicker();
  virtual ~medInteractorSegmentationPicker();

private:
  medInteractorSegmentationPicker(const medInteractorSegmentationPicker&);  // Not implemented.
  void operator=(const medInteractorSegmentationPicker&);  // Not implemented.
};
#endif 
