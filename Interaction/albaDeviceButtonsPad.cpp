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

#include "albaDeviceButtonsPad.h"

#include "albaEventInteraction.h"
#include "vtkTimerLog.h"

albaCxxTypeMacro(albaDeviceButtonsPad)

//------------------------------------------------------------------------------
albaDeviceButtonsPad::albaDeviceButtonsPad()
//------------------------------------------------------------------------------
{
  m_ButtonState = NULL;
  SetNumberOfButtons(3); // by default allocate 3 buttons
}

//------------------------------------------------------------------------------
albaDeviceButtonsPad::~albaDeviceButtonsPad()
//------------------------------------------------------------------------------
{
  if (m_ButtonState)
  {
    delete [] m_ButtonState;
  }
}
//------------------------------------------------------------------------------
void albaDeviceButtonsPad::SetNumberOfButtons(int num)
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
int albaDeviceButtonsPad::FlagCheck(bool event,bool &flag)
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
void albaDeviceButtonsPad::SetButtonState(int num, bool value, unsigned long modifiers)
//------------------------------------------------------------------------------
{
  if (num<0||num>=m_NumberOfButtons)
    return ;

  switch (this->FlagCheck(value,m_ButtonState[num]))
  {
    case 1:
      {
        // issue OnButtonDown
        albaEventInteraction e(this,DEVICE_BUTTON_DOWN,num,modifiers);
        SendButtonEvent(&e);
      }
      break;
    case -1:
      {
        // issue OnButtonUp
        albaEventInteraction e(this,DEVICE_BUTTON_UP,num,modifiers);
        this->SendButtonEvent(&e);
      }
      break;
    default: // do not issue an event
      break;
  }
}

//------------------------------------------------------------------------------
void albaDeviceButtonsPad::SendButtonEvent(albaEventInteraction *e)
//------------------------------------------------------------------------------
{
  this->InvokeEvent(e,(albaID)MCH_INPUT);
}

//------------------------------------------------------------------------------
bool albaDeviceButtonsPad::GetButtonState(int num)
//------------------------------------------------------------------------------
{
  return (num>=0&&num<m_NumberOfButtons)?m_ButtonState[num]:false;
}
