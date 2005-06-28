/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgTimeBar.h,v $
  Language:  C++
  Date:      $Date: 2005-06-28 09:51:07 $
  Version:   $Revision: 1.6 $
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
#include "mmgGui.h"
#include "mmgGuiHolder.h"

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
	
	/** Build the time bar settings panel and call ShowGui() method. */
	//void ShowSettings();

	/** Show the time bar settings. */
	//void ShowGui();
	
	/** Hide the time bar settings. */
	//void HideGui();

protected:
  double m_time; 
  double m_time_min; 
  double m_time_max; 
  int   m_time_step; 

  wxBoxSizer		  *m_sizer;
  mmgPicButton		*m_b[5];
  wxTextCtrl		  *m_entry;
  wxTextCtrl		  *m_entry2;
  mmgFloatSlider	*m_slider; 
  wxTimer          m_timer;

  mafObserver *m_Listener;
	mmgGui      	   *m_gui;
	mmgGuiHolder     *m_guih;

  /** Update the time and send the TIME_SET event to synchronize all the application. */
	void OnTimer(wxTimerEvent &event);
  
	/** Update the time bar interface. */
	void Update();

DECLARE_EVENT_TABLE()
};
#endif
