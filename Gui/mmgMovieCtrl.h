/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMovieCtrl.h,v $
  Language:  C++
  Date:      $Date: 2006-01-12 10:32:21 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgMovieCtrl_H__
#define __mmgMovieCtrl_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include "mmgPanel.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgPicButton;    
class mmgValidator; 

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
#define ID_TIMER  702
#define MOVIE_BUTTONS_NUM 6

//----------------------------------------------------------------------------
// mmgMovieCtrl :
/** mmgMovieCtrl is a wxPanel with a set of widget to handle time. */
//----------------------------------------------------------------------------
class mmgMovieCtrl: public mmgPanel , public mafObserver
{
public:
  mmgMovieCtrl(wxWindow* parent,wxWindowID id = -1);
  virtual ~mmgMovieCtrl();
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  void OnEvent(mafEventBase *maf_event);
  
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
  mmgPicButton		*m_TimeBarButtons[MOVIE_BUTTONS_NUM];
  wxTimer          m_Timer;

  mafObserver     *m_Listener;

  /** Update the movie ctrl interface. */
  void Update();

  /** Update the time and send the TIME_SET event to synchronize all the application. */
	void OnTimer(wxTimerEvent &event);
DECLARE_EVENT_TABLE()
};
#endif
