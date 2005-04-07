/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgTimeBar.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-07 08:37:04 $
  Version:   $Revision: 1.4 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#include "mmgTimeBar.h"  

#include <wx/treectrl.h> // per floor

#include "mafDecl.h"

#include "mmgBitmaps.h" //to be removed
#include "mafPics.h"  //SIL. 7-4-2005: 


#include "mmgPicButton.h"
#include "mmgValidator.h"
#include "mmgFloatSlider.h"
//----------------------------------------------------------------------------
// constant
//----------------------------------------------------------------------------
enum 
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
m_timer(NULL, ID_TIMER)         
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
  m_timer.SetOwner(this, ID_TIMER);
  
  m_sizer =  new wxBoxSizer( wxHORIZONTAL );
  this->SetAutoLayout( TRUE );
  this->SetSizer( m_sizer );
  m_sizer->Fit(this);
  m_sizer->SetSizeHints(this);
  
  m_guih = NULL;
	m_gui  = NULL;

  m_time      = 0;
  m_time_min  = 0; 
  m_time_max  = 100; 
  m_time_step = 1;

  wxStaticText *lab1 = new wxStaticText(this,-1," time "); 
  m_entry = new wxTextCtrl  (this, ID_ENTRY, "" , wxDefaultPosition, wxSize(40,17), 0 );
  m_entry->SetValidator(mmgValidator(this,ID_ENTRY,m_entry,&m_time,m_time_min,m_time_max));

  wxStaticText *lab2 = new wxStaticText(this,-1," speed "); 
  m_entry2 = new wxTextCtrl  (this, TIME_BAR_VELOCITY, "" , wxDefaultPosition, wxSize(40,17), 0 );
  m_entry2 ->SetValidator(mmgValidator(this,TIME_BAR_VELOCITY,m_entry2,&m_time_step,1,100));

  m_slider = new mmgFloatSlider(this,ID_SLIDER,m_time,m_time_min,m_time_max );
  m_slider->SetValidator( mmgValidator(this,ID_SLIDER,m_slider,&m_time,m_entry) );
  m_slider->SetNumberOfSteps(500);
  TransferDataToWindow();
  m_sizer->Add( m_slider ,1,wxEXPAND);
  m_sizer->Add( lab1,0,wxALIGN_CENTER);
  m_sizer->Add( m_entry,0,wxALIGN_CENTER);
  m_sizer->Add( lab2,0,wxALIGN_CENTER);
  m_sizer->Add( m_entry2,0,wxALIGN_CENTER);

  m_b[0] = new mmgPicButton(this, "TIME_BEGIN", TIME_BEGIN);  
  m_b[1] = new mmgPicButton(this, "TIME_PREV",  TIME_PREV);  
  m_b[2] = new mmgPicButton(this, "TIME_PLAY",  TIME_PLAY);  
  m_b[3] = new mmgPicButton(this, "TIME_NEXT",  TIME_NEXT);  
  m_b[4] = new mmgPicButton(this, "TIME_END",   TIME_END);  

  for(int i = 0; i < 5; i++)
  {
    m_b[i]->SetListener(this);  
    m_sizer->Add( m_b[i],0,0);
  }
}
//----------------------------------------------------------------------------
mmgTimeBar::~mmgTimeBar( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgTimeBar::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
  bool play = false;

  switch(e.GetId())
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
      //m_time -= m_time_step;
			m_time -= 1.0;
      m_time = (m_time <= m_time_min) ? m_time_min : m_time;
    break;
    case TIME_NEXT:
      //m_time += m_time_step;
			m_time += 1.0;
      m_time = (m_time >= m_time_max) ? m_time_max : m_time;
    break;
    case TIME_BEGIN:
      m_time = m_time_min;
    break;
    case TIME_END:
      m_time = m_time_max;
    break;
    default:
      e.Log();
    break;
  }
  if(play)
  {
    m_b[2]->SetBitmap("TIME_STOP");
    m_b[2]->SetEventId(TIME_STOP);
    m_timer.Start(1);
  }
  else
  {
    m_b[2]->SetBitmap("TIME_PLAY");
    m_b[2]->SetEventId(TIME_PLAY);
    m_timer.Stop();
  }
  
  if(e.GetId() != TIME_STOP )
    mafEventMacro(mafEvent(this,TIME_SET,m_time));
  Update();
}
//----------------------------------------------------------------------------
void mmgTimeBar::OnTimer(wxTimerEvent &event)
//----------------------------------------------------------------------------
{
  m_time += m_time_step;
  m_time = (m_time >= m_time_max) ? m_time_min : m_time;
  mafEventMacro(mafEvent(this,TIME_SET,m_time,0));
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
void mmgTimeBar::SetBounds(float min, float max)
//----------------------------------------------------------------------------
{
  if(max <= min) max = min+1;

  m_timer.Stop();
  m_time_max = max;
  m_time_min = min;
  m_time_step = 1;

  if(m_time < min) 
  {
    m_time = min;
    mafEventMacro(mafEvent(this,TIME_SET,m_time,0));
  }
  if(m_time > max) 
  {
    m_time = max;
    mafEventMacro(mafEvent(this,TIME_SET,m_time,0));
  }

  m_slider->SetRange(min,max,m_time);
  m_slider->Refresh();

  m_entry->SetValidator(mmgValidator(this,ID_ENTRY,m_entry,&m_time,m_time_min,m_time_max));
  Update();
}
/* - removed  //SIL. 4-4-2005: 
//----------------------------------------------------------------------------
void mmgTimeBar::ShowSettings()
//----------------------------------------------------------------------------
{
	if(m_gui != NULL) return; //already showed

	m_gui = new mmgGui(this);
  m_gui->SetListener(this);
	m_gui->Label("Velocity (1..100)");
	m_gui->Slider(TIME_BAR_VELOCITY,"",&m_time_step,1,100);
	m_gui->Label("");
  m_gui->Button(ID_CLOSE_BUTTON, "Close");

  ShowGui();
}
//----------------------------------------------------------------------------
void mmgTimeBar::ShowGui()
//----------------------------------------------------------------------------
{
  assert(m_gui); 
	m_guih = new mmgGuiHolder(mafGetFrame(),-1);
	m_guih->Put(m_gui);
	m_guih->SetTitle(wxString::Format(" Time Bar Preferences:"));
	// make modal the preferences panel
	
  //SIL. 23-3-2005: temporary commented out
  //wxUpdateUIEvent ui_e(ID_TIMEBAR_SETTINGS);
	//mafEventMacro(mafEvent(this,UPDATE_UI,&ui_e));
	//mafEventMacro(mafEvent(this,OP_SHOW_GUI,(wxWindow *)m_guih));
}
//----------------------------------------------------------------------------
void mmgTimeBar::HideGui()
//----------------------------------------------------------------------------
{
	assert(m_gui); 
	mafEventMacro(mafEvent(this,OP_HIDE_GUI,(wxWindow *)m_guih));
	// end modal
	wxUpdateUIEvent ui_e(ID_TIMEBAR_SETTINGS);
	mafEventMacro(mafEvent(this,UPDATE_UI,&ui_e));
	delete m_guih;
	m_guih = NULL;
	m_gui  = NULL;
}
*/
