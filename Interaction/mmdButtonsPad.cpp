/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdButtonsPad.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-21 07:57:09 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mmdButtonsPad.h"
#include "mmuIdFactory.h"

#include "mafEventInteraction.h"
#include "vtkTimerLog.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mmdButtonsPad::BUTTON_DOWN)
MAF_ID_IMP(mmdButtonsPad::BUTTON_UP)

mafCxxTypeMacro(mmdButtonsPad)

//------------------------------------------------------------------------------
mmdButtonsPad::mmdButtonsPad()
//------------------------------------------------------------------------------
{
  m_ButtonState = NULL;
  SetNumberOfButtons(3); // by default allocate 3 buttons
}

//------------------------------------------------------------------------------
mmdButtonsPad::~mmdButtonsPad()
//------------------------------------------------------------------------------
{
  if (m_ButtonState)
  {
    delete [] m_ButtonState;
  }
}
//------------------------------------------------------------------------------
void mmdButtonsPad::SetNumberOfButtons(int num)
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
int mmdButtonsPad::FlagCheck(bool event,bool &flag)
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
void mmdButtonsPad::SetButtonState(int num, bool value, unsigned long modifiers)
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
void mmdButtonsPad::SendButtonEvent(mafEventInteraction *e)
//------------------------------------------------------------------------------
{
  this->InvokeEvent(e,MCH_INPUT);
}

//------------------------------------------------------------------------------
bool mmdButtonsPad::GetButtonState(int num)
//------------------------------------------------------------------------------
{
  return (num>=0&&num<m_NumberOfButtons)?m_ButtonState[num]:false;
}