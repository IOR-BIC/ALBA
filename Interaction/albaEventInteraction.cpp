/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventInteraction
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaEventInteraction.h"

albaCxxTypeMacro(albaEventInteraction)

//------------------------------------------------------------------------------
void albaEventInteraction::Set2DPosition(double x,double y)
//------------------------------------------------------------------------------
{
  m_X=x;
  m_Y=y;
  m_XYFlag=true;
}
//------------------------------------------------------------------------------
void albaEventInteraction::Set2DPosition(const double pos[2])
//------------------------------------------------------------------------------
{
  m_X=pos[0];
  m_Y=pos[1];
  m_XYFlag=true;
}
//------------------------------------------------------------------------------
void albaEventInteraction::Get2DPosition(double pos[2])
//------------------------------------------------------------------------------
{
  pos[0]=m_X;
  pos[1]=m_Y;
  //Modified();
}

//------------------------------------------------------------------------------
void albaEventInteraction::SetXYFlag(bool val)
//------------------------------------------------------------------------------
{
  m_XYFlag=val;
  //Modified();
}
//------------------------------------------------------------------------------
bool albaEventInteraction::GetXYFlag()
//------------------------------------------------------------------------------
{
  return m_XYFlag;
}

//------------------------------------------------------------------------------
void albaEventInteraction::SetButton(int button)
//------------------------------------------------------------------------------
{
  m_Button=button;
  //Modified();
}

//------------------------------------------------------------------------------
void albaEventInteraction::SetModifier(unsigned long idx,bool value)
//------------------------------------------------------------------------------
{
  int flag = 1<<idx;
  if (value)
  {
    m_Modifiers=m_Modifiers|flag;
  }
  else
  {
    m_Modifiers=m_Modifiers&(~flag);
  }

  //Modified();
}

//------------------------------------------------------------------------------
bool albaEventInteraction::GetModifier(unsigned long idx)
//------------------------------------------------------------------------------
{
  return (m_Modifiers&(1<<idx))!=0;
}
//------------------------------------------------------------------------------
void albaEventInteraction::SetModifiers(unsigned long modifiers)
//------------------------------------------------------------------------------
{
  m_Modifiers = modifiers;
}

//------------------------------------------------------------------------------
void albaEventInteraction::DeepCopy(const albaEventBase *event)
//------------------------------------------------------------------------------
{
  typedef const albaEventInteraction const_alba_interaction_event;
  if (event->IsALBAType(const_alba_interaction_event))
  {
    const_alba_interaction_event *e=(const_alba_interaction_event *)event;
    
    Superclass::DeepCopy(e);
    m_Button=e->m_Button;
    m_XYFlag=e->m_XYFlag;
    m_X=e->m_X;;
    m_Y=e->m_Y;
    m_Modifiers=e->m_Modifiers;
  }
  else
  {
    assert(true);
  }

}
