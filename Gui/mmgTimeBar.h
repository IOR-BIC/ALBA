/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgTimeBar.h,v $
  Language:  C++
  Date:      $Date: 2005-12-23 11:59:45 $
  Version:   $Revision: 1.7 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgTimeBar_H__
#define __mmgTimeBar_H__

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
class mmgFloatSlider;

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
# define ID_TITLE  700
# define ID_CLOSE  701
# define ID_TIMER  702

//----------------------------------------------------------------------------
// mmgTimeBar :
/** mmgTimeBar is a wxPanel with a set of widget to handle time. */
//----------------------------------------------------------------------------
class mmgTimeBar: public mmgPanel , public mafObserver
{
public:
  mmgTimeBar(wxWindow* parent,wxWindowID id = -1,bool CloseButton = false); 
  virtual ~mmgTimeBar();
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  void OnEvent(mafEventBase *maf_event);
  
	/** Set the bounds for the time bar. */
	void SetBounds(double min, double max);

protected:
  double m_Time; 
  double m_TimeMin; 
  double m_TimeMax; 
  int   m_TimeStep; 

  wxBoxSizer		  *m_Sizer;
  mmgPicButton		*m_TimeBarButtons[5];
  wxTextCtrl		  *m_TimeBarEntry;
  wxTextCtrl		  *m_TimeBarEntryVelocity;
  mmgFloatSlider	*m_TimeBarSlider; 
  wxTimer          m_Timer;

  mafObserver     *m_Listener;

  /** Update the time and send the TIME_SET event to synchronize all the application. */
	void OnTimer(wxTimerEvent &event);
  
	/** Update the time bar interface. */
	void Update();

DECLARE_EVENT_TABLE()
};
#endif
