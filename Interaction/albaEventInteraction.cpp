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
#include "albaMatrix.h"
#include "albaView.h"
#include "albaDeviceButtonsPadTracker.h"

albaCxxTypeMacro(albaEventInteraction)

//------------------------------------------------------------------------------
void albaEventInteraction::Set2DPosition(double x,double y)
//------------------------------------------------------------------------------
{
  m_X=x;
  m_Y=y;
  m_XYFlag=true;
  ResetPick();
  //Modified();
}
//------------------------------------------------------------------------------
void albaEventInteraction::Set2DPosition(const double pos[2])
//------------------------------------------------------------------------------
{
  m_X=pos[0];
  m_Y=pos[1];
  m_XYFlag=true;
  ResetPick();
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
  ResetPick();
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
    ResetPick();
  }
  else
  {
    assert(true);
  }

}

//------------------------------------------------------------------------------
bool albaEventInteraction::FindPokedVme(albaView *view, albaDevice *device, vtkProp3D *&pickedProp, albaVME *&pickedVme, albaInteractionBehavior *&pickedBehavior)
//------------------------------------------------------------------------------
{
  if (!m_PickDone || view != m_PickView || device != m_PickDevice)
  {
    ResetPick();
    m_PickDone = true;
    m_PickView = view;
    m_PickDevice = device;

    if (view)
    {
      // trackers pick with their pose, other devices with the 2D position
      albaMatrix pointPose;
      if (albaDeviceButtonsPadTracker::SafeDownCast(device) && m_Matrix)
        pointPose = *m_Matrix;
      else
      {
        pointPose.SetElement(0, 3, m_X);
        pointPose.SetElement(1, 3, m_Y);
      }

      m_Picked = view->FindPokedVme(device, pointPose, m_PickedProp, m_PickedVme, m_PickedBehavior);
      if (m_Picked)
        view->GetPickedPosition(m_PickedPosition);
    }
  }

  pickedProp = m_PickedProp;
  pickedVme = m_PickedVme;
  pickedBehavior = m_PickedBehavior;
  return m_Picked;
}

//------------------------------------------------------------------------------
void albaEventInteraction::GetPickedPosition(double pos[3])
//------------------------------------------------------------------------------
{
  pos[0] = m_PickedPosition[0];
  pos[1] = m_PickedPosition[1];
  pos[2] = m_PickedPosition[2];
}

//------------------------------------------------------------------------------
void albaEventInteraction::ResetPick()
//------------------------------------------------------------------------------
{
  m_PickDone = false;
  m_Picked = false;
  m_PickView = NULL;
  m_PickDevice = NULL;
  m_PickedProp = NULL;
  m_PickedVme = NULL;
  m_PickedBehavior = NULL;
  m_PickedPosition[0] = m_PickedPosition[1] = m_PickedPosition[2] = 0.0;
}