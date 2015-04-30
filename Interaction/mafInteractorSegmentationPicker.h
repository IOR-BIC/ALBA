/*=========================================================================

 Program: MAF2
 Module: mafInteractorSegmentationPicker
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafInteractorSegmentationPicker_h
#define __mafInteractorSegmentationPicker_h

#include "mafDefines.h"
#include "mafInteractorCameraMove.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

class MAF_EXPORT mafInteractorSegmentationPicker : public mafInteractorCameraMove
{
public:
  enum GUI_IDS
  {
    VME_ALT_PICKED = MINID,
  };
  mafTypeMacro(mafInteractorSegmentationPicker,mafInteractorCameraMove);

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
  mafInteractorSegmentationPicker();

  /** destructor. */
  virtual ~mafInteractorSegmentationPicker();

};
#endif 
