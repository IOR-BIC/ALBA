/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractorSegmentationPicker.h,v $
Language:  C++
Date:      $Date: 2012-04-06 08:39:52 $
Version:   $Revision: 1.1.2.5 $
Authors:   Matteo Giacomoni, Gianluigi Crimi
==========================================================================
Copyright (c) 2010 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __medInteractorSegmentationPicker_h
#define __medInteractorSegmentationPicker_h

#include "medInteractionDefines.h"
#include "mafInteractorCameraMove.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

class MED_INTERACTION_EXPORT medInteractorSegmentationPicker : public mafInteractorCameraMove
{
public:
  enum GUI_IDS
  {
    VME_ALT_PICKED = MINID,
  };
  mafTypeMacro(medInteractorSegmentationPicker,mafInteractorCameraMove);

  /** Redefined to send picking events if continuous picking is enabled */
  virtual void OnEvent(mafEventBase *event);

  /** Determine if picking modality is enabled with click and CTRL + click or with CTRL + click and ALT + click */
  void SetFullModifiersMode(bool enable){m_FullModifiersMode = enable;};

protected:
  /**	This function is called when on mouse button down */ 
  virtual void OnLeftButtonDown(mafEventInteraction *e);
  /**	This function is called when on mouse button release */ 
  virtual void OnLeftButtonUp();

  /** Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position. */
  void SendPickingInformation(mafView *v, double *mouse_pos = NULL, int msg_id = VME_PICKED, mafMatrix *tracker_pos = NULL, bool mouse_flag = true);

  bool m_IsPicking;         //< Determine if interactor is picking
  bool m_FullModifiersMode; //< Determine if picking modality is enabled with click and CTRL + click or with CTRL + click and ALT + click

  /** constructor. */
  medInteractorSegmentationPicker();

  /** destructor. */
  virtual ~medInteractorSegmentationPicker();

};
#endif 
