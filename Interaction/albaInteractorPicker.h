/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPicker
 Authors: Marco Petrone, originally by Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaInteractorPicker_h
#define __albaInteractorPicker_h

#include "albaInteractor.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaObserver;

/** interactor implementing a picking operation */
class ALBA_EXPORT albaInteractorPicker : public albaInteractor
{
public:
  albaTypeMacro(albaInteractorPicker,albaInteractor);

  /** Redefined to send picking events if continuous picking is enabled */
  virtual void OnEvent(albaEventBase *event);

  /** Enable/disable continuous picking in OnEvent. */
  void EnableContinuousPicking(bool enable) {m_ContinuousPickingFlag = enable;};

  bool IsContinuousPicking(){return m_ContinuousPickingFlag;};

protected:
  albaInteractorPicker();
  virtual ~albaInteractorPicker();
  
  virtual void OnButtonDown   (albaEventInteraction *e);
  virtual void OnButtonUp     (albaEventInteraction *e);

  bool m_ContinuousPickingFlag;

  /** 
  Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position. */
  void SendPickingInformation(albaView *v, double *mouse_pos = NULL, int msg_id = VME_PICKED, albaMatrix *tracker_pos = NULL, bool mouse_flag = true);
  
private:
  albaInteractorPicker(const albaInteractorPicker&);  // Not implemented.
  void operator=(const albaInteractorPicker&);  // Not implemented.
};

#endif 
