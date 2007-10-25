/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUISettingsTimeBar.cpp,v $
Language:  C++
Date:      $Date: 2007-10-25 10:17:15 $
Version:   $Revision: 1.3 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGUISettingsTimeBar.h"

#include "mafDecl.h"
#include "mmgGui.h"

//----------------------------------------------------------------------------
mafGUISettingsTimeBar::mafGUISettingsTimeBar(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  m_RealTimeMode = 0;
  m_TimeMultiplier = 1000.0;
  m_Loop = 1;

  m_Fps = 25;
  m_SpeedId = 2;

  m_AnimateInSubrange = 0;
  m_SubRange[0] = 0.0; 
  m_SubRange[1] = 1.0;

  m_PlayInActiveViewport = 0;

  InitializeSettings();
}
//----------------------------------------------------------------------------
mafGUISettingsTimeBar::~mafGUISettingsTimeBar()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::CreateGui()
//----------------------------------------------------------------------------
{
  wxString speed_choices[5] = {"1/4x", "1/2x", "1x", "2x", "4x"};

  m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_REAL_TIME, _("real time"), &m_RealTimeMode, 1);
  m_Gui->Double(ID_TIME_MULTIPLIER, _("multiplier: "), &m_TimeMultiplier, 0.000001);
  m_Gui->Bool(ID_LOOP, _("loop playback"), &m_Loop, 1);
  m_Gui->Divider(2);
  m_Gui->Integer(ID_FPS, _("fps: "), &m_Fps, 1);
  m_Gui->Radio(ID_SPEED, _("speed: "), &m_SpeedId, 5, speed_choices);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_ANIMATE_IN_SUBRANGE, _("animate in subrange"), &m_AnimateInSubrange, 1);
  m_Gui->VectorN(ID_SUBRANGE, _("subrange: "), m_SubRange, 2, 0.0);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_PLAY_ACTIVE_VIEWPORT, _("active view playback"), &m_PlayInActiveViewport, 1);
  m_Gui->Divider();

  EnableWidgets();
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case ID_REAL_TIME:
      m_Config->Write("RealTimeMode", m_RealTimeMode);
    break;
    case ID_TIME_MULTIPLIER:
      m_Config->Write("TimeMultiplier", m_TimeMultiplier);
    break;
    case ID_ANIMATE_IN_SUBRANGE:
      m_Config->Write("AnimateInSubrange", m_AnimateInSubrange);
    break;
    case ID_SUBRANGE:
      m_Config->Write("SubRange0", m_SubRange[0]);
      m_Config->Write("SubRange1", m_SubRange[1]);
    break;
    case ID_LOOP:
      m_Config->Write("Loop", m_Loop);
    break;
    case ID_PLAY_ACTIVE_VIEWPORT:
      m_Config->Write("PlayInActiveViewport", m_PlayInActiveViewport);
    break;
    case ID_SPEED:
      m_Config->Write("SpeedId", m_SpeedId);
    break;
    case ID_FPS:
      m_Config->Write("Fps", m_Fps);
    break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
  // Update the time bar.
  maf_event->SetSender(this);
  mafEventMacro(*maf_event);
  Update();
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::InitializeSettings()
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
    m_TimeMultiplier = double_item;
  }
  else
  {
    SetTimeMultiplier(m_TimeMultiplier);
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
  if(m_Config->Read("SpeedId", &long_item))
  {
    m_SpeedId = long_item;
  }
  else
  {
    SetSpeedId(m_SpeedId);
  }
  if(m_Config->Read("Fps", &long_item))
  {
    m_Fps = long_item;
  }
  else
  {
    SetFPS(m_Fps);
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::EnableWidgets()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_TIME_MULTIPLIER, m_RealTimeMode == 1);
  m_Gui->Enable(ID_FPS, m_RealTimeMode != 1);
  m_Gui->Enable(ID_SPEED, m_RealTimeMode != 1);
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::SetSubrange(double min, double max)
//----------------------------------------------------------------------------
{
  m_SubRange[0] = min < max ? min : max;
  m_SubRange[1] = min < max ? max : min;
  m_Config->Write("SubRange0", m_SubRange[0]);
  m_Config->Write("SubRange1", m_SubRange[1]);
  Update();
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::SetSubrange(double sub_range[2])
//----------------------------------------------------------------------------
{
  SetSubrange(sub_range[0], sub_range[1]);
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::SetRealTimeMode(int realTime)
//----------------------------------------------------------------------------
{
  m_RealTimeMode = realTime;
  m_Config->Write("RealTimeMode", m_RealTimeMode);
  Update();
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::SetFPS(int fps)
//----------------------------------------------------------------------------
{
  m_Fps = fps;
  m_Config->Write("Fps", m_Fps);
  Update();
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::LoopAnimation(int loop)
//----------------------------------------------------------------------------
{
  m_Loop = loop;
  m_Config->Write("Loop", m_Loop);
  Update();
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::AminateInSubrange(int animate_subrange)
//----------------------------------------------------------------------------
{
  m_AnimateInSubrange = animate_subrange;
  m_Config->Write("AnimateInSubrange", m_AnimateInSubrange);
  Update();
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::SetSpeedId(int idx)
//----------------------------------------------------------------------------
{
  m_SpeedId = idx;
  m_Config->Write("SpeedId", m_SpeedId);
  Update();
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::SetTimeMultiplier(double tmult)
//----------------------------------------------------------------------------
{
  m_TimeMultiplier = tmult;
  m_Config->Write("TimeMultiplier", m_TimeMultiplier);
  Update();
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::PlayInActiveViewport(int active_viewport)
//----------------------------------------------------------------------------
{
  m_PlayInActiveViewport = active_viewport;
  m_Config->Write("PlayInActiveViewport", m_PlayInActiveViewport);
  Update();
}
//----------------------------------------------------------------------------
void mafGUISettingsTimeBar::Update()
//----------------------------------------------------------------------------
{
  if(m_Gui)
  {
    m_Gui->Update();
    EnableWidgets();
  }

  m_Config->Flush();
}
