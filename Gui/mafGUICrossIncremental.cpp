/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUICrossIncremental.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-24 08:39:51 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2008
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <wx/laywin.h>

#include "mafEvent.h"

#include "mmgGui.h"
#include "mafGUICrossIncremental.h"
#include "mmgButton.h"
#include "mmgValidator.h"

//----------------------------------------------------------------------------
// costants :
//----------------------------------------------------------------------------
const wxPoint dp = wxDefaultPosition; 

//----------------------------------------------------------------------------
// mafGUICrossIncremental
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

BEGIN_EVENT_TABLE(mafGUICrossIncremental,mmgPanel)

END_EVENT_TABLE()


#define FONTSIZE 9

//----------------------------------------------------------------------------
mafGUICrossIncremental::mafGUICrossIncremental(wxWindow* parent, wxWindowID id, const char * label, double *stepVariable, double *topBottomVariable, double *leftRightVariable, bool boldLabel /* = true */, int modality /* = ID_COMPLETE_LAYOUT */, const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxDefaultSize */, double min /* = MINFLOAT */, double max /* = MAXFLOAT */, int decimal_digit /* = -1 */, long style /* = wxTAB_TRAVERSAL | wxCLIP_CHILDREN */)
:mmgPanel(parent,id,pos,size,style) 
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

	m_Sizer = NULL;
  m_Increment = 0;
	m_Bold = boldLabel;
	
	
  CreateWidgetTopBottom();
	CreateWidgetLeftRight();
	CreateWidgetTextEntry(min, max, decimal_digit );

  LayoutStyle(label);
  
	this->SetAutoLayout( TRUE );
	this->SetSizer( m_Sizer );
	m_Sizer->Fit(this);
	m_Sizer->SetSizeHints(this);

}
//----------------------------------------------------------------------------
void mafGUICrossIncremental::CreateWidgetTopBottom()
//----------------------------------------------------------------------------
{
	if(m_IdLayout == ID_TOP_BOTTOM_LAYOUT || m_IdLayout == ID_COMPLETE_LAYOUT )
	{
		m_ButtonTop = new mmgButton   (this, ID_BUTTON_TOP/*w_id*/, "+",dp, wxSize(EW, BH) );
		m_ButtonTop->SetValidator( mmgValidator(this,ID_BUTTON_TOP/*w_id*/,m_ButtonTop) );
		//  m_ButtonTop->SetFont(m_Font);
		//if(!tooltip.IsEmpty()) 
		//  m_ButtonTop->SetToolTip(tooltip.GetCStr());

		m_ButtonBottom = new mmgButton   (this, ID_BUTTON_BOTTOM/*w_id*/, "-",dp, wxSize(EW, BH) );

		m_ButtonBottom->SetValidator( mmgValidator(this,ID_BUTTON_BOTTOM/*w_id*/,m_ButtonBottom) );
		//m_ButtonBottom->SetFont(m_Font);
		//if(!tooltip.IsEmpty()) 
		//   butt->SetToolTip(tooltip.GetCStr());
	}
	
}

//----------------------------------------------------------------------------
void mafGUICrossIncremental::CreateWidgetLeftRight()
//----------------------------------------------------------------------------
{
	if(m_IdLayout == ID_LEFT_RIGHT_LAYOUT || m_IdLayout == ID_COMPLETE_LAYOUT)
	{
		m_ButtonLeft = new mmgButton   (this, ID_BUTTON_LEFT/*w_id*/, "-",dp, wxSize(EW, BH) );
		m_ButtonLeft->SetValidator( mmgValidator(this,ID_BUTTON_LEFT/*w_id*/,m_ButtonLeft) );
		//m_ButtonLeft->SetFont(m_Font);
		//if(!tooltip.IsEmpty()) 
		//  m_ButtonTop->SetToolTip(tooltip.GetCStr());

		m_ButtonRight = new mmgButton   (this, ID_BUTTON_RIGHT/*w_id*/, "+",dp, wxSize(EW, BH) );
		m_ButtonRight->SetValidator( mmgValidator(this,ID_BUTTON_RIGHT/*w_id*/,m_ButtonRight) );
		//m_ButtonRight->SetFont(m_Font);
		//if(!tooltip.IsEmpty()) 
		//  m_ButtonTop->SetToolTip(tooltip.GetCStr());
	}
}
//----------------------------------------------------------------------------
void mafGUICrossIncremental::CreateWidgetTextEntry(double min , double max , int decimal_digit )
//----------------------------------------------------------------------------
{
	
	//w_id = ((mmgGui *) parent)->GetWidgetId(w_id);
	m_StepText = new wxTextCtrl  (this, ID_STEP_ENTRY/*w_id*/, wxString::Format("%.2f", *m_StepVariable)   , dp, wxSize(EW,BH)/*, m_EntryStyle  */);
	m_StepText->SetValidator( mmgValidator(this,ID_STEP_ENTRY/*w_id*/,m_StepText,m_StepVariable,min,max,decimal_digit)  );
	//m_StepText->SetFont(m_Font);
	//if(!tooltip.IsEmpty())
	//  text->SetToolTip(tooltip.GetCStr());
}
//----------------------------------------------------------------------------
mafGUICrossIncremental::~mafGUICrossIncremental( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUICrossIncremental::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
    default:
      e->Log();
      break;
    }

    
		e->SetId(GetId());
		mafEventMacro(*e);
  }
}
//----------------------------------------------------------------------------
void mafGUICrossIncremental::EnableStep(bool value)
//----------------------------------------------------------------------------
{
	if(m_StepText != NULL)
	{
		m_StepText->Enable(value);
		m_StepText->Update();
	}
}
//----------------------------------------------------------------------------
void mafGUICrossIncremental::LayoutStyle(const char* label)
//----------------------------------------------------------------------------
{
  switch(m_IdLayout)
  {
  case ID_TOP_BOTTOM_LAYOUT:
    {
      wxStaticText *lab  = new wxStaticText(this, -1, label, dp, wxSize(2*LW, LH), wxALIGN_LEFT | wxST_NO_AUTORESIZE );
      wxFont font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32   
      font.SetPointSize(9);
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
      sizerMiddle->Add( m_StepText, 0, wxALIGN_CENTRE, 0);
      
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
      m_Sizer->Add( m_StepText, 0, wxALIGN_LEFT, 0);
      m_Sizer->Add( m_ButtonRight, 0, wxALIGN_LEFT, 0);

      //((mmgGui *)parent)->Add(sizer,0,wxALL, 0);

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

      //((mmgGui *)parent)->Add(sizer,0,wxALL, 0);

    }
    break;
  }
  
}