/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceButtonsPad.h,v $
  Language:  C++
  Date:      $Date: 2009-05-25 14:48:12 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafDeviceButtonsPad_h
#define __mafDeviceButtonsPad_h

#include "mafDevice.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafEventInteraction;

/** Device implementing interface for buttons.
  mafDeviceButtonsPad is a class providing interface for a button device.
  @sa mafDevice 
*/
class mafDeviceButtonsPad : public mafDevice
{
public:
  mafTypeMacro(mafDeviceButtonsPad,mafDevice); 

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      Issued when one of the tracker's buttons is pressed */
  MAF_ID_DEC(BUTTON_DOWN)
  /** @ingroup Events
      Issued when one of the tracker's buttons is released */
  MAF_ID_DEC(BUTTON_UP)

  /**
   Internally used to set the last button state. After setting the button state eventual
   BUTTON_UP or BUTTON_DOWN are rised. A copy of the last pose matrix is sent within
   the event. */
  void SetButtonState(int num, bool value, unsigned long modifiers = 0);

  /** return current state of the button */
  bool GetButtonState(int num);
  
  /** return the number of buttons for this device */
  int GetNumberOfButtons() {return m_NumberOfButtons;} 
  
  /** 
    Set the number of buttons for this device. Do not change this
    if you don't know what you are doing.*/
  void SetNumberOfButtons(int num);

protected:
  mafDeviceButtonsPad();
  virtual ~mafDeviceButtonsPad();

  /** 
    Internally used to issue the event rised by a button pressure,
    can be reimplemented by subclasses to add extrainformation to
    the event, like mouse position or tracker pose */
  virtual void SendButtonEvent(mafEventInteraction *event);

  /**  Internally used to check if a button state has changed (used to issue events) */
  int FlagCheck(bool event,bool &flag);

  bool *m_ButtonState;
  int   m_NumberOfButtons;

private:
  mafDeviceButtonsPad(const mafDeviceButtonsPad&);  // Not implemented.
  void operator=(const mafDeviceButtonsPad&);  // Not implemented.
};

#endif 
