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

#include "albaEventInteraction.h"
#include "albaMatrix.h"

albaCxxTypeMacro(albaEventInteraction)

//------------------------------------------------------------------------------
void albaEventInteraction::Set2DPosition(double x,double y)
//------------------------------------------------------------------------------
{
  m_X=x;
  m_Y=y;
  m_XYFlag=true;
  //Modified();
}
//------------------------------------------------------------------------------
void albaEventInteraction::Set2DPosition(const double pos[2])
//------------------------------------------------------------------------------
{
  m_X=pos[0];
  m_Y=pos[1];
  m_XYFlag=true;
  //Modified();
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
void albaEventInteraction::SetKey(unsigned char key)
//------------------------------------------------------------------------------
{
  m_Key=key;
  //Modified();
}

//------------------------------------------------------------------------------
albaMatrix *albaEventInteraction::GetMatrix()
//------------------------------------------------------------------------------
{
  return m_Matrix;
}

//------------------------------------------------------------------------------
void albaEventInteraction::SetMatrix(albaMatrix *matrix)
//------------------------------------------------------------------------------
{
  m_Matrix=matrix;
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
    m_Matrix=e->m_Matrix;
    m_XYFlag=e->m_XYFlag;
    m_X=e->m_X;;
    m_Y=e->m_Y;
    m_Modifiers=e->m_Modifiers;
    m_Key=e->m_Key;
  }
  else
  {
    assert(true);
  }
  
}