/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventInteraction.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-14 17:42:28 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafEventInteraction.h"
#include "mafMatrix.h"

mafCxxTypeMacro(mafEventInteraction)

//------------------------------------------------------------------------------
void mafEventInteraction::Set2DPosition(double x,double y)
//------------------------------------------------------------------------------
{
  m_X=x;
  m_Y=y;
  m_XYFlag=true;
  //Modified();
}
//------------------------------------------------------------------------------
void mafEventInteraction::Set2DPosition(const double pos[2])
//------------------------------------------------------------------------------
{
  m_X=pos[0];
  m_Y=pos[1];
  m_XYFlag=true;
  //Modified();
}
//------------------------------------------------------------------------------
void mafEventInteraction::Get2DPosition(double pos[2])
//------------------------------------------------------------------------------
{
  pos[0]=m_X;
  pos[1]=m_Y;
  //Modified();
}

//------------------------------------------------------------------------------
void mafEventInteraction::SetXYFlag(bool val)
//------------------------------------------------------------------------------
{
  m_XYFlag=val;
  //Modified();
}
//------------------------------------------------------------------------------
bool mafEventInteraction::GetXYFlag()
//------------------------------------------------------------------------------
{
  return m_XYFlag;
}

//------------------------------------------------------------------------------
void mafEventInteraction::SetButton(int button)
//------------------------------------------------------------------------------
{
  m_Button=button;
  //Modified();
}

//------------------------------------------------------------------------------
void mafEventInteraction::SetKey(unsigned char key)
//------------------------------------------------------------------------------
{
  m_Key=key;
  //Modified();
}

//------------------------------------------------------------------------------
mafMatrix *mafEventInteraction::GetMatrix()
//------------------------------------------------------------------------------
{
  return m_Matrix;
}

//------------------------------------------------------------------------------
void mafEventInteraction::SetMatrix(mafMatrix *matrix)
//------------------------------------------------------------------------------
{
  m_Matrix=matrix;
}

//------------------------------------------------------------------------------
void mafEventInteraction::SetModifier(unsigned long idx,bool value)
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
bool mafEventInteraction::GetModifier(unsigned long idx)
//------------------------------------------------------------------------------
{
  return (m_Modifiers&(1<<idx))!=0;
}
//------------------------------------------------------------------------------
void mafEventInteraction::SetModifiers(unsigned long modifiers)
//------------------------------------------------------------------------------
{
  m_Modifiers = modifiers;
}

//------------------------------------------------------------------------------
void mafEventInteraction::DeepCopy(const mafEventBase *event)
//------------------------------------------------------------------------------
{
  typedef const mafEventInteraction const_maf_interaction_event;
  if (event->IsMAFType(const_maf_interaction_event))
  {
    const_maf_interaction_event *e=(const_maf_interaction_event *)event;
    
    Superclass::DeepCopy(e);
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