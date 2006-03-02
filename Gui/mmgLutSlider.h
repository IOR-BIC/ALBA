/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLutSlider.h,v $
  Language:  C++
  Date:      $Date: 2006-03-02 21:59:25 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgLutSlider_H__
#define __mmgLutSlider_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward reference :
//----------------------------------------------------------------------------
class mmgLutButt;
class mafEvent;
class mafObserver;

//----------------------------------------------------------------------------
// mmgLutSlider :
/**
mmgLutSlider is a Label that notify user-click using the normal wxEvents.
It is used on the titlebar of mmgNamedPanel to popup a menu.
*/
//----------------------------------------------------------------------------
class mmgLutSlider : public wxPanel
{
public:
  mmgLutSlider(
        wxWindow *parent, 
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0
        );
 ~mmgLutSlider();
  
  void SetListener(mafObserver *listener)   {m_Listener=listener;}; 

  /** Set the range of the double slider. */
	void SetRange(int  rmin, int  rmax);

  /** Get the range of the double slider. */
  void GetRange(int *rmin, int *rmax) {*rmin = m_MinValue; *rmax = m_MaxValue;};

  /** Set the values of the slider's cursors. */
  void SetSubRange(int  low, int  hi ); 

  /** Get the values of the slider's cursors. */
  void GetSubRange(int *low, int *hi ) {*low = m_LowValue; *hi = m_HighValue;};

  /** Set the position of the slider's cursors and send the event foe range modified. */
  void MoveButton(int id, int pos);

  /** Show an entry to insert a numerical values from keyboard. */
	void ShowEntry(int id);

  /** Set the label text. */
	void SetText(long i, wxString text);

  /** Set the label background colour. */
	void SetColour(long i, wxColour colour);

protected:  
  /** Update slider's cursors. */
  void OnSize(wxSizeEvent &event);
 
	/** Update slider's cursors position. */
  void UpdateButtons();

  mmgLutButt* m_MinButton;
  mmgLutButt* m_MiddleButton;
  mmgLutButt* m_MaxButton;
	wxTextCtrl *m_MinEntry;
	wxTextCtrl *m_MaxEntry;

	wxStaticText *m_MinLabel;
	wxStaticText *m_MaxLabel;
  
	int m_MinValue;
  int m_MaxValue;
  int m_LowValue;
  int m_HighValue;

  mafObserver  *m_Listener;     
DECLARE_EVENT_TABLE()
};
#endif    // __mmgLutSlider_H__
