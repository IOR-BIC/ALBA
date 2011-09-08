/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractorSegmentationPicker.h,v $
Language:  C++
Date:      $Date: 2011-09-08 08:53:22 $
Version:   $Revision: 1.1.2.4 $
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
  enum GUI_IDS
  {
    VME_ALT_PICKED = MINID,
  };
  mafTypeMacro(medInteractorSegmentationPicker,mafInteractorCameraMove);

  /** Redefined to send picking events if continuous picking is enabled */
  virtual void OnEvent(mafEventBase *event);

protected:
  /**	This function is called when on mouse button down */ 
  virtual void OnLeftButtonDown(mafEventInteraction *e);
  /**	This function is called when on mouse button release */ 
  virtual void OnLeftButtonUp();

  /** Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position. */
  void SendPickingInformation(mafView *v, double *mouse_pos = NULL, int msg_id = VME_PICKED, mafMatrix *tracker_pos = NULL, bool mouse_flag = true);

  bool m_IsPicking;

  /** constructor. */
  medInteractorSegmentationPicker();
  /** destructor. */
  virtual ~medInteractorSegmentationPicker();

};
#endif 
