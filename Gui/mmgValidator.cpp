/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgValidator.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-29 06:05:38 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mmgValidator.h"
#include <wx/colordlg.h>
#include <math.h>
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgFloatSlider.h"
#include "mafString.h"
//----------------------------------------------------------------------------
// mmgValidator
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE (mmgValidator, wxValidator)
    EVT_CHAR      (mmgValidator::OnChar)
    EVT_KILL_FOCUS(mmgValidator::OnKillFocus)
    EVT_BUTTON(wxOK, mmgValidator::OnButton)
    EVT_BUTTON(wxCANCEL, mmgValidator::OnButton)
    EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_BUTTON_CLICKED,   mmgValidator::OnButton)
    EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_CHECKBOX_CLICKED ,mmgValidator::OnCommandEvent) 
    EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_RADIOBOX_SELECTED,mmgValidator::OnCommandEvent) 
    EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_COMBOBOX_SELECTED,mmgValidator::OnCommandEvent) 
    EVT_SCROLL (mmgValidator::OnScrollEvent)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
void mmgValidator::Init(mafEventListener* listener, int mid, wxControl *win)
//----------------------------------------------------------------------------
{
  assert(win);
  m_Listener = listener;
  m_mid = mid;
	m_decimal_digits = -1;

  m_mode = VAL_WRONG;
  m_StaticText = NULL;
  m_TextCtrl   = NULL;
  m_Slider     = NULL;
  m_FloatSlider= NULL;
  m_CheckBox   = NULL;
  m_RadioBox   = NULL;
  m_ComboBox   = NULL;
  m_Button     = NULL;
  m_StaticText = NULL;

  m_fvar = NULL;
	m_dvar = NULL;
  m_ivar = NULL;
  m_svar = NULL;
  m_svar2= NULL;
  m_cvar = NULL;

  m_fmax =-1; 
  m_fmin =-1; 
  m_dmax =-1; 
  m_dmin =-1; 
  m_imax =-1;
  m_imin =-1; 
}
//----------------------------------------------------------------------------
bool mmgValidator::IsValid()
//----------------------------------------------------------------------------
{
  // Can happen to receive events when the widget has been destroyed:
  // ES: on closing of the application wxWindows send an OnKillFocus to the active control(1)
  // then destroy the controls(2) and the associated validators(3).
  // The message KillFocus can arrive to the validator on the time(2).

  //TODO if ( !(m_gui && m_gui->IsKindOf(CLASSINFO(mmgGui)))   ) return false;
  if ( !m_Listener   ) return false;

  switch (m_mode) 
	{
    case VAL_WRONG:
			return false;
    break;
    case VAL_LABEL:
      if ( !(m_StaticText && m_StaticText->IsKindOf(CLASSINFO(wxStaticText)))  ) return false;
      if ( !m_svar ) return false;
    break;
    case VAL_FLOAT:
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_fvar ) return false;
      if (  m_fmin >= m_fmax  ) return false;
			if ( m_decimal_digits < -1 ) return false;
    break;
    case VAL_DOUBLE:
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_dvar ) return false;
      if (  m_dmin >= m_dmax  ) return false;
			if ( m_decimal_digits < -1 ) return false;
    break;
    case VAL_INTEGER:
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_ivar ) return false;
      if (  m_imin >= m_imax  ) return false;
    break;
    case VAL_STRING:
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_svar && !m_svar2 ) return false;
    break;
    case VAL_SLIDER:
		case VAL_SLIDER_2:
      if ( !(m_Slider && m_Slider->IsKindOf(CLASSINFO(wxSlider)))  ) return false;
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_ivar ) return false;
    break;
    case VAL_FLOAT_SLIDER:
		case VAL_FLOAT_SLIDER_2:
      if ( !(m_FloatSlider && m_FloatSlider->IsKindOf(CLASSINFO(mmgFloatSlider)))  ) return false;
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_fvar ) return false;
    break;
		case VAL_CHECKBOX:
      if ( !(m_CheckBox && m_CheckBox->IsKindOf(CLASSINFO(wxCheckBox)))  ) return false;
      if ( !m_ivar ) return false;
    break;
    case VAL_RADIOBOX:
      if ( !(m_RadioBox && m_RadioBox->IsKindOf(CLASSINFO(wxRadioBox)))  ) return false;
      if ( !m_ivar ) return false;
    break;
    case VAL_COMBOBOX:
      if ( !(m_ComboBox && m_ComboBox->IsKindOf(CLASSINFO(wxComboBox)))  ) return false;
      if ( !m_ivar ) return false;
    break;
    case VAL_BUTTON:
      if ( !(m_Button && m_Button->IsKindOf(CLASSINFO(wxButton)))  ) return false;
    break;
    case VAL_DIROPEN:
    case VAL_FILEOPEN:
    case VAL_FILESAVE:
      if ( !(m_Button     && m_Button->IsKindOf(CLASSINFO(wxButton)))  ) return false;
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_svar ) return false;
    break;
    case VAL_COLOR:
      if ( !(m_Button && m_Button->IsKindOf(CLASSINFO(wxButton)))  ) return false;
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_cvar ) return false;
    break;
    default:
      assert(false);
    break;
  }
  return true;
}
//----------------------------------------------------------------------------
bool mmgValidator::Copy(const mmgValidator& val)
//----------------------------------------------------------------------------
{
  wxValidator::Copy(val);
  m_Listener    = val.m_Listener;     
  m_mid         = val.m_mid;
	m_decimal_digits = val.m_decimal_digits;

  m_mode        = val.m_mode;     

  m_StaticText  = val.m_StaticText; 
  m_TextCtrl    = val.m_TextCtrl; 
  m_Slider      = val.m_Slider;   
  m_FloatSlider	= val.m_FloatSlider;   
  m_CheckBox    = val.m_CheckBox; 
  m_RadioBox    = val.m_RadioBox; 
  m_ComboBox    = val.m_ComboBox; 
  m_Button      = val.m_Button; 
  m_StaticText  = val.m_StaticText; 

  m_fvar        = val.m_fvar;
  m_dvar        = val.m_dvar;
  m_ivar        = val.m_ivar;
  m_svar        = val.m_svar;
  m_cvar        = val.m_cvar;

  m_fmax        = val.m_fmax;
  m_fmin        = val.m_fmin;
  m_dmax        = val.m_dmax;
  m_dmin        = val.m_dmin;
  m_imax        = val.m_imax;
  m_imin        = val.m_imin;

  m_wildcard		= val.m_wildcard;

  return TRUE;
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxStaticText *win,wxString* var) //String
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);
  m_mode = VAL_LABEL;
  m_StaticText=win; 
  m_svar      =var;     
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxTextCtrl *win,wxString* var) //String
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_STRING;
  m_TextCtrl  =win; 
  m_svar      =var;     
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxTextCtrl *win,mafString* var) //String
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_STRING;
  m_TextCtrl  =win; 
  m_svar2     =var;     
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxTextCtrl *win,   int*   var,   int min,   int max)//Integer
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_INTEGER;
  m_TextCtrl  =win; 
  m_ivar      =var;     
  m_imax      =max;     
  m_imin      =min;    
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxTextCtrl *win, float* var, float min, float max, int dec_digits)//Float
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_FLOAT;
  m_TextCtrl				= win; 
  m_fvar						= var;     
  m_fmax						= max;     
  m_fmin						= min;
	m_decimal_digits	= dec_digits;
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxTextCtrl *win, double* var, double min, double max, int dec_digits)//Double
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_DOUBLE;
  m_TextCtrl				= win;
  m_dvar						= var;
  m_dmax						= max;
  m_dmin						= min;
	m_decimal_digits	= dec_digits;
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxSlider *win, int* var, wxTextCtrl* lab) //Slider     
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_SLIDER;
  m_TextCtrl  =lab;
  m_Slider    =win;
  m_ivar      =var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener,int mid,wxTextCtrl *win, int* var, wxSlider* lab, int min, int max)
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_SLIDER_2;
  m_TextCtrl  =win;
  m_Slider    =lab;
  m_ivar      =var;
	m_imin			=min;
	m_imax			=max;
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, mmgFloatSlider  *win, float*  var, wxTextCtrl* lab) //FloatSlider
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_FLOAT_SLIDER;
  m_TextCtrl    =lab;
  m_FloatSlider =win;
  m_fvar        =var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener,int mid,wxTextCtrl *win, float* var, mmgFloatSlider* lab,float min,float max)
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_FLOAT_SLIDER_2;
  m_FloatSlider	=lab;
  m_TextCtrl		=win;
  m_fvar        =var;
	m_fmin				=min;
	m_fmax				=max;
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxCheckBox *win,   int*   var) //CheckBox    
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_CHECKBOX;
  m_CheckBox  =win; 
  m_ivar      =var;     
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxRadioBox *win,   int*   var) //RadioBox                     
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_RADIOBOX;
  m_RadioBox  =win; 
  m_ivar      =var;     
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxComboBox *win,   int*   var) //Combo 
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_COMBOBOX;
  m_ComboBox  =win;
  m_ivar      =var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxButton *win) //Button
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_BUTTON;
  m_Button    =win; 
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxButton *win, wxString* var, wxTextCtrl* lab,  bool  openfile, const wxString wildcard)                                                
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  if (openfile) m_mode = VAL_FILEOPEN; else m_mode = VAL_FILESAVE;
  m_Button    =win;
  m_TextCtrl  =lab;
  m_svar      =var;
  m_wildcard  =wildcard;
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxButton *win, wxString* var, wxTextCtrl* lab)
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_DIROPEN;
  m_Button    =win;
  m_TextCtrl  =lab;
  m_svar      =var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
mmgValidator::mmgValidator(mafEventListener* listener, int mid, wxButton *win, wxColour* var, wxTextCtrl* lab)
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  m_mode = VAL_COLOR;
  m_Button    =win;
  m_TextCtrl  =lab;
  m_cvar      =var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
bool mmgValidator::Validate(wxWindow *parent)
//----------------------------------------------------------------------------
{
  assert(IsValid());

  if (m_mode==VAL_FLOAT )
  {
    double val = m_fmin-1;
    wxString s = m_TextCtrl->GetValue();
	  bool res   = s.ToDouble(&val);
    if (!res )
    {
	    if(this->m_decimal_digits == -1)
				s.Printf("%g",*m_fvar);
			else
				s.Printf("%g",this->RoundValue(*m_fvar));
	    m_TextCtrl->SetValue(s) ;
    }
    if (val<m_fmin )
    {
	    s.Printf("%g",m_fmin);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val>m_fmax )
    {
	    s.Printf("%g",m_fmax);
	    m_TextCtrl->SetValue(s) ;
    }
  }
  if (m_mode==VAL_FLOAT_SLIDER_2)
	{
    double val = m_fmin-1;
    wxString s = m_TextCtrl->GetValue();
	  bool res   = s.ToDouble(&val);
    if (!res )
    {
	    if(this->m_decimal_digits == -1)
				s.Printf("%g",*m_fvar);
			else
				s.Printf("%g",this->RoundValue(*m_fvar));
	    m_TextCtrl->SetValue(s) ;
    }
    if (val<m_fmin )
    {
	    s.Printf("%g",m_fmin);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val>m_fmax )
    {
	    s.Printf("%g",m_fmax);
	    m_TextCtrl->SetValue(s) ;
    }
		this->m_FloatSlider->SetValue(*m_fvar);
	}
  if (m_mode==VAL_DOUBLE )
  {
    double val = m_dmin-1;
    wxString s = m_TextCtrl->GetValue();
	  bool res   = s.ToDouble(&val);
    if (!res )
    {
	    if(this->m_decimal_digits == -1)
				s.Printf("%g",*m_dvar);
			else
				s.Printf("%g",this->RoundValue(*m_dvar));
	    m_TextCtrl->SetValue(s) ;
    }
    if (val<m_dmin )
    {
	    s.Printf("%g",m_dmin);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val>m_dmax )
    {
	    s.Printf("%g",m_dmax);
	    m_TextCtrl->SetValue(s) ;
    }
  }
  if ( m_mode==VAL_INTEGER )
  {
	  long val   = m_imin-1;
    wxString s = m_TextCtrl->GetValue();
	  bool res   = s.ToLong(&val);
    if (!res )
    {
	    s.Printf("%d",*m_ivar);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val<m_imin )
    {
	    s.Printf("%d",m_imin);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val>m_imax )
    {
	    s.Printf("%d",m_imax);
	    m_TextCtrl->SetValue(s) ;
    }
  }
	if ( m_mode==VAL_SLIDER_2 )
  {
	  long val   = m_imin-1;
    wxString s = m_TextCtrl->GetValue();
	  bool res   = s.ToLong(&val);
    if (!res )
    {
	    s.Printf("%d",*m_ivar);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val<m_imin )
    {
	    s.Printf("%d",m_imin);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val>m_imax )
    {
	    s.Printf("%d",m_imax);
	    m_TextCtrl->SetValue(s) ;
    }
		this->m_Slider->SetValue(*m_ivar);
  }
  return true;
}
//----------------------------------------------------------------------------
bool mmgValidator::TransferToWindow(void)
//----------------------------------------------------------------------------
{
  assert(IsValid());
  wxString s;
  wxString path;
  wxString name;
  wxString ext;
  
  switch (m_mode) 
	{
    case VAL_LABEL:
			m_StaticText->SetLabel(*m_svar);
    break;
    case VAL_FLOAT:
      if(*m_fvar < m_fmin ) *m_fvar = m_fmin;
      if(*m_fvar > m_fmax ) *m_fvar = m_fmax;
			if(this->m_decimal_digits == -1)
				s.Printf("%g",*m_fvar);
			else
				s.Printf("%g",this->RoundValue(*m_fvar));
      m_TextCtrl->SetValue(s);
    break;
    case VAL_DOUBLE:
			if(*m_dvar < m_dmin ) *m_dvar = m_dmin;
			if(*m_dvar > m_dmax ) *m_dvar = m_dmax;
			if(this->m_decimal_digits == -1)
				s.Printf("%g",*m_dvar);
			else
				s.Printf("%g",this->RoundValue(*m_dvar));
      m_TextCtrl->SetValue(s);
    break;
    case VAL_INTEGER:
      if(*m_ivar < m_imin ) *m_ivar = m_imin;
      if(*m_ivar > m_imax ) *m_ivar = m_imax;
	    s.Printf("%d",*m_ivar);
      m_TextCtrl->SetValue(s);
    break;
    case VAL_STRING:
      if (m_svar) m_TextCtrl->SetValue(*m_svar);
      if (m_svar2) m_TextCtrl->SetValue(m_svar2->GetCStr());
    break;
    case VAL_SLIDER:
      m_Slider->SetValue(*m_ivar);
	    s.Printf("%d",*m_ivar);
      m_TextCtrl->SetValue(s);
    break;
		case VAL_SLIDER_2:
      if(*m_ivar < m_imin ) *m_ivar = m_imin;
      if(*m_ivar > m_imax ) *m_ivar = m_imax;
	    s.Printf("%d",*m_ivar);
      m_TextCtrl->SetValue(s);
      m_Slider->SetValue(*m_ivar);
		break;
    case VAL_FLOAT_SLIDER:
      m_FloatSlider->SetValue(*m_fvar);
	    s.Printf("%g",*m_fvar);
      m_TextCtrl->SetValue(s);
    break;
		case VAL_FLOAT_SLIDER_2:
      if(*m_fvar < m_fmin ) *m_fvar = m_fmin;
      if(*m_fvar > m_fmax ) *m_fvar = m_fmax;
			if(this->m_decimal_digits == -1)
				s.Printf("%g",*m_fvar);
			else
				s.Printf("%g",this->RoundValue(*m_fvar));
      m_TextCtrl->SetValue(s);
      m_FloatSlider->SetValue(*m_fvar);
    break;
    case VAL_CHECKBOX:
      m_CheckBox->SetValue(*m_ivar ? true : false);
    break;
    case VAL_RADIOBOX:
      m_RadioBox->SetSelection(*m_ivar);
    break;
    case VAL_COMBOBOX:
      m_ComboBox->SetSelection(*m_ivar);
    break;
    case VAL_DIROPEN:
			path = m_svar->c_str();
			if( ! ::wxDirExists(path) ) wxSplitPath(m_svar->c_str(), &path, &name, &ext); // it is a filename
			m_TextCtrl->SetValue(path);
			if(path != "")
			{ 
				m_TextCtrl->SetToolTip(path);
			}
  	break;
    case VAL_FILEOPEN:
    case VAL_FILESAVE:
			wxSplitPath(m_svar->c_str(), &path, &name, &ext);
			if (ext.Len() >0 )
			{
				name += ".";
				name += ext;
			}
			m_TextCtrl->SetValue(name);
			path = m_svar->c_str();
			if(path != "")
			{ 
				m_TextCtrl->SetToolTip(path);
			}
  	break;
    case VAL_COLOR:
      m_TextCtrl->SetBackgroundColour(*m_cvar);
      m_TextCtrl->Refresh();
    break;
  }
  return TRUE;
}
///////////////////// Paolo
//----------------------------------------------------------------------------
float mmgValidator::RoundValue(float f_in)
//----------------------------------------------------------------------------
{
	float f_tmp = f_in * pow(10,m_decimal_digits);
	int b = ( f_tmp >= 0 ) ? static_cast<int>( f_tmp + .5):static_cast<int>( f_tmp - .5);
	return b / pow(10,m_decimal_digits);
}
//----------------------------------------------------------------------------
double mmgValidator::RoundValue(double d_in)
//----------------------------------------------------------------------------
{
	double d_tmp = d_in * pow(10,m_decimal_digits);
	int b = ( d_tmp >= 0 ) ? static_cast<int>( d_tmp + .5):static_cast<int>( d_tmp - .5);
	return b / pow(10,m_decimal_digits);
}
///////////////////// 
//----------------------------------------------------------------------------
bool mmgValidator::TransferFromWindow(void)
//----------------------------------------------------------------------------
{
  assert(IsValid());

  wxString s; 
  bool   res; 
  long   ival;
  double dval;

  switch (m_mode) 
	{
    case VAL_FLOAT:
			s = m_TextCtrl->GetValue();
			res = s.ToDouble(&dval);
			if (res) 
        res = *m_fvar != dval;
			if (res) 
        *m_fvar = dval;
			return res;
    break;
 		case VAL_FLOAT_SLIDER_2:
			s = m_TextCtrl->GetValue();
			res = s.ToDouble(&dval);
			if (res) 
        res = *m_fvar != dval;
			if (res) 
				*m_fvar = dval;
			return res;
    break;
    case VAL_DOUBLE:
			s = m_TextCtrl->GetValue();
			res = s.ToDouble(&dval);
			if (res) 
        res = *m_dvar != dval;
			if (res) 
        *m_dvar = dval;
			return res;
    break;
    case VAL_INTEGER:
			s = m_TextCtrl->GetValue();
			res = s.ToLong(&ival);
			if (res) 
        res = *m_ivar != ival;
      if (res) 
        *m_ivar = ival;
			return res;
    break;
    case VAL_STRING:
      if (m_svar)  *m_svar = m_TextCtrl->GetValue();
      if (m_svar2) *m_svar2 = m_TextCtrl->GetValue();
    break;
    case VAL_SLIDER:
			*m_ivar = m_Slider->GetValue();
    break;
		case VAL_SLIDER_2:
			s = m_TextCtrl->GetValue();
			res = s.ToLong(&ival);
			if (res) 
        res = *m_ivar != ival;
			if (res) 
				*m_ivar = ival;
			return res;
		break;
    case VAL_FLOAT_SLIDER:
			*m_fvar = m_FloatSlider->GetValue();
    break;
    case VAL_CHECKBOX:
			*m_ivar = m_CheckBox->GetValue();
    break;
    case VAL_RADIOBOX:
			*m_ivar = m_RadioBox->GetSelection();
    break;
    case VAL_COMBOBOX:
			*m_ivar = m_ComboBox->GetSelection();
    break;
    case VAL_DIROPEN:
    case VAL_FILEOPEN:
    case VAL_FILESAVE:
    break;
    case VAL_COLOR:
			*m_cvar=m_TextCtrl->GetBackgroundColour();
    break;
  }
  return TRUE;
}
//----------------------------------------------------------------------------
void mmgValidator::OnChar(wxKeyEvent& event)
//----------------------------------------------------------------------------
{
  //Filter key for TextCtrl used for numeric input
  int keyCode = (int)event.KeyCode();

  if (keyCode == WXK_RETURN && m_mode == VAL_STRING)
  {
    // Return is received only from widget with the wxTE_PROCESS_ENTER style flag enabled
    // i.e. console widget
    TransferFromWindow();
    mafEventMacro(mafEvent(m_TextCtrl, m_mid));
    return; // eat message
  }

  // don't filter special keys and Delete
  if (keyCode < WXK_SPACE || keyCode == WXK_DELETE || keyCode > WXK_START)
  {
		event.Skip();
		return;
  }
  if ( m_mode==VAL_FLOAT || m_mode==VAL_DOUBLE || m_mode==VAL_FLOAT_SLIDER_2 )
  {
		if( !(wxIsdigit(keyCode) ||keyCode== '-' ||keyCode== ',' ||keyCode== '.' ||keyCode== 'e' ||keyCode== 'E') )
			return; // eat message
  }
  if ( m_mode==VAL_INTEGER || m_mode==VAL_SLIDER_2 )
  {
		if( !(wxIsdigit(keyCode) ||keyCode== '-') )
			return; // eat message
  }
  event.Skip();
}
//----------------------------------------------------------------------------
void mmgValidator::OnKillFocus(wxFocusEvent& event)
//----------------------------------------------------------------------------
{
  if (m_mode == VAL_STRING || m_mode == VAL_INTEGER || m_mode == VAL_FLOAT || 
			m_mode == VAL_DOUBLE || m_mode == VAL_FLOAT_SLIDER_2 || m_mode == VAL_SLIDER_2)
    if ( IsValid() )
    {
      Validate(NULL);
      bool send_message = TransferFromWindow();
      if(send_message)
        mafEventMacro(mafEvent(m_TextCtrl, m_mid));
    }
  event.Skip();
}
//----------------------------------------------------------------------------
void mmgValidator::OnScrollEvent(wxScrollEvent& event)
//----------------------------------------------------------------------------
{
  wxString s;
  if ( IsValid() )
  {
    switch (m_mode) 
		{
      case VAL_SLIDER:
				TransferFromWindow();
				s.Printf("%d",*m_ivar);
				m_TextCtrl->SetValue(s);
				mafEventMacro(mafEvent(m_Slider, m_mid));
      break;
      case VAL_FLOAT_SLIDER:
				TransferFromWindow();
				s.Printf("%g",*m_fvar);
				m_TextCtrl->SetValue(s);
				mafEventMacro(mafEvent(m_FloatSlider, m_mid));
      break;
		}
  }
  event.Skip();
}
//----------------------------------------------------------------------------
void mmgValidator::OnCommandEvent(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  if ( IsValid() )
  {
    TransferFromWindow();
    switch (m_mode) 
		{
      case VAL_CHECKBOX:
           mafEventMacro(mafEvent(m_CheckBox, m_mid));
      break;
      case VAL_RADIOBOX:
           mafEventMacro(mafEvent(m_RadioBox, m_mid));
      break;
      case VAL_COMBOBOX:
           mafEventMacro(mafEvent(m_ComboBox, m_mid));
      break;
    }
  }
  event.Skip();
}
//----------------------------------------------------------------------------
void mmgValidator::OnButton(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  bool cancel = false;
	int ret_code;
	wxString path;
  wxString name;
  wxString ext;
  
  if ( IsValid() )
  {
    switch(m_mode)
    {
      case VAL_BUTTON:
      break;
      case VAL_DIROPEN:
      {
        wxSplitPath(m_svar->c_str(), &path, &name, &ext);
        wxDirDialog dialog(m_Button,"", path, 0, m_Button->GetPosition());
				dialog.SetReturnCode(wxID_OK);
				ret_code = dialog.ShowModal();
        if (ret_code == wxID_OK)
        {
          path = dialog.GetPath();
         *m_svar = path;
          m_TextCtrl->SetLabel(path);
        }
				else
        {
          cancel = true;
        }
      }
      break;
      case VAL_FILEOPEN:
      {
        wxSplitPath(m_svar->c_str(), &path, &name, &ext);
				if (ext.Len() >0 )
				{
					name += ".";
					name += ext;
				}
        wxFileDialog dialog(m_Button,"Open File", path, name, m_wildcard, wxOPEN|wxFILE_MUST_EXIST|wxHIDE_READONLY , m_Button->GetPosition());
        dialog.SetReturnCode(wxID_OK);
				ret_code = dialog.ShowModal();
				if (ret_code == wxID_OK)
        {
          path = dialog.GetPath();
          name = dialog.GetFilename();
         *m_svar = path;
          m_TextCtrl->SetLabel(name);
        }
				else
        {
          cancel = true;
        }
      }
      break;
      case VAL_FILESAVE:
      {
        wxSplitPath(m_svar->c_str(), &path, &name, &ext);
				if (ext.Len() >0 )
				{
					name += ".";
					name += ext;
				}
        wxFileDialog dialog(m_Button,"Save File", path, name, m_wildcard, wxSAVE|wxOVERWRITE_PROMPT|wxHIDE_READONLY , m_Button->GetPosition());
        dialog.SetReturnCode(wxID_OK);
				ret_code = dialog.ShowModal();
				if (ret_code == wxID_OK)
        {
          *m_svar = dialog.GetPath();
          m_TextCtrl->SetLabel(dialog.GetFilename());
        }
				else
        {
          cancel = true;
        }
      }
      break;
      case VAL_COLOR:
			{
				wxColourDialog dialog(m_Button);
				dialog.SetReturnCode(wxID_OK);
				ret_code = dialog.ShowModal();
				if (ret_code == wxID_OK)
				{
					wxColourData retData = dialog.GetColourData();
					m_TextCtrl->SetBackgroundColour(retData.GetColour());
					m_TextCtrl->Refresh();
					TransferFromWindow();
				}
				else
				{
					cancel = true;
				}
			}    
      break;
    }

		mafYield(); // update gui to let the dialogs disappear

		( (wxPanel*)m_Button->GetParent() )->SetDefaultItem(NULL);
		mafYield(); 

		if (!cancel) mafEventMacro(mafEvent(m_Button, m_mid));
	}
}
