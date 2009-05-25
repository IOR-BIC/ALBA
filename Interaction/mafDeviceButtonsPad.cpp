/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceButtonsPad.cpp,v $
  Language:  C++
  Date:      $Date: 2009-05-25 14:48:12 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDeviceButtonsPad.h"
#include "mmuIdFactory.h"

#include "mafEventInteraction.h"
#include "vtkTimerLog.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafDeviceButtonsPad::BUTTON_DOWN)
MAF_ID_IMP(mafDeviceButtonsPad::BUTTON_UP)

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
        mafEventInteraction e(this,BUTTON_DOWN,num,modifiers);
        SendButtonEvent(&e);
      }
      break;
    case -1:
      {
        // issue OnButtonUp
        mafEventInteraction e(this,BUTTON_UP,num,modifiers);
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
