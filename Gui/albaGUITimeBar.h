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

#ifndef __albaGUITimeBar_H__
#define __albaGUITimeBar_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/datetime.h>
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaGUIPanel.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIPicButton;    
class albaGUIValidator; 
class albaGUIFloatSlider;
class albaGUISettingsTimeBar;

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
# define ID_TITLE  700
# define ID_CLOSE  701
# define ID_TIMER  702

//----------------------------------------------------------------------------
// albaGUITimeBar :
/** albaGUITimeBar is a wxPanel with a set of widget to handle time. */
//----------------------------------------------------------------------------
class albaGUITimeBar: public albaGUIPanel , public albaObserver
{
public:
  albaGUITimeBar(wxWindow* parent,wxWindowID id = -1,bool CloseButton = false); 
  virtual ~albaGUITimeBar();
  void SetListener(albaObserver *Listener) {m_Listener = Listener;};

  void OnEvent(albaEventBase *alba_event);
  
	/** Set the time bounds for the time bar.
  Time bar manage time expressed in milliseconds.*/
	void SetBounds(double min, double max);

  /** Set the multiplier for timer. Default value is to show time data represented in seconds.*/
  void SetMultiplier(double mult = 1000.0);

  /** Set the reference to the time setting panel.*/
  void SetTimeSettings(albaGUISettingsTimeBar *settings);

  /** Set the number of subdivisions for the slider.*/
  void SetNumberOfIntervals(int intervals);

protected:
  double m_Time; 
  double m_TimeMin; 
  double m_TimeMax; 
  double m_TimeStep;
  double m_TimeStart; ///< ALBA time when the play button was pressed.

  int m_NumberOfIntervals; ///< Number of subdivision intervals for the time bar.

  albaString m_TimeMinString;
  albaString m_TimeMaxString;

  wxDateTime       m_WorldTimeStart; ///< World time when the play button was pressed.

  wxBoxSizer		  *m_Sizer;
  albaGUIPicButton		*m_TimeBarButtons[5];
  wxTextCtrl		  *m_TimeBarEntry;
  albaGUIFloatSlider	*m_TimeBarSlider; 
  wxTimer          m_Timer;

  albaObserver     *m_Listener;
  albaGUISettingsTimeBar *m_TimeBarSettings; ///< Settings panel for the time bar.

  /** Update the time and send the TIME_SET event to synchronize all the application. */
	void OnTimer(wxTimerEvent &event);
  
	/** Update the time bar interface. */
	void Update();

  /** Calculate the Time step according to the number of intervals.*/
  void UpdateTimeStep();

DECLARE_EVENT_TABLE()
};
#endif
