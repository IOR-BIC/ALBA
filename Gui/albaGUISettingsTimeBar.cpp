/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISettingsTimeBar
 Authors: Paolo Quadrani
 
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

#include "albaGUISettingsTimeBar.h"

#include "albaDecl.h"
#include "albaGUI.h"

//----------------------------------------------------------------------------
albaGUISettingsTimeBar::albaGUISettingsTimeBar(albaObserver *Listener, const albaString &label):
albaGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  m_RealTimeMode = 0;
  m_TimeSpeed = 1.0;
  m_TimeScale = 1000.0;
  m_Loop = 1;

  m_AnimateInSubrange = 0;
  m_SubRange[0] = 0.0; 
  m_SubRange[1] = 1.0;

  m_PlayInActiveViewport = 0;

  m_NumberOfFrames = 500;
  m_TimeStep = 0.2;

  m_TimeMin = 0;
  m_TimeMax = m_TimeMin + (m_NumberOfFrames - 1) * m_TimeStep;

  InitializeSettings();
}
//----------------------------------------------------------------------------
albaGUISettingsTimeBar::~albaGUISettingsTimeBar()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);
  m_Gui->Bool(ID_REAL_TIME, _("real time"), &m_RealTimeMode, 1);
  m_Gui->Double(ID_TIME_SCALE, _("scale:"), &m_TimeScale, 0.000001, MAXDOUBLE, -1, _("time scale referring to the time unit: seconds"));
  m_Gui->Double(ID_TIME_SPEED, _("speed Nx:"), &m_TimeSpeed, 0.000001, MAXDOUBLE, -1, _("time multiplier to speed up the animation"));
  m_Gui->Integer(ID_NUMBER_OF_FRAMES, _("frames:"), &m_NumberOfFrames, 1, MAXINT, _("Number of frame representing the animation"));
  m_Gui->Double(ID_TIME_STEP, _("step:"), &m_TimeStep, 0.000001, MAXDOUBLE, -1, _("time step between two keyframes"));
  m_Gui->Bool(ID_LOOP, _("loop playback"), &m_Loop, 1);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_ANIMATE_IN_SUBRANGE, _("animate in subrange"), &m_AnimateInSubrange, 1);
  m_Gui->VectorN(ID_SUBRANGE, _("subrange: "), m_SubRange, 2, 0.0);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_PLAY_ACTIVE_VIEWPORT, _("active view playback"), &m_PlayInActiveViewport, 1);
  m_Gui->Divider();

  EnableWidgets();
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::SetTimeBounds(double min, double max)
//----------------------------------------------------------------------------
{
  m_TimeMin = min;
  m_TimeMax = max;
  //m_TimeStep = (m_TimeMax - m_TimeMin) / m_NumberOfFrames;
//  m_Config->Write("TimeStep", m_TimeStep);
//  m_Config->Write("NumberOfFrames", m_NumberOfFrames);
  Update();
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
  {
    case ID_REAL_TIME:
      SetRealTimeMode(m_RealTimeMode);
    break;
    case ID_TIME_SPEED:
      m_Config->Write("TimeMultiplier", m_TimeSpeed);
    break;
    case ID_TIME_STEP:
      SetTimeStep(m_TimeStep);
    break;
    case ID_NUMBER_OF_FRAMES:
      SetNumberOfFrames(m_NumberOfFrames);
    break;
    case ID_TIME_SCALE:
      SetTimeScale(m_TimeScale);
    break;
    case ID_ANIMATE_IN_SUBRANGE:
      AminateInSubrange(m_AnimateInSubrange);
    break;
    case ID_SUBRANGE:
      SetSubrange(m_SubRange[0], m_SubRange[1]);
    break;
    case ID_LOOP:
      LoopAnimation(m_Loop);
    break;
    case ID_PLAY_ACTIVE_VIEWPORT:
      PlayInActiveViewport(m_PlayInActiveViewport);
    break;
    default:
      albaEventMacro(*alba_event);
    break; 
  }
  // Update the time bar.
  alba_event->SetSender(this);
  albaEventMacro(*alba_event);
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::InitializeSettings()
//----------------------------------------------------------------------------
{
  wxString string_item;
  long long_item;
  double double_item;
  if(m_Config->Read("RealTimeMode", &long_item))
  {
    m_RealTimeMode = long_item;
  }
  else
  {
    SetRealTimeMode(m_RealTimeMode);
  }
  if(m_Config->Read("TimeMultiplier", &double_item))
  {
    m_TimeSpeed = double_item;
  }
  else
  {
    SetTimeMultiplier(m_TimeSpeed);
  }
  if(m_Config->Read("TimeScale", &double_item))
  {
    m_TimeScale = double_item;
  }
  else
  {
    SetTimeScale(m_TimeScale);
  }
  if(m_Config->Read("AnimateInSubrange", &long_item))
  {
    m_AnimateInSubrange = long_item;
  }
  else
  {
    AminateInSubrange(m_AnimateInSubrange);
  }
  if(m_Config->Read("SubRange0", &double_item))
  {
    m_SubRange[0] = double_item;
  }
  else
  {
    SetSubrange(m_SubRange);
  }
  if(m_Config->Read("SubRange1", &double_item))
  {
    m_SubRange[1] = double_item;
  }
  else
  {
    SetSubrange(m_SubRange);
  }
  if(m_Config->Read("Loop", &long_item))
  {
    m_Loop = long_item;
  }
  else
  {
    LoopAnimation(m_Loop);
  }
  if(m_Config->Read("PlayInActiveViewport", &long_item))
  {
    m_PlayInActiveViewport = long_item;
  }
  else
  {
    PlayInActiveViewport(m_PlayInActiveViewport);
  }
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::EnableWidgets()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_TIME_SPEED, m_RealTimeMode == 0);
  m_Gui->Enable(ID_NUMBER_OF_FRAMES, m_RealTimeMode == 0);
  m_Gui->Enable(ID_TIME_STEP, m_RealTimeMode == 0);
  m_Gui->Enable(ID_TIME_SCALE, m_RealTimeMode == 1);
  m_Gui->Enable(ID_SUBRANGE, m_AnimateInSubrange == 1);
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::SetNumberOfFrames(int frames)
//----------------------------------------------------------------------------
{
  m_NumberOfFrames = frames < 1 ? 1 : frames;
  if (m_NumberOfFrames == 1)
  {
    m_TimeStep = m_TimeMax - m_TimeMin;
  }
  else
  {
    m_TimeStep = (m_TimeMax - m_TimeMin) / (m_NumberOfFrames - 1);
  }
//  m_Config->Write("NumberOfFrames", m_NumberOfFrames);
//  m_Config->Write("TimeStep", m_TimeStep);
  Update();
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::SetTimeStep(double step)
//----------------------------------------------------------------------------
{
  m_TimeStep = step;
  m_NumberOfFrames = (int)((m_TimeMax - m_TimeMin) / m_TimeStep) + 1;
  if (m_NumberOfFrames <= 1)
  {
    m_NumberOfFrames = 1;
    m_TimeStep = m_TimeMax - m_TimeMin;
  }
//  m_Config->Write("TimeStep", m_TimeStep);
//  m_Config->Write("NumberOfFrames", m_NumberOfFrames);
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::SetSubrange(double min, double max)
//----------------------------------------------------------------------------
{
  m_SubRange[0] = min < max ? min : max;
  m_SubRange[1] = min < max ? max : min;
  m_Config->Write("SubRange0", m_SubRange[0]);
  m_Config->Write("SubRange1", m_SubRange[1]);
  Update();
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::SetSubrange(double sub_range[2])
//----------------------------------------------------------------------------
{
  SetSubrange(sub_range[0], sub_range[1]);
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::SetRealTimeMode(int realTime)
//----------------------------------------------------------------------------
{
  m_RealTimeMode = realTime;
  m_Config->Write("RealTimeMode", m_RealTimeMode);
  Update();
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::LoopAnimation(int loop)
//----------------------------------------------------------------------------
{
  m_Loop = loop;
  m_Config->Write("Loop", m_Loop);
  Update();
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::AminateInSubrange(int animate_subrange)
//----------------------------------------------------------------------------
{
  m_AnimateInSubrange = animate_subrange;
  m_Config->Write("AnimateInSubrange", m_AnimateInSubrange);
  Update();
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::SetTimeMultiplier(double tmult)
//----------------------------------------------------------------------------
{
  m_TimeSpeed = tmult;
  m_Config->Write("TimeMultiplier", m_TimeSpeed);
  Update();
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::SetTimeScale(double tscale)
//----------------------------------------------------------------------------
{
  m_TimeScale = tscale;
  m_Config->Write("TimeScale", m_TimeScale);
  Update();
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::PlayInActiveViewport(int active_viewport)
//----------------------------------------------------------------------------
{
  m_PlayInActiveViewport = active_viewport;
  m_Config->Write("PlayInActiveViewport", m_PlayInActiveViewport);
  Update();
}
//----------------------------------------------------------------------------
void albaGUISettingsTimeBar::Update()
//----------------------------------------------------------------------------
{
  if(m_Gui)
  {
    m_Gui->Update();
    EnableWidgets();
  }

  m_Config->Flush();
}
