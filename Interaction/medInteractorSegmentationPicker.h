/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractorSegmentationPicker.h,v $
Language:  C++
Date:      $Date: 2011-06-13 16:02:54 $
Version:   $Revision: 1.1.2.2 $
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
  /**	This function is called when on mouse button down */ 
  virtual void OnButtonDown   (mafEventInteraction *e);
  /**	This function is called when on mouse button release */ 
  virtual void OnButtonUp     (mafEventInteraction *e);

  /** Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position. */
  void SendPickingInformation(mafView *v, double *mouse_pos = NULL, int msg_id = VME_PICKED, mafMatrix *tracker_pos = NULL, bool mouse_flag = true);

  bool m_ContinuousPickingFlag;

  /** constructor. */
  medInteractorSegmentationPicker();
  /** destructor. */
  virtual ~medInteractorSegmentationPicker();

};
#endif 
