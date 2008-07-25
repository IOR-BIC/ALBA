/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUILutSlider.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGUILutSlider_H__
#define __mafGUILutSlider_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward reference :
//----------------------------------------------------------------------------
class mafGUILutButt;
class mafEvent;
class mafObserver;

//----------------------------------------------------------------------------
// mafGUILutSlider :
/**
mafGUILutSlider is a Label that notify user-click using the normal wxEvents.
It is used on the title bar of mafGUINamedPanel to popup a menu.
*/
//----------------------------------------------------------------------------
class mafGUILutSlider : public wxPanel
{
public:
  mafGUILutSlider(
        wxWindow *parent, 
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const char* middleButtonTitle = "windowing"
        );
 ~mafGUILutSlider();
 
 enum ID_ARG_MOUSE_MOVE
 {
   ID_MOUSE_RELEASE = 0,
   ID_MOUSE_MOVE,
 };

  void SetListener(mafObserver *listener)   {m_Listener=listener;}; 

  /** Set the range of the double slider. */
	void SetRange(double  rmin, double  rmax);

  /** Get the range of the double slider. */
  void GetRange(double *rmin, double *rmax) {*rmin = m_MinValue; *rmax = m_MaxValue;};

  /** Set the values of the slider's cursors. */
  void SetSubRange(double  low, double  hi ); 

  /** Get the values of the slider's cursors. */
  void GetSubRange(double *low, double *hi ) {*low = m_LowValue; *hi = m_HighValue;};

  /** Set the position of the slider's cursors and send the event foe range modified. */
  void MoveButton(int id, int pos);

  /** Set the position of the slider's cursors and send the event foe range modified. */
  void ReleaseButton();

  /** Show an entry to insert a numerical values from keyboard. */
	void ShowEntry(int id);

  /** Set the label text. */
	void SetText(long i, wxString text);

  /** Set the label background colour. */
	void SetColour(long i, wxColour colour);

  /** Set the label background colour. */
  void SetFloatingPointTextOn(){m_FloatingPointText = true;};
  void SetFloatingPointTextOff(){m_FloatingPointText = false;};

  /** Turn On the fixed text for labels */
  void SetFixedTextOn(){m_FixedText = true;};

  /** Turn Off the fixed text for labels */
  void SetFixedTextOff(){m_FixedText = false;};

  /** Set the fixed text for min button.*/
  void SetFixedTextMinButton(const char* label);

  /** Set the fixed text for max button.*/
  void SetFixedTextMaxButton(const char* label);

protected:  
  /** Update slider's cursors. */
  void OnSize(wxSizeEvent &event);
 
	/** Update slider's cursors position. */
  void UpdateButtons();

  mafGUILutButt* m_MinButton;
  mafGUILutButt* m_MiddleButton;
  mafGUILutButt* m_MaxButton;
	wxTextCtrl *m_MinEntry;
	wxTextCtrl *m_MaxEntry;

	wxStaticText *m_MinLabel;
	wxStaticText *m_MaxLabel;
  
	double m_MinValue;
  double m_MaxValue;
  double m_LowValue;
  double m_HighValue;

  bool m_FloatingPointText; ///< flag used for visualization of decimal in the title of button, default : false
  bool m_FixedText; ///< flag used for visualization of fixed text, default: false

  mafObserver  *m_Listener;     
DECLARE_EVENT_TABLE()
};
#endif    // __mafGUILutSlider_H__
