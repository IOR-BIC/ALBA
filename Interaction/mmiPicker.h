/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiPicker.h,v $
  Language:  C++
  Date:      $Date: 2005-12-06 10:35:43 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone, originally by Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmiPicker_h
#define __mmiPicker_h

#include "mafInteractor.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafObserver;

/** interactor implementing a picking operation */
class mmiPicker : public mafInteractor
{
public:
  mafTypeMacro(mmiPicker,mafInteractor);

  /** Redefined to send picking events if continuous picking is enabled */
  virtual void OnEvent(mafEventBase *event);

  /** Enable/disable continuous picking in OnEvent. */
  void EnableContinuousPicking(bool enable) {m_ContinuousPickingFlag = enable;};

protected:
  mmiPicker();
  virtual ~mmiPicker();
  
  virtual void OnButtonDown   (mafEventInteraction *e);
  virtual void OnButtonUp     (mafEventInteraction *e);

  bool m_ContinuousPickingFlag;

  /** 
  Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position. */
  void SendPickingInformation(mafView *v, double *mouse_pos = NULL, int msg_id = VME_PICKED, mafMatrix *tracker_pos = NULL, bool mouse_flag = true);
  
private:
  mmiPicker(const mmiPicker&);  // Not implemented.
  void operator=(const mmiPicker&);  // Not implemented.
};

#endif 
