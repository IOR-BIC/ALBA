/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMovieCtrl.cpp,v $
  Language:  C++
  Date:      $Date: 2006-01-12 10:32:21 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
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



#include "mmgMovieCtrl.h"  
#include "mafDecl.h"
#include "mafPics.h"
#include "mmgPicButton.h"
#include "mmgValidator.h"

//----------------------------------------------------------------------------
// mmgMovieCtrl
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgMovieCtrl,mmgPanel)
  EVT_TIMER(ID_TIMER, mmgMovieCtrl::OnTimer)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
#define mmgMovieCtrlStyle wxNO_BORDER | wxWANTS_CHARS | wxTAB_TRAVERSAL

enum MOVIECTRL_WIDGETS_ID
{
  ID_LOOP = MINID,
};
//----------------------------------------------------------------------------
mmgMovieCtrl::mmgMovieCtrl( wxWindow* parent,wxWindowID id)
:mmgPanel(parent,id,wxDefaultPosition,wxDefaultSize,mmgMovieCtrlStyle), 
m_Timer(NULL, ID_TIMER)         
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_Timer.SetOwner(this, ID_TIMER);
  
  m_Frame    = 0;
  m_FrameMin = 0; 
  m_FrameMax = 100; 
  m_FrameStep= 1;

  m_Loop = false;

  m_LoopCheck = new wxCheckBox(this, ID_LOOP, "loop", wxDefaultPosition, wxSize(-1,17), 0 );
  m_LoopCheck->SetValidator( mmgValidator(this,ID_LOOP,m_LoopCheck,&m_Loop) );
  TransferDataToWindow();

  m_Sizer =  new wxBoxSizer( wxHORIZONTAL );
  m_TimeBarButtons[MOVIE_BEGIN] = new mmgPicButton(this, "TIME_BEGIN", TIME_BEGIN, this);
  m_TimeBarButtons[MOVIE_PREV] = new mmgPicButton(this, "TIME_PREV",  TIME_PREV , this);
  m_TimeBarButtons[MOVIE_PLAY] = new mmgPicButton(this, "TIME_PLAY",  TIME_PLAY, this);
  m_TimeBarButtons[MOVIE_NEXT] = new mmgPicButton(this, "TIME_NEXT",  TIME_NEXT , this);
  m_TimeBarButtons[MOVIE_END] = new mmgPicButton(this, "TIME_END",   TIME_END  , this);
  m_TimeBarButtons[MOVIE_REC] = new mmgPicButton(this, "MOVIE_RECORD",  MOVIE_RECORD , this);

  for(int i = 0; i < MOVIE_BUTTONS_NUM; i++)
    m_Sizer->Add(m_TimeBarButtons[i],0,0);
  m_Sizer->Add(m_LoopCheck,0,wxLEFT,5);

  this->SetAutoLayout( TRUE );
  this->SetSizer( m_Sizer );
  m_Sizer->Fit(this);
  m_Sizer->SetSizeHints(this);
}
//----------------------------------------------------------------------------
mmgMovieCtrl::~mmgMovieCtrl()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgMovieCtrl::OnEvent(mafEventBase *maf_event)
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
      break;
      case ID_LOOP:
      break;
      case TIME_PREV:
        m_Frame -= m_FrameStep;
        m_Frame = (m_Frame <= m_FrameMin) ? m_FrameMin : m_Frame;
        mafEventMacro(mafEvent(this,TIME_SET, m_Frame));
      break;
      case TIME_NEXT:
        m_Frame += m_FrameStep;
        m_Frame = (m_Frame >= m_FrameMax) ? m_FrameMax : m_Frame;
        mafEventMacro(mafEvent(this,TIME_SET, m_Frame));
      break;
      case TIME_BEGIN:
        m_Frame = m_FrameMin;
        mafEventMacro(mafEvent(this,TIME_SET, m_Frame));
      break;
      case TIME_END:
        m_Frame = m_FrameMax;
        mafEventMacro(mafEvent(this,TIME_SET, m_Frame));
      break;
      case MOVIE_RECORD:
        mafEventMacro(mafEvent(this,MOVIE_RECORD));
      break;
      default:
        e->Log();
      break;
    }
    if(play)
    {
      m_TimeBarButtons[MOVIE_PLAY]->SetBitmap("TIME_STOP");
      m_TimeBarButtons[MOVIE_PLAY]->SetEventId(TIME_STOP);
      
      m_TimeBarButtons[MOVIE_BEGIN]->Enable(false);
      m_TimeBarButtons[MOVIE_PREV]->Enable(false);
      m_TimeBarButtons[MOVIE_NEXT]->Enable(false);
      m_TimeBarButtons[MOVIE_END]->Enable(false);
      m_TimeBarButtons[MOVIE_REC]->Enable(false);
      m_LoopCheck->Enable(false);
      m_Timer.Start(1);
    }
    else
    {
      m_TimeBarButtons[MOVIE_PLAY]->SetBitmap("TIME_PLAY");
      m_TimeBarButtons[MOVIE_PLAY]->SetEventId(TIME_PLAY);

      m_TimeBarButtons[MOVIE_BEGIN]->Enable();
      m_TimeBarButtons[MOVIE_PREV]->Enable();
      m_TimeBarButtons[MOVIE_NEXT]->Enable();
      m_TimeBarButtons[MOVIE_END]->Enable();
      m_TimeBarButtons[MOVIE_REC]->Enable();
      m_LoopCheck->Enable();
      m_Timer.Stop();
    }

    Update();
  }
}
//----------------------------------------------------------------------------
void mmgMovieCtrl::OnTimer(wxTimerEvent &event)
//----------------------------------------------------------------------------
{
  m_Frame += m_FrameStep;
  if (m_Loop)
  {
    m_Frame = (m_Frame > m_FrameMax) ? m_FrameMin : m_Frame;
  }
  else
  {
    if(m_Frame > m_FrameMax)
    {
      m_Frame = m_FrameMin;
      mafEvent ev_stop(this,TIME_STOP);
      OnEvent(&ev_stop);
    }
  }
  mafEventMacro(mafEvent(this,TIME_SET,m_Frame,0));
  Update();
}
//----------------------------------------------------------------------------
void mmgMovieCtrl::SetFrameBounds(double min, double max, double step)
//----------------------------------------------------------------------------
{
  if(max <= min) max = min+1;

  m_Timer.Stop();
  m_FrameMax = max;
  m_FrameMin = min;
  m_FrameStep = (step > (max - min)) ? (max - min) : step;

  if(m_Frame < min) 
  {
    m_Frame = min;
    mafEventMacro(mafEvent(this,TIME_SET,m_Frame,0));
  }
  if(m_Frame > max) 
  {
    m_Frame = max;
    mafEventMacro(mafEvent(this,TIME_SET,m_Frame,0));
  }
}
//----------------------------------------------------------------------------
void mmgMovieCtrl::Update()
//----------------------------------------------------------------------------
{
  TransferDataToWindow();
  mafYield();
}
