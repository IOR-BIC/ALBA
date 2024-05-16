/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITimeBar
 Authors: Silvano Imboden - Paolo Quadrani
 
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



#include "albaGUITimeBar.h"  
#include <wx/treectrl.h> // per floor
#include "albaDecl.h"
#include "albaPics.h"  //SIL. 7-4-2005: 
#include "albaGUIPicButton.h"
#include "albaGUIValidator.h"
#include "albaGUIFloatSlider.h"
#include "albaGUISettingsTimeBar.h"

//----------------------------------------------------------------------------
// constant
//----------------------------------------------------------------------------
enum TIMEBAR_CONSTANTS
{
  ID_ENTRY = MINID,
  ID_SLIDER,
	TIME_BAR_VELOCITY,
	ID_CLOSE_BUTTON,
  ID_REAL_TIME,
};
//----------------------------------------------------------------------------
// albaGUITimeBar
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUITimeBar,albaGUIPanel)
  EVT_TIMER(ID_TIMER, albaGUITimeBar::OnTimer)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
#define albaGUITimeBarStyle wxNO_BORDER | wxWANTS_CHARS | wxTAB_TRAVERSAL

//----------------------------------------------------------------------------
albaGUITimeBar::albaGUITimeBar( wxWindow* parent,wxWindowID id,bool CloseButton)
:albaGUIPanel(parent,id,wxDefaultPosition,wxDefaultSize,albaGUITimeBarStyle), 
m_Timer(NULL, ID_TIMER)
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_TimeBarSettings = NULL;
  m_Timer.SetOwner(this, ID_TIMER);
  
  m_Sizer =  new wxBoxSizer( wxHORIZONTAL );
  this->SetAutoLayout( true );
  this->SetSizer( m_Sizer );
  m_Sizer->Fit(this);
  m_Sizer->SetSizeHints(this);
  
  m_Time     = 0;
  m_TimeMin  = 0; 
  m_TimeMax  = 1;

  m_TimeMinString = "";
  m_TimeMinString << m_TimeMin;
  m_TimeMaxString = "";
  m_TimeMaxString << m_TimeMax;

  wxStaticText *labTimeMin = new wxStaticText(this, -1, m_TimeMinString.GetCStr());
  labTimeMin->SetValidator( albaGUIValidator(this, -1, labTimeMin, &m_TimeMinString) );
  
  wxStaticText *labTimeMax = new wxStaticText(this, -1, m_TimeMaxString.GetCStr());
  labTimeMax->SetValidator( albaGUIValidator(this, -1, labTimeMax, &m_TimeMaxString) );

  wxStaticText *lab1 = new wxStaticText(this,-1," time ");
  m_TimeBarEntry = new wxTextCtrl  (this, ID_ENTRY, "" , wxDefaultPosition, wxSize(40,17), 0 );
  m_TimeBarEntry->SetValidator(albaGUIValidator(this,ID_ENTRY,m_TimeBarEntry,&m_Time,m_TimeMin,m_TimeMax));

  m_TimeBarSlider = new albaGUIFloatSlider(this,ID_SLIDER,m_Time,m_TimeMin,m_TimeMax);
  m_TimeBarSlider->SetValidator( albaGUIValidator(this,ID_SLIDER,m_TimeBarSlider,&m_Time,m_TimeBarEntry) );
  m_TimeBarSlider->SetNumberOfSteps(m_NumberOfIntervals);
  
  TransferDataToWindow();
  m_Sizer->Add(labTimeMin,0,wxALIGN_CENTER);
  m_Sizer->Add(m_TimeBarSlider ,1,wxEXPAND);
  m_Sizer->Add(labTimeMax,0,wxALIGN_CENTER);
  m_Sizer->Add(lab1,0,wxALIGN_CENTER);
  m_Sizer->Add(m_TimeBarEntry,0,wxALIGN_CENTER);

  m_TimeBarButtons[0] = new albaGUIPicButton(this, "TIME_BEGIN", TIME_BEGIN, this);
  m_TimeBarButtons[1] = new albaGUIPicButton(this, "TIME_PREV",  TIME_PREV , this);
  m_TimeBarButtons[2] = new albaGUIPicButton(this, "TIME_PLAY",  TIME_PLAY , this);
  m_TimeBarButtons[3] = new albaGUIPicButton(this, "TIME_NEXT",  TIME_NEXT , this);
  m_TimeBarButtons[4] = new albaGUIPicButton(this, "TIME_END",   TIME_END  , this);

  for(int i = 0; i < 5; i++)
    m_Sizer->Add(m_TimeBarButtons[i],0,0);

  SetNumberOfIntervals(500);
}
//----------------------------------------------------------------------------
albaGUITimeBar::~albaGUITimeBar()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUITimeBar::SetNumberOfIntervals(int intervals)
//----------------------------------------------------------------------------
{
  m_NumberOfIntervals = intervals < 1 ? 1 : intervals;
  m_TimeBarSlider->SetNumberOfSteps(m_NumberOfIntervals);
  if (m_Timer.IsRunning())
  {
    m_Timer.Stop();
  }
  if (m_TimeBarSettings != NULL && m_TimeBarSettings->GetRealTimeMode() == 0)
  {
    UpdateTimeStep();
  }
}
//----------------------------------------------------------------------------
void albaGUITimeBar::UpdateTimeStep()
//----------------------------------------------------------------------------
{
  if (m_NumberOfIntervals == 1)
  {
    m_TimeStep = m_TimeMax - m_TimeMin;
  }
  else
  {
    m_TimeStep = (m_TimeMax - m_TimeMin) / (m_NumberOfIntervals - 1);
  }
  if (m_TimeBarSettings != NULL)
  {
    m_TimeStep *= m_TimeBarSettings->GetTimeMultiplier();
  }
}
//----------------------------------------------------------------------------
void albaGUITimeBar::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (alba_event->GetSender() == m_TimeBarSettings)
  {
    switch(alba_event->GetId())
    {
      case albaGUISettingsTimeBar::ID_TIME_STEP:
      case albaGUISettingsTimeBar::ID_NUMBER_OF_FRAMES:
        SetNumberOfIntervals(m_TimeBarSettings->GetNumberOfFrames());
      break;
      case albaGUISettingsTimeBar::ID_REAL_TIME:
      case albaGUISettingsTimeBar::ID_TIME_SPEED:
        if (m_Timer.IsRunning())
        {
          m_Timer.Stop();
        }
        if (m_TimeBarSettings->GetRealTimeMode() == 0)
        {
          UpdateTimeStep();
        }
        Update();
      break;
    }
  }
  else
  {
    bool play = false;
    // Check for sub-range animation
    double time_max, time_min;
    time_min = (m_TimeBarSettings->GetAnimateSubrange() == 1) ? m_TimeBarSettings->GetSubrange()[0] : m_TimeMin;
    time_max = (m_TimeBarSettings->GetAnimateSubrange() == 1) ? m_TimeBarSettings->GetSubrange()[1] : m_TimeMax;

    if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
    {
      switch(e->GetId())
      {
        case TIME_PLAY:
          play = true;
          if (m_Time >= time_max || m_Time <= time_min)
          {
            m_Time = time_min;
          }
        break; 
        case TIME_STOP:
        case ID_ENTRY:
        case ID_SLIDER:
          // time set is called later on
        break;
        case ID_CLOSE_BUTTON:
        break;
        case TIME_PREV:
          m_Time -= m_TimeStep;
          m_Time = (m_Time <= time_min) ? time_min : m_Time;
        break;
        case TIME_NEXT:
          m_Time += m_TimeStep;
          m_Time = (m_Time >= time_max) ? time_max : m_Time;
        break;
        case TIME_BEGIN:
          m_Time = time_min;
        break;
        case TIME_END:
          m_Time = time_max;
        break;
        default:
          e->Log();
        break;
      }
      if(play)
      {
        m_TimeBarButtons[2]->SetBitmap("TIME_STOP");
        m_TimeBarButtons[2]->SetEventId(TIME_STOP);

        m_TimeStart = m_Time;
        m_WorldTimeStart = wxDateTime::UNow();

        m_Timer.Start(1);
      }
      else
      {
        m_TimeBarButtons[2]->SetBitmap("TIME_PLAY");
        m_TimeBarButtons[2]->SetEventId(TIME_PLAY);
        m_Timer.Stop();
      }

      if(e->GetId() != TIME_STOP )
        albaEventMacro(albaEvent(this,TIME_SET,m_Time));
      Update();
    }
  }
}
//----------------------------------------------------------------------------
void albaGUITimeBar::OnTimer(wxTimerEvent &event)
//----------------------------------------------------------------------------
{
  // Check for sub-range animation
  double time_max, time_min;
  time_min = (m_TimeBarSettings->GetAnimateSubrange() == 1) ? m_TimeBarSettings->GetSubrange()[0] : m_TimeMin;
  time_max = (m_TimeBarSettings->GetAnimateSubrange() == 1) ? m_TimeBarSettings->GetSubrange()[1] : m_TimeMax;

  if (m_TimeBarSettings->GetRealTimeMode() == 0)
  {
    m_Time += m_TimeStep;
  }
  else
  {
    wxDateTime dt = wxDateTime::UNow();
    wxTimeSpan ts = dt.Subtract(m_WorldTimeStart);
    wxLongLong delta = ts.GetMilliseconds();
    m_Time = m_TimeStart + delta.ToDouble() / m_TimeBarSettings->GetTimeScale();
  }
  
  if (m_Time >= time_max)
  {
    if (m_TimeBarSettings->GetLoop() == 1)
    {
      m_Time = time_min;
      m_TimeStart = m_Time;
      m_WorldTimeStart = wxDateTime::UNow();
    }
    else
    {
      m_Time = time_max;
      OnEvent(&albaEvent(this, TIME_STOP));
    }
  }

  albaEventMacro(albaEvent(this, TIME_SET, m_Time, 0));
  Update();
}
//----------------------------------------------------------------------------
void albaGUITimeBar::Update()
//----------------------------------------------------------------------------
{
  TransferDataToWindow();
  albaYield();
}
//----------------------------------------------------------------------------
void albaGUITimeBar::SetBounds(double min, double max)
//----------------------------------------------------------------------------
{
  if(max <= min) 
    max = min + 1;

  m_Timer.Stop();

  m_TimeMax = max;
  m_TimeMin = min;
  m_TimeBarSettings->SetTimeBounds(min, max);
  //SetNumberOfIntervals(m_TimeBarSettings->GetNumberOfFrames());
  if (m_TimeBarSettings->GetRealTimeMode() == 0)
  {
    UpdateTimeStep();
  }

  if(m_Time < min) 
  {
    m_Time = min;
    albaEventMacro(albaEvent(this,TIME_SET,m_Time,0));
  }
  if(m_Time > max) 
  {
    m_Time = max;
    albaEventMacro(albaEvent(this,TIME_SET,m_Time,0));
  }

  m_TimeBarSlider->SetRange(min,max,m_Time);
  m_TimeBarSlider->Refresh();

  m_TimeBarEntry->SetValidator(albaGUIValidator(this,ID_ENTRY,m_TimeBarEntry,&m_Time,m_TimeMin,m_TimeMax));

  m_TimeMinString = "";
  m_TimeMinString << m_TimeMin;
  m_TimeMaxString = "";
  m_TimeMaxString << m_TimeMax;

  //m_TimeBarSettings->SetSubrange(m_TimeMin,m_TimeMax);
  Update();
}
//----------------------------------------------------------------------------
void albaGUITimeBar::SetMultiplier(double mult)
//----------------------------------------------------------------------------
{
  m_TimeBarSettings->SetTimeMultiplier(mult);
}
//----------------------------------------------------------------------------
void albaGUITimeBar::SetTimeSettings(albaGUISettingsTimeBar *settings)
//----------------------------------------------------------------------------
{
  m_TimeBarSettings = settings;
  m_TimeBarSettings->SetTimeBounds(m_TimeMin, m_TimeMax);
  m_TimeBarSettings->SetNumberOfFrames(m_NumberOfIntervals);
}
