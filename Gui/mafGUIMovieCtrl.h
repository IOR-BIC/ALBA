/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIMovieCtrl.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGUIMovieCtrl_H__
#define __mafGUIMovieCtrl_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafGUIPanel.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUIPicButton;    
class mafGUIValidator; 

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
#define ID_TIMER  702
#define MOVIE_BUTTONS_NUM 6

//----------------------------------------------------------------------------
// mafGUIMovieCtrl :
/** mafGUIMovieCtrl is a wxPanel with a set of widget to handle time. */
//----------------------------------------------------------------------------
class mafGUIMovieCtrl: public mafGUIPanel , public mafObserver
{
public:
  mafGUIMovieCtrl(wxWindow* parent,wxWindowID id = -1);
  virtual ~mafGUIMovieCtrl();
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
  mafGUIPicButton		*m_TimeBarButtons[MOVIE_BUTTONS_NUM];
  wxTimer          m_Timer;

  mafObserver     *m_Listener;

  /** Update the movie ctrl interface. */
  void Update();

  /** Update the time and send the TIME_SET event to synchronize all the application. */
	void OnTimer(wxTimerEvent &event);
DECLARE_EVENT_TABLE()
};
#endif
