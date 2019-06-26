/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceButtonsPad
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaDeviceButtonsPad_h
#define __albaDeviceButtonsPad_h

#include "albaDevice.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaEventInteraction;

/** Device implementing interface for buttons.
  albaDeviceButtonsPad is a class providing interface for a button device.
  @sa albaDevice 
*/
class ALBA_EXPORT albaDeviceButtonsPad : public albaDevice
{
public:
  albaTypeMacro(albaDeviceButtonsPad,albaDevice); 

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      Issued when one of the tracker's buttons is pressed */
  // ALBA_ID_DEC(BUTTON_DOWN)
  /** @ingroup Events
      Issued when one of the tracker's buttons is released */
  // ALBA_ID_DEC(BUTTON_UP)

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

  /** return the button down id */
  static albaID GetButtonDownId();

  /** return the button up id */
  static albaID GetButtonUpId();

	/** return the Wheel id */
	static albaID GetWheelId();

protected:
  albaDeviceButtonsPad();
  virtual ~albaDeviceButtonsPad();

  /** 
    Internally used to issue the event rised by a button pressure,
    can be reimplemented by subclasses to add extrainformation to
    the event, like mouse position or tracker pose */
  virtual void SendButtonEvent(albaEventInteraction *event);

  /**  Internally used to check if a button state has changed (used to issue events) */
  int FlagCheck(bool event,bool &flag);

  bool *m_ButtonState;
  int   m_NumberOfButtons;

private:
  albaDeviceButtonsPad(const albaDeviceButtonsPad&);  // Not implemented.
  void operator=(const albaDeviceButtonsPad&);  // Not implemented.
};

#endif 
