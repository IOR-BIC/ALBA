/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgValidator.h,v $
  Language:  C++
  Date:      $Date: 2005-05-24 14:34:28 $
  Version:   $Revision: 1.5 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgValidator_H__
#define __mmgValidator_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/validate.h>
#include "mafDecl.h"
#include "mafString.h"

//-----------------------------------------------------------------------------
//forward ref
//-----------------------------------------------------------------------------
class mmgFloatSlider;
class mafObserver;

//-----------------------------------------------------------------------------
// constant :
//-----------------------------------------------------------------------------
enum validator_modes
{
  VAL_WRONG =0,
  VAL_LABEL,  
  VAL_STRING,  //TEXT
  VAL_INTEGER, //TEXT 
  VAL_FLOAT,   //TEXT
	VAL_DOUBLE,
  VAL_SLIDER,
	VAL_SLIDER_2,
  VAL_FLOAT_SLIDER,
	VAL_FLOAT_SLIDER_2,
  VAL_CHECKBOX,
  VAL_RADIOBOX,
  VAL_COMBOBOX,
  VAL_BUTTON,
  VAL_FILEOPEN,
  VAL_FILESAVE,
  VAL_DIROPEN,
  VAL_COLOR
};
//-----------------------------------------------------------------------------
// mmgValidator :
//-----------------------------------------------------------------------------
class mmgValidator : public wxValidator
{
public:
  mmgValidator (mafObserver* listener,int mid,wxStaticText *win,mafString* var);                                             //String
  mmgValidator (mafObserver* listener,int mid,wxTextCtrl   *win,wxString* var);   //String
  mmgValidator (mafObserver* listener,int mid,wxTextCtrl   *win,mafString* var);  //String
  mmgValidator (mafObserver* listener,int mid,wxTextCtrl   *win,   int*		var, int		min=-32000,			 int		max=32000);     //Integer
  mmgValidator (mafObserver* listener,int mid,wxTextCtrl   *win,   float*	var, float	min=-2000000000, float	max=2000000000, int dec_digits = 2);//Float
  mmgValidator (mafObserver* listener,int mid,wxTextCtrl   *win,   double* var, double min=-2000000000, double max=2000000000, int dec_digits = 2);//Double
  mmgValidator (mafObserver* listener,int mid,wxSlider     *win,   int*   var, wxTextCtrl* lab);
	mmgValidator (mafObserver* listener,int mid,wxTextCtrl   *win,   int*   var, wxSlider* lab, int min=-32000,	int max=32000);
  mmgValidator (mafObserver* listener,int mid,mmgFloatSlider *win, float* var, wxTextCtrl* lab);
	mmgValidator (mafObserver* listener,int mid,wxTextCtrl		*win,		float* var, mmgFloatSlider* lab, float min=-2000000000, float	max=2000000000);
  mmgValidator (mafObserver* listener,int mid,wxCheckBox   *win,   int*   var);
  mmgValidator (mafObserver* listener,int mid,wxRadioBox   *win,   int*   var);
  mmgValidator (mafObserver* listener,int mid,wxComboBox   *win,   int*   var);
  mmgValidator (mafObserver* listener,int mid,wxButton     *win);
  mmgValidator (mafObserver* listener,int mid,wxButton     *win,mafString* var, wxTextCtrl* lab, bool openfile, const mafString wildcard); // FileOpen/Save
  mmgValidator (mafObserver* listener,int mid,wxButton     *win,mafString* var, wxTextCtrl* lab); // DirOpen
  mmgValidator (mafObserver* listener,int mid,wxButton     *win,wxColour* var, wxTextCtrl* lab);

  mmgValidator(const mmgValidator& val) {Copy(val);};
 ~mmgValidator() {};
  virtual wxObject *Clone(void) const {return new mmgValidator(*this);};
  bool Copy(const mmgValidator& val);

  virtual bool IsValid();
  virtual bool Validate(wxWindow *parent);
  virtual bool TransferToWindow(void);
  virtual bool TransferFromWindow(void);

	float RoundValue(float f_in);
	double RoundValue(double d_in);

  void Init(mafObserver* listener, int mid, wxControl *win);
  void OnChar(wxKeyEvent& event);
  void OnKillFocus(wxFocusEvent& event);
  void OnScrollEvent(wxScrollEvent& event);
  void OnCommandEvent(wxCommandEvent& event);
  void OnButton(wxCommandEvent& event);

DECLARE_EVENT_TABLE()

protected:
  enum validator_modes m_mode;
                         // associated control (one of these)
  wxStaticText  *m_StaticText;
  wxTextCtrl    *m_TextCtrl;
  wxSlider      *m_Slider;
  mmgFloatSlider *m_FloatSlider;
  wxCheckBox    *m_CheckBox;
  wxRadioBox    *m_RadioBox;
  wxComboBox    *m_ComboBox;
  wxButton      *m_Button;
                        // associated variable (one of these)
  float      *m_fvar;
  double     *m_dvar;
  int        *m_ivar;
  wxString   *m_svar;
  mafString  *m_svar2;
  wxColour   *m_cvar;
                        // associated valid range (int or float)
  float       m_fmax;
  float       m_fmin;
  double      m_dmin;
  double      m_dmax;
  int         m_imax;
  int         m_imin;
  
  mafObserver    *m_Listener;
  int             m_mid;
	int							m_decimal_digits;
  mafString       m_wildcard;
};
#endif