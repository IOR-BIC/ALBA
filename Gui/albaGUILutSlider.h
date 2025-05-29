/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILutSlider
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUILutSlider_H__
#define __albaGUILutSlider_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaObserver.h"

//----------------------------------------------------------------------------
// forward reference :
//----------------------------------------------------------------------------
class albaGUILutButt;
class albaEvent;
class albaObserver;

//----------------------------------------------------------------------------
// albaGUILutSlider :
/**
albaGUILutSlider is a Label that notify user-click using the normal wxEvents.
It is used on the title bar of albaGUINamedPanel to popup a menu.
*/
//----------------------------------------------------------------------------
class ALBA_EXPORT albaGUILutSlider : public wxPanel
{
public:
  albaGUILutSlider(
        wxWindow *parent, 
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxBORDER_SIMPLE | wxCLIP_CHILDREN,
        const char* middleButtonTitle = "windowing"
        );
 ~albaGUILutSlider();
 
 enum ID_ARG_MOUSE_MOVE
 {
   ID_MOUSE_RELEASE = 0,
   ID_MOUSE_MOVE,
   ID_MOUSE_D_CLICK_LEFT,
 };

 enum ID_BUTTON
 {
   MIN_BUTTON = 1,
   MAX_BUTTON,
   MIDDLE_BUTTON,
 };

  void SetListener(albaObserver *listener)   {m_Listener=listener;}; 

  /** Set the range of the double slider. */
	void SetRange(double  rmin, double  rmax);

	/** Set the range of the double slider. */
	void SetRange(double *range) { SetRange(range[0], range[1]); };

	/** Get the range of the double slider. */
	void GetRange(double *rmin, double *rmax) { *rmin = m_MinValue; *rmax = m_MaxValue; };

	/** Get the range of the double slider. */
	void GetRange(double *range) { range[0] = m_MinValue; range[1] = m_MaxValue; };

  /** Set the values of the slider's cursors. */
  void SetSubRange(double  low, double  hi ); 

	/** Set the values of the slider's cursors. */
	void SetSubRange(const double *range) { SetSubRange(range[0], range[1]); };

	/** Get the values of the slider's cursors. */
	void GetSubRange(double *low, double *hi) { *low = m_LowValue; *hi = m_HighValue; };

	/** Get the values of the slider's cursors. */
	void GetSubRange(double *range) { range[0] = m_LowValue; range[1] = m_HighValue; };

  /** Set the position of the slider's cursors and send the event foe range modified. */
  void MoveButton(int id, int pos);

  /** Set the position of the slider's cursors and send the event foe range modified. */
  void ReleaseButton();

  /** Show an entry to insert a Range numerical values from keyboard. */
	void ShowRangeEntry(int id);

	/** Show an entry to insert a Sub Range numerical values from keyboard. */
	void ShowSubRangeEntry(int id);

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

  /** Enable/Disable middle button */
  void EnableMiddleButton(bool enable);

  /** Enable/Disable max button */
  void EnableMaxButton(bool enable);

  /** Enable/Disable min button */
  void EnableMinButton(bool enable);

protected:  
  /** Update slider's cursors. */
  void OnSize(wxSizeEvent &event);
 
	/** Update slider's cursors position. */
  void UpdateButtons();

  albaGUILutButt* m_MinButton;
  albaGUILutButt* m_MiddleButton;
  albaGUILutButt* m_MaxButton;
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

  albaObserver  *m_Listener;     
DECLARE_EVENT_TABLE()
};
#endif    // __albaGUILutSlider_H__
