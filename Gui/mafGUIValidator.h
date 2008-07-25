/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIValidator.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIValidator_H__
#define __mafGUIValidator_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/validate.h>
#include "mafDecl.h"
#include "mafString.h"

//-----------------------------------------------------------------------------
//forward ref
//-----------------------------------------------------------------------------
class mafGUIFloatSlider;
class mafObserver;

//-----------------------------------------------------------------------------
// constant :
//-----------------------------------------------------------------------------
enum VALIDATOR_MODES
{
  VAL_WRONG =0,
  VAL_LABEL,  
  VAL_STRING,  //TEXT
  VAL_MAF_STRING,  //TEXT
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
  VAL_BUTTON,
  VAL_FILEOPEN,
  VAL_FILESAVE,
  VAL_DIROPEN,
  VAL_COLOR
};
//-----------------------------------------------------------------------------
// mafGUIValidator :
//-----------------------------------------------------------------------------
class mafGUIValidator : public wxValidator
{
public:
  mafGUIValidator (mafObserver* listener,int mid,wxStaticText *win, wxString* var);   //String
  mafGUIValidator (mafObserver* listener,int mid,wxStaticText *win, mafString* var);  //String
  mafGUIValidator (mafObserver* listener,int mid,wxTextCtrl   *win,wxString* var);   //String
  mafGUIValidator (mafObserver* listener,int mid,wxTextCtrl   *win,mafString* var);  //String
  mafGUIValidator (mafObserver* listener,int mid,wxTextCtrl   *win,   int*		var, int		min=-32000,			 int		max=32000);     //Integer
  mafGUIValidator (mafObserver* listener,int mid,wxTextCtrl   *win,   float*	var, float	min=-2000000000, float	max=2000000000, int dec_digits = 2);//Float
  mafGUIValidator (mafObserver* listener,int mid,wxTextCtrl   *win,   double* var, double min=-2000000000, double max=2000000000, int dec_digits = 2);//Double
  mafGUIValidator (mafObserver* listener,int mid,wxSlider     *win,   int*   var, wxTextCtrl* lab);
	mafGUIValidator (mafObserver* listener,int mid,wxTextCtrl   *win,   int*   var, wxSlider* lab, int min=-32000,	int max=32000);
  mafGUIValidator (mafObserver* listener,int mid,mafGUIFloatSlider *win, double* var, wxTextCtrl* lab);
	mafGUIValidator (mafObserver* listener,int mid,wxTextCtrl		*win,	 double* var, mafGUIFloatSlider* lab, double min=-2000000000, double	max=2000000000);
  mafGUIValidator (mafObserver* listener,int mid,wxCheckBox   *win,   int*   var);
  mafGUIValidator (mafObserver* listener,int mid,wxRadioBox   *win,   int*   var);
  mafGUIValidator (mafObserver* listener,int mid,wxComboBox   *win,   int*   var);
  mafGUIValidator (mafObserver* listener,int mid,wxListBox    *win);
  mafGUIValidator (mafObserver* listener,int mid,wxButton     *win);
  mafGUIValidator (mafObserver* listener,int mid,wxButton     *win,mafString* var, wxTextCtrl* lab, bool openfile, const mafString wildcard); // FileOpen/Save
  mafGUIValidator (mafObserver* listener,int mid,wxButton     *win,mafString* var, wxTextCtrl* lab); // DirOpen
  mafGUIValidator (mafObserver* listener,int mid,wxButton     *win,wxColour* var, wxTextCtrl* lab);

  mafGUIValidator(const mafGUIValidator& val) {Copy(val);};
 ~mafGUIValidator() {};
  virtual wxObject *Clone(void) const {return new mafGUIValidator(*this);};
  bool Copy(const mafGUIValidator& val);

  virtual bool IsValid();
  virtual bool Validate(wxWindow *parent);
  virtual bool TransferToWindow(void);
  virtual bool TransferFromWindow(void);

//	float RoundValue(float f_in);
//	double RoundValue(double d_in);

  void Init(mafObserver* listener, int mid, wxControl *win);
  void OnChar(wxKeyEvent& event);
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
  mafGUIFloatSlider *m_FloatSlider;
  wxCheckBox     *m_CheckBox;
  wxRadioBox     *m_RadioBox;
  wxComboBox     *m_ComboBox;
  wxButton       *m_Button;
  wxListBox      *m_ListBox;
                        // associated variable (one of these)
  float      *m_FloatVar;
  double     *m_DoubleVar;
  int        *m_IntVar;
  wxString   *m_StringVar;
  mafString  *m_MafStringVar;
  wxColour   *m_ColorVar;
                        // associated valid range (int or float)
  float       m_FloatMax;
  float       m_FloatMin;
  double      m_DoubleMin;
  double      m_DoubleMax;
  int         m_IntMax;
  int         m_IntMin;
  
  WidgetDataType  m_WidgetData; ///< Used in Collaborative Mode and contains the active widget informations

  mafObserver    *m_Listener;
  int             m_ModuleId;
	int							m_DecimalDigits; ///< Number of decimal digits to consider and to show into the widget.
  mafString       m_Wildcard;
};
#endif
