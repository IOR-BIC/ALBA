/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUICrossIncremental
 Authors: Daniele Giunchi
 
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

#include <wx/laywin.h>

#include "albaEvent.h"

#include "albaGUI.h"
#include "albaGUICrossIncremental.h"
#include "albaGUIButton.h"
#include "albaGUIValidator.h"
#include "albaGUIComboBox.h"

//----------------------------------------------------------------------------
// costants :
//----------------------------------------------------------------------------
const wxPoint dp = wxDefaultPosition; 

//----------------------------------------------------------------------------
// albaGUICrossIncremental
//----------------------------------------------------------------------------

const int  M	= 1;											// margin all around a row  
const int LM	= 5;											// label margin             
const int HM	= 2*M;										// horizontal margin        (2)

const int LH	= 18;											// label/entry height       
const int BH	= 20;											// button height            

//const int LW	= 60;											// label width
//const int EW	= 60;											// entry width  - (was 48)  

#ifdef WIN32
const int LW	= 60;	// label width Windows
#else
const int LW	= 100;	// label width Linux
#endif
const int EW	= 45;
const int MEDIUM	= 70;

BEGIN_EVENT_TABLE(albaGUICrossIncremental,albaGUIPanel)

END_EVENT_TABLE()


#define FONTSIZE 8

//----------------------------------------------------------------------------
albaGUICrossIncremental::albaGUICrossIncremental(wxWindow* parent, wxWindowID id, const char * label, double *stepVariable, double *topBottomVariable, double *leftRightVariable, bool boldLabel /* = true */, int modality /* = ID_COMPLETE_LAYOUT */, const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxDefaultSize */, double min /* = MINFLOAT */, double max /* = MAXFLOAT */, int decimal_digit /* = -1 */, long style /* = wxTAB_TRAVERSAL | wxCLIP_CHILDREN */, bool comboStep /* = false */, albaString *buttonUpDown_text /* = NULL */, albaString *buttonLeftRight_text /* = NULL */)
:albaGUIPanel(parent,id,pos,size,style) 
//----------------------------------------------------------------------------
{
  m_Listener = NULL;
	m_TopBottomVariable = topBottomVariable; 
	m_LeftRightVariable = leftRightVariable;
  m_StepVariable = stepVariable;

	m_IdLayout = modality;

	m_ButtonTop= NULL;
	m_ButtonBottom= NULL;
	m_ButtonLeft= NULL;
	m_ButtonRight= NULL;
	m_StepText= NULL;
  m_StepComboBox = NULL;

	m_Sizer = NULL;
  m_Increment = 0;
	m_Bold = boldLabel;
  m_Digits = decimal_digit;

  m_IsComboStep = comboStep;
	
	
  CreateWidgetTopBottom(buttonUpDown_text);
	CreateWidgetLeftRight(buttonLeftRight_text);

  if(!comboStep)
	  CreateWidgetTextEntry(min, max );
  else
    CreateWidgetComboBox();

  LayoutStyle(label);
  
	this->SetAutoLayout( true );
	this->SetSizer( m_Sizer );
	m_Sizer->Fit(this);
	m_Sizer->SetSizeHints(this);

}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::CreateWidgetTopBottom(albaString *button_text)
//----------------------------------------------------------------------------
{
	if(m_IdLayout == ID_TOP_BOTTOM_LAYOUT || m_IdLayout == ID_COMPLETE_LAYOUT )
	{
    albaString buttonTextToApply[2];
    if (button_text != NULL)
    {
      buttonTextToApply[0] = button_text[0];
      buttonTextToApply[1] = button_text[1];
    }
    else
    {
      buttonTextToApply[0] = "+";
      buttonTextToApply[1] = "-";
    }

		m_ButtonTop = new albaGUIButton   (this, ID_BUTTON_TOP/*w_id*/, buttonTextToApply[0].GetCStr(),dp, wxSize(EW, BH + 2) );
		m_ButtonTop->SetValidator( albaGUIValidator(this,ID_BUTTON_TOP/*w_id*/,m_ButtonTop) );
		//  m_ButtonTop->SetFont(m_Font);
		//if(!tooltip.IsEmpty()) 
		//  m_ButtonTop->SetToolTip(tooltip.GetCStr());

		m_ButtonBottom = new albaGUIButton   (this, ID_BUTTON_BOTTOM/*w_id*/, buttonTextToApply[1].GetCStr(),dp, wxSize(EW, BH+2) );

		m_ButtonBottom->SetValidator( albaGUIValidator(this,ID_BUTTON_BOTTOM/*w_id*/,m_ButtonBottom) );
		//m_ButtonBottom->SetFont(m_Font);
		//if(!tooltip.IsEmpty()) 
		//   butt->SetToolTip(tooltip.GetCStr());
	}
	
}

//----------------------------------------------------------------------------
void albaGUICrossIncremental::CreateWidgetLeftRight(albaString *button_text)
//----------------------------------------------------------------------------
{
	if(m_IdLayout == ID_LEFT_RIGHT_LAYOUT || m_IdLayout == ID_COMPLETE_LAYOUT)
	{
    albaString buttonTextToApply[2];
    if (button_text != NULL)
    {
      buttonTextToApply[0] = button_text[0];
      buttonTextToApply[1] = button_text[1];
    }
    else
    {
      buttonTextToApply[0] = "-";
      buttonTextToApply[1] = "+";
    }
		m_ButtonLeft = new albaGUIButton   (this, ID_BUTTON_LEFT/*w_id*/, buttonTextToApply[0].GetCStr(),dp, wxSize(EW, BH+2) );
		m_ButtonLeft->SetValidator( albaGUIValidator(this,ID_BUTTON_LEFT/*w_id*/,m_ButtonLeft) );
		//m_ButtonLeft->SetFont(m_Font);
		//if(!tooltip.IsEmpty()) 
		//  m_ButtonTop->SetToolTip(tooltip.GetCStr());

		m_ButtonRight = new albaGUIButton   (this, ID_BUTTON_RIGHT/*w_id*/, buttonTextToApply[1].GetCStr(),dp, wxSize(EW, BH+2) );
		m_ButtonRight->SetValidator( albaGUIValidator(this,ID_BUTTON_RIGHT/*w_id*/,m_ButtonRight) );
		//m_ButtonRight->SetFont(m_Font);
		//if(!tooltip.IsEmpty()) 
		//  m_ButtonTop->SetToolTip(tooltip.GetCStr());
	}
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::CreateWidgetTextEntry(double min , double max )
//----------------------------------------------------------------------------
{
	
	//w_id = ((albaGUI *) parent)->GetWidgetId(w_id);
  if(m_Digits != 0)
  {
    m_StepText = new wxTextCtrl  (this, ID_STEP_ENTRY/*w_id*/, albaString::Format("%.1f", *m_StepVariable)   , dp, wxSize(EW,BH)/*, m_EntryStyle  */);
    m_StepText->SetValidator( albaGUIValidator(this,ID_STEP_ENTRY/*w_id*/,m_StepText,m_StepVariable,min,max,m_Digits)  );
  }
  else
  {
    m_StepText = new wxTextCtrl  (this, ID_STEP_ENTRY/*w_id*/, albaString::Format("%.0f", *m_StepVariable)   , dp, wxSize(EW,BH)/*, m_EntryStyle  */);
    m_StepText->SetValidator( albaGUIValidator(this,ID_STEP_ENTRY/*w_id*/,m_StepText,m_StepVariable,min,max,m_Digits)  );
  }
	
	//m_StepText->SetFont(m_Font);
	//if(!tooltip.IsEmpty())
	//  text->SetToolTip(tooltip.GetCStr());
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::CreateWidgetComboBox()
//----------------------------------------------------------------------------
{
  wxArrayString array;
  m_StepComboBox= new albaGUIComboBox(this, ID_COMBO_ENTRY ,array ,wxDefaultPosition, wxSize(EW,BH),wxCB_READONLY );
  m_StepComboBox->SetListener(this);
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::SetComboBoxItems(wxArrayString &array, int selected)
//----------------------------------------------------------------------------
{
  if(m_StepComboBox)
  {
    m_StepComboBox->Clear();
    for(int i=0; i< array.Count(); i++)
    {
      m_StepComboBox->Insert(array[i], i);
    }
    m_StepComboBox->SetSelection(selected);
  }
}
//----------------------------------------------------------------------------
albaGUICrossIncremental::~albaGUICrossIncremental( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
		int eventId = e->GetId();
		//eventId = eventId - m_Id;
    switch(eventId)
    {
    case ID_BUTTON_TOP:
      if(NULL != m_TopBottomVariable)
      {
			   *m_TopBottomVariable += *m_StepVariable;

         m_TopBottomVariation = -*m_StepVariable;
         m_LeftRightVariation = 0.0;
         m_Increment = ID_INCREMENT;
      }
			e->SetArg(ID_INCREMENT);
      break;
		case ID_BUTTON_LEFT:
      if(NULL != m_LeftRightVariable)
      {
			  *m_LeftRightVariable -= *m_StepVariable;
        m_LeftRightVariation = -*m_StepVariable;
        m_TopBottomVariation = 0.0;
        m_Increment = ID_DECREMENT;
      }
			e->SetArg(ID_DECREMENT);
			break;
		case ID_BUTTON_RIGHT:
      if(NULL != m_LeftRightVariable)
      {
			  *m_LeftRightVariable += *m_StepVariable;
        m_LeftRightVariation = *m_StepVariable;
        m_TopBottomVariation = 0.0;
        m_Increment = ID_INCREMENT;
      }
			e->SetArg(ID_INCREMENT);
			break;
		case ID_BUTTON_BOTTOM:
      if(NULL != m_TopBottomVariable)
      {
			  *m_TopBottomVariable -= *m_StepVariable;
        
        m_TopBottomVariation = *m_StepVariable;
        m_LeftRightVariation = 0.0;

        m_Increment = ID_DECREMENT;
      }
			e->SetArg(ID_DECREMENT);
			break;
		case ID_STEP_ENTRY:
			e->SetArg(ID_CHANGE_STEP);
      m_TopBottomVariation = 0.0;
      m_LeftRightVariation = 0.0;
			break;
    case ID_COMBO_ENTRY:
      {
        ConvertStepComboIntoStepVariable();
        /*albaString value;
        value << *m_StepVariable;
        wxMessageBox(value.GetCStr());*/
      }
      break;
    default:
      e->Log();
      break;
    }

    if(eventId != ID_COMBO_ENTRY)
    {
		  e->SetId(GetId());
		  albaEventMacro(*e);
    }
  }
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::EnableStep(bool value)
//----------------------------------------------------------------------------
{
	if(m_StepText != NULL)
	{
		m_StepText->Enable(value);
		m_StepText->Update();
	}
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::SetTextButtonTop(const char *text)
//----------------------------------------------------------------------------
{
  if(NULL != m_ButtonTop)
  {
    m_ButtonTop->SetLabel(text);
  }
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::SetTextButtonBottom(const char *text)
//----------------------------------------------------------------------------
{
  if(NULL != m_ButtonBottom)
  {
    m_ButtonBottom->SetLabel(text);
  }
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::SetTextButtonLeft(const char *text)
//----------------------------------------------------------------------------
{
  if(NULL != m_ButtonLeft)
  {
    m_ButtonLeft->SetLabel(text);
  }
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::SetTextButtonRight(const char *text)
//----------------------------------------------------------------------------
{
  if(NULL != m_ButtonRight)
  {
    m_ButtonRight->SetLabel(text);
  }
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::LayoutStyle(const char* label)
//----------------------------------------------------------------------------
{
  switch(m_IdLayout)
  {
  case ID_TOP_BOTTOM_LAYOUT:
    {
      wxStaticText *lab  = new wxStaticText(this, -1, label, dp, wxSize(2*LW, LH), wxALIGN_LEFT | wxST_NO_AUTORESIZE );
      wxFont font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32   
      font.SetPointSize(8);
#endif
      if(m_Bold)
      {
        font.SetWeight(wxBOLD);
      }

      lab->SetFont(font);
      lab->Update();

      //wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
      m_Sizer =  new wxBoxSizer( wxVERTICAL );
      wxBoxSizer *verticalSizer = new wxBoxSizer(wxHORIZONTAL);
      wxBoxSizer *horizontalSizer = new wxBoxSizer(wxVERTICAL);
      wxBoxSizer *sizerLabel = new wxBoxSizer(wxHORIZONTAL);
      wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
      wxBoxSizer *sizerMiddle = new wxBoxSizer(wxHORIZONTAL);
      wxBoxSizer *sizerBottom = new wxBoxSizer(wxHORIZONTAL);

      sizerLabel->Add(lab, 0, wxALIGN_LEFT, 0);

      sizerTop->AddSpacer(LM);
      sizerTop->Add( m_ButtonTop, 0, wxALIGN_LEFT, 0);

      sizerMiddle->AddSpacer(LM);    
      //sizerMiddle->AddSpacer(EW);
      if(!m_IsComboStep)
        sizerMiddle->Add( m_StepText, 0, wxALIGN_CENTRE, 0);
      else
      {
        sizerMiddle->Add( m_StepComboBox, 0, wxALIGN_CENTRE, 0);
        sizerMiddle->AddSpacer(4);
      }
      
      sizerBottom->AddSpacer(LM);
      sizerBottom->Add( m_ButtonBottom, 0, wxALIGN_LEFT, 0);


      //horizontalSizer->Add( sizerLabel, 0, wxALIGN_TOP, 0);

      
      horizontalSizer->Add( sizerTop, 0, wxALL, 0);
      horizontalSizer->Add( sizerMiddle, 0, wxALL, 0);
      horizontalSizer->Add( sizerBottom, 0, wxALL, 0);

      verticalSizer->AddSpacer(MEDIUM);
      verticalSizer->Add( horizontalSizer, 0, wxALL, 0);

      m_Sizer->Add( sizerLabel, 0, wxALIGN_LEFT, 0);
      m_Sizer->Add( verticalSizer, 0, wxALL, 0);

    }
    break;
  case ID_LEFT_RIGHT_LAYOUT:
    {
      wxStaticText *lab  = new wxStaticText(this, -1, label, dp, wxSize(LW, LH), wxALIGN_LEFT | wxST_NO_AUTORESIZE );
      wxFont font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32   
      font.SetPointSize(FONTSIZE);
#endif
      if(m_Bold)
      {
        font.SetWeight(wxBOLD);
      }

      lab->SetFont(font);
      lab->Update();

      m_Sizer =  new wxBoxSizer( wxHORIZONTAL );     
     
      m_Sizer->AddSpacer(LM);
      m_Sizer->Add(lab, 0, wxALIGN_LEFT, 0);

      m_Sizer->Add( m_ButtonLeft, 0, wxALIGN_LEFT, 0);
      if(!m_IsComboStep)
        m_Sizer->Add( m_StepText, 0, wxALIGN_LEFT, 0);
      else
        m_Sizer->Add( m_StepComboBox, 0, wxALIGN_LEFT, 0);
      m_Sizer->Add( m_ButtonRight, 0, wxALIGN_LEFT, 0);

      //((albaGUI *)parent)->Add(sizer,0,wxALL, 0);

    }
    break;
  case ID_COMPLETE_LAYOUT:
    {
      wxStaticText *lab  = new wxStaticText(this, -1, label, dp, wxSize(2*LW, LH), wxALIGN_LEFT | wxST_NO_AUTORESIZE );
      wxFont font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32   
      font.SetPointSize(FONTSIZE);
#endif
      if(m_Bold)
      {
        font.SetWeight(wxBOLD);
      }

      lab->SetFont(font);
      lab->Update();

      //wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
      m_Sizer =  new wxBoxSizer( wxVERTICAL );
      wxBoxSizer *verticalSizer = new wxBoxSizer(wxHORIZONTAL);
      wxBoxSizer *horizontalSizer = new wxBoxSizer(wxVERTICAL);
      wxBoxSizer *sizerLabel = new wxBoxSizer(wxHORIZONTAL);
      wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
      wxBoxSizer *sizerMiddle = new wxBoxSizer(wxHORIZONTAL);
      wxBoxSizer *sizerBottom = new wxBoxSizer(wxHORIZONTAL);

      sizerLabel->Add(lab, 0, wxALIGN_LEFT, 0);

      sizerTop->AddSpacer(LM);
      if(m_ButtonTop != NULL) sizerTop->Add( m_ButtonTop, 0, wxALIGN_LEFT, 0);

      sizerMiddle->AddSpacer(LM);
      if(m_ButtonLeft != NULL) sizerMiddle->Add( m_ButtonLeft, 0, wxALIGN_CENTRE, 0);
      //sizerMiddle->AddSpacer(EW);
      if(m_StepText != NULL) sizerMiddle->Add( m_StepText, 0, wxALIGN_CENTRE, 0);
      if(m_StepComboBox != NULL) sizerMiddle->Add( m_StepComboBox, 0, wxALIGN_CENTRE, 0);
      if(m_ButtonRight != NULL) sizerMiddle->Add( m_ButtonRight, 0, wxALIGN_CENTRE, 0);


      sizerBottom->AddSpacer(LM);
      if(m_ButtonBottom != NULL) sizerBottom->Add( m_ButtonBottom, 0, wxALIGN_LEFT, 0);
      
      horizontalSizer->Add( sizerTop, 0, wxALIGN_CENTRE, 0);
      horizontalSizer->Add( sizerMiddle, 0, wxALIGN_CENTRE, 0);
      horizontalSizer->Add( sizerBottom, 0, wxALIGN_CENTRE, 0);
      
      verticalSizer->AddSpacer(LW);
      verticalSizer->Add( horizontalSizer, 0, wxALL, 0);

      m_Sizer->Add( sizerLabel, 0, wxALIGN_LEFT, 0);
      m_Sizer->Add( verticalSizer, 0, wxALL, 0);

      //((albaGUI *)parent)->Add(sizer,0,wxALL, 0);

    }
    break;
  } 
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::ConvertStepComboIntoStepVariable()
//----------------------------------------------------------------------------
{
  if(m_StepComboBox)
  {
    *m_StepVariable = atof(m_StepComboBox->GetValue());
  }
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::SetComboValue(int index)
//----------------------------------------------------------------------------
{
  m_StepComboBox->SetSelection(index);
  m_StepComboBox->Update();

  ConvertStepComboIntoStepVariable();
}
//----------------------------------------------------------------------------
void albaGUICrossIncremental::SetStepVariable(double step)
//----------------------------------------------------------------------------
{
  if(m_StepText)
  {
    *m_StepVariable = step;
    if(m_Digits != 0)
    {
      m_StepText->SetValue(albaString::Format("%.1f", *m_StepVariable));
    }
    else
    {
      m_StepText->SetValue(albaString::Format("%.0f", *m_StepVariable));
    }
    
  }
}