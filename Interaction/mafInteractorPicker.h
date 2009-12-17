/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractorPicker.h,v $
  Language:  C++
  Date:      $Date: 2009-12-17 11:46:40 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone, originally by Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafInteractorPicker_h
#define __mafInteractorPicker_h

#include "mafInteractor.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafObserver;

/** interactor implementing a picking operation */
class mafInteractorPicker : public mafInteractor
{
public:
  mafTypeMacro(mafInteractorPicker,mafInteractor);

  /** Redefined to send picking events if continuous picking is enabled */
  virtual void OnEvent(mafEventBase *event);

  /** Enable/disable continuous picking in OnEvent. */
  void EnableContinuousPicking(bool enable) {m_ContinuousPickingFlag = enable;};

  bool IsContinuousPicking(){return m_ContinuousPickingFlag;};

protected:
  mafInteractorPicker();
  virtual ~mafInteractorPicker();
  
  virtual void OnButtonDown   (mafEventInteraction *e);
  virtual void OnButtonUp     (mafEventInteraction *e);

  bool m_ContinuousPickingFlag;

  /** 
  Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position. */
  void SendPickingInformation(mafView *v, double *mouse_pos = NULL, int msg_id = VME_PICKED, mafMatrix *tracker_pos = NULL, bool mouse_flag = true);
  
private:
  mafInteractorPicker(const mafInteractorPicker&);  // Not implemented.
  void operator=(const mafInteractorPicker&);  // Not implemented.
};

#endif 
