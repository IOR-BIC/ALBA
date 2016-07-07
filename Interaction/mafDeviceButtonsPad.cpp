/*=========================================================================

 Program: MAF2
 Module: mafDeviceButtonsPad
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDeviceButtonsPad.h"
#include "mmuIdFactory.h"

#include "mafEventInteraction.h"
#include "vtkTimerLog.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
/** Event ID used to know if the VME should serialize itself as a single or multiple binary files.*/
// MAF_ID_IMP(mafDeviceButtonsPad::BUTTON_DOWN)
// MAF_ID_IMP(mafDeviceButtonsPad::BUTTON_UP)

mafCxxTypeMacro(mafDeviceButtonsPad)

//------------------------------------------------------------------------------
mafDeviceButtonsPad::mafDeviceButtonsPad()
//------------------------------------------------------------------------------
{
  m_ButtonState = NULL;
  SetNumberOfButtons(3); // by default allocate 3 buttons
}

//------------------------------------------------------------------------------
mafDeviceButtonsPad::~mafDeviceButtonsPad()
//------------------------------------------------------------------------------
{
  if (m_ButtonState)
  {
    delete [] m_ButtonState;
  }
}
//------------------------------------------------------------------------------
void mafDeviceButtonsPad::SetNumberOfButtons(int num)
//------------------------------------------------------------------------------
{
  if (m_ButtonState)
  {
    delete [] m_ButtonState;
    m_ButtonState = NULL;
  }

  m_NumberOfButtons = num;

  if (num>0)
  {
    m_ButtonState = new bool[num];
    for (int i=0;i<num;i++)
      m_ButtonState[i]=false;
  }
}

//------------------------------------------------------------------------------
int mafDeviceButtonsPad::FlagCheck(bool event,bool &flag)
//------------------------------------------------------------------------------
{
  if (event)
  {
    if (!flag)
	  {
	    flag=true;
	    return 1;
	  }    
  }
  else
  {
    if (flag)
	  { 
	    flag=false;
	    return -1;
	  }
  }
  return 0;
}


//------------------------------------------------------------------------------
void mafDeviceButtonsPad::SetButtonState(int num, bool value, unsigned long modifiers)
//------------------------------------------------------------------------------
{
  if (num<0||num>=m_NumberOfButtons)
    return ;

  switch (this->FlagCheck(value,m_ButtonState[num]))
  {
    case 1:
      {
        // issue OnButtonDown
        mafEventInteraction e(this,GetButtonDownId(),num,modifiers);
        SendButtonEvent(&e);
      }
      break;
    case -1:
      {
        // issue OnButtonUp
        mafEventInteraction e(this,GetButtonUpId(),num,modifiers);
        this->SendButtonEvent(&e);
      }
      break;
    default: // do not issue an event
      break;
  }
}

//------------------------------------------------------------------------------
void mafDeviceButtonsPad::SendButtonEvent(mafEventInteraction *e)
//------------------------------------------------------------------------------
{
  this->InvokeEvent(e,MCH_INPUT);
}

//------------------------------------------------------------------------------
bool mafDeviceButtonsPad::GetButtonState(int num)
//------------------------------------------------------------------------------
{
  return (num>=0&&num<m_NumberOfButtons)?m_ButtonState[num]:false;
}
//------------------------------------------------------------------------------
mafID mafDeviceButtonsPad::GetButtonDownId()
//------------------------------------------------------------------------------
{
  static const mafID buttonDownId = mmuIdFactory::GetNextId("BUTTON_DOWN");
  return buttonDownId;
}
//------------------------------------------------------------------------------
mafID mafDeviceButtonsPad::GetButtonUpId()
//------------------------------------------------------------------------------
{
  static const mafID buttonUpId = mmuIdFactory::GetNextId("BUTTON_UP");
  return buttonUpId;
}

//------------------------------------------------------------------------------
mafID mafDeviceButtonsPad::GetWheelId()
//------------------------------------------------------------------------------
{
	static const mafID buttonUpId = mmuIdFactory::GetNextId("MOUSE_WHEEL");
	return buttonUpId;
}
