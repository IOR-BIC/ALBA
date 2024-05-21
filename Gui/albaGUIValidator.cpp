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


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaGUIValidator.h"
#include <wx/colordlg.h>
#include "wx/filename.h"
//#include <math.h>
#include "albaEvent.h"
#include "albaGUIFloatSlider.h"

//----------------------------------------------------------------------------
// albaGUIValidator
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE (albaGUIValidator, wxValidator)
		EVT_KEY_UP(albaGUIValidator::OnKeyUp)
		EVT_CHAR(albaGUIValidator::OnChar)
		EVT_KILL_FOCUS(albaGUIValidator::OnKillFocus)
    EVT_BUTTON(wxOK, albaGUIValidator::OnButton)
    EVT_BUTTON(wxCANCEL, albaGUIValidator::OnButton)
    EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_BUTTON_CLICKED,   albaGUIValidator::OnButton)
    EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_CHECKBOX_CLICKED ,albaGUIValidator::OnCommandEvent) 
    EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_RADIOBOX_SELECTED,albaGUIValidator::OnCommandEvent) 
    EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_COMBOBOX_SELECTED,albaGUIValidator::OnCommandEvent) 
    EVT_SCROLL (albaGUIValidator::OnScrollEvent)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
void albaGUIValidator::Init(albaObserver* listener, int mid, wxControl *win)
//----------------------------------------------------------------------------
{
  assert(win);
  m_Listener = listener;
  m_ModuleId = mid;
	m_DecimalDigits = -1;

  m_Mode = VAL_WRONG;
  m_StaticText = NULL;
  m_TextCtrl   = NULL;
  m_Slider     = NULL;
  m_FloatSlider= NULL;
  m_CheckBox   = NULL;
  m_RadioBox   = NULL;
  m_ComboBox   = NULL;
  m_Button     = NULL;
  m_StaticText = NULL;
  m_ListBox    = NULL;
  m_ListCtrl   = NULL;

  m_FloatVar    = NULL;
	m_DoubleVar   = NULL;
  m_IntVar      = NULL;
  m_StringVar   = NULL;
  m_MafStringVar= NULL;
  m_ColorVar    = NULL;

  m_FloatMax  = -1; 
  m_FloatMin  = -1; 
  m_DoubleMax = -1; 
  m_DoubleMin = -1; 
  m_IntMax    = -1;
  m_IntMin    = -1; 

  m_WidgetData.dType  = NULL_DATA;
  m_WidgetData.dValue = 0.0;
  m_WidgetData.fValue = 0.0;
  m_WidgetData.iValue = 0;
  m_WidgetData.sValue = "";
}
//----------------------------------------------------------------------------
bool albaGUIValidator::IsValid()
//----------------------------------------------------------------------------
{
  // Can happen to receive events when the widget has been destroyed:
  // ES: on closing of the application wxWindows send an OnKillFocus to the active control(1)
  // then destroy the controls(2) and the associated validators(3).
  // The message KillFocus can arrive to the validator on the time(2).

  //TODO if ( !(m_gui && m_gui->IsKindOf(CLASSINFO(albaGUI)))   ) return false;
  if ( !m_Listener   ) return false;

  switch (m_Mode) 
	{
    case VAL_WRONG:
			return false;
    break;
    case VAL_LABEL:
      if ( !(m_StaticText && m_StaticText->IsKindOf(CLASSINFO(wxStaticText)))  ) return false;
      if ( !m_MafStringVar && !m_StringVar  ) return false;
    break;
    case VAL_FLOAT:
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_FloatVar ) return false;
      if (  m_FloatMin >= m_FloatMax  ) return false;
			if ( m_DecimalDigits < -1 ) return false;
    break;
    case VAL_DOUBLE:
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_DoubleVar ) return false;
      if (  m_DoubleMin > m_DoubleMax  ) return false;
			if ( m_DecimalDigits < -1 ) return false;
    break;
    case VAL_INTEGER:
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_IntVar ) return false;
      if (  m_IntMin >= m_IntMax  ) return false;
    break;
    case VAL_STRING:
		case VAL_INTERACTIVE_STRING:
		case VAL_MULTILINE_STRING:
		case VAL_MULTILINE_INTERACTIVE_STRING:
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_StringVar ) return false;
    break;
    case VAL_ALBA_STRING:
		case VAL_ALBA_INTERACTIVE_STRING:
		case VAL_ALBA_MULTILINE_INTERACTIVE_STRING:
		case VAL_ALBA_MULTILINE_STRING:
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_MafStringVar ) return false;
    break;
    case VAL_SLIDER:
		case VAL_SLIDER_2:
      if ( !(m_Slider && m_Slider->IsKindOf(CLASSINFO(wxSlider)))  ) return false;
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_IntVar ) return false;
    break;
    case VAL_FLOAT_SLIDER:
		case VAL_FLOAT_SLIDER_2:
      if ( !(m_FloatSlider && m_FloatSlider->IsKindOf(CLASSINFO(albaGUIFloatSlider)))  ) return false;
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_DoubleVar ) return false;
    break;
		case VAL_CHECKBOX:
      if ( !(m_CheckBox && m_CheckBox->IsKindOf(CLASSINFO(wxCheckBox)))  ) return false;
      if ( !m_IntVar ) return false;
    break;
    case VAL_RADIOBOX:
      if ( !(m_RadioBox && m_RadioBox->IsKindOf(CLASSINFO(wxRadioBox)))  ) return false;
      if ( !m_IntVar ) return false;
    break;
    case VAL_COMBOBOX:
      if ( !(m_ComboBox && m_ComboBox->IsKindOf(CLASSINFO(wxComboBox)))  ) return false;
      if ( !m_IntVar ) return false;
    break;
    case VAL_LISTBOX:
      if ( !(m_ListBox && m_ListBox->IsKindOf(CLASSINFO(wxListBox)))  ) return false;
    break;
	case VAL_LISTCTRL:
	  if ( !(m_ListCtrl && m_ListCtrl->IsKindOf(CLASSINFO(wxListCtrl)))  ) return false;
	break;
    case VAL_BUTTON:
      if ( !(m_Button && m_Button->IsKindOf(CLASSINFO(wxButton)))  ) return false;
    break;
    case VAL_DIROPEN:
    case VAL_FILEOPEN:
    case VAL_FILESAVE:
      if ( !(m_Button     && m_Button->IsKindOf(CLASSINFO(wxButton)))  ) return false;
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_MafStringVar ) return false;
    break;
    case VAL_COLOR:
      if ( !(m_Button && m_Button->IsKindOf(CLASSINFO(wxButton)))  ) return false;
      if ( !(m_TextCtrl && m_TextCtrl->IsKindOf(CLASSINFO(wxTextCtrl)))  ) return false;
      if ( !m_ColorVar ) return false;
    break;
    default:
      assert(false);
    break;
  }
  return true;
}
//----------------------------------------------------------------------------
bool albaGUIValidator::Copy(const albaGUIValidator& val)
//----------------------------------------------------------------------------
{
  wxValidator::Copy(val);
  m_Listener        = val.m_Listener;     
  m_ModuleId        = val.m_ModuleId;
	m_DecimalDigits   = val.m_DecimalDigits;

  m_Mode            = val.m_Mode;     

  m_StaticText      = val.m_StaticText; 
  m_TextCtrl        = val.m_TextCtrl; 
  m_Slider          = val.m_Slider;   
  m_FloatSlider	    = val.m_FloatSlider;   
  m_CheckBox        = val.m_CheckBox; 
  m_RadioBox        = val.m_RadioBox; 
  m_ComboBox        = val.m_ComboBox;
  m_ListBox         = val.m_ListBox;
  m_ListCtrl        = val.m_ListCtrl;
  m_Button          = val.m_Button;
  m_StaticText      = val.m_StaticText; 

  m_FloatVar        = val.m_FloatVar;
  m_DoubleVar       = val.m_DoubleVar;
  m_IntVar          = val.m_IntVar;
  m_StringVar       = val.m_StringVar;
  m_MafStringVar    = val.m_MafStringVar;
  m_ColorVar        = val.m_ColorVar;

  m_FloatMax        = val.m_FloatMax;
  m_FloatMin        = val.m_FloatMin;
  m_DoubleMax       = val.m_DoubleMax;
  m_DoubleMin       = val.m_DoubleMin;
  m_IntMax          = val.m_IntMax;
  m_IntMin          = val.m_IntMin;

  m_Wildcard		    = val.m_Wildcard;

  m_WidgetData.dType  = val.m_WidgetData.dType;
  m_WidgetData.dValue = val.m_WidgetData.dValue;
  m_WidgetData.fValue = val.m_WidgetData.fValue;
  m_WidgetData.iValue = val.m_WidgetData.iValue;
  m_WidgetData.sValue = val.m_WidgetData.sValue;

  return true;
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxStaticText *win,albaString* var) //String
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);
  m_Mode        = VAL_LABEL;
  m_StaticText  = win; 
  m_MafStringVar= var;     
  m_WidgetData.dType  = STRING_DATA;
  m_WidgetData.sValue = var->GetCStr();
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxStaticText *win, wxString* var) //String
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);
  m_Mode        = VAL_LABEL;
  m_StaticText  = win; 
  m_StringVar= var;     
  m_WidgetData.dType  = STRING_DATA;
  m_WidgetData.sValue = var->char_str();
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxTextCtrl *win, wxString *var, bool interactive, bool multiline) //String
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);
	
	if(multiline)
		m_Mode = (interactive) ? VAL_MULTILINE_INTERACTIVE_STRING : VAL_MULTILINE_STRING;
	else
		m_Mode = (interactive) ? VAL_INTERACTIVE_STRING : VAL_STRING;
	
	m_TextCtrl  = win; 
  m_StringVar = var;
  m_WidgetData.dType  = STRING_DATA;
  m_WidgetData.sValue = var->char_str();
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxTextCtrl *win, albaString* var, bool interactive, bool multiline) //String
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
	
	if (multiline)
		m_Mode = (interactive) ? VAL_ALBA_MULTILINE_INTERACTIVE_STRING : VAL_ALBA_MULTILINE_STRING;
	else
		m_Mode = (interactive) ? VAL_ALBA_INTERACTIVE_STRING : VAL_ALBA_STRING; 
	
	m_TextCtrl = win;
	m_MafStringVar= var;     
  m_WidgetData.dType  = STRING_DATA;
  m_WidgetData.sValue = var->GetCStr();
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxTextCtrl *win,   int*   var,   int min,   int max)//Integer
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
  m_Mode      = VAL_INTEGER;
  m_TextCtrl  = win; 
  m_IntVar    = var;     
  m_IntMax    = max;     
  m_IntMin    = min;    
  m_WidgetData.dType  = INT_DATA;
  m_WidgetData.iValue = *var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxTextCtrl *win, float* var, float min, float max, int dec_digits)//Float
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
  m_Mode          = VAL_FLOAT;
  m_TextCtrl			= win; 
  m_FloatVar			= var;     
  m_FloatMax			= max;     
  m_FloatMin			= min;
	m_DecimalDigits	= dec_digits;
  m_WidgetData.dType  = FLOAT_DATA;
  m_WidgetData.fValue = *var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxTextCtrl *win, double* var, double min, double max, int dec_digits)//Double
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
  m_Mode          = VAL_DOUBLE;
  m_TextCtrl			= win;
  m_DoubleVar			= var;
  m_DoubleMax			= max;
  m_DoubleMin			= min;
	m_DecimalDigits	= dec_digits;
  m_WidgetData.dType  = DOUBLE_DATA;
  m_WidgetData.iValue = *var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxSlider *win, int* var, wxTextCtrl* lab) //Slider     
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
  m_Mode      = VAL_SLIDER;
  m_TextCtrl  = lab;
  m_Slider    = win;
  m_IntVar    = var;
  m_WidgetData.dType  = INT_DATA;
  m_WidgetData.iValue = *var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener,int mid,wxTextCtrl *win, int* var, wxSlider* lab, int min, int max)
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);
  m_Mode      = VAL_SLIDER_2;
  m_TextCtrl  = win;
  m_Slider    = lab;
  m_IntVar    = var;
	m_IntMin		= min;
	m_IntMax		= max;
  m_WidgetData.dType  = INT_DATA;
  m_WidgetData.iValue = *var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, albaGUIFloatSlider  *win, double*  var, wxTextCtrl* lab) //FloatSlider
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
  m_Mode        = VAL_FLOAT_SLIDER;
  m_TextCtrl    = lab;
  m_FloatSlider = win;
  m_DoubleVar   = var;
  m_WidgetData.dType  = DOUBLE_DATA;
  m_WidgetData.dValue = *var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener,int mid,wxTextCtrl *win, double* var, albaGUIFloatSlider* lab,double min,double max)
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
  m_Mode        = VAL_FLOAT_SLIDER_2;
  m_FloatSlider	= lab;
  m_TextCtrl		= win;
  m_DoubleVar   = var;
	m_DoubleMin		= min;
	m_DoubleMax		= max;
  m_WidgetData.dType  = DOUBLE_DATA;
  m_WidgetData.dValue = *var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxCheckBox *win,   int*   var) //CheckBox    
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);
  m_Mode      = VAL_CHECKBOX;
  m_CheckBox  = win;
  m_IntVar    = var;
  m_WidgetData.dType  = INT_DATA;
  m_WidgetData.iValue = *var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener,int mid,wxListBox *win)
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
  m_Mode      = VAL_LISTBOX;
  m_ListBox   = win;
  m_WidgetData.dType = INT_DATA;
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener,int mid,wxListCtrl *win)
//----------------------------------------------------------------------------
{
	Init(listener,mid,win);  
	m_Mode      = VAL_LISTCTRL;
	m_ListCtrl   = win;
	m_WidgetData.dType = INT_DATA;
	assert(IsValid());
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxRadioBox *win,   int*   var) //RadioBox                     
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
  m_Mode      = VAL_RADIOBOX;
  m_RadioBox  = win; 
  m_IntVar    = var;     
  m_WidgetData.dType  = INT_DATA;
  m_WidgetData.iValue = *var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxComboBox *win,   int*   var) //Combo 
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
  m_Mode      = VAL_COMBOBOX;
  m_ComboBox  = win;
  m_IntVar    = var;
  m_WidgetData.dType  = INT_DATA;
  m_WidgetData.iValue = *var;
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxButton *win) //Button
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
  m_Mode    = VAL_BUTTON;
  m_Button  = win; 
  m_WidgetData.dType = NULL_DATA;
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxButton *win, albaString* var, wxTextCtrl* lab,  bool  openfile, const albaString wildcard)
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
  if (openfile) 
    m_Mode = VAL_FILEOPEN; 
  else 
    m_Mode = VAL_FILESAVE;
  m_Button        = win;
  m_TextCtrl      = lab;
  m_MafStringVar  = var;
  m_Wildcard      = wildcard;
  m_WidgetData.dType  = STRING_DATA;
  m_WidgetData.sValue = var->GetCStr();
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxButton *win, albaString *var, wxTextCtrl* lab)
//----------------------------------------------------------------------------
{
  Init(listener,mid,win);  
  m_Mode          = VAL_DIROPEN;
  m_Button        = win;
  m_TextCtrl      = lab;
  m_MafStringVar  = var;
  m_WidgetData.dType  = STRING_DATA;
  m_WidgetData.sValue = var->GetCStr();
  assert(IsValid());
}
//----------------------------------------------------------------------------
albaGUIValidator::albaGUIValidator(albaObserver* listener, int mid, wxButton *win, wxColour* var, wxTextCtrl* lab)
//----------------------------------------------------------------------------
{
  Init(listener,mid,win); 
  m_Mode      = VAL_COLOR;
  m_Button    = win;
  m_TextCtrl  = lab;
  m_ColorVar  = var;
  m_WidgetData.dType = NULL_DATA;
  assert(IsValid());
}
//----------------------------------------------------------------------------
bool albaGUIValidator::Validate(wxWindow *parent)
//----------------------------------------------------------------------------
{
  assert(IsValid());

  if (m_Mode == VAL_FLOAT )
  {
    double val = m_FloatMin-1;
    wxString s = m_TextCtrl->GetValue();
	  bool res   = s.ToDouble(&val);
    if (!res )
    {
	    if(this->m_DecimalDigits == -1)
				s.Printf("%g",*m_FloatVar);
			else
				s.Printf("%g",RoundValue(*m_FloatVar,m_DecimalDigits));
	    m_TextCtrl->SetValue(s) ;
    }
    if (val < m_FloatMin )
    {
	    s.Printf("%g",m_FloatMin);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val > m_FloatMax )
    {
	    s.Printf("%g",m_FloatMax);
	    m_TextCtrl->SetValue(s) ;
    }
    m_WidgetData.fValue = *m_FloatVar;
  }
  else if (m_Mode == VAL_FLOAT_SLIDER_2)
	{
    double val = m_FloatMin-1;
    wxString s = m_TextCtrl->GetValue();
	  bool res   = s.ToDouble(&val);
    if (!res )
    {
	    if(this->m_DecimalDigits == -1)
				s.Printf("%g",*m_DoubleVar);
			else
				s.Printf("%g",RoundValue(*m_DoubleVar,m_DecimalDigits));
	    m_TextCtrl->SetValue(s) ;
    }
    if (val < m_DoubleMin )
    {
	    s.Printf("%g",m_DoubleMin);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val > m_DoubleMax )
    {
	    s.Printf("%g",m_DoubleMax);
	    m_TextCtrl->SetValue(s) ;
    }
		this->m_FloatSlider->SetValue(*m_DoubleVar);
    m_WidgetData.dValue = *m_DoubleVar;
	}
  else if (m_Mode == VAL_DOUBLE )
  {
    double val = m_DoubleMin-1;
    wxString s = m_TextCtrl->GetValue();
	  bool res   = s.ToDouble(&val);
    if (!res )
    {
	    if(this->m_DecimalDigits == -1)
				s.Printf("%g",*m_DoubleVar);
			else
				s.Printf("%g",RoundValue(*m_DoubleVar,m_DecimalDigits));
	    m_TextCtrl->SetValue(s) ;
    }
    if (val < m_DoubleMin )
    {
	    s.Printf("%g",m_DoubleMin);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val > m_DoubleMax )
    {
	    s.Printf("%g",m_DoubleMax);
	    m_TextCtrl->SetValue(s) ;
    }
    m_WidgetData.dValue = *m_DoubleVar;
  }
  else if ( m_Mode == VAL_INTEGER )
  {
	  long val   = m_IntMin-1;
    wxString s = m_TextCtrl->GetValue();
	  bool res   = s.ToLong(&val);
    if (!res )
    {
	    s.Printf("%d",*m_IntVar);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val < m_IntMin )
    {
	    s.Printf("%d",m_IntMin);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val > m_IntMax )
    {
	    s.Printf("%d",m_IntMax);
	    m_TextCtrl->SetValue(s) ;
    }
    m_WidgetData.iValue = *m_IntVar;
  }
	else if ( m_Mode == VAL_SLIDER_2 )
  {
	  long val   = m_IntMin-1;
    wxString s = m_TextCtrl->GetValue();
	  bool res   = s.ToLong(&val);
    if (!res )
    {
	    s.Printf("%d",*m_IntVar);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val < m_IntMin )
    {
	    s.Printf("%d",m_IntMin);
	    m_TextCtrl->SetValue(s) ;
    }
    if (val > m_IntMax )
    {
	    s.Printf("%d",m_IntMax);
	    m_TextCtrl->SetValue(s) ;
    }
		this->m_Slider->SetValue(*m_IntVar);
    m_WidgetData.iValue = *m_IntVar;
  }
  return true;
}
//----------------------------------------------------------------------------
bool albaGUIValidator::TransferToWindow(void)
//----------------------------------------------------------------------------
{
  assert(IsValid());
  wxString s;
  wxString path;
  wxString name;
  wxString ext;
  
  switch (m_Mode) 
	{
    case VAL_LABEL:
			if(m_MafStringVar)
        m_StaticText->SetLabel(m_MafStringVar->GetCStr());
      if(m_StringVar)
        m_StaticText->SetLabel(m_StringVar->char_str());
    break;
    case VAL_FLOAT:
      if(*m_FloatVar < m_FloatMin ) *m_FloatVar = m_FloatMin;
      if(*m_FloatVar > m_FloatMax ) *m_FloatVar = m_FloatMax;
			if(this->m_DecimalDigits == -1)
				s.Printf("%g",*m_FloatVar);
			else
				s.Printf("%g",RoundValue(*m_FloatVar,m_DecimalDigits));
      m_TextCtrl->SetValue(s);
    break;
    case VAL_DOUBLE:
			if(*m_DoubleVar < m_DoubleMin ) *m_DoubleVar = m_DoubleMin;
			if(*m_DoubleVar > m_DoubleMax ) *m_DoubleVar = m_DoubleMax;
			if(this->m_DecimalDigits == -1)
				s.Printf("%g",*m_DoubleVar);
			else
				s.Printf("%g",RoundValue(*m_DoubleVar,m_DecimalDigits));
      m_TextCtrl->SetValue(s);
    break;
    case VAL_INTEGER:
      if(*m_IntVar < m_IntMin ) *m_IntVar = m_IntMin;
      if(*m_IntVar > m_IntMax ) *m_IntVar = m_IntMax;
	    s.Printf("%d",*m_IntVar);
      m_TextCtrl->SetValue(s);
    break;
    case VAL_STRING:
		case VAL_INTERACTIVE_STRING:
		case VAL_MULTILINE_STRING:
		case VAL_MULTILINE_INTERACTIVE_STRING:
      if (m_StringVar) m_TextCtrl->SetValue(*m_StringVar);
    break;
    case VAL_ALBA_STRING:
		case VAL_ALBA_INTERACTIVE_STRING:
		case VAL_ALBA_MULTILINE_STRING:
		case VAL_ALBA_MULTILINE_INTERACTIVE_STRING:
      if (m_MafStringVar) m_TextCtrl->SetValue(wxString(m_MafStringVar->GetCStr()));
    break;
    case VAL_SLIDER:
      m_Slider->SetValue(*m_IntVar);
	    s.Printf("%d",*m_IntVar);
      m_TextCtrl->SetValue(s);
    break;
		case VAL_SLIDER_2:
      if(*m_IntVar < m_IntMin ) *m_IntVar = m_IntMin;
      if(*m_IntVar > m_IntMax ) *m_IntVar = m_IntMax;
	    s.Printf("%d",*m_IntVar);
      m_TextCtrl->SetValue(s);
      m_Slider->SetValue(*m_IntVar);
		break;
    case VAL_FLOAT_SLIDER:
      m_FloatSlider->SetValue(*m_DoubleVar);
	    s.Printf("%g",*m_DoubleVar);
      m_TextCtrl->SetValue(s);
    break;
		case VAL_FLOAT_SLIDER_2:
      if(*m_DoubleVar < m_DoubleMin ) *m_DoubleVar = m_DoubleMin;
      if(*m_DoubleVar > m_DoubleMax ) *m_DoubleVar = m_DoubleMax;
			if(this->m_DecimalDigits == -1)
				s.Printf("%g",*m_DoubleVar);
			else
				s.Printf("%g",RoundValue(*m_DoubleVar,m_DecimalDigits));
      m_TextCtrl->SetValue(s);
      m_FloatSlider->SetValue(*m_DoubleVar);
    break;
    case VAL_CHECKBOX:
      m_CheckBox->SetValue(*m_IntVar ? true : false);
    break;
    case VAL_LISTBOX:
      if (m_ListBox->GetCount()>0)
      {
        m_ListBox->SetSelection(m_WidgetData.iValue,true);
      }
    break;
	case VAL_LISTCTRL:
	if (m_ListCtrl->GetItemCount()>0)
		{
			m_ListCtrl->SetItemState(m_WidgetData.iValue, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		}
	break;
    case VAL_RADIOBOX:
      m_RadioBox->SetSelection(*m_IntVar);
    break;
    case VAL_COMBOBOX:
      m_ComboBox->SetSelection(*m_IntVar);
    break;
    case VAL_DIROPEN:
			path = m_MafStringVar->GetCStr();
			if( ! ::wxDirExists(path) ) wxFileName::SplitPath(m_MafStringVar->GetCStr(), &path, &name, &ext); // it is a filename
			m_TextCtrl->SetValue(path);
			if(path != "")
			{ 
				m_TextCtrl->SetToolTip(path);
			}
  	break;
    case VAL_FILEOPEN:
    case VAL_FILESAVE:
			wxFileName::SplitPath(m_MafStringVar->GetCStr(), &path, &name, &ext);
			if (ext.Len() >0 )
			{
				name += ".";
				name += ext;
			}
			m_TextCtrl->SetValue(name);
			path = m_MafStringVar->GetCStr();
			if(path != "")
			{ 
				m_TextCtrl->SetToolTip(path);
			}
  	break;
    case VAL_COLOR:
      m_TextCtrl->SetBackgroundColour(*m_ColorVar);
      m_TextCtrl->Refresh();
    break;
  }
  return true;
}
/*//----------------------------------------------------------------------------
float albaGUIValidator::RoundValue(float f_in)
//----------------------------------------------------------------------------
{
	float f_tmp = f_in * pow(10,m_DecimalDigits);
	int b = ( f_tmp >= 0 ) ? static_cast<int>( f_tmp + .5):static_cast<int>( f_tmp - .5);
	return b / pow(10,m_DecimalDigits);
}
//----------------------------------------------------------------------------
double albaGUIValidator::RoundValue(double d_in)
//----------------------------------------------------------------------------
{
	double d_tmp = d_in * pow(10,m_DecimalDigits);
	int b = ( d_tmp >= 0 ) ? static_cast<int>( d_tmp + .5):static_cast<int>( d_tmp - .5);
	return b / pow(10,m_DecimalDigits);
}*/
//----------------------------------------------------------------------------
bool albaGUIValidator::TransferFromWindow(void)
//----------------------------------------------------------------------------
{
  assert(IsValid());

  wxString s;
  wxString sf;
  bool   res; 
  long   ival;
  double dval;
  double f2d;
  long itemIndex = -1;

  switch (m_Mode) 
	{
    case VAL_FLOAT:
			s = m_TextCtrl->GetValue();
			res = s.ToDouble(&dval);
      sf << *m_FloatVar;
      sf.ToDouble(&f2d);
			if (res) 
        res = !albaEquals(f2d,dval);
			if (res)
      {
        *m_FloatVar = dval;
        m_WidgetData.fValue = *m_FloatVar;
      }
    break;
 		case VAL_FLOAT_SLIDER_2:
			s = m_TextCtrl->GetValue();
			res = s.ToDouble(&dval);
			if (res) 
        res = !albaEquals(*m_DoubleVar,dval);
			if (res)
      {
        m_FloatSlider->SetValue(dval);
				*m_DoubleVar = m_FloatSlider->GetValue();
				if (*m_DoubleVar != dval)
				{
					wxString str;
					str.Printf("%f", *m_DoubleVar);
					m_TextCtrl->SetValue(str);
				}
        m_WidgetData.dValue = *m_DoubleVar;
      }
    break;
    case VAL_DOUBLE:
			s = m_TextCtrl->GetValue();
			res = s.ToDouble(&dval);
			if (res) 
        res = !albaEquals(*m_DoubleVar, dval);
			if (res)
      {
        *m_DoubleVar = dval;
        m_WidgetData.dValue = *m_DoubleVar;
      }
    break;
    case VAL_INTEGER:
			s = m_TextCtrl->GetValue();
			res = s.ToLong(&ival);
			if (res) 
        res = *m_IntVar != ival;
      if (res) 
      {
        *m_IntVar = ival;
        m_WidgetData.iValue = *m_IntVar;
      }
    break;
    case VAL_STRING:
		case VAL_INTERACTIVE_STRING:
		case VAL_MULTILINE_STRING:
		case VAL_MULTILINE_INTERACTIVE_STRING:
      if (m_StringVar)  
      {
        s = m_TextCtrl->GetValue();
        res = s != *m_StringVar;
        if (res)
        {
          *m_StringVar = s;
          m_WidgetData.sValue = *m_StringVar;
        }
      }
    break;
    case VAL_ALBA_STRING:
		case VAL_ALBA_INTERACTIVE_STRING:
		case VAL_ALBA_MULTILINE_STRING:
		case VAL_ALBA_MULTILINE_INTERACTIVE_STRING:

      if (m_MafStringVar)
      {
        s = m_TextCtrl->GetValue();
        res = !m_MafStringVar->Equals(s.ToAscii());
        if (res)
        {
          *m_MafStringVar = s;
          m_WidgetData.sValue = m_MafStringVar->GetCStr();
        }
      }
    break;
    case VAL_SLIDER:
			*m_IntVar = m_Slider->GetValue();
      m_WidgetData.iValue = *m_IntVar;
    break;
		case VAL_SLIDER_2:
			s = m_TextCtrl->GetValue();
			res = s.ToLong(&ival);
			if (res) 
        res = *m_IntVar != ival;
			if (res) 
      {
				*m_IntVar = ival;
        m_WidgetData.iValue = *m_IntVar;
      }
		break;
    case VAL_FLOAT_SLIDER:
			*m_DoubleVar = m_FloatSlider->GetSliderValue();
      m_WidgetData.dValue = *m_DoubleVar;
    break;
    case VAL_CHECKBOX:
			*m_IntVar = m_CheckBox->GetValue();
      m_WidgetData.iValue = *m_IntVar;
    break;
    case VAL_LISTBOX:
      m_WidgetData.iValue = m_ListBox->GetSelection();
    break;
	case VAL_LISTCTRL:
	  for (;;) 
	  {
		  m_WidgetData.iValue = itemIndex;
		  itemIndex = m_ListCtrl->GetNextItem(itemIndex,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
		  if (itemIndex == -1) break;
	  }
	break;
    case VAL_RADIOBOX:
			*m_IntVar = m_RadioBox->GetSelection();
      m_WidgetData.iValue = *m_IntVar;
    break;
    case VAL_COMBOBOX:
			*m_IntVar = m_ComboBox->GetSelection();
      m_WidgetData.iValue = *m_IntVar;
    break;
    case VAL_DIROPEN:
    case VAL_FILEOPEN:
    case VAL_FILESAVE:
    break;
    case VAL_COLOR:
			*m_ColorVar = m_TextCtrl->GetBackgroundColour();
    break;
  }
  return true;
}
//----------------------------------------------------------------------------
void albaGUIValidator::OnChar(wxKeyEvent& event)
//----------------------------------------------------------------------------
{
  //Filter key for TextCtrl used for numeric input
  int keyCode = (int)event.GetKeyCode();

  if (((keyCode == WXK_RETURN || keyCode == WXK_TAB) &&
      (m_Mode == VAL_STRING  || m_Mode == VAL_ALBA_STRING ||
				m_Mode == VAL_MULTILINE_STRING || m_Mode == VAL_ALBA_MULTILINE_STRING ||
       m_Mode == VAL_INTEGER || m_Mode == VAL_FLOAT || 
       m_Mode == VAL_DOUBLE  || m_Mode == VAL_FLOAT_SLIDER_2)))     
  {
    // Return is received only from widget with the wxTE_PROCESS_ENTER style flag enabled
    // i.e. console widget
    TransferFromWindow();
    albaEventMacro(albaEvent(m_TextCtrl, m_ModuleId));
		if (m_Mode == VAL_MULTILINE_STRING || m_Mode == VAL_ALBA_MULTILINE_STRING)
			event.Skip();
		else
	    return; // eat message
  }

  // don't filter special keys and Delete
  if (keyCode < WXK_SPACE || keyCode == WXK_DELETE || keyCode > WXK_START)
  {
		event.Skip();
		return;
  }
  if ( m_Mode == VAL_FLOAT || m_Mode == VAL_DOUBLE || m_Mode == VAL_FLOAT_SLIDER_2 )
  {
		if( !(wxIsdigit(keyCode) ||keyCode== '-' ||keyCode== ',' ||keyCode== '.' ||keyCode== 'e' ||keyCode== 'E') )
			return; // eat message
  }
  if ( m_Mode == VAL_INTEGER || m_Mode == VAL_SLIDER_2 )
  {
		if( !(wxIsdigit(keyCode) ||keyCode== '-') )
			return; // eat message
  }
  event.Skip();
}

//----------------------------------------------------------------------------
void albaGUIValidator::OnKeyUp(wxKeyEvent& event)
//----------------------------------------------------------------------------
{
	if(m_Mode == VAL_ALBA_INTERACTIVE_STRING || m_Mode == VAL_INTERACTIVE_STRING || m_Mode == VAL_MULTILINE_INTERACTIVE_STRING || m_Mode == VAL_ALBA_MULTILINE_INTERACTIVE_STRING)
	{
		// Return is received only from widget with the wxTE_PROCESS_ENTER style flag enabled
		// i.e. console widget
		TransferFromWindow();
		albaEventMacro(albaEvent(m_TextCtrl, m_ModuleId));
		return; // eat message
	}
	event.Skip();
}

//----------------------------------------------------------------------------
void albaGUIValidator::OnKillFocus(wxFocusEvent& event)
//----------------------------------------------------------------------------
{
  if (m_Mode == VAL_STRING || m_Mode == VAL_ALBA_STRING || m_Mode == VAL_INTEGER || m_Mode == VAL_FLOAT || 
			m_Mode == VAL_DOUBLE || m_Mode == VAL_FLOAT_SLIDER_2 || m_Mode == VAL_SLIDER_2)
    if ( IsValid() )
    {
      Validate(NULL);
      bool send_message = TransferFromWindow();
      if(send_message)
        albaEventMacro(albaEvent(m_TextCtrl, m_ModuleId));
    }
  event.Skip();
}
//----------------------------------------------------------------------------
void albaGUIValidator::OnScrollEvent(wxScrollEvent& event)
//----------------------------------------------------------------------------
{
  wxString s;
  if ( IsValid() )
  {
    switch (m_Mode) 
		{
      case VAL_SLIDER:
				TransferFromWindow();
				s.Printf("%d",*m_IntVar);
				m_TextCtrl->SetValue(s);
				albaEventMacro(albaEvent(m_Slider, m_ModuleId));
      break;
      case VAL_FLOAT_SLIDER:
				TransferFromWindow();
				s.Printf("%g",*m_DoubleVar);
				m_TextCtrl->SetValue(s);
				albaEventMacro(albaEvent(m_FloatSlider, m_ModuleId, m_FloatSlider,event.GetEventType()));
      break;
		}
  }
  event.Skip();
}
//----------------------------------------------------------------------------
void albaGUIValidator::OnCommandEvent(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  if ( IsValid() )
  {
    TransferFromWindow();
    switch (m_Mode) 
		{
      case VAL_CHECKBOX:
           albaEventMacro(albaEvent(m_CheckBox, m_ModuleId));
      break;
      case VAL_RADIOBOX:
           albaEventMacro(albaEvent(m_RadioBox, m_ModuleId));
      break;
      case VAL_COMBOBOX:
           albaEventMacro(albaEvent(m_ComboBox, m_ModuleId));
      break;
    }
  }
  event.Skip();
}
//----------------------------------------------------------------------------
void albaGUIValidator::OnButton(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  bool cancel = false;
	int ret_code;
	wxString path;
  wxString name;
  wxString ext;
  
  if ( IsValid() )
  {
    switch(m_Mode)
    {
      case VAL_BUTTON:
      break;
      case VAL_DIROPEN:
      {
				if (wxDirExists(m_MafStringVar->GetCStr()))
					path = m_MafStringVar->GetCStr();
				else
					wxFileName::SplitPath(m_MafStringVar->GetCStr(), &path, &name, &ext);

        wxDirDialog dialog(m_Button,"", path, 0, m_Button->GetPosition());
				dialog.SetReturnCode(wxID_OK);
				ret_code = dialog.ShowModal();
        if (ret_code == wxID_OK)
        {
          path = dialog.GetPath();
          *m_MafStringVar = path;
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
        wxFileName::SplitPath(m_MafStringVar->GetCStr(), &path, &name, &ext);
				if (ext.Len() >0 )
				{
					name += ".";
					name += ext;
				}
        wxFileDialog dialog(m_Button,"Open File", path, name, m_Wildcard.GetCStr(), wxFD_OPEN|wxFD_FILE_MUST_EXIST , m_Button->GetPosition());
        dialog.SetReturnCode(wxID_OK);
				ret_code = dialog.ShowModal();
				if (ret_code == wxID_OK)
        {
          path = dialog.GetPath();
          name = dialog.GetFilename();
          *m_MafStringVar = path;
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
        wxFileName::SplitPath(m_MafStringVar->GetCStr(), &path, &name, &ext);
				if (ext.Len() >0 )
				{
					name += ".";
					name += ext;
				}
        wxFileDialog dialog(m_Button,"Save File", path, name, m_Wildcard.GetCStr(), wxFD_SAVE|wxFD_OVERWRITE_PROMPT , m_Button->GetPosition());
        dialog.SetReturnCode(wxID_OK);
				ret_code = dialog.ShowModal();
				if (ret_code == wxID_OK)
        {
          *m_MafStringVar = dialog.GetPath();
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

		if(m_Mode != VAL_BUTTON) 
    {
        albaYield(); // update gui to let the dialogs disappear

       // ( (wxPanel*)m_Button->GetParent() )->SetDefaultItem(NULL);

        albaYield(); 
    }

		if (!cancel) 
      albaEventMacro(albaEvent(m_Button, m_ModuleId));
	}
}
//----------------------------------------------------------------------------
void albaGUIValidator::GetWidgetData(WidgetDataType &widget_data)
//----------------------------------------------------------------------------
{
  widget_data.dType  = m_WidgetData.dType;
  widget_data.dValue = m_WidgetData.dValue;
  widget_data.fValue = m_WidgetData.fValue;
  widget_data.iValue = m_WidgetData.iValue;
  widget_data.sValue = m_WidgetData.sValue;
}
//----------------------------------------------------------------------------
void albaGUIValidator::SetWidgetData(WidgetDataType &widget_data)
//----------------------------------------------------------------------------
{
  switch(widget_data.dType) 
  {
    case INT_DATA:
      if (m_IntVar)
      {
        *m_IntVar = widget_data.iValue;
      }
      else if (m_Mode == VAL_LISTBOX)
      {
        m_WidgetData.iValue = widget_data.iValue;
      }
    break;
    case FLOAT_DATA:
      *m_FloatVar = widget_data.fValue;
    break;
    case DOUBLE_DATA:
      *m_DoubleVar = widget_data.dValue;
    break;
    case STRING_DATA:
      if (m_StringVar)
      {
        *m_StringVar = widget_data.sValue;
      }
      if (m_MafStringVar)
      {
        *m_MafStringVar = widget_data.sValue;
      }
    break;
  }
  TransferToWindow();
}
