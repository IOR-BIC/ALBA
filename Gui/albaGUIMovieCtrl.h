/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIMovieCtrl
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIMovieCtrl_H__
#define __albaGUIMovieCtrl_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaGUIPanel.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIPicButton;    
class albaGUIValidator; 

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
#define ID_TIMER  702
#define MOVIE_BUTTONS_NUM 6

//----------------------------------------------------------------------------
// albaGUIMovieCtrl :
/** albaGUIMovieCtrl is a wxPanel with a set of widget to handle time. */
//----------------------------------------------------------------------------
class albaGUIMovieCtrl: public albaGUIPanel , public albaObserver
{
public:
  albaGUIMovieCtrl(wxWindow* parent,wxWindowID id = -1);
  virtual ~albaGUIMovieCtrl();
  void SetListener(albaObserver *Listener) {m_Listener = Listener;};

  void OnEvent(albaEventBase *alba_event);
  
  /** Set the frames bounds for the movie.*/
  void SetFrameBounds(double min, double max, double step = 1);

  /** Enable/Disable loop.*/
  void Loop(bool enable = true) {m_Loop = enable;};

protected:
  double  m_Frame;
  double  m_FrameMin;
  double  m_FrameMax;
  int     m_FrameStep;
  int     m_Loop;
  wxCheckBox *m_LoopCheck;

  //----------------------------------------------------------------------------
  // constant
  //----------------------------------------------------------------------------
  enum MOVIECTRL_CONSTANTS
  {
    MOVIE_BEGIN = 0,
    MOVIE_PREV,
    MOVIE_PLAY,
    MOVIE_NEXT,
    MOVIE_END,
    MOVIE_REC,
  };

  wxBoxSizer		  *m_Sizer;
  albaGUIPicButton		*m_TimeBarButtons[MOVIE_BUTTONS_NUM];
  wxTimer          m_Timer;

  albaObserver     *m_Listener;

  /** Update the movie ctrl interface. */
  void Update();

  /** Update the time and send the TIME_SET event to synchronize all the application. */
	void OnTimer(wxTimerEvent &event);
DECLARE_EVENT_TABLE()
};
#endif
