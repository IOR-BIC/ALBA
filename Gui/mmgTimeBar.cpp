/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgTimeBar.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-23 11:59:45 $
  Version:   $Revision: 1.11 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "mmgTimeBar.h"  
#include <wx/treectrl.h> // per floor
#include "mafDecl.h"
#include "mafPics.h"  //SIL. 7-4-2005: 
#include "mmgPicButton.h"
#include "mmgValidator.h"
#include "mmgFloatSlider.h"
//----------------------------------------------------------------------------
// constant
//----------------------------------------------------------------------------
enum TIMEBAR_CONSTANTS
{
  ID_ENTRY = MINID,
  ID_SLIDER,
	TIME_BAR_VELOCITY,
	ID_CLOSE_BUTTON,
};
//----------------------------------------------------------------------------
// mmgTimeBar
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgTimeBar,mmgPanel)
  EVT_TIMER(ID_TIMER, mmgTimeBar::OnTimer)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
#define mmgTimeBarStyle wxNO_BORDER | wxWANTS_CHARS | wxTAB_TRAVERSAL

//----------------------------------------------------------------------------
mmgTimeBar::mmgTimeBar( wxWindow* parent,wxWindowID id,bool CloseButton)
:mmgPanel(parent,id,wxDefaultPosition,wxDefaultSize,mmgTimeBarStyle), 
m_Timer(NULL, ID_TIMER)         
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_Timer.SetOwner(this, ID_TIMER);
  
  m_Sizer =  new wxBoxSizer( wxHORIZONTAL );
  this->SetAutoLayout( TRUE );
  this->SetSizer( m_Sizer );
  m_Sizer->Fit(this);
  m_Sizer->SetSizeHints(this);
  
  m_Time      = 0;
  m_TimeMin  = 0; 
  m_TimeMax  = 100; 
  m_TimeStep = 1;

  wxStaticText *lab1 = new wxStaticText(this,-1," time "); 
  m_TimeBarEntry = new wxTextCtrl  (this, ID_ENTRY, "" , wxDefaultPosition, wxSize(40,17), 0 );
  m_TimeBarEntry->SetValidator(mmgValidator(this,ID_ENTRY,m_TimeBarEntry,&m_Time,m_TimeMin,m_TimeMax));

  wxStaticText *lab2 = new wxStaticText(this,-1," speed "); 
  m_TimeBarEntryVelocity = new wxTextCtrl  (this, TIME_BAR_VELOCITY, "" , wxDefaultPosition, wxSize(40,17), 0 );
  m_TimeBarEntryVelocity ->SetValidator(mmgValidator(this,TIME_BAR_VELOCITY,m_TimeBarEntryVelocity,&m_TimeStep,1,100));

  m_TimeBarSlider = new mmgFloatSlider(this,ID_SLIDER,m_Time,m_TimeMin,m_TimeMax );
  m_TimeBarSlider->SetValidator( mmgValidator(this,ID_SLIDER,m_TimeBarSlider,&m_Time,m_TimeBarEntry) );
  m_TimeBarSlider->SetNumberOfSteps(500);
  TransferDataToWindow();
  m_Sizer->Add( m_TimeBarSlider ,1,wxEXPAND);
  m_Sizer->Add( lab1,0,wxALIGN_CENTER);
  m_Sizer->Add( m_TimeBarEntry,0,wxALIGN_CENTER);
  m_Sizer->Add( lab2,0,wxALIGN_CENTER);
  m_Sizer->Add( m_TimeBarEntryVelocity,0,wxALIGN_CENTER);

  m_TimeBarButtons[0] = new mmgPicButton(this, "TIME_BEGIN", TIME_BEGIN, this);
  m_TimeBarButtons[1] = new mmgPicButton(this, "TIME_PREV",  TIME_PREV , this);
  m_TimeBarButtons[2] = new mmgPicButton(this, "TIME_PLAY",  TIME_PLAY , this);
  m_TimeBarButtons[3] = new mmgPicButton(this, "TIME_NEXT",  TIME_NEXT , this);
  m_TimeBarButtons[4] = new mmgPicButton(this, "TIME_END",   TIME_END  , this);

  for(int i = 0; i < 5; i++)
    m_Sizer->Add(m_TimeBarButtons[i],0,0);
}
//----------------------------------------------------------------------------
mmgTimeBar::~mmgTimeBar()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgTimeBar::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  bool play = false;

  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case TIME_PLAY:
      play = true;
      break; 
    case TIME_STOP:
    case ID_ENTRY:
    case ID_SLIDER:
      // time set is called later on
      break;
    case TIME_BAR_VELOCITY:
      break;
    case ID_CLOSE_BUTTON:
      //HideGui();
      break;
    case TIME_PREV:
      //m_Time -= m_TimeStep;
      m_Time -= 1.0;
      m_Time = (m_Time <= m_TimeMin) ? m_TimeMin : m_Time;
      break;
    case TIME_NEXT:
      //m_Time += m_TimeStep;
      m_Time += 1.0;
      m_Time = (m_Time >= m_TimeMax) ? m_TimeMax : m_Time;
      break;
    case TIME_BEGIN:
      m_Time = m_TimeMin;
      break;
    case TIME_END:
      m_Time = m_TimeMax;
      break;
    default:
      e->Log();
      break;
    }
    if(play)
    {
      m_TimeBarButtons[2]->SetBitmap("TIME_STOP");
      m_TimeBarButtons[2]->SetEventId(TIME_STOP);
      m_Timer.Start(1);
    }
    else
    {
      m_TimeBarButtons[2]->SetBitmap("TIME_PLAY");
      m_TimeBarButtons[2]->SetEventId(TIME_PLAY);
      m_Timer.Stop();
    }

    if(e->GetId() != TIME_STOP )
      mafEventMacro(mafEvent(this,TIME_SET,m_Time));
    Update();
  }
}
//----------------------------------------------------------------------------
void mmgTimeBar::OnTimer(wxTimerEvent &event)
//----------------------------------------------------------------------------
{
  m_Time += m_TimeStep;
  m_Time = (m_Time >= m_TimeMax) ? m_TimeMin : m_Time;
  mafEventMacro(mafEvent(this,TIME_SET,m_Time,0));
  Update();
}
//----------------------------------------------------------------------------
void mmgTimeBar::Update()
//----------------------------------------------------------------------------
{
  TransferDataToWindow();
  mafYield();
}
//----------------------------------------------------------------------------
void mmgTimeBar::SetBounds(double min, double max)
//----------------------------------------------------------------------------
{
  if(max <= min) max = min+1;

  m_Timer.Stop();
  m_TimeMax = max;
  m_TimeMin = min;
  m_TimeStep = 1;

  if(m_Time < min) 
  {
    m_Time = min;
    mafEventMacro(mafEvent(this,TIME_SET,m_Time,0));
  }
  if(m_Time > max) 
  {
    m_Time = max;
    mafEventMacro(mafEvent(this,TIME_SET,m_Time,0));
  }

  m_TimeBarSlider->SetRange(min,max,m_Time);
  m_TimeBarSlider->Refresh();

  m_TimeBarEntry->SetValidator(mmgValidator(this,ID_ENTRY,m_TimeBarEntry,&m_Time,m_TimeMin,m_TimeMax));
  Update();
}
