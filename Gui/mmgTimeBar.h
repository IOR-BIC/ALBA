/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgTimeBar.h,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:04 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgTimeBar_H__
#define __mmgTimeBar_H__

#include "mafDefines.h" //important: mafDefines should always be included as first

#include "mafEvent.h"
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
class mmgTimeBar: public mmgPanel , public mafEventListener
{
public:
  mmgTimeBar(wxWindow* parent,wxWindowID id = -1,bool CloseButton = false); 
  virtual ~mmgTimeBar();
  void SetListener(mafEventListener *Listener) {m_Listener = Listener;};

  void OnEvent(mafEvent& e);
  
	/** Set the bounds for the time bar. */
	void SetBounds(float min, float max);
	
	/** Build the time bar settings panel and call ShowGui() method. */
	void ShowSettings();

	/** Show the time bar settings. */
	void ShowGui();
	
	/** Hide the time bar settings. */
	void HideGui();

protected:
  float m_time; 
  float m_time_min; 
  float m_time_max; 
  int   m_time_step; 

  wxBoxSizer		  *m_sizer;
  mmgPicButton		*m_b[5];
  wxTextCtrl		  *m_entry;
  mmgFloatSlider	*m_slider; 
  wxTimer          m_timer;

  mafEventListener *m_Listener;
	mmgGui      	   *m_gui;
	mmgGuiHolder     *m_guih;

  /** Update the time and send the TIME_SET event to synchronize all the application. */
	void OnTimer(wxTimerEvent &event);
  
	/** Update the time bar interface. */
	void Update();

DECLARE_EVENT_TABLE()
};
#endif
