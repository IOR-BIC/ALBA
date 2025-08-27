/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUI
 Authors: Silvano Imboden - Paolo Quadrani
 
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

#include <wx/statline.h>

#include "albaGUIFloatSlider.h"
#include "albaGUICheckListBox.h"
#include "albaGUICrossIncremental.h"

#include "albaGUIValidator.h"
#include "albaGUI.h"
#include "albaGUIButton.h"
#include "albaGUIPicButton.h"
#include "albaGUIRollOut.h"


#ifdef ALBA_USE_VTK //:::::::::::::::::::::::::::::::::
#include "albaGUILutSwatch.h"
#endif             //:::::::::::::::::::::::::::::::::

#include "albaString.h"
#include <wx/image.h>

// ugly hack to make FindWindow Work
// if you remove this line you will have a Compile-Error "FindWindowA is not defined for wxWindow"
// .... waiting a better workaround. SIL 30/11/05 
#if WIN32  
  #include <wx/msw/winundef.h> 
#endif
#include "wx/sizer.h"
#include "albaPics.h"
// end of hack

//----------------------------------------------------------------------------
// constant
//----------------------------------------------------------------------------
/*
     the GUI area is divided in 4 main columns as follow,
		(all widget except Slider and VectorN use this convention)

		 +------------------------------------------------+ 
		 | +----------------+--+------+-+------+-+------+ |
     | |                |  |      | |      | |      | |  rows height is BH (for buttons) or LH (for labels and everything else)
		 | +----------------+--+------+-+------+-+------+ |
		 | +----------------+--+------+-+------+-+------+ |    
     | |                |  |      | |      | |      | |
		 | +----------------+--+------+-+------+-+------+ |
		 | +----------------+--+------+-+------+-+------+ |  rows are separated by a margin M all around
     | |                |  |      | |      | |      | |
		 | +----------------+--+------+-+------+-+------+ | 

		                       <------> <------> <------>    EW: entry width
        <--------------->                                LW: label width
				                <-->                             LM: label margin
                                   <->      <->          HM: horizontal margin
     <->                                            <->   M: margin alla around a row
	      <------------------------------------------->    FW: full width
                           <------------------------>    DW: data width

*/
const int  M	= 1;											// margin all around a row  
const int LM	= 5;											// label margin             
const int HM	= 2*M;										// horizontal margin        (2)

const int LH	= 18;											// label/entry height       
const int BH	= 20;											// button height            

//const int LW	= 60;											// label width
//const int EW	= 60;											// entry width  - (was 48)  

#ifdef WIN32
const int LW	= 55;	// label width Windows
#else
const int LW	= 100;	// label width Linux
#endif
const int EW	= 45;											// entry width  - (was 48)  
const int FW	= LW+LM+EW+HM+EW+HM+EW;		// full width               (304)
const int DW	= EW+HM+EW+HM+EW;					// Data Width - Full Width without the Label (184)

static wxPoint dp = wxDefaultPosition; 

// int ALBAWidgetId = MINID;

//----------------------------------------------------------------------------
// albaGUI
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUI,albaGUIPanel)
    EVT_COMMAND_RANGE( MINID,MAXID,wxEVT_COMMAND_LISTBOX_SELECTED,      albaGUI::OnListBox)
		EVT_COMMAND_RANGE( MINID,MAXID,wxEVT_COMMAND_LIST_ITEM_SELECTED,    albaGUI::OnListCtrl)
    EVT_COMMAND_RANGE( MINID,MAXID,wxEVT_COMMAND_CHECKLISTBOX_TOGGLED , albaGUI::OnCheckListBox)
		EVT_COMMAND_RANGE(MINID, MAXID, wxEVT_COMMAND_RADIOBUTTON_SELECTED, albaGUI::OnRadioButton)
	  EVT_COMMAND_RANGE( MINID,MAXID,wxEVT_COMMAND_SLIDER_UPDATED,				albaGUI::OnSlider)
    EVT_MOUSEWHEEL(albaGUI::OnMouseWheel)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUI::albaGUI(albaObserver *listener) :
	albaGUIPanel(albaGetFrame(), -1, dp, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
	m_Listener = listener;

	m_UseBackgroundColor = false;
	m_BackgroundColor = wxColour(251, 251, 253);
	if (m_UseBackgroundColor) this->SetBackgroundColour(m_BackgroundColor);

	m_EntryStyle = wxBORDER_SIMPLE | wxTE_PROCESS_ENTER;

	m_Sizer = new wxBoxSizer(wxVERTICAL);
	this->SetAutoLayout(true);
	this->SetSizer(m_Sizer);
	m_Sizer->Fit(this);
	m_Sizer->SetSizeHints(this);

	m_BoldFont = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
	m_Font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32   
	m_BoldFont.SetPointSize(9);
#endif
	m_BoldFont.SetWeight(wxBOLD);

	m_WidgetTableID.clear();

	this->Show(false);
}
//----------------------------------------------------------------------------
albaGUI::~albaGUI()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUI::FitGui()
{
	this->SetMinSize(wxSize(FW + M + M, m_Sizer->GetMinSize().GetHeight()));  //SIL. 22-may-2006 : 
	this->SetSize(wxSize(FW + M + M, m_Sizer->GetMinSize().GetHeight()));
}
//----------------------------------------------------------------------------
int albaGUI::GetMetrics(int id)
{
	switch (id)
	{
	case GUI_ROW_MARGIN:
		return M;
		break;
	case GUI_LABEL_MARGIN:
		return LM;
		break;
	case GUI_WIDGET_MARGIN:
		return HM;
		break;
	case GUI_LABEL_HEIGHT:
		return LH;
		break;
	case GUI_BUTTON_HEIGHT:
		return BH;
		break;
	case GUI_LABEL_WIDTH:
		return LW;
		break;
	case GUI_WIDGET_WIDTH:
		return EW;
		break;
	case GUI_FULL_WIDTH:
		return FW;
		break;
	case GUI_DATA_WIDTH:
		return DW;
		break;
	case GUI_HOLDER_WIDTH:
		return FW + 18;
		break; // 18 is the scrollbar width
	default:
		return 0;
	}
}

/**  \par implementation details:
called by the widgets (via a albaGUIValidator), forward the events to the eventListener
\todo
make it protected, and make albaGUIValidator friend
*/
//----------------------------------------------------------------------------
void albaGUI::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		int id = e->GetId();
		if (id >= MINID && id < MAXID)
			id = GetModuleId(id);
		e->SetSender(this);
		e->SetId(id);
		albaEventMacro(*e);
	}
}
//----------------------------------------------------------------------------
void albaGUI::AddGui(albaGUI*  gui, int option, int flag, int border)
{
	gui->Reparent(this);
	gui->FitGui();
	gui->Show(true);

	Add(gui, option, flag, border);
}
//----------------------------------------------------------------------------
void albaGUI::Update()
{
  TransferDataToWindow();
}
//----------------------------------------------------------------------------
void albaGUI::Enable(int mod_id, bool enable)
{
	if (mod_id == wxOK || mod_id == wxCANCEL)
	{
		wxWindow* win = FindWindow(mod_id);
		if (win)
			win->Enable(enable);
	}
	else
	{
		for (int i = MINID; i <= (*GetALBAWidgetId()); i++)
		{
			if (m_WidgetTableID[i - MINID] == mod_id)
			{
				wxWindow* win = FindWindow(i);
				if (win)
					win->Enable(enable);
			}
		}
	}
}

// WIDGETS

// warning!: related to the label of all widgets
// if the string is longer of the available space for the label and it contain some spaces
// the text following the space is not shown
// max length for long label:  "12345678901234567890123456789012345" 
// max length for short label: "1234567890"

//----------------------------------------------------------------------------
void albaGUI::Divider(long style)
{
	if (style == 0) //simple empty space
	{
		wxStaticText* div = new wxStaticText(this, -1, "", dp, wxSize(FW, 2), 0);
		Add(div, 0, wxALL, M);
	}
	else
	{
		wxStaticLine *div = new wxStaticLine(this, -1, dp, wxSize(FW, 1));
		Add(div, 0, wxALL, 2 * M);
	}
}

//----------------------------------------------------------------------------
/** in the multiline case - explicit newline must be inserted in the string - also append a newline at the end */
void albaGUI::Label(albaString label, bool bold, bool multiline)
{
	int h = (multiline) ? -1 : LH;
	wxStaticText* lab = new wxStaticText(this, -1, label.GetCStr(), dp, wxSize(-1, h), wxALIGN_LEFT);
	
	if (m_UseBackgroundColor) lab->SetBackgroundColour(m_BackgroundColor);
	
	if (bold)
		lab->SetFont(m_BoldFont);
	else
		lab->SetFont(m_Font);
	
	Add(lab, 0, wxEXPAND | wxALL, M);
}
//----------------------------------------------------------------------------
void albaGUI::Label(albaString *var, bool bold, bool multiline)
{
	int h = (multiline) ? -1 : LH;
	wxStaticText* lab = new wxStaticText(this, -1, var->GetCStr(), dp, wxSize(-1, h), wxALIGN_LEFT);
	
	if (m_UseBackgroundColor) lab->SetBackgroundColour(m_BackgroundColor);
	
	if (bold)
		lab->SetFont(m_BoldFont);
	else
		lab->SetFont(m_Font);

	lab->SetValidator(albaGUIValidator(this, -1, lab, var));
	
	Add(lab, 0, wxEXPAND | wxALL, M);
}
//----------------------------------------------------------------------------
void albaGUI::Label(albaString label1, albaString label2, bool bold_label, bool bold_var, double customSizer)
{
	int fw = FW;
	int lw = LW;
	int dw = -1;
	long labStyle = wxALIGN_RIGHT | wxST_NO_AUTORESIZE;

	if (customSizer <1.0 && customSizer > 0.0)
	{
		lw = (fw * customSizer) - (2 * LM);
		dw = (fw - lw) - LM;
		fw *= customSizer;
		labStyle = wxALIGN_LEFT | wxST_NO_AUTORESIZE;
	}

	wxStaticText* lab1 = new wxStaticText(this, -1, label1.GetCStr(), dp, wxSize(lw, LH), labStyle);
	wxStaticText* lab2 = new wxStaticText(this, -1, label2.GetCStr(), dp, wxSize(dw, LH), wxALIGN_LEFT);
	
	if (bold_label)
		lab1->SetFont(m_BoldFont);
	else
		lab1->SetFont(m_Font);
	
	if (bold_var)
		lab2->SetFont(m_BoldFont);
	else
		lab2->SetFont(m_Font);

	if (m_UseBackgroundColor) 
	{
		lab1->SetBackgroundColour(m_BackgroundColor);
		lab2->SetBackgroundColour(m_BackgroundColor);
	}

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab1, 0, wxRIGHT, LM);
	sizer->Add(lab2, 0, wxEXPAND);
	
	Add(sizer, 0, wxALL, M);
}
//----------------------------------------------------------------------------
void albaGUI::Label(albaString label1, albaString *var, bool bold_label, bool bold_var, bool multiline, double customSizer)
{
	int h = (multiline) ? -1 : LH;

	int fw = FW;
	int lw = LW;
	int dw = -1;
	long labStyle = wxALIGN_RIGHT | wxST_NO_AUTORESIZE;

	if (customSizer <1.0 && customSizer > 0.0)
	{
		lw = (fw * customSizer) - (2 * LM);
		dw = (fw - lw) - LM;
		fw *= customSizer;
		labStyle = wxALIGN_LEFT | wxST_NO_AUTORESIZE;
	}

	wxStaticText* lab1 = new wxStaticText(this, -1, label1.GetCStr(), dp, wxSize(lw, h), labStyle);
	wxStaticText* lab2 = new wxStaticText(this, -1, var->GetCStr(), dp, wxSize(dw, h), wxALIGN_LEFT);

	if (m_UseBackgroundColor)
	{
		lab1->SetBackgroundColour(m_BackgroundColor);
		lab2->SetBackgroundColour(m_BackgroundColor);
	}

	if (bold_label)
		lab1->SetFont(m_BoldFont);
	else
		lab1->SetFont(m_Font);

	if (bold_var)
		lab2->SetFont(m_BoldFont);
	else
		lab2->SetFont(m_Font);

	lab2->SetValidator(albaGUIValidator(this, -1, lab2, var));

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab1, 0, wxRIGHT, LM);
	sizer->Add(lab2, 0, wxEXPAND);

	Add(sizer, 0, wxALL, M);
}

//----------------------------------------------------------------------------
void albaGUI::HintBox(int id, wxString label, wxString title, int mode, bool showIcon)
{
	wxStaticBoxSizer *boxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, title);

	if (showIcon)
	{
		wxString iconName = "HINT_IMAGE_ICON";
		if (mode == 1) iconName = "INFO_IMAGE_ICON";
		if (mode == 2) iconName = "WARN_IMAGE_ICON";

		int w_id = GetWidgetId(id);
		wxBitmap bitmap = albaPictureFactory::GetPictureFactory()->GetBmp(iconName);
		wxBitmapButton *bmpButton = new wxBitmapButton(this, w_id, bitmap, wxPoint(0, 0), wxSize(19, 20));
		bmpButton->SetValidator(albaGUIValidator(this, w_id, bmpButton));

		boxSizer->GetStaticBox()->SetBackgroundColour(wxColor(255, 255, 224));
		boxSizer->Add(bmpButton);
		boxSizer->Add(new wxStaticText(this, NULL, " ")); // Separator
	}

	boxSizer->Add(new wxStaticText(this, NULL, label));

	Add(boxSizer, 0, wxALL, M);
}

//----------------------------------------------------------------------------
void albaGUI::Button(int id, albaString button_text, albaString label, albaString tooltip)
{
	if (label.IsEmpty())
	{
		int w_id = GetWidgetId(id);
		albaGUIButton *butt = new albaGUIButton(this, w_id, button_text.GetCStr(), dp, wxSize(FW, BH));
		butt->SetValidator(albaGUIValidator(this, w_id, butt));
		butt->SetFont(m_Font);
		if (!tooltip.IsEmpty())
			butt->SetToolTip(tooltip.GetCStr());

		Add(butt, 0, wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);
		int w_id = GetWidgetId(id);

		albaGUIButton *butt = new albaGUIButton(this, w_id, button_text.GetCStr(), dp, wxSize(DW, BH));
		butt->SetValidator(albaGUIValidator(this, w_id, butt));
		butt->SetFont(m_Font);
		if (!tooltip.IsEmpty())
			butt->SetToolTip(tooltip.GetCStr());

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxALIGN_CENTRE | wxRIGHT, LM);
		sizer->Add(butt, 0);
		Add(sizer, 0, wxALL, M);
	}
}
//----------------------------------------------------------------------------
void albaGUI::Button(int id, albaString *label, albaString button_text, albaString tooltip)
{
	int w_id = GetWidgetId(id);
	wxStaticText *lab = new wxStaticText(this, w_id, label->GetCStr(), dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	lab->SetValidator(albaGUIValidator(this, w_id, lab, label));
	lab->SetFont(m_Font);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);

	w_id = GetWidgetId(id);
	albaGUIButton    *butt = new albaGUIButton(this, w_id, button_text.GetCStr(), dp, wxSize(DW, BH));
	butt->SetValidator(albaGUIValidator(this, w_id, butt));
	butt->SetFont(m_Font);
	if (!tooltip.IsEmpty())
		butt->SetToolTip(tooltip.GetCStr());

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab, 0, wxALIGN_CENTRE | wxRIGHT, LM);
	sizer->Add(butt, 0);
	Add(sizer, 0, wxALL, M);
}

//----------------------------------------------------------------------------
albaGUICrossIncremental *albaGUI::CrossIncremental(int id, const char* label, double *stepVariable, double *topBottomVariable, double *leftRightVariable, int modality, wxString tooltip /* ="" */, bool boldLabel /* = true */, bool comboStep /* = false */, int digits /* = -1 */, albaString *buttonUpDown_text /* = NULL */, albaString *buttonLeftRight_text /* = NULL */)
{
	int width = (label == "") ? FW : DW;

	albaGUICrossIncremental *cI = new albaGUICrossIncremental(this, GetWidgetId(id), label, stepVariable, topBottomVariable, leftRightVariable, boldLabel, modality, dp, wxDefaultSize, MINFLOAT, MAXFLOAT, digits, wxTAB_TRAVERSAL | wxCLIP_CHILDREN, comboStep, buttonUpDown_text, buttonLeftRight_text);
	cI->SetListener(this);

	Add(cI, 0, wxALL, M);

	return cI;
}

// integer vector form 1
//----------------------------------------------------------------------------
void albaGUI::Vector(int id, wxString label, int var[3], int min, int max, wxString tooltip, wxColour *bg_colour)
{
	int w_id;
	wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);
	lab->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text1 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text1->SetValidator(albaGUIValidator(this, w_id, text1, var, min, max));
	text1->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text2 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text2->SetValidator(albaGUIValidator(this, w_id, text2, &(var[1]), min, max));
	text2->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text3 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text3->SetValidator(albaGUIValidator(this, w_id, text3, &(var[2]), min, max));
	text3->SetFont(m_Font);

	if (tooltip != "")
	{
		text1->SetToolTip(tooltip);
		text2->SetToolTip(tooltip);
		text3->SetToolTip(tooltip);
	}
	if (bg_colour != NULL)
	{
		text1->SetBackgroundColour(bg_colour[0]);
		text2->SetBackgroundColour(bg_colour[1]);
		text3->SetBackgroundColour(bg_colour[2]);
	}

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab, 0, wxRIGHT, LM);
	sizer->Add(text1, 0, wxRIGHT, HM);
	sizer->Add(text2, 0, wxRIGHT, HM);
	sizer->Add(text3, 0, wxRIGHT, HM);
	Add(sizer, 0, wxALL, M);
}
// integer vector form 2
//----------------------------------------------------------------------------
void albaGUI::Vector(int id, wxString label, int var[3], int minx, int maxx, int miny, int maxy, int minz, int maxz, wxString tooltip, wxColour *bg_colour)
{
	int w_id;
	wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text1 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text1->SetValidator(albaGUIValidator(this, w_id, text1, var, minx, maxx));
	text1->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text2 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text2->SetValidator(albaGUIValidator(this, w_id, text2, &(var[1]), miny, maxy));
	text2->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text3 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text3->SetValidator(albaGUIValidator(this, w_id, text3, &(var[2]), minz, maxz));
	text3->SetFont(m_Font);

	if (tooltip != "")
	{
		text1->SetToolTip(tooltip);
		text2->SetToolTip(tooltip);
		text3->SetToolTip(tooltip);
	}
	if (bg_colour != NULL)
	{
		text1->SetBackgroundColour(bg_colour[0]);
		text2->SetBackgroundColour(bg_colour[1]);
		text3->SetBackgroundColour(bg_colour[2]);
	}

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab, 0, wxRIGHT, LM);
	sizer->Add(text1, 0, wxRIGHT, HM);
	sizer->Add(text2, 0, wxRIGHT, HM);
	sizer->Add(text3, 0, wxRIGHT, HM);
	Add(sizer, 0, wxALL, M);
}
// float vector form 1
//----------------------------------------------------------------------------
void albaGUI::Vector(int id, wxString label, float var[3], float min, float max, int decimal_digit, wxString tooltip, wxColour *bg_colour) // <*> togliere la seconda forma
{
	int w_id;
	wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor) lab->SetBackgroundColour(m_BackgroundColor);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text1 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text1->SetValidator(albaGUIValidator(this, w_id, text1, var, min, max, decimal_digit));
	text1->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text2 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text2->SetValidator(albaGUIValidator(this, w_id, text2, &(var[1]), min, max, decimal_digit));
	text2->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text3 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text3->SetValidator(albaGUIValidator(this, w_id, text3, &(var[2]), min, max, decimal_digit));
	text3->SetFont(m_Font);

	if (tooltip != "")
	{
		text1->SetToolTip(tooltip);
		text2->SetToolTip(tooltip);
		text3->SetToolTip(tooltip);
	}
	if (bg_colour != NULL)
	{
		text1->SetBackgroundColour(bg_colour[0]);
		text2->SetBackgroundColour(bg_colour[1]);
		text3->SetBackgroundColour(bg_colour[2]);
	}

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab, 0, wxRIGHT, LM);
	sizer->Add(text1, 0, wxRIGHT, HM);
	sizer->Add(text2, 0, wxRIGHT, HM);
	sizer->Add(text3, 0, wxRIGHT, HM);
	Add(sizer, 0, wxALL, M);
}
// float vector form 2
//----------------------------------------------------------------------------
void albaGUI::Vector(int id, wxString label, float var[3], float minx, float maxx, float miny, float maxy, float minz, float maxz, int decimal_digit, wxString tooltip, wxColour *bg_colour)
{
	int w_id;
	wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);
	lab->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text1 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text1->SetValidator(albaGUIValidator(this, w_id, text1, var, minx, maxx, decimal_digit));
	text1->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text2 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text2->SetValidator(albaGUIValidator(this, w_id, text2, &(var[1]), miny, maxy, decimal_digit));
	text2->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text3 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text3->SetValidator(albaGUIValidator(this, w_id, text3, &(var[2]), minz, maxz, decimal_digit));
	text3->SetFont(m_Font);

	if (tooltip != "")
	{
		text1->SetToolTip(tooltip);
		text2->SetToolTip(tooltip);
		text3->SetToolTip(tooltip);
	}
	if (bg_colour != NULL)
	{
		text1->SetBackgroundColour(bg_colour[0]);
		text2->SetBackgroundColour(bg_colour[1]);
		text3->SetBackgroundColour(bg_colour[2]);
	}

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab, 0, wxRIGHT, LM);
	sizer->Add(text1, 0, wxRIGHT, HM);
	sizer->Add(text2, 0, wxRIGHT, HM);
	sizer->Add(text3, 0, wxRIGHT, HM);
	Add(sizer, 0, wxALL, M);
}
// double vector form 1
//----------------------------------------------------------------------------
void albaGUI::Vector(int id, wxString label, double var[3], double min, double max, int decimal_digit, wxString tooltip, wxColour *bg_colour)
{
	int w_id;
	wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);
	lab->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text1 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text1->SetValidator(albaGUIValidator(this, w_id, text1, var, min, max, decimal_digit));
	text1->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text2 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text2->SetValidator(albaGUIValidator(this, w_id, text2, &(var[1]), min, max, decimal_digit));
	text2->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text3 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text3->SetValidator(albaGUIValidator(this, w_id, text3, &(var[2]), min, max, decimal_digit));
	text3->SetFont(m_Font);

	if (tooltip != "")
	{
		text1->SetToolTip(tooltip);
		text2->SetToolTip(tooltip);
		text3->SetToolTip(tooltip);
	}
	if (bg_colour != NULL)
	{
		text1->SetBackgroundColour(bg_colour[0]);
		text2->SetBackgroundColour(bg_colour[1]);
		text3->SetBackgroundColour(bg_colour[2]);
	}

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab, 0, wxRIGHT, LM);
	sizer->Add(text1, 0, wxRIGHT, HM);
	sizer->Add(text2, 0, wxRIGHT, HM);
	sizer->Add(text3, 0, wxRIGHT, HM);
	Add(sizer, 0, wxALL, M);
}
// double vector form 2
//----------------------------------------------------------------------------
void albaGUI::Vector(int id, wxString label, double var[3], double minx, double maxx, double miny, double maxy, double minz, double maxz, int decimal_digit, wxString tooltip, wxColour *bg_colour)
{
	int w_id;
	wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);
	lab->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text1 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text1->SetValidator(albaGUIValidator(this, w_id, text1, var, minx, maxx, decimal_digit));
	text1->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text2 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text2->SetValidator(albaGUIValidator(this, w_id, text2, &(var[1]), miny, maxy, decimal_digit));
	text2->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text3 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text3->SetValidator(albaGUIValidator(this, w_id, text3, &(var[2]), minz, maxz, decimal_digit));
	text3->SetFont(m_Font);

	if (tooltip != "")
	{
		text1->SetToolTip(tooltip);
		text2->SetToolTip(tooltip);
		text3->SetToolTip(tooltip);
	}
	if (bg_colour != NULL)
	{
		text1->SetBackgroundColour(bg_colour[0]);
		text2->SetBackgroundColour(bg_colour[1]);
		text3->SetBackgroundColour(bg_colour[2]);
	}

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab, 0, wxRIGHT, LM);
	sizer->Add(text1, 0, wxRIGHT, HM);
	sizer->Add(text2, 0, wxRIGHT, HM);
	sizer->Add(text3, 0, wxRIGHT, HM);
	Add(sizer, 0, wxALL, M);
}

//----------------------------------------------------------------------------
void albaGUI::String(int id, wxString label, wxString* var, wxString tooltip, bool multiline, bool password, bool interactive, double customSizer)
{
	int lh = LH;
	int fw = FW;
	int lw = LW;
	int dw = DW;

	long e_style = m_EntryStyle;
	if (multiline)
	{
		lh *= 5;
		e_style |= wxTE_MULTILINE | wxTE_WORDWRAP;
	}
	if (password)
		e_style |= wxTE_PASSWORD;

	long labStyle = wxALIGN_RIGHT | wxST_NO_AUTORESIZE;

	if (customSizer <1.0 && customSizer > 0.0)
	{
		lw = (fw * customSizer) - (2 * LM);
		dw = (fw - lw) - LM;
		fw *= customSizer;
		labStyle = wxALIGN_LEFT | wxST_NO_AUTORESIZE;
	}

	if (label == "")
	{
		int w_id = GetWidgetId(id);
		wxTextCtrl *text = NULL;

		text = new wxTextCtrl(this, w_id, "", dp, wxSize(fw, lh), e_style);
		text->SetValidator(albaGUIValidator(this, w_id, text, var,interactive,multiline));
		text->SetFont(m_Font);

		if (tooltip != "")
			text->SetToolTip(tooltip);
		Add(text, 0, wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(lw, lh), labStyle);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);

		int w_id = GetWidgetId(id);
		wxTextCtrl *text = NULL;
		text = new wxTextCtrl(this, w_id, "", dp, wxSize(dw, lh), e_style);
		text->SetValidator(albaGUIValidator(this, w_id, text, var, interactive,multiline));
		text->SetFont(m_Font);

		if (tooltip != "")
			text->SetToolTip(tooltip);

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(text, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}
}
//----------------------------------------------------------------------------
void albaGUI::String(int id, albaString label, albaString *var, albaString tooltip, bool multiline, bool password, bool interactive, double customSizer)
{
	int lh = LH;
	int fw = FW;
	int lw = LW;
	int dw = DW;

	long e_style = m_EntryStyle;

	if (multiline)
	{
		lh *= 5;
		e_style |= wxTE_MULTILINE | wxTE_WORDWRAP;
	}
	
	if (password)
		e_style |= wxTE_PASSWORD;

	long labStyle = wxALIGN_RIGHT | wxST_NO_AUTORESIZE;

	if (customSizer <1.0 && customSizer > 0.0)
	{
		lw = (fw * customSizer) - (2 * LM);
		dw = (fw - lw) - LM;
		fw *= customSizer;
		labStyle = wxALIGN_LEFT | wxST_NO_AUTORESIZE;
	}

	if (label.IsEmpty())
	{
		int w_id = GetWidgetId(id);
		wxTextCtrl  *text = NULL;
		text = new wxTextCtrl(this, w_id, "", dp, wxSize(FW, lh), e_style);
		text->SetValidator(albaGUIValidator(this, w_id, text, var, interactive, multiline));
		text->SetFont(m_Font);

		if (!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());

		Add(text, 0, wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(lw, lh), labStyle);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);

		int w_id = GetWidgetId(id);
		wxTextCtrl  *text = NULL;
		text = new wxTextCtrl(this, w_id, "", dp, wxSize(dw, lh), e_style);
		text->SetValidator(albaGUIValidator(this, w_id, text, var,interactive,multiline));
		text->SetFont(m_Font);

		if (!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(text, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}
}
//----------------------------------------------------------------------------
void albaGUI::Integer(int id,albaString label,int* var,int min, int max, albaString tooltip, bool labelAlwaysEnable, double customSizer)
{
	int w_id;
	int fw = FW;
	int lw = LW;
	int dw = DW;
	long labStyle = wxALIGN_RIGHT | wxST_NO_AUTORESIZE;

	if (customSizer < 1.0 && customSizer > 0.0)
	{
		lw = (fw * customSizer) - (2 * LM);
		dw = (fw - lw) - LM;
		fw *= customSizer;

		labStyle = wxALIGN_LEFT | wxST_NO_AUTORESIZE;
	}

	if (label.IsEmpty())
	{
		w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl(this, w_id, "", dp, wxSize(fw, LH), m_EntryStyle);
		text->SetValidator(albaGUIValidator(this, w_id, text, var, min, max));
		text->SetFont(m_Font);
		if (!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());
		Add(text, 0, wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, labelAlwaysEnable ? -1 : GetWidgetId(id), label.GetCStr(), dp, wxSize(lw, LH), labStyle);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);

		w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl(this, w_id, "", dp, wxSize(dw, LH), m_EntryStyle);
		text->SetValidator(albaGUIValidator(this, w_id, text, var, min, max));
		text->SetFont(m_Font);
		if (!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(text, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}
}
//----------------------------------------------------------------------------
void albaGUI::Float(int id, albaString label, float* var, float min, float max, int flag, int decimal_digit, albaString tooltip, double customSizer)
{
	int w_id;
	int fw = FW;
	int lw = LW;
	int dw = DW;
	long labStyle = wxALIGN_RIGHT | wxST_NO_AUTORESIZE;

	if (customSizer < 1.0 && customSizer > 0.0)
	{
		lw = (fw * customSizer) - (2 * LM);
		dw = (fw - lw) - LM;
		fw *= customSizer;

		labStyle = wxALIGN_LEFT | wxST_NO_AUTORESIZE;
	}

	if (label.IsEmpty())
	{
		w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl(this, w_id, "", dp, wxSize(fw, LH), m_EntryStyle);
		text->SetValidator(albaGUIValidator(this, w_id, text, var, min, max, decimal_digit));
		text->SetFont(m_Font);

		if (!tooltip.IsEmpty()) text->SetToolTip(tooltip.GetCStr());
		Add(text, 0, wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(lw, LH), labStyle);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);

		w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl(this, w_id, "", dp, wxSize(dw, LH), m_EntryStyle);
		text->SetValidator(albaGUIValidator(this, w_id, text, var, min, max, decimal_digit));
		text->SetFont(m_Font);

		if (!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(text, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}
}
//----------------------------------------------------------------------------
void albaGUI::Double(int id, albaString label, double* var, double min, double max, int decimal_digit, albaString tooltip, bool labelAlwaysEnable, double customSizer, wxColour fontColor)
{
	int w_id;
	int fw = FW;
	int lw = LW;
	int dw = DW;
	long labStyle = wxALIGN_RIGHT | wxST_NO_AUTORESIZE;

	if (customSizer <1.0 && customSizer > 0.0)
	{
		lw = (fw * customSizer) - (2 * LM);
		dw = (fw - lw) - LM;
		fw *= customSizer;
		labStyle = wxALIGN_LEFT | wxST_NO_AUTORESIZE;
	}

	if (label.IsEmpty())
	{
		w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl(this, w_id, "", dp, wxSize(lw, LH), m_EntryStyle);
		text->SetValidator(albaGUIValidator(this, w_id, text, var, min, max, decimal_digit));
		text->SetFont(m_Font);
		text->SetForegroundColour(fontColor);
		if (!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());
		Add(text, 0, wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, labelAlwaysEnable ? -1 : GetWidgetId(id), label.GetCStr(), dp, wxSize(lw, LH), labStyle);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetForegroundColour(fontColor);
		lab->SetFont(m_Font);
		
		w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl(this, w_id, "", dp, wxSize(dw, LH), m_EntryStyle);
		text->SetValidator(albaGUIValidator(this, w_id, text, var, min, max, decimal_digit));
		text->SetFont(m_Font);
		text->SetForegroundColour(fontColor);
		if (!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(text, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}
}
//----------------------------------------------------------------------------
void albaGUI::Bool(int id, albaString label, int* var, int flag, albaString tooltip)   
{
	// <*> non mi piace il flag - trovare una sintassi piu comprensibile
	// <*> ci vorrebbe anche il layout check al centro, label a destra

	int w_id;
	if (flag != 0) // text on left
	{
		w_id = GetWidgetId(id);
		wxCheckBox *check = new wxCheckBox(this, w_id, label.GetCStr(), dp, wxSize(-1, BH), 0);
		check->SetValidator(albaGUIValidator(this, w_id, check, var));
		check->SetFont(m_Font);
		if (m_UseBackgroundColor)
			check->SetBackgroundColour(m_BackgroundColor);
		if (!tooltip.IsEmpty())
			check->SetToolTip(tooltip.GetCStr());
		Add(check, 0, wxALL, M);
	}
	else      // text on right
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);

		w_id = GetWidgetId(id);
		wxCheckBox *check = new wxCheckBox(this, w_id, "", dp, wxSize(DW, LH), m_EntryStyle);
		check->SetValidator(albaGUIValidator(this, w_id, check, var));
		check->SetFont(m_Font);
		if (m_UseBackgroundColor)
			check->SetBackgroundColour(m_BackgroundColor);
		if (!tooltip.IsEmpty())
			check->SetToolTip(tooltip.GetCStr());

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(check, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}
}

//----------------------------------------------------------------------------
wxSlider *albaGUI::Slider(int id, wxString label, int* var, int min, int max, wxString tooltip, bool showText)
{
	wxTextCtrl   *text = NULL;
	wxSlider     *sli = NULL;
	wxStaticText *lab = NULL;
	int w_id_text;
	int w_id_sli;

	if (label == "" && showText)
	{
		int text_w = EW*0.8;
		int slider_w = FW - text_w;
		w_id_text = GetWidgetId(id);
		text = new wxTextCtrl(this, w_id_text, "", dp, wxSize(text_w, LH), m_EntryStyle);
		text->SetFont(m_Font);

		w_id_sli = GetWidgetId(id);
		sli = new wxSlider(this, w_id_sli, min, min, max, dp, wxSize(slider_w, LH));
		if (m_UseBackgroundColor)
			sli->SetBackgroundColour(m_BackgroundColor);

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(text, 0);
		sizer->Add(sli, 0);
		Add(sizer, 0, wxALL, M);
	}
	else if (showText)
	{
		int text_w = EW*0.8;
		int slider_w = DW - text_w;
		lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);

		w_id_text = GetWidgetId(id);
		text = new wxTextCtrl(this, w_id_text, "", dp, wxSize(text_w, LH), m_EntryStyle);
		text->SetFont(m_Font);

		w_id_sli = GetWidgetId(id);
		sli = new wxSlider(this, w_id_sli, min, min, max, dp, wxSize(slider_w, LH));
		if (m_UseBackgroundColor)
			sli->SetBackgroundColour(m_BackgroundColor);

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(text, 0);
		sizer->Add(sli, 0);
		Add(sizer, 0, wxALL, M);
	}
	else if (!showText)
	{
		int text_w = EW*0.8;
		int slider_w = FW;
		w_id_text = GetWidgetId(id);
		text = new wxTextCtrl(this, w_id_text, "", dp, wxSize(text_w, LH), m_EntryStyle);
		text->SetFont(m_Font);

		w_id_sli = GetWidgetId(id);
		sli = new wxSlider(this, w_id_sli, min, min, max, dp, wxSize(slider_w, LH));
		if (m_UseBackgroundColor)
			sli->SetBackgroundColour(m_BackgroundColor);

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(text, 0);
		sizer->Add(sli, 0);
		Add(sizer, 0, wxALL, M);
		text->Show(showText);
	}
	sli->SetValidator(albaGUIValidator(this, w_id_sli, sli, var, text));
	text->SetValidator(albaGUIValidator(this, w_id_text, text, var, sli, min, max)); //- if uncommented, remove also wxTE_READONLY from the text (in both places)
	if (tooltip != "")	text->SetToolTip(tooltip);

	return sli;
}
//----------------------------------------------------------------------------
albaGUIFloatSlider *albaGUI::FloatSlider(int id, wxString label, double *var, double min, double max, wxSize size, wxString tooltip, bool textBoxEnable)
{
	//<*> Si puo Chiamare Slider lo stesso 
	//<*> verificare se le entry erano abilitate o no

	wxTextCtrl     *text = NULL;
	albaGUIFloatSlider *sli = NULL;
	wxStaticText   *lab = NULL;
	int w_id_text;
	int w_id_sli;
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	if (label == "")
	{
		int text_w = EW * 0.8;
		int slider_w = FW - text_w;
		w_id_text = GetWidgetId(id);
		text = new wxTextCtrl(this, w_id_text, "", dp, wxSize(text_w, LH), m_EntryStyle);
		text->SetFont(m_Font);
		text->Enable(textBoxEnable);

		w_id_sli = GetWidgetId(id);
		sli = new albaGUIFloatSlider(this, w_id_sli, *var, min, max, dp, size);
		if (m_UseBackgroundColor)
			sli->SetBackgroundColour(m_BackgroundColor);
		text->SetValidator(albaGUIValidator(this, w_id_text, text, var, sli, min, max));
		sli->SetValidator(albaGUIValidator(this, w_id_sli, sli, var, text));
		sizer->Add(text, 0);
		sizer->Add(sli, 0);
	}
	else
	{
		int text_w = EW * 0.8;
		int slider_w = DW - text_w;
		lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);

		w_id_text = GetWidgetId(id);
		text = new wxTextCtrl(this, w_id_text, "", dp, wxSize(text_w, LH), m_EntryStyle);
		text->SetFont(m_Font);
		text->Enable(textBoxEnable);
		w_id_sli = GetWidgetId(id);
		sli = new albaGUIFloatSlider(this, w_id_sli, *var, min, max, dp, size);
		if (m_UseBackgroundColor)
			sli->SetBackgroundColour(m_BackgroundColor);

		text->SetValidator(albaGUIValidator(this, w_id_text, text, var, sli, min, max));
		sli->SetValidator(albaGUIValidator(this, w_id_sli, sli, var, text));
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(text, 0);
		sizer->Add(sli, 0);
	}

	Add(sizer, 0, wxALL, M);
	if (tooltip != "")
		text->SetToolTip(tooltip);
	return sli;
}
//----------------------------------------------------------------------------
albaGUIFloatSlider *albaGUI::FloatExpandedSlider(int id, wxString label, double *var, double min, double max, wxSize size, wxString tooltip, bool textBoxEnable)
{
	//<*> Si puo Chiamare Slider lo stesso 

	wxTextCtrl     *text = NULL;
	albaGUIFloatSlider *sli = NULL;
	wxStaticText   *lab = NULL;

	int w_id;
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

	if (label == "")
	{
		int text_w = EW * 0.8;
		int slider_w = FW - text_w;
		w_id = GetWidgetId(id);
		text = new wxTextCtrl(this, w_id, "", dp, wxSize(text_w, LH), m_EntryStyle);
		text->SetFont(m_Font);
		text->Enable(textBoxEnable);

		sli = new albaGUIFloatSlider(this, w_id, *var, min, max, dp, size);
		if (m_UseBackgroundColor)
			sli->SetBackgroundColour(m_BackgroundColor);
		text->SetValidator(albaGUIValidator(this, w_id, text, var, sli, min, max));
		sli->SetValidator(albaGUIValidator(this, w_id, sli, var, text));
		sizer->Add(text, 0);
		sizer->Add(sli, wxEXPAND);
	}
	else
	{
		int text_w = EW * 0.8;
		int slider_w = DW - text_w;
		lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);

		w_id = GetWidgetId(id);
		text = new wxTextCtrl(this, w_id, "", dp, wxSize(text_w, LH), m_EntryStyle);
		text->SetFont(m_Font);
		text->Enable(textBoxEnable);

		sli = new albaGUIFloatSlider(this, w_id, *var, min, max, dp, size);
		if (m_UseBackgroundColor)
			sli->SetBackgroundColour(m_BackgroundColor);

		text->SetValidator(albaGUIValidator(this, w_id, text, var, sli, min, max));
		sli->SetValidator(albaGUIValidator(this, w_id, sli, var, text));
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(text, 0);
		sizer->Add(sli, wxEXPAND);
	}

	Add(sizer, 0, wxEXPAND, M);

	if (tooltip != "")
		text->SetToolTip(tooltip);

	return sli;
}
//----------------------------------------------------------------------------
albaGUIFloatSlider *albaGUI::FloatSlider(int id, double *var, double min, double max, wxString minLab, wxString maxLab, wxSize size, wxString tooltip, bool textBoxEnable) //<*> Si puo Chiamare Slider lo stesso 
//----------------------------------------------------------------------------                                            //<*> verificare se le entry erano abilitate o no
{
	wxTextCtrl     *text = NULL;
	albaGUIFloatSlider *sli = NULL;
	wxStaticText   *minText = NULL;
	wxStaticText   *maxText = NULL;
	int w_id_sli;
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

	int text_w = 0.1;
	int slider_w = FW - EW * 2;
	minText = new wxStaticText(this, GetWidgetId(id), minLab, dp, wxSize(EW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		minText->SetBackgroundColour(m_BackgroundColor);
	minText->SetFont(m_Font);

	maxText = new wxStaticText(this, GetWidgetId(id), maxLab, dp, wxSize(EW, LH), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		maxText->SetBackgroundColour(m_BackgroundColor);
	maxText->SetFont(m_Font);

	w_id_sli = GetWidgetId(id);
	sli = new albaGUIFloatSlider(this, w_id_sli, *var, min, max, dp, size);
	if (m_UseBackgroundColor)
		sli->SetBackgroundColour(m_BackgroundColor);

	int w_id_text = GetWidgetId(id);
	text = new wxTextCtrl(this, w_id_text, "", dp, wxSize(text_w, LH), m_EntryStyle);
	text->SetFont(m_Font);
	text->Enable(textBoxEnable);

	text->SetValidator(albaGUIValidator(this, w_id_text, text, var, sli, min, max));
	sli->SetValidator(albaGUIValidator(this, w_id_sli, sli, var, text));

	sizer->Add(minText, 0, wxRIGHT, LM);
	sizer->Add(sli, 0, 0, LM);
	sizer->Add(maxText, 0);

	Add(sizer, 0, wxALL, M);

	return sli;
}

//----------------------------------------------------------------------------
void albaGUI::Radio(int id, wxString label, int* var, int numchoices, const wxString choices[], int dim, wxString tooltip, int style)
{
	// SIL: 24/04/03
	// wxWindows assign by itself the RadioButtons id's in a way 
	// that it break the correct tab-ordering - and there's is no workaround
	// workaround: - use combo instead
	// SIL. 23-may-2006 : -- fixed with wxWidgets 2.6.3

	wxRadioBox *radio = NULL;
	int w_id;

	if (!label.IsEmpty())
	{
		wxBoxSizer *sizer = NULL;
		sizer = new wxBoxSizer(wxHORIZONTAL);
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, -1), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor) lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);

		w_id = GetWidgetId(id);
		radio = new wxRadioBox(this, w_id, "", dp, wxSize(DW, -1), numchoices, choices, dim, style | wxBORDER_NONE | wxTAB_TRAVERSAL);
	
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(radio, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}
	else
	{
		w_id = GetWidgetId(id);
		radio = new wxRadioBox(this, w_id, "", dp, wxSize(FW, -1), numchoices, choices, dim, style | m_EntryStyle | wxTAB_TRAVERSAL);
		Add(radio, 0, wxRIGHT, HM);
	}

	if (m_UseBackgroundColor)
		radio->SetBackgroundColour(m_BackgroundColor);
	radio->SetValidator(albaGUIValidator(this, w_id, radio, var));
	radio->SetFont(m_Font);
	if (tooltip != "")
		radio->SetToolTip(tooltip);
}

//----------------------------------------------------------------------------
void albaGUI::RadioButton(int id, wxString label, int selected, wxString tooltip /*= ""*/)
{
	wxRadioButton *radioButton = NULL;
	int w_id = GetWidgetId(id);
	radioButton = new wxRadioButton(this, w_id, label);

	radioButton->SetValue(selected);
	
	this->Add(radioButton);
 	if (tooltip != "")
 		radioButton->SetToolTip(tooltip);
}

//----------------------------------------------------------------------------
wxComboBox *albaGUI::Combo(int id, albaString label, int* var, int numchoices, const wxString choices[], albaString tooltip, double customSizer)
{
	wxComboBox *combo = NULL;
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	int w_id;

	int fw = FW;
	int lw = LW;
	int dw = DW;
	long labStyle = wxALIGN_RIGHT | wxST_NO_AUTORESIZE;

	if (customSizer < 1.0 && customSizer > 0.0)
	{
		lw = (fw * customSizer) - (2 * LM);
		dw = (fw - lw) - LM;
		fw *= customSizer;
		labStyle = wxALIGN_LEFT | wxST_NO_AUTORESIZE;
	}

	if (!label.IsEmpty())
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(lw, -1), labStyle);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);

		w_id = GetWidgetId(id);
		combo = new wxComboBox(this, w_id, "", dp, wxSize(dw, -1), numchoices, choices, wxCB_READONLY);
		combo->SetFont(m_Font);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(combo, 0, wxRIGHT, HM);
	}
	else
	{
		w_id = GetWidgetId(id);
		combo = new wxComboBox(this, w_id, "", dp, wxSize(fw, -1), numchoices, choices, wxCB_READONLY);
		combo->SetFont(m_Font);
		sizer->Add(combo, 0, wxRIGHT, HM);
	}

	combo->SetValidator(albaGUIValidator(this, w_id, combo, var));
	if (!tooltip.IsEmpty())
		combo->SetToolTip(tooltip.GetCStr());

	Add(sizer, 0, wxALL, M);
	return combo;
}

//----------------------------------------------------------------------------
void albaGUI::FileOpen(int id, albaString label, albaString* var, const albaString wildcard, albaString tooltip)
{
	int text_w = EW + HM + EW;
	int butt_w;
	wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW, BH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);
	lab->SetFont(m_Font);
	int w_id = GetWidgetId(id);
	if (label.IsEmpty())
	{
		butt_w = FW - text_w - HM;
	}
	else
	{
		butt_w = EW;
	}
	albaGUIButton  *butt = new albaGUIButton(this, w_id, "Open", dp, wxSize(butt_w, BH));
	wxTextCtrl *text = new wxTextCtrl(this, GetWidgetId(id), "", dp, wxSize(text_w, BH), wxTE_READONLY | m_EntryStyle);
	text->SetFont(m_Font);
	butt->SetValidator(albaGUIValidator(this, w_id, butt, var, text, true, wildcard));
	butt->SetFont(m_Font);

	if (!tooltip.IsEmpty())
	{
		text->SetToolTip(tooltip.GetCStr());
		butt->SetToolTip(tooltip.GetCStr());
	}
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

	if (!label.IsEmpty())
	{
		sizer->Add(lab, 0, wxRIGHT, LM);
	}

	sizer->Add(butt, 0, wxRIGHT, HM);
	sizer->Add(text, 0);
	Add(sizer, 0, wxALL, M);
}
//----------------------------------------------------------------------------
void albaGUI::DirOpen(int id, albaString label, albaString *var, albaString tooltip)
{
	int text_w = EW + HM + EW;
	int butt_w;
	wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW, BH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);
	lab->SetFont(m_Font);
	int w_id = GetWidgetId(id);
	if (label.IsEmpty())
	{
		butt_w = FW - text_w - HM;
	}
	else
	{
		butt_w = EW;
	}
	albaGUIButton    *butt = new albaGUIButton(this, w_id, "Browse", dp, wxSize(butt_w, BH));
	wxTextCtrl   *text = new wxTextCtrl(this, GetWidgetId(id), "", dp, wxSize(text_w, BH), wxTE_READONLY | m_EntryStyle);
	text->SetFont(m_Font);
	butt->SetValidator(albaGUIValidator(this, w_id, butt, var, text));
	butt->SetFont(m_Font);
	if (!tooltip.IsEmpty())
	{
		text->SetToolTip(tooltip.GetCStr());
		butt->SetToolTip(tooltip.GetCStr());
	}
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	if (!label.IsEmpty())
	{
		sizer->Add(lab, 0, wxRIGHT, LM);
	}
	sizer->Add(butt, 0, wxRIGHT, HM);
	sizer->Add(text, 0);
	Add(sizer, 0, wxALL, M);
}
//----------------------------------------------------------------------------
void albaGUI::FileSave(int id, albaString label, albaString* var, const albaString wildcard, albaString tooltip, bool enableTextCtrl)
{
	int text_w = EW + HM + EW;
	int butt_w;

	wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW, BH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);
	lab->SetFont(m_Font);
	int w_id = GetWidgetId(id);
	if (label.IsEmpty())
	{
		butt_w = FW - text_w - HM;
	}
	else
	{
		butt_w = EW;
	}
	albaGUIButton *butt = new albaGUIButton(this, w_id, "save", dp, wxSize(butt_w, BH));
	wxTextCtrl *text = new wxTextCtrl(this, GetWidgetId(id), "", dp, wxSize(text_w, BH), wxTE_READONLY | m_EntryStyle);
	text->SetFont(m_Font);
	butt->SetValidator(albaGUIValidator(this, w_id, butt, var, text, false, wildcard));
	butt->SetFont(m_Font);

	if (!tooltip.IsEmpty())
	{
		text->SetToolTip(tooltip.GetCStr());
		butt->SetToolTip(tooltip.GetCStr());
	}

	text->Enable(enableTextCtrl);
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	if (!label.IsEmpty())
	{
		sizer->Add(lab, 0, wxRIGHT, LM);
	}

	sizer->Add(butt, 0, wxRIGHT, HM);
	sizer->Add(text, 0);
	Add(sizer, 0, wxALL, M);
}

//----------------------------------------------------------------------------
void albaGUI::Color(int id, wxString label, wxColour* var, wxString tooltip)
{
	wxStaticText	*lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);
	lab->SetFont(m_Font);

	wxTextCtrl		*text = new wxTextCtrl(this, GetWidgetId(id), "", dp, wxSize(EW, LH), wxTE_READONLY | m_EntryStyle);
	text->SetFont(m_Font);

	int w_id = GetWidgetId(id);
	albaGUIButton *butt = new albaGUIButton(this, w_id, "...", dp, wxSize(BH, BH));
	butt->SetValidator(albaGUIValidator(this, w_id, butt, var, text));
	butt->SetFont(m_Font);
	if (tooltip != "")
		butt->SetToolTip(tooltip);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab, 0, wxRIGHT, LM);
	sizer->Add(text, 0, wxRIGHT, HM);
	sizer->Add(butt, 0);
	Add(sizer, 0, wxALL, M);
}

//----------------------------------------------------------------------------
albaGUILutSwatch *albaGUI::Lut(int id, wxString label, vtkLookupTable *lut)
{
	wxStaticText	*lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);
	lab->SetFont(m_Font);

	albaGUILutSwatch *luts = new albaGUILutSwatch(this, GetWidgetId(id), dp, wxSize(DW, 18), wxTAB_TRAVERSAL | wxSIMPLE_BORDER);
	luts->SetLut(lut);
	luts->SetEditable(true);
	luts->SetListener(this);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab, 0, wxRIGHT, LM);
	sizer->Add(luts, 0, wxEXPAND, HM);
	Add(sizer, 0, wxALL, M);
	return luts;
}

//----------------------------------------------------------------------------
void albaGUI::TwoButtons(int firstID, int secondID, const char* label1, const char* label2, int alignment /* = wxALL */, int width)
{
	int w = width != -1 ? width : FW / 2;
	int w_idFirst = GetWidgetId(firstID);
	int w_idSecond = GetWidgetId(secondID);

	albaGUIButton *b1 = new albaGUIButton(this, w_idFirst, label1, dp, wxSize(w, BH));
	b1->SetValidator(albaGUIValidator(this, w_idFirst, b1));
	b1->SetFont(m_Font);

	albaGUIButton *b2 = new albaGUIButton(this, w_idSecond, label2, dp, wxSize(w, BH));
	b2->SetValidator(albaGUIValidator(this, w_idSecond, b2));
	b2->SetFont(m_Font);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(b1, 0);
	sizer->Add(b2, 0);
	Add(sizer, 0, wxALL | alignment, M);
}

//----------------------------------------------------------------------------
void albaGUI::DoubleUpDown(int labelID, int firstID, int secondID, albaString label, double *var, double min /*= MINDOUBLE*/, double max /*= MAXDOUBLE*/, int decimal_digit /*= -1*/, albaString tooltip /*= ""*/, bool labelAlwaysEnable /*= false*/)
{
	// Double entry
	int w_id;
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	int btnW = LH;

	if (label.IsEmpty())
	{
		w_id = GetWidgetId(labelID);
		wxTextCtrl  *text = new wxTextCtrl(this, w_id, "", dp, wxSize(FW - (2 * btnW) - HM, BH), m_EntryStyle);
		text->SetValidator(albaGUIValidator(this, w_id, text, var, min, max, decimal_digit));
		text->SetFont(m_Font);
		if (!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());

		sizer->Add(text, 0, wxRIGHT, HM);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, labelAlwaysEnable ? -1 : GetWidgetId(labelID), label.GetCStr(), dp, wxSize(LW, BH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);

		w_id = GetWidgetId(labelID);
		wxTextCtrl  *text = new wxTextCtrl(this, w_id, "", dp, wxSize(DW - (2 * btnW) - HM, BH), m_EntryStyle);
		text->SetValidator(albaGUIValidator(this, w_id, text, var, min, max, decimal_digit));
		text->SetFont(m_Font);
		if (!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());

		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(text, 0, wxRIGHT, HM);
	}

	// Two Buttons
	int w_idFirst = GetWidgetId(firstID);
	albaGUIButton    *b1 = new albaGUIButton(this, w_idFirst, "-", dp, wxSize(btnW, BH));
	b1->SetValidator(albaGUIValidator(this, w_idFirst, b1));
	b1->SetFont(m_Font);

	int w_idSecond = GetWidgetId(secondID);
	albaGUIButton    *b2 = new albaGUIButton(this, w_idSecond, "+", dp, wxSize(btnW, BH));
	b2->SetValidator(albaGUIValidator(this, w_idSecond, b2));
	b2->SetFont(m_Font);
	
	sizer->Add(b1, 0, wxRIGHT, 0);
	sizer->Add(b2, 0, wxRIGHT, 0);

	Add(sizer, 0, wxALL, M);
}

//----------------------------------------------------------------------------
void albaGUI::ImageButton(int id, const char* label, wxBitmap bitmap, albaString tooltip)
{
	int w_id = GetWidgetId(id);

	int offset = 4;
	wxSize size(bitmap.GetWidth() + offset, bitmap.GetHeight() + offset); //wxSize(DW, BH));

	albaGUIPicButton *btn = new albaGUIPicButton();

	btn->Create(this, w_id, bitmap, wxDefaultPosition, size);
	btn->SetBitmapLabel(bitmap);
	btn->SetFont(m_Font);
	btn->Refresh();
	btn->SetValidator(albaGUIValidator(this, w_id, btn));

	if (!tooltip.IsEmpty())
		btn->SetToolTip(tooltip.GetCStr());

	wxStaticText *lab = new wxStaticText(this, w_id, label, dp, wxSize(DW, LH), wxALIGN_LEFT | wxST_NO_AUTORESIZE);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(btn, 0);
	sizer->Add(lab, 0);
	
	Add(sizer, 0, wxALL | wxEXPAND, M);
}

//----------------------------------------------------------------------------
void albaGUI::ButtonAndHelp(int firstID, int secondID, const char* label1, albaString tooltip, int alignment /* = wxALL */, int width)
{
	int w = width != -1 ? width : FW - 20;
	int w_idFirst = GetWidgetId(firstID);
	int w_idSecond = GetWidgetId(secondID);

	albaGUIButton *b1 = new albaGUIButton(this, w_idFirst, label1, dp, wxSize(w, BH));
	b1->SetValidator(albaGUIValidator(this, w_idFirst, b1));
	b1->SetFont(m_Font);
	if (!tooltip.IsEmpty())
		b1->SetToolTip(tooltip.GetCStr());

	albaGUIPicButton *b2 = new albaGUIPicButton(this, "BUTTON_HELP", secondID, this, 4);
	b2->SetValidator(albaGUIValidator(this, w_idSecond, b2));
	b2->SetFont(m_Font);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(b1, 0);
	sizer->Add(b2, 0);
	Add(sizer, 0, wxALL | alignment, M);
}
//----------------------------------------------------------------------------
void albaGUI::MultipleButtons(int numButtons, int numColumns, std::vector<int> &ids, std::vector<const char*> &labels, int alignment)
{
	std::vector<int> w_ids;
	std::vector<albaGUIButton *> button_list;
	for (int i = 0; i < ids.size(); i++)
	{
		w_ids.push_back(GetWidgetId(ids[i]));
		button_list.push_back(new albaGUIButton(this, w_ids[i], labels[i], dp, wxSize(FW / numColumns, BH)));
		button_list[i]->SetValidator(albaGUIValidator(this, w_ids[i], button_list[i]));
		button_list[i]->SetFont(m_Font);
	}

	int rows = numButtons / numColumns;
	wxFlexGridSizer *fgSizer = new wxFlexGridSizer(rows, numColumns, 1, 1);
	for (int i = 0; i < button_list.size(); i++)
	{
		fgSizer->Add(button_list[i], 0, 0);
	}

	Add(fgSizer, 0, wxALL | alignment, M);
}
//----------------------------------------------------------------------------
void albaGUI::MultipleImageButtons(int numButtons, int numColumns, std::vector<int> &ids, std::vector<const char*> &labels, std::vector<const char*> &images, int alignment)
{
	std::vector<int> w_ids;
	std::vector<albaGUIPicButton *> button_list;
	for (int i = 0; i < ids.size(); i++)
	{
		w_ids.push_back(GetWidgetId(ids[i]));

		albaGUIPicButton *btn = new albaGUIPicButton();
		wxBitmap bitmap = albaPictureFactory::GetPictureFactory()->GetBmp(images[i]);

		btn->Create(this, w_ids[i], bitmap, wxDefaultPosition, wxSize(FW / numColumns, bitmap.GetHeight()));
		btn->SetBitmapLabel(bitmap);
		btn->SetFont(m_Font);
		btn->Refresh();
		btn->SetValidator(albaGUIValidator(this, w_ids[i], btn));

		wxString tooltip = labels[i];

		if (!tooltip.IsEmpty())
			btn->SetToolTip(tooltip);

		button_list.push_back(btn);
	}

	int rows = numButtons / numColumns;
	wxFlexGridSizer *fgSizer = new wxFlexGridSizer(rows, numColumns, 1, 1);
	for (int i = 0; i < button_list.size(); i++)
	{
		fgSizer->Add(button_list[i], 0, 0);
	}

	Add(fgSizer, 0, wxALL | alignment, M);
}
//----------------------------------------------------------------------------
void albaGUI::OkCancel()
{
	albaGUIButton *b1 = new albaGUIButton(this, wxOK, "Ok", dp, wxSize(FW / 2, BH));
	b1->SetValidator(albaGUIValidator(this, wxOK, b1));
	b1->SetFont(m_Font);

	albaGUIButton *b2 = new albaGUIButton(this, wxCANCEL, "Cancel", dp, wxSize(FW / 2, BH));
	b2->SetValidator(albaGUIValidator(this, wxCANCEL, b2));
	b2->SetFont(m_Font);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(b1, 0);
	sizer->Add(b2, 0);
	Add(sizer, 0, wxALL, M);
}

//----------------------------------------------------------------------------
wxListBox *albaGUI::ListBox(int id, wxString label, int height, wxString tooltip, long lbox_style, int width) //<*> togliere direction
{
	if (width < 0)
		width = (label == "") ? FW : DW;

	int w_id = GetWidgetId(id);
	wxListBox *lb = new wxListBox(this, w_id, dp, wxSize(width, height), 0, NULL, lbox_style | m_EntryStyle);  
	lb->SetValidator(albaGUIValidator(this, w_id, lb));
	lb->SetFont(m_Font);
	if (tooltip != "") lb->SetToolTip(tooltip);

	if (label == "")
	{
		Add(lb, 0, wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);
		if (tooltip != "")
			lab->SetToolTip(tooltip);

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(lb, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}

	return lb;
}
//----------------------------------------------------------------------------
wxListCtrl *albaGUI::ListCtrl(int id, wxString label, int height, wxString tooltip, long lbox_style, int width) //<*> togliere direction
//----------------------------------------------------------------------------
{
	if (width < 0)
		width = (label == "") ? FW : DW;

	int w_id = GetWidgetId(id);
	wxListCtrl *lb = new wxListCtrl(this, w_id, dp, wxSize(width, height), lbox_style | m_EntryStyle);
	lb->SetValidator(albaGUIValidator(this, w_id, lb));
	lb->SetFont(m_Font);
	if (tooltip != "") lb->SetToolTip(tooltip);

	if (label == "")
	{
		Add(lb, 0, wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);
		if (tooltip != "")
			lab->SetToolTip(tooltip);

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(lb, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}

	return lb;
}
//----------------------------------------------------------------------------
wxGrid *albaGUI::Grid(int id, wxString label, int height, int row, int cols, wxString tooltip)
{
	//<*> le griglie mettono le scrollbar alla vfc
	//    inoltre ListBox,CheckListBoz e Grid possono avere le scrollbar  
	//    che stanno male quando anche il GuiHolder ha la scrollbar

	int width = (label == "") ? FW : DW;
	wxGrid *grid = new wxGrid(this, GetWidgetId(id), dp, wxSize(width, height));
	grid->CreateGrid(row, cols);
	grid->SetFont(m_Font);
	if (tooltip != "")
		grid->SetToolTip(tooltip);

	grid->EnableEditing(false);
	grid->SetColLabelSize(LH);
	grid->SetRowLabelSize(wxVERTICAL);
	grid->SetColLabelSize(LW);
	grid->SetColSize(0, EW);
	grid->SetColSize(1, EW);

	grid->SetColLabelValue(0, "");
	grid->SetColLabelValue(1, "");
	grid->SetRowLabelValue(0, "");
	grid->SetRowLabelValue(1, "");

	if (label == "")
	{
		Add(grid, 0, wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);
		if (tooltip != "")
			lab->SetToolTip(tooltip);

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(grid, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}

	return grid;
}

//----------------------------------------------------------------------------
albaGUICheckListBox* albaGUI::CheckList(int id, wxString label, int height, wxString tooltip)
{
	int width = (label == "") ? FW : DW;
	albaGUICheckListBox *clb = new albaGUICheckListBox(this, GetWidgetId(id), dp, wxSize(width, height));

	clb->SetListener(this);
	if (tooltip != "")
		clb->SetToolTip(tooltip);

	if (label == "")
	{
		Add(clb, 0, wxALL, LM);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);
		if (tooltip != "")
			lab->SetToolTip(tooltip);

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(clb, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}

	return clb;
}

//----------------------------------------------------------------------------
void albaGUI::VectorN(int id, wxString label, double *var, int num_elem, double min, double max, int decimal_digit, wxString tooltip)
{
	// if the label is empty the available space for widget is FW (full width)
	// if label is non empty the available space for widget is DW = FW-LW (full width minus label width) 
	int space = (label == "") ? FW : DW;

	// the space available for widget is divided in num_elem pieces
	// and widgetS ARE placed in each piece 
	int w = space / num_elem - M;

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	if (label != "")
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);
		sizer->Add(lab, 0, wxRIGHT, LM);
	}
	for (int i = 0; i < num_elem; i++)
	{
		int w_id = GetWidgetId(id);
		wxTextCtrl *tex = new wxTextCtrl(this, w_id, "", dp, wxSize(w, LH), m_EntryStyle);
		tex->SetFont(m_Font);
		tex->SetValidator(albaGUIValidator(this, w_id, tex, &var[i], min, max, decimal_digit));
		if (tooltip != "")
			tex->SetToolTip(tooltip);
		sizer->Add(tex, 0, wxRIGHT, HM);
	}
	Add(sizer, 0, wxEXPAND, M);
}
//----------------------------------------------------------------------------
void albaGUI::VectorN(int id, wxString label, int *var, int num_elem, int min, int max, wxString tooltip)
{
	// if the label is empty the available space for widget is FW (full width)
	// if label is non empty the available space for widget is DW = FW-LW (full width minus label width) 
	int space = (label == "") ? FW : DW;

	// the space available for widget is divided in num_elem pieces
	// and widgetS ARE placed in each piece 
	int w = space / num_elem - M;

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	if (label != "")
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);
		sizer->Add(lab, 0, wxRIGHT, LM);
	}

	for (int i = 0; i < num_elem; i++)
	{
		int w_id = GetWidgetId(id);
		wxTextCtrl *tex = new wxTextCtrl(this, w_id, "", dp, wxSize(w, LH), m_EntryStyle);
		tex->SetFont(m_Font);
		tex->SetValidator(albaGUIValidator(this, w_id, tex, &var[i], min, max));
		if (tooltip != "")
			tex->SetToolTip(tooltip);
		sizer->Add(tex, 0, wxRIGHT, HM);
	}

	Add(sizer, 0, wxEXPAND, M);
}

//----------------------------------------------------------------------------
albaGUIRollOut *albaGUI::RollOut(int id, albaString title, albaGUI *roll_gui, bool rollOutOpen)
{
	int w_id = GetWidgetId(id);
	albaGUIRollOut *roll = new albaGUIRollOut(this, title, roll_gui, w_id, rollOutOpen);
	roll->SetListener(this);
	return roll;
}

//----------------------------------------------------------------------------
void albaGUI::OnMouseWheel(wxMouseEvent &event)
  //----------------------------------------------------------------------------
{
  OnEvent(&albaEvent(this,MOUSE_WHEEL,(long) event.GetWheelRotation()));
}

//----------------------------------------------------------------------------
void albaGUI::DisableRecursive()
{
	DisableRecursive(this);
	Update();
}

//----------------------------------------------------------------------------
void albaGUI::DisableRecursive(wxWindow* parent)
{
	parent->Disable();
	wxWindowList& children = parent->GetChildren();
	for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it)
	{
		wxWindow* child = *it;
		DisableRecursive(child); // Recursive call
	}
}

//----------------------------------------------------------------------------
void albaGUI::OnListBox (wxCommandEvent &event)
//----------------------------------------------------------------------------
{
  wxListBox *lb = (wxListBox *)event.GetEventObject();
  ((albaGUIValidator *)lb->GetValidator())->TransferFromWindow();
  OnEvent(&albaEvent(this, event.GetId(),(long) event.GetInt()));
}
//----------------------------------------------------------------------------
void albaGUI::OnListCtrl (wxCommandEvent &event)
//----------------------------------------------------------------------------
{
	wxListCtrl *lb = (wxListCtrl *)event.GetEventObject();
	((albaGUIValidator *)lb->GetValidator())->TransferFromWindow();
	long index = ((wxListEvent *)&event)->GetIndex();
	OnEvent(&albaEvent(this, event.GetId(),index));
}
//----------------------------------------------------------------------------
void albaGUI::OnCheckListBox (wxCommandEvent &event)
{
  OnEvent(&albaEvent(this, event.GetId(),(long) event.GetInt()));
}
//----------------------------------------------------------------------------
void albaGUI::OnRadioButton(wxCommandEvent &event)
{
	OnEvent(&albaEvent(this, event.GetId()));
}

//----------------------------------------------------------------------------
void albaGUI::Reparent(wxWindow *parent)
{
	assert(parent);
	wxWindow::Reparent(parent);
	this->FitGui();
	this->Update();
	this->Show(true);
}

//----------------------------------------------------------------------------
void albaGUI::AddMenuItem(wxMenu *menu, int id, wxString label, char **icon/*=NULL*/)
{
	int osVersion;
	wxGetOsVersion(&osVersion);

	//menu icons does not work on this version of wxwindows under winxp
	if (icon != NULL && osVersion > 5)
	{
		wxMenuItem *menuItem = new wxMenuItem(menu, id, label, label);
		menuItem->SetBitmap(wxImage(icon));
		menu->Append(menuItem);
		menu->UpdateUI();
	}
	else
	{
		menu->Append(id, label, (wxMenu *)NULL, label);
	}
}

//----------------------------------------------------------------------------
void albaGUI::GetWidgetValue(long widget_id, WidgetDataType &widget_data)
{
	albaGUIValidator *validator = NULL;
	wxWindow *widget = NULL;
	widget = this->FindWindow(widget_id);

	if (widget == NULL)
		return;

	validator = (albaGUIValidator *)widget->GetValidator();
	if (validator == NULL)
		return;

	validator->GetWidgetData(widget_data);
}
//----------------------------------------------------------------------------
void albaGUI::SetWidgetValue(int id, WidgetDataType &widget_data)
{
	albaGUIValidator *validator = NULL;
	wxWindow *widget = NULL;
	widget = this->FindWindow(id);

	if (widget == NULL)
		return;

	validator = (albaGUIValidator *)widget->GetValidator();
	if (validator == NULL)
		return;

	validator->SetWidgetData(widget_data);

	if (id >= MINID && id < MAXID)
		id = GetModuleId(id);

	albaEventMacro(albaEvent(this, id));
}
//----------------------------------------------------------------------------
int* albaGUI::GetALBAWidgetId()
{
	static int ALBAWidgetId = MINID;
	return &ALBAWidgetId;
}
//----------------------------------------------------------------------------
int albaGUI::GetWidgetId(int mod_id)
{
	int *ALBAWidgetId = GetALBAWidgetId();
	(*ALBAWidgetId)++;
	m_WidgetTableID[(*ALBAWidgetId) - MINID] = mod_id;
	return (*ALBAWidgetId);
}

//----------------------------------------------------------------------------
void albaGUI::BoolGrid(int numRows, int numColumns, std::vector<int> &ids, std::vector<const char*> &labelsRows,std::vector<const char*> &labelsColumns, int *var, albaString tooltip /* = "" */ )
{
  std::vector<int> w_ids;
  std::vector<wxCheckBox *> check_list;
  for(int i=0; i< ids.size(); i++)
  {
    w_ids.push_back(GetWidgetId(ids[i]));
    check_list.push_back(new wxCheckBox(this, w_ids[i], "" ,dp, wxSize(FW/(numColumns+1),BH),m_EntryStyle ));
    check_list[i]->SetValidator( albaGUIValidator(this,w_ids[i],check_list[i],&(var[i])));
    check_list[i]->SetFont(m_Font);
  }

  wxStaticText* div = new wxStaticText(this, -1, "",dp, wxSize(-1, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);

  wxFlexGridSizer *fgSizer=  new wxFlexGridSizer( numRows+1, numColumns+1, 1, 1 );
  
  fgSizer->Add(div);//ad the position 0,0 an empty cell

  for (int i=0;i<labelsColumns.size();i++)
  {
    const char *a = labelsColumns[i];
    wxStaticText* lab = new wxStaticText(this, -1, labelsColumns[i],dp, wxSize(FW/(numColumns+1),LH), wxALIGN_LEFT);
    fgSizer->Add(lab);
  }

  for(int i = 0,j=0,k=0; i < (numColumns+1)*numRows; i++)
  {
    if (i%(numColumns+1)!=0)
    {
    	fgSizer->Add(check_list[j]);
      j++;
    }
    else//Add label of rows
    {
      const char *a = labelsRows[k];
      wxStaticText* lab = new wxStaticText(this, -1, labelsRows[k],dp, wxSize(FW/(numColumns+1),LH), wxALIGN_LEFT);
      fgSizer->Add(lab);
      k++;
    }
  }

  Add(fgSizer,0,wxALL, M);
}
