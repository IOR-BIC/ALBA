/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIValidator
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIValidator_H__
#define __albaGUIValidator_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/validate.h>
#include "wx/listctrl.h"
#include "albaDecl.h"
#include "albaString.h"

//-----------------------------------------------------------------------------
//forward ref
//-----------------------------------------------------------------------------
class albaGUIFloatSlider;
class albaObserver;

//-----------------------------------------------------------------------------
// constant :
//-----------------------------------------------------------------------------
enum VALIDATOR_MODES
{
  VAL_WRONG =0,
  VAL_LABEL,  
	VAL_STRING,  //TEXT
	VAL_INTERACTIVE_STRING,  //TEXT
	VAL_ALBA_STRING,  //TEXT
	VAL_ALBA_INTERACTIVE_STRING,  //TEXT
	VAL_MULTILINE_STRING,  //TEXT
	VAL_MULTILINE_INTERACTIVE_STRING,  //TEXT
	VAL_ALBA_MULTILINE_STRING,  //TEXT
	VAL_ALBA_MULTILINE_INTERACTIVE_STRING,  //TEXT
  VAL_INTEGER, //TEXT 
  VAL_FLOAT,   //TEXT
	VAL_DOUBLE,  //TEXT
  VAL_SLIDER,
	VAL_SLIDER_2,
  VAL_FLOAT_SLIDER,
	VAL_FLOAT_SLIDER_2,
  VAL_CHECKBOX,
  VAL_RADIOBOX,
  VAL_COMBOBOX,
  VAL_LISTBOX,
  VAL_LISTCTRL,
  VAL_BUTTON,
  VAL_FILEOPEN,
  VAL_FILESAVE,
  VAL_DIROPEN,
  VAL_COLOR
};
//-----------------------------------------------------------------------------
// albaGUIValidator :
//-----------------------------------------------------------------------------
class ALBA_EXPORT albaGUIValidator : public wxValidator
{
public:
  albaGUIValidator (albaObserver* listener,int mid,wxStaticText *win, wxString* var);   //String
  albaGUIValidator (albaObserver* listener,int mid,wxStaticText *win, albaString* var);  //String
  albaGUIValidator (albaObserver* listener,int mid,wxTextCtrl   *win, albaString* var, bool interactive = false, bool multiline=false);  //String
  albaGUIValidator (albaObserver* listener,int mid,wxTextCtrl   *win,wxString* var, bool interactive = false, bool multiline=false);   //String
  albaGUIValidator (albaObserver* listener,int mid,wxTextCtrl   *win,   int*		var, int		min=-2147483647-1,			 int		max=2147483647);     //Integer
  albaGUIValidator (albaObserver* listener,int mid,wxTextCtrl   *win,   float*	var, float	min=-1.0e+38F, float	max=1.0e+38F, int dec_digits = 2);//Float
  albaGUIValidator (albaObserver* listener,int mid,wxTextCtrl   *win,   double* var, double min=-1.0e+299, double max=1.0e+299, int dec_digits = 2);//Double
  albaGUIValidator (albaObserver* listener,int mid,wxSlider     *win,   int*   var, wxTextCtrl* lab);
	albaGUIValidator (albaObserver* listener,int mid,wxTextCtrl   *win,   int*   var, wxSlider* lab, int min=-2147483647-1,	int max=2147483647);
  albaGUIValidator (albaObserver* listener,int mid,albaGUIFloatSlider *win, double* var, wxTextCtrl* lab);
	albaGUIValidator (albaObserver* listener,int mid,wxTextCtrl		*win,	 double* var, albaGUIFloatSlider* lab, double min=-1.0e+299, double	max=1.0e+299);
  albaGUIValidator (albaObserver* listener,int mid,wxCheckBox   *win,   int*   var);
  albaGUIValidator (albaObserver* listener,int mid,wxRadioBox   *win,   int*   var);
  albaGUIValidator (albaObserver* listener,int mid,wxComboBox   *win,   int*   var);
  albaGUIValidator (albaObserver* listener,int mid,wxListBox    *win);
  albaGUIValidator (albaObserver* listener,int mid,wxListCtrl   *win);
  albaGUIValidator (albaObserver* listener,int mid,wxButton     *win);
  albaGUIValidator (albaObserver* listener,int mid,wxButton     *win,albaString* var, wxTextCtrl* lab, bool openfile, const albaString wildcard); // FileOpen/Save
  albaGUIValidator (albaObserver* listener,int mid,wxButton     *win,albaString* var, wxTextCtrl* lab); // DirOpen
  albaGUIValidator (albaObserver* listener,int mid,wxButton     *win,wxColour* var, wxTextCtrl* lab);

  albaGUIValidator(const albaGUIValidator& val) {Copy(val);};
 ~albaGUIValidator() {};
  virtual wxObject *Clone(void) const {return new albaGUIValidator(*this);};
  bool Copy(const albaGUIValidator& val);

  virtual bool IsValid();
  virtual bool Validate(wxWindow *parent);
  virtual bool TransferToWindow(void);
  virtual bool TransferFromWindow(void);

//	float RoundValue(float f_in);
//	double RoundValue(double d_in);

  void Init(albaObserver* listener, int mid, wxControl *win);
	void OnChar(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnKillFocus(wxFocusEvent& event);
  void OnScrollEvent(wxScrollEvent& event);
  void OnCommandEvent(wxCommandEvent& event);
  void OnButton(wxCommandEvent& event);

  /** Fill widget_data with the active widget value */
  void GetWidgetData(WidgetDataType &widget_data);

  /** Fill m_WidgetData member variable with widget_data information and validate it */
  void SetWidgetData(WidgetDataType &widget_data);

DECLARE_EVENT_TABLE()

protected:
  enum VALIDATOR_MODES m_Mode;
                         // associated control (one of these)
  wxStaticText   *m_StaticText;
  wxTextCtrl     *m_TextCtrl;
  wxSlider       *m_Slider;
  albaGUIFloatSlider *m_FloatSlider;
  wxCheckBox     *m_CheckBox;
  wxRadioBox     *m_RadioBox;
  wxComboBox     *m_ComboBox;
  wxButton       *m_Button;
  wxListBox      *m_ListBox;
  wxListCtrl     *m_ListCtrl;
                        // associated variable (one of these)
  float      *m_FloatVar;
  double     *m_DoubleVar;
  int        *m_IntVar;
  wxString   *m_StringVar;
  albaString  *m_MafStringVar;
  wxColour   *m_ColorVar;
                        // associated valid range (int or float)
  float       m_FloatMax;
  float       m_FloatMin;
  double      m_DoubleMin;
  double      m_DoubleMax;
  int         m_IntMax;
  int         m_IntMin;
  
  WidgetDataType  m_WidgetData; ///< Used in Collaborative Mode and contains the active widget informations

  albaObserver    *m_Listener;
  int             m_ModuleId;
	int							m_DecimalDigits; ///< Number of decimal digits to consider and to show into the widget.
  albaString       m_Wildcard;
};
#endif
