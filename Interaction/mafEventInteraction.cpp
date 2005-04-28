/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: mafEventInteraction.cpp,v $
Language:  C++
Date:      $Date: 2005-04-28 16:10:11 $
Version:   $Revision: 1.1 $

=========================================================================*/

#include "mflEventInteraction.h"
#include "mflMatrix.h"

//------------------------------------------------------------------------------
mflEventInteraction *mflEventInteraction::New(unsigned long id,vtkObject *sender,mflMatrix *data,int button,unsigned long modifiers)
//------------------------------------------------------------------------------
{
  mflEventInteraction *e=new mflEventInteraction(id,sender,data,button,modifiers);
  e->ReferenceCount++;
  return e;
}

//------------------------------------------------------------------------------
mflEventInteraction *mflEventInteraction::New(unsigned long id,vtkObject *sender,double x,double y,int button,unsigned long modifiers)
//------------------------------------------------------------------------------
{
  mflEventInteraction *e=new mflEventInteraction(id,sender,x,y,button,modifiers);
  e->ReferenceCount++;
  return e;
}

//------------------------------------------------------------------------------
void mflEventInteraction::Set2DPosition(double x,double y)
//------------------------------------------------------------------------------
{
  X=x;
  Y=y;
  XYFlag=true;
  Modified();
}
//------------------------------------------------------------------------------
void mflEventInteraction::Set2DPosition(double pos[2])
//------------------------------------------------------------------------------
{
  X=pos[0];
  Y=pos[1];
  XYFlag=true;
  Modified();
}
//------------------------------------------------------------------------------
void mflEventInteraction::Get2DPosition(double pos[2])
//------------------------------------------------------------------------------
{
  pos[0]=X;
  pos[1]=Y;
  Modified();
}

//------------------------------------------------------------------------------
void mflEventInteraction::SetXYFlag(bool val)
//------------------------------------------------------------------------------
{
  XYFlag=val;
  Modified();
}
//------------------------------------------------------------------------------
bool mflEventInteraction::GetXYFlag()
//------------------------------------------------------------------------------
{
  return XYFlag;
}

//------------------------------------------------------------------------------
void mflEventInteraction::SetButton(int button)
//------------------------------------------------------------------------------
{
  Button=button;
  Modified();
}

//------------------------------------------------------------------------------
void mflEventInteraction::SetKey(unsigned char key)
//------------------------------------------------------------------------------
{
  Key=key;
  Modified();
}

//------------------------------------------------------------------------------
mflMatrix *mflEventInteraction::GetMatrix()
//------------------------------------------------------------------------------
{
  return Matrix;
}

//------------------------------------------------------------------------------
void mflEventInteraction::SetMatrix(mflMatrix *matrix)
//------------------------------------------------------------------------------
{
  vtkSetObjectBodyMacro(Matrix,mflMatrix,matrix);
}

//------------------------------------------------------------------------------
void mflEventInteraction::SetModifier(unsigned long idx,bool value)
//------------------------------------------------------------------------------
{
  int flag = 1<<idx;
  if (value)
  {
    Modifiers=Modifiers|flag;
  }
  else
  {
    Modifiers=Modifiers&(~flag);
  }

  Modified();
}

//------------------------------------------------------------------------------
bool mflEventInteraction::GetModifier(unsigned long idx)
//------------------------------------------------------------------------------
{
  return (Modifiers&(1<<idx))!=0;
}
//------------------------------------------------------------------------------
void mflEventInteraction::SetModifiers(unsigned long modifiers)
//------------------------------------------------------------------------------
{
  Modifiers = modifiers;
}

//------------------------------------------------------------------------------
void mflEventInteraction::DeepCopy(mflEventInteraction *e)
//------------------------------------------------------------------------------
{
  SetSender(this);
  SetID(e->GetID());
  SetMatrix(e->GetMatrix());
  SetXYFlag(e->GetXYFlag());
  X=e->X;;
  Y=e->Y;
  SetModifiers(e->GetModifiers());
  SetKey(e->GetKey());
}