/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUITimeBar.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden - Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGUITimeBar_H__
#define __mafGUITimeBar_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/datetime.h>
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafGUIPanel.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUIPicButton;    
class mafGUIValidator; 
class mafGUIFloatSlider;
class mafGUISettingsTimeBar;

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
# define ID_TITLE  700
# define ID_CLOSE  701
# define ID_TIMER  702

//----------------------------------------------------------------------------
// mafGUITimeBar :
/** mafGUITimeBar is a wxPanel with a set of widget to handle time. */
//----------------------------------------------------------------------------
class mafGUITimeBar: public mafGUIPanel , public mafObserver
{
public:
  mafGUITimeBar(wxWindow* parent,wxWindowID id = -1,bool CloseButton = false); 
  virtual ~mafGUITimeBar();
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  void OnEvent(mafEventBase *maf_event);
  
	/** Set the time bounds for the time bar.
  Time bar manage time expressed in milliseconds.*/
	void SetBounds(double min, double max);

  /** Set the multiplier for timer. Default value is to show time data represented in seconds.*/
  void SetMultiplier(double mult = 1000.0);

  /** Set the reference to the time setting panel.*/
  void SetTimeSettings(mafGUISettingsTimeBar *settings);

  /** Set the number of subdivisions for the slider.*/
  void SetNumberOfIntervals(int intervals);

protected:
  double m_Time; 
  double m_TimeMin; 
  double m_TimeMax; 
  double m_TimeStep;
  double m_TimeStart; ///< MAF time when the play button was pressed.

  int m_NumberOfIntervals; ///< Number of subdivision intervals for the time bar.

  mafString m_TimeMinString;
  mafString m_TimeMaxString;

  wxDateTime       m_WorldTimeStart; ///< World time when the play button was pressed.

  wxBoxSizer		  *m_Sizer;
  mafGUIPicButton		*m_TimeBarButtons[5];
  wxTextCtrl		  *m_TimeBarEntry;
  mafGUIFloatSlider	*m_TimeBarSlider; 
  wxTimer          m_Timer;

  mafObserver     *m_Listener;
  mafGUISettingsTimeBar *m_TimeBarSettings; ///< Settings panel for the time bar.

  /** Update the time and send the TIME_SET event to synchronize all the application. */
	void OnTimer(wxTimerEvent &event);
  
	/** Update the time bar interface. */
	void Update();

  /** Calculate the Time step according to the number of intervals.*/
  void UpdateTimeStep();

DECLARE_EVENT_TABLE()
};
#endif
