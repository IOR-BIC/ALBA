/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUI.cpp,v $
  Language:  C++
  Date:      $Date: 2008-12-02 15:05:17 $
  Version:   $Revision: 1.2.2.2 $
  Authors:   Silvano Imboden - Paolo Quadrani
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

#include <wx/statline.h>

#include "mafGUIFloatSlider.h"
#include "mafGUICheckListBox.h"
#include "mafGUICrossIncremental.h"

#include "mafGUIValidator.h"
#include "mafGUI.h"
#include "mafGUIButton.h"
#include "mafGUIPicButton.h"
#include "mafGUIRollOut.h"


#ifdef MAF_USE_VTK //:::::::::::::::::::::::::::::::::
#include "mafGUILutSwatch.h"
#endif             //:::::::::::::::::::::::::::::::::

#include "mafString.h"

// ugly hack to make FindWindow Work
// if you remove this line you will have a Compile-Error "FindWindowA is not defined for wxWindow"
// .... waiting a better workaround. SIL 30/11/05 
#if WIN32  
  #include <wx/msw/winundef.h> 
#endif
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

int MAFWidgetId = MINID;

//----------------------------------------------------------------------------
// mafGUI
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUI,mafGUIPanel)
    EVT_COMMAND_RANGE( MINID,MAXID,wxEVT_COMMAND_LISTBOX_SELECTED,      mafGUI::OnListBox)
    EVT_COMMAND_RANGE( MINID,MAXID,wxEVT_COMMAND_CHECKLISTBOX_TOGGLED , mafGUI::OnCheckListBox)
	  EVT_COMMAND_RANGE( MINID,MAXID,wxEVT_COMMAND_SLIDER_UPDATED,				mafGUI::OnSlider)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUI::mafGUI(mafObserver *listener) :
mafGUIPanel(mafGetFrame(),-1,dp,wxDefaultSize,wxNO_BORDER | wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
//----------------------------------------------------------------------------
{
	 m_Listener = listener;

   m_UseBackgroundColor = false;  
   m_BackgroundColor = wxColour(251,251,253); 
   if(m_UseBackgroundColor) this->SetBackgroundColour(m_BackgroundColor);

   m_EntryStyle = wxSUNKEN_BORDER /*| wxTE_PROCESS_TAB*/;

   m_Sizer =  new wxBoxSizer( wxVERTICAL );
   this->SetAutoLayout( TRUE );
   this->SetSizer( m_Sizer );
   m_Sizer->Fit(this);
   m_Sizer->SetSizeHints(this);

   m_BoldFont = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
   m_Font     = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32   
   m_BoldFont.SetPointSize(9);
#endif
   m_BoldFont.SetWeight(wxBOLD);

   m_WidgetTableID.clear();

	 this->Show(false);

   m_CollaborateStatus = false;
}
//----------------------------------------------------------------------------
mafGUI::~mafGUI()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUI::FitGui() 
//----------------------------------------------------------------------------
{
  this->SetMinSize( wxSize(FW+M+M , m_Sizer->GetMinSize().GetHeight() ));  //SIL. 22-may-2006 : 
  this->SetSize( wxSize(FW+M+M , m_Sizer->GetMinSize().GetHeight() ));
}
//----------------------------------------------------------------------------
int mafGUI::GetMetrics(int id)
//----------------------------------------------------------------------------
{
  switch(id)
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
//----------------------------------------------------------------------------
void mafGUI::OnEvent(mafEventBase *maf_event)
/**  \par implementation details:
called by the widgets (via a mafGUIValidator), forward the events to the eventListener 
\todo 
make it protected, and make mafGUIValidator friend
*/
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    int id = e->GetId();
    if(m_CollaborateStatus)
      mafEventMacro(mafEvent(this,REMOTE_PARAMETER, (long)id));
    if(id >= MINID && id <MAXID)
      id = GetModuleId(id);
    e->SetSender(this);
    e->SetId(id);
    mafEventMacro(*e);
  }
}
//----------------------------------------------------------------------------
void mafGUI::AddGui(mafGUI*  gui, int option, int flag, int border)
//----------------------------------------------------------------------------
{
  gui->Reparent(this);
	gui->FitGui();
	gui->Show(true);
	Add(gui,option,flag,border);
}
//----------------------------------------------------------------------------
void mafGUI::Update()
//----------------------------------------------------------------------------
{
  TransferDataToWindow();
}
//----------------------------------------------------------------------------
void mafGUI::Enable(int mod_id, bool enable)
//----------------------------------------------------------------------------
{
  if(mod_id == wxOK || mod_id == wxCANCEL)
  {
		wxWindow* win = FindWindow(mod_id);
		if (win)
			win->Enable(enable);
  }
  else
  {
    for(int i = MINID; i <= MAFWidgetId; i++) 
    {
      if(m_WidgetTableID[i - MINID] == mod_id)   
      {
        wxWindow* win = FindWindow(i);
        if (win)
					win->Enable(enable);
      }
    }
  }
}

// warning!: related to the label of all widgets
// if the string is longer of the available space for the label and it contain some spaces
// the text following the space is not shown
// max length for long label:  "12345678901234567890123456789012345" 
// max length for short label: "1234567890"

//----------------------------------------------------------------------------
void mafGUI::Divider (long style)
//----------------------------------------------------------------------------
{
  if(style == 0) //simple empty space
  {
    wxStaticText* div = new wxStaticText(this, -1, "",dp, wxSize(FW, 2), 0);
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
void mafGUI::Label(mafString label, bool bold, bool multiline ) 
//----------------------------------------------------------------------------
{
  int h = (multiline) ? -1 : LH;
  wxStaticText* lab = new wxStaticText(this, -1, label.GetCStr(),   dp, wxSize(-1,h), wxALIGN_LEFT );
  if(m_UseBackgroundColor) lab->SetBackgroundColour(m_BackgroundColor);
  if(bold) 
    lab->SetFont(m_BoldFont);
  else
    lab->SetFont(m_Font);
	Add(lab,0,wxEXPAND|wxALL, M);
}
//----------------------------------------------------------------------------
void mafGUI::Label(mafString *var, bool bold, bool multiline)
//----------------------------------------------------------------------------
{
  int h = (multiline) ? -1 : LH;
	wxStaticText* lab = new wxStaticText(this, -1, var->GetCStr(),    dp, wxSize(-1,h), wxALIGN_LEFT );
	if(m_UseBackgroundColor) lab->SetBackgroundColour(m_BackgroundColor);
  if(bold) 
    lab->SetFont(m_BoldFont);
  else
    lab->SetFont(m_Font);
	lab->SetValidator( mafGUIValidator(this,-1,lab,var) );
	Add(lab,0,wxEXPAND|wxALL, M);
}
//----------------------------------------------------------------------------
void mafGUI::Label(mafString label1, mafString label2, bool bold_label, bool bold_var)
//----------------------------------------------------------------------------
{
	wxStaticText* lab1 = new wxStaticText(this, -1, label1.GetCStr(), dp, wxSize(LW,LH), wxALIGN_RIGHT  | wxST_NO_AUTORESIZE ); 
	wxStaticText* lab2 = new wxStaticText(this, -1, label2.GetCStr(), dp, wxSize(-1,LH), wxALIGN_LEFT ); 
	if(bold_label) 
    lab1->SetFont(m_BoldFont);
  else
    lab1->SetFont(m_Font);
	if(bold_var) 
    lab2->SetFont(m_BoldFont);
  else
    lab2->SetFont(m_Font);
  if(m_UseBackgroundColor) lab1->SetBackgroundColour(m_BackgroundColor);
  if(m_UseBackgroundColor) lab2->SetBackgroundColour(m_BackgroundColor);	
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add( lab1, 0, wxRIGHT, LM);
	sizer->Add( lab2, 0, wxEXPAND);
	Add(sizer,0,wxALL, M); 
}
//----------------------------------------------------------------------------
void mafGUI::Label(mafString label1,mafString *var, bool bold_label, bool bold_var)
//----------------------------------------------------------------------------
{
	wxStaticText* lab1 = new wxStaticText(this, -1, label1.GetCStr(), dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
	wxStaticText* lab2 = new wxStaticText(this, -1, var->GetCStr(),   dp, wxSize(-1,LH), wxALIGN_LEFT );
  if(m_UseBackgroundColor) 
    lab1->SetBackgroundColour(m_BackgroundColor);
  if(m_UseBackgroundColor)
    lab2->SetBackgroundColour(m_BackgroundColor);	
	lab2->SetValidator( mafGUIValidator(this,-1,lab2,var) );
	if(bold_label) 
    lab1->SetFont(m_BoldFont);
  else
    lab1->SetFont(m_Font);
	if(bold_var) 
    lab2->SetFont(m_BoldFont);
  else
    lab2->SetFont(m_Font);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add( lab1, 0, wxRIGHT, LM);
	sizer->Add( lab2, 0, wxEXPAND);
	Add(sizer,0,wxALL, M); 
}
//----------------------------------------------------------------------------
void mafGUI::Button(int id,mafString button_text,mafString label, mafString tooltip) 
//----------------------------------------------------------------------------
{
	if(label.IsEmpty())
	{
    int w_id = GetWidgetId(id);
		mafGUIButton *butt = new mafGUIButton(this, w_id, button_text.GetCStr(),dp, wxSize(FW,BH) );
		butt->SetValidator( mafGUIValidator(this,w_id, butt) );
    butt->SetFont(m_Font);
    if(!tooltip.IsEmpty()) 
      butt->SetToolTip(tooltip.GetCStr());
		Add(butt,0,wxALL, M);
	}
  else
	{
		wxStaticText *lab  = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
    int w_id = GetWidgetId(id);
		mafGUIButton    *butt = new mafGUIButton   (this, w_id, button_text.GetCStr(),dp, wxSize(EW, BH) );
		butt->SetValidator( mafGUIValidator(this,w_id,butt) );
    butt->SetFont(m_Font);
		if(!tooltip.IsEmpty()) 
      butt->SetToolTip(tooltip.GetCStr());

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add( lab, 0, wxALIGN_CENTRE|wxRIGHT, LM);
		sizer->Add( butt,0);
		Add(sizer,0,wxALL, M); 
	}
}
//----------------------------------------------------------------------------
void mafGUI::Button  (int id,mafString *label,mafString button_text, mafString tooltip)
//----------------------------------------------------------------------------
{
  int w_id = GetWidgetId(id);
	wxStaticText *lab  = new wxStaticText(this, w_id, label->GetCStr(), dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  lab->SetValidator( mafGUIValidator(this,w_id,lab,label) );
  lab->SetFont(m_Font);
  if(m_UseBackgroundColor) 
    lab->SetBackgroundColour(m_BackgroundColor);
  w_id = GetWidgetId(id);
  mafGUIButton    *butt = new mafGUIButton   (this, w_id, button_text.GetCStr(), dp, wxSize(DW, BH) );
	butt->SetValidator( mafGUIValidator(this,w_id,butt) );
  butt->SetFont(m_Font);
	if(!tooltip.IsEmpty()) 
    butt->SetToolTip(tooltip.GetCStr());

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add( lab, 0, wxALIGN_CENTRE|wxRIGHT, LM);
	sizer->Add( butt,0);
	Add(sizer,0,wxALL, M); 
}
//----------------------------------------------------------------------------
mafGUICrossIncremental *mafGUI::CrossIncremental(int id,const char* label, double *stepVariable, double *topBottomVariable, double *leftRightVariable, int modality ,wxString tooltip /* = */, bool boldLabel /* = true */)
//----------------------------------------------------------------------------
{
  int width = (label == "") ? FW : DW;
  
  mafGUICrossIncremental *cI =  new mafGUICrossIncremental(this, GetWidgetId(id), label, stepVariable, topBottomVariable, leftRightVariable, boldLabel, modality, dp);
  cI->SetListener(this);
  
	Add(cI,0,wxALL, M); 
  
  return cI;
}
// integer vector form 1
//----------------------------------------------------------------------------
void mafGUI::Vector(int id,wxString label,int var[3],int min, int max, wxString tooltip, wxColour *bg_colour)
//----------------------------------------------------------------------------
{
  int w_id;
	wxStaticText *lab  = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) 
    lab->SetBackgroundColour(m_BackgroundColor);
  lab->SetFont(m_Font);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text1 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text1->SetValidator( mafGUIValidator(this,w_id,text1,var,      min,max) );
  text1->SetFont(m_Font);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text2 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text2->SetValidator( mafGUIValidator(this,w_id,text2,&(var[1]),min,max) );
  text2->SetFont(m_Font);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text3 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
	text3->SetValidator( mafGUIValidator(this,w_id,text3,&(var[2]),min,max) );
  text3->SetFont(m_Font);
	if(tooltip != "")
	{
		text1->SetToolTip(tooltip);
		text2->SetToolTip(tooltip);
		text3->SetToolTip(tooltip);
	}
  if(bg_colour != NULL)
  {
    text1->SetBackgroundColour(bg_colour[0]);
    text2->SetBackgroundColour(bg_colour[1]);
    text3->SetBackgroundColour(bg_colour[2]);
  }
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add( lab,  0, wxRIGHT, LM);
	sizer->Add( text1,0, wxRIGHT, HM);
	sizer->Add( text2,0, wxRIGHT, HM);
	sizer->Add( text3,0, wxRIGHT, HM);
	Add(sizer,0,wxALL, M); 
}
// integer vector form 2
//----------------------------------------------------------------------------
void mafGUI::Vector(int id,wxString label,int var[3],int minx,int maxx,int miny,int maxy,int minz,int maxz, wxString tooltip, wxColour *bg_colour)
//----------------------------------------------------------------------------
{
	int w_id;
  wxStaticText *lab  = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) 
    lab->SetBackgroundColour(m_BackgroundColor);
	w_id = GetWidgetId(id);
  wxTextCtrl  *text1 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text1->SetValidator( mafGUIValidator(this,w_id,text1,var,      minx,maxx) );
  text1->SetFont(m_Font);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text2 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text2->SetValidator( mafGUIValidator(this,w_id,text2,&(var[1]),miny,maxy) );
  text2->SetFont(m_Font);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text3 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
	text3->SetValidator( mafGUIValidator(this,w_id,text3,&(var[2]),minz,maxz) );
  text3->SetFont(m_Font);
	if(tooltip != "")
	{
		text1->SetToolTip(tooltip);
		text2->SetToolTip(tooltip);
		text3->SetToolTip(tooltip);
	}
  if(bg_colour != NULL)
  {
    text1->SetBackgroundColour(bg_colour[0]);
    text2->SetBackgroundColour(bg_colour[1]);
    text3->SetBackgroundColour(bg_colour[2]);
  }
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add( lab,  0, wxRIGHT, LM);
	sizer->Add( text1,0, wxRIGHT, HM);
	sizer->Add( text2,0, wxRIGHT, HM);
	sizer->Add( text3,0, wxRIGHT, HM);
	Add(sizer,0,wxALL, M); 
}
// float vector form 1
//----------------------------------------------------------------------------
void mafGUI::Vector(int id,wxString label,float var[3],float min, float max, int decimal_digit, wxString tooltip, wxColour *bg_colour) // <*> togliere la seconda forma
//----------------------------------------------------------------------------
{   
  int w_id;
	wxStaticText *lab  = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) lab->SetBackgroundColour(m_BackgroundColor);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text1 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text1->SetValidator( mafGUIValidator(this,w_id,text1,var,      min,max, decimal_digit) );
  text1->SetFont(m_Font);
  w_id = GetWidgetId(id);
  wxTextCtrl  *text2 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text2->SetValidator( mafGUIValidator(this,w_id,text2,&(var[1]),min,max, decimal_digit) );
  text2->SetFont(m_Font);
  w_id = GetWidgetId(id);
  wxTextCtrl  *text3 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text3->SetValidator( mafGUIValidator(this,w_id,text3,&(var[2]),min,max, decimal_digit) );
  text3->SetFont(m_Font);
	if(tooltip != "")
	{
		text1->SetToolTip(tooltip);
		text2->SetToolTip(tooltip);
		text3->SetToolTip(tooltip);
	}
  if(bg_colour != NULL)
  {
    text1->SetBackgroundColour(bg_colour[0]);
    text2->SetBackgroundColour(bg_colour[1]);
    text3->SetBackgroundColour(bg_colour[2]);
  }
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add( lab,  0, wxRIGHT, LM);
	sizer->Add( text1,0, wxRIGHT, HM);
	sizer->Add( text2,0, wxRIGHT, HM);
	sizer->Add( text3,0, wxRIGHT, HM);
  Add(sizer,0,wxALL, M); 
}
// float vector form 2
//----------------------------------------------------------------------------
void mafGUI::Vector(int id,wxString label,float var[3],float minx,float maxx,float miny,float maxy,float minz,float maxz, int decimal_digit, wxString tooltip, wxColour *bg_colour)
//----------------------------------------------------------------------------
{
  int w_id;
	wxStaticText *lab  = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) 
    lab->SetBackgroundColour(m_BackgroundColor);
  lab->SetFont(m_Font);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text1 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text1->SetValidator( mafGUIValidator(this,w_id,text1,var,      minx,maxx, decimal_digit) );
  text1->SetFont(m_Font);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text2 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text2->SetValidator( mafGUIValidator(this,w_id,text2,&(var[1]),miny,maxy, decimal_digit) );
  text2->SetFont(m_Font);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text3 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
	text3->SetValidator( mafGUIValidator(this,w_id,text3,&(var[2]),minz,maxz, decimal_digit) );
  text3->SetFont(m_Font);
	if(tooltip != "")
	{
		text1->SetToolTip(tooltip);
		text2->SetToolTip(tooltip);
		text3->SetToolTip(tooltip);
	}
  if(bg_colour != NULL)
  {
    text1->SetBackgroundColour(bg_colour[0]);
    text2->SetBackgroundColour(bg_colour[1]);
    text3->SetBackgroundColour(bg_colour[2]);
  }
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add( lab,  0, wxRIGHT, LM);
	sizer->Add( text1,0, wxRIGHT, HM);
	sizer->Add( text2,0, wxRIGHT, HM);
	sizer->Add( text3,0, wxRIGHT, HM);
  Add(sizer,0,wxALL, M); 
}
// double vector form 1
//----------------------------------------------------------------------------
void mafGUI::Vector(int id,wxString label,double var[3],double min, double max, int decimal_digit, wxString tooltip, wxColour *bg_colour)
//----------------------------------------------------------------------------
{   
  int w_id;
	wxStaticText *lab  = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) 
    lab->SetBackgroundColour(m_BackgroundColor);
  lab->SetFont(m_Font);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text1 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text1->SetValidator( mafGUIValidator(this,w_id,text1,var,      min,max, decimal_digit) );
  text1->SetFont(m_Font);
  w_id = GetWidgetId(id);
  wxTextCtrl  *text2 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text2->SetValidator( mafGUIValidator(this,w_id,text2,&(var[1]),min,max, decimal_digit) );
  text2->SetFont(m_Font);
  w_id = GetWidgetId(id);
  wxTextCtrl  *text3 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text3->SetValidator( mafGUIValidator(this,w_id,text3,&(var[2]),min,max, decimal_digit) );
  text3->SetFont(m_Font);
	if(tooltip != "")
	{
		text1->SetToolTip(tooltip);
		text2->SetToolTip(tooltip);
		text3->SetToolTip(tooltip);
	}
  if(bg_colour != NULL)
  {
    text1->SetBackgroundColour(bg_colour[0]);
    text2->SetBackgroundColour(bg_colour[1]);
    text3->SetBackgroundColour(bg_colour[2]);
  }
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add( lab,  0, wxRIGHT, LM);
  sizer->Add( text1,0, wxRIGHT, HM);
  sizer->Add( text2,0, wxRIGHT, HM);
  sizer->Add( text3,0, wxRIGHT, HM);
  Add(sizer,0,wxEXPAND, M); 
}
// double vector form 2
//----------------------------------------------------------------------------
void mafGUI::Vector(int id,wxString label,double var[3],double minx,double maxx,double miny,double maxy,double minz,double maxz, int decimal_digit, wxString tooltip, wxColour *bg_colour)
//----------------------------------------------------------------------------
{
  int w_id;
	wxStaticText *lab  = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) 
    lab->SetBackgroundColour(m_BackgroundColor);
  lab->SetFont(m_Font);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text1 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text1->SetValidator( mafGUIValidator(this,w_id,text1,var,      minx,maxx, decimal_digit) );
  text1->SetFont(m_Font);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text2 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
  text2->SetValidator( mafGUIValidator(this,w_id,text2,&(var[1]),miny,maxy, decimal_digit) );
  text2->SetFont(m_Font);
  w_id = GetWidgetId(id);
	wxTextCtrl  *text3 = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(EW,LH), m_EntryStyle );
	text3->SetValidator( mafGUIValidator(this,w_id,text3,&(var[2]),minz,maxz, decimal_digit) );
  text3->SetFont(m_Font);
	if(tooltip != "")
	{
		text1->SetToolTip(tooltip);
		text2->SetToolTip(tooltip);
		text3->SetToolTip(tooltip);
	}
  if(bg_colour != NULL)
  {
    text1->SetBackgroundColour(bg_colour[0]);
    text2->SetBackgroundColour(bg_colour[1]);
    text3->SetBackgroundColour(bg_colour[2]);
  }
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add( lab,  0, wxRIGHT, LM);
	sizer->Add( text1,0, wxRIGHT, HM);
	sizer->Add( text2,0, wxRIGHT, HM);
	sizer->Add( text3,0, wxRIGHT, HM);
  Add(sizer,0,wxEXPAND, M); 
}
//----------------------------------------------------------------------------
void mafGUI::String(int id,wxString label,wxString* var, wxString tooltip, bool multiline, bool password)
//----------------------------------------------------------------------------
{
  int sw = LH;
  long e_style = m_EntryStyle;
  if(multiline)
  {
    sw *= 5;
    e_style |= wxTE_MULTILINE|wxTE_WORDWRAP;
  }
  if(password)
    e_style |= wxTE_PASSWORD;

  if(label == "")
	{
    int w_id = GetWidgetId(id);
		wxTextCtrl *text = NULL;

    text = new wxTextCtrl  (this, w_id, "", dp, wxSize(FW,sw), e_style);
		text->SetValidator( mafGUIValidator(this,w_id,text,var) );
    text->SetFont(m_Font);
		if(tooltip != "") 
      text->SetToolTip(tooltip);
	  Add(text,0,wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
    int w_id = GetWidgetId(id);
		wxTextCtrl *text = NULL;
    
    text = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(DW,sw), e_style  );
		text->SetValidator( mafGUIValidator(this,w_id,text,var) );
    text->SetFont(m_Font);

		if(tooltip != "")
			text->SetToolTip(tooltip);
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add( lab,  0, wxRIGHT, LM);
		sizer->Add( text, 0, wxRIGHT, HM);
		Add(sizer,0,wxALL, M);
	}
}
//----------------------------------------------------------------------------
void mafGUI::String(int id,mafString label, mafString *var, mafString tooltip, bool multiline, bool password)
//----------------------------------------------------------------------------
{
  int sw = LH;
  long e_style = m_EntryStyle;
  if(multiline)
  {
    sw *= 5;
    e_style |= wxTE_MULTILINE|wxTE_WORDWRAP;
  }
  if(password)
    e_style |= wxTE_PASSWORD;

  if(label.IsEmpty())
	{
    int w_id = GetWidgetId(id);
		wxTextCtrl  *text = NULL;
    text = new wxTextCtrl  (this, w_id, "", dp, wxSize(FW,sw), e_style );
		text->SetValidator( mafGUIValidator(this,w_id,text,var) );
    text->SetFont(m_Font);
		if(!tooltip.IsEmpty())
      text->SetToolTip(tooltip.GetCStr());
	  Add(text,0,wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
    int w_id = GetWidgetId(id);
		wxTextCtrl  *text = NULL;
    text = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(DW,sw), e_style );
		text->SetValidator( mafGUIValidator(this,w_id,text,var) );
    text->SetFont(m_Font);
		if(!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add( lab,  0, wxRIGHT, LM);
		sizer->Add( text, 0, wxRIGHT, HM);
		Add(sizer,0,wxALL, M);
	}
}
//----------------------------------------------------------------------------
void mafGUI::Integer(int id,mafString label,int* var,int min, int max, mafString tooltip)
//----------------------------------------------------------------------------
{
  int w_id;
	if(label.IsEmpty())
	{
    w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl  (this, w_id, "", dp, wxSize(FW,LH), m_EntryStyle  );
		text->SetValidator( mafGUIValidator(this,w_id,text,var,min,max) );
    text->SetFont(m_Font);
		if(!tooltip.IsEmpty()) 
      text->SetToolTip(tooltip.GetCStr());
	  Add(text,0,wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
    w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(DW,LH), m_EntryStyle  );
		text->SetValidator( mafGUIValidator(this,w_id,text,var,min,max) );
    text->SetFont(m_Font);
		if(!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add( lab,  0, wxRIGHT, LM);
		sizer->Add( text, 0, wxRIGHT, HM);
		Add(sizer,0,wxALL, M);
	}
}
//----------------------------------------------------------------------------
void mafGUI::Float(int id,mafString label,float* var,float min, float max, int flag, int decimal_digit, mafString tooltip)
//----------------------------------------------------------------------------
{
  int w_id;
	if(label.IsEmpty())
	{
    w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl  (this, w_id, "", dp, wxSize(FW,LH), m_EntryStyle  );
		text->SetValidator( mafGUIValidator(this,w_id,text,var,min,max, decimal_digit) );
    text->SetFont(m_Font);

		if(!tooltip.IsEmpty()) text->SetToolTip(tooltip.GetCStr());
	  Add(text,0,wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
    w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(DW,LH), m_EntryStyle  );
		text->SetValidator( mafGUIValidator(this,w_id,text,var,min,max, decimal_digit) );
    text->SetFont(m_Font);

		if(!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add( lab,  0, wxRIGHT, LM);
		sizer->Add( text, 0, wxRIGHT, HM);
		Add(sizer,0,wxALL, M);
	}
}
//----------------------------------------------------------------------------
void mafGUI::Double(int id,mafString label,double* var,double min, double max, int decimal_digit, mafString tooltip)
//----------------------------------------------------------------------------
{
  int w_id;
	if(label.IsEmpty())
	{
    w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl  (this, w_id, "", dp, wxSize(FW,LH), m_EntryStyle  );
		text->SetValidator( mafGUIValidator(this,w_id,text,var,min,max,decimal_digit) );
    text->SetFont(m_Font);
		if(!tooltip.IsEmpty()) 
      text->SetToolTip(tooltip.GetCStr());
	  Add(text,0,wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
    w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl  (this, w_id, ""   , dp, wxSize(DW,LH), m_EntryStyle  );
		text->SetValidator( mafGUIValidator(this,w_id,text,var,min,max,decimal_digit) );
    text->SetFont(m_Font);
		if(!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add( lab,  0, wxRIGHT, LM);
		sizer->Add( text, 0, wxRIGHT, HM);
		Add(sizer,0,wxALL, M);
	}
}
//----------------------------------------------------------------------------
void mafGUI::Bool(int id,mafString label,int* var, int flag, mafString tooltip)  // <*> non mi piace il flag - trovare una sintassi piu comprensibile
//----------------------------------------------------------------------------  // <*> ci vorrebbe anche il layout check al centro, label a destra
{
  int w_id;
  if (flag != 0) // text on left
  {
    w_id = GetWidgetId(id);
		wxCheckBox *check = new wxCheckBox(this, w_id, label.GetCStr(), dp, wxSize(-1,BH), 0 );
    check->SetValidator( mafGUIValidator(this,w_id,check,var) );
    check->SetFont(m_Font);
		if(m_UseBackgroundColor) 
      check->SetBackgroundColour(m_BackgroundColor);
		if(!tooltip.IsEmpty()) 
      check->SetToolTip(tooltip.GetCStr());
		Add(check,0,wxALL, M);
  } 
	else      // text on right
	{
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
    w_id = GetWidgetId(id);
		wxCheckBox *check = new wxCheckBox  (this, w_id, "",    dp, wxSize(DW,LH), m_EntryStyle );
    check->SetValidator( mafGUIValidator(this,w_id,check,var) );
    check->SetFont(m_Font);
    if(m_UseBackgroundColor) 
      check->SetBackgroundColour(m_BackgroundColor);
		if(!tooltip.IsEmpty()) 
      check->SetToolTip(tooltip.GetCStr());

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add( lab,  0, wxRIGHT, LM);
		sizer->Add( check,0, wxRIGHT, HM);
		Add(sizer,0,wxALL, M); 
  }
}
//----------------------------------------------------------------------------
wxSlider *mafGUI::Slider(int id,wxString label,int* var,int min, int max, wxString tooltip,bool showText)
//----------------------------------------------------------------------------
{
  wxTextCtrl   *text = NULL;
	wxSlider     *sli  = NULL;
	wxStaticText *lab  = NULL;
  int w_id_text;
  int w_id_sli;

	if(label == "" && showText)
	{
    int text_w   = EW*0.8;
    int slider_w = FW-text_w;
    w_id_text = GetWidgetId(id);
		text = new wxTextCtrl (this, w_id_text, "", dp, wxSize(text_w,  LH), m_EntryStyle);
    text->SetFont(m_Font);
    w_id_sli = GetWidgetId(id);
		sli = new wxSlider(this, w_id_sli,min,min,max, dp, wxSize(slider_w,LH));
    if(m_UseBackgroundColor) 
      sli->SetBackgroundColour(m_BackgroundColor);
    
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(text, 0);
		sizer->Add(sli,  0);
		Add(sizer,0,wxALL, M); 
	}
	else if(showText)
	{
    int text_w   = EW*0.8;
		int slider_w = DW-text_w;
		lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
    w_id_text = GetWidgetId(id);
		text = new wxTextCtrl  (this, w_id_text, "", dp, wxSize(text_w,LH), m_EntryStyle);
    text->SetFont(m_Font);
    w_id_sli  = GetWidgetId(id);
		sli = new wxSlider(this, w_id_sli,min,min,max, dp, wxSize(slider_w,LH));
    if(m_UseBackgroundColor) 
      sli->SetBackgroundColour(m_BackgroundColor);
		
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab,  0, wxRIGHT, LM);
		sizer->Add(text, 0);
		sizer->Add(sli,  0);
		Add(sizer,0,wxALL, M); 
	}
  else if(!showText)
  {
    int text_w   = EW*0.8;
    int slider_w = FW;
    w_id_text = GetWidgetId(id);
    text = new wxTextCtrl (this, w_id_text, "", dp, wxSize(text_w,  LH), m_EntryStyle);
    text->SetFont(m_Font);
    w_id_sli = GetWidgetId(id);
    sli = new wxSlider(this, w_id_sli,min,min,max, dp, wxSize(slider_w,LH));
    if(m_UseBackgroundColor) 
      sli->SetBackgroundColour(m_BackgroundColor);

    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(text, 0);
    sizer->Add(sli,  0);
    Add(sizer,0,wxALL, M); 
    text->Show(showText);
  }
  sli->SetValidator(mafGUIValidator(this,w_id_sli,sli,var,text));
  text->SetValidator(mafGUIValidator(this,w_id_text,text,var,sli,min,max)); //- if uncommented, remove also wxTE_READONLY from the text (in both places)
	if(tooltip != "")	text->SetToolTip(tooltip);
	
  return sli;
}
//----------------------------------------------------------------------------
mafGUIFloatSlider *mafGUI::FloatSlider(int id,wxString label,double *var,double min, double max, wxSize size, wxString tooltip, bool textBoxEnable) //<*> Si puo Chiamare Slider lo stesso 
//----------------------------------------------------------------------------                                            //<*> verificare se le entry erano abilitate o no
{
  wxTextCtrl     *text = NULL;
	mafGUIFloatSlider *sli  = NULL;
	wxStaticText   *lab  = NULL;
  int w_id_text;
  int w_id_sli;
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	if(label == "")
	{
    int text_w   = EW * 0.8;
    int slider_w = FW - text_w;
    w_id_text = GetWidgetId(id);
	  text = new wxTextCtrl    (this, w_id_text, "", dp, wxSize(text_w,  LH), m_EntryStyle/*|wxTE_READONLY*/);
	  text->SetFont(m_Font);
    text->Enable(textBoxEnable);
    w_id_sli  = GetWidgetId(id);
		sli  = new mafGUIFloatSlider(this, w_id_sli,*var,min,max, dp, size);
    if(m_UseBackgroundColor) 
      sli->SetBackgroundColour(m_BackgroundColor);
	  text->SetValidator(mafGUIValidator(this,w_id_text,text,var,sli,min,max));
    sli->SetValidator(mafGUIValidator(this,w_id_sli,sli,var,text));
		sizer->Add(text, 0);
		sizer->Add(sli,  0);
	}
	else
	{
    int text_w   = EW * 0.8;
		int slider_w = DW - text_w;
		lab = new wxStaticText  (this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
    w_id_text = GetWidgetId(id);
		text = new wxTextCtrl    (this, w_id_text, "", dp, wxSize(text_w,LH), m_EntryStyle/*|wxTE_READONLY*/);
    text->SetFont(m_Font);
    text->Enable(textBoxEnable);
    w_id_sli = GetWidgetId(id);
		sli = new mafGUIFloatSlider(this, w_id_sli,*var,min,max, dp, size);
    if(m_UseBackgroundColor) 
      sli->SetBackgroundColour(m_BackgroundColor);

		text->SetValidator(mafGUIValidator(this,w_id_text,text,var,sli,min,max));
    sli->SetValidator(mafGUIValidator(this,w_id_sli,sli,var,text));
		sizer->Add(lab,  0, wxRIGHT, LM);
		sizer->Add(text, 0);
		sizer->Add(sli,  0);
	}
	Add(sizer,0,wxALL, M); 
	if(tooltip != "")	
    text->SetToolTip(tooltip);
	return sli;
}
//----------------------------------------------------------------------------
mafGUIFloatSlider *mafGUI::FloatSlider(int id,double *var, double min, double max, wxString minLab, wxString maxLab, wxSize size, wxString tooltip, bool textBoxEnable) //<*> Si puo Chiamare Slider lo stesso 
//----------------------------------------------------------------------------                                            //<*> verificare se le entry erano abilitate o no
{
  wxTextCtrl     *text = NULL;
  mafGUIFloatSlider *sli  = NULL;
  wxStaticText   *minText  = NULL;
  wxStaticText   *maxText  = NULL;
  int w_id_sli;
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

  int text_w   = 0.1;
  int slider_w = FW - EW*2;
  minText = new wxStaticText  (this, GetWidgetId(id), minLab, dp, wxSize(EW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) 
    minText->SetBackgroundColour(m_BackgroundColor);
  minText->SetFont(m_Font);

  maxText = new wxStaticText  (this, GetWidgetId(id), maxLab, dp, wxSize(EW,LH), wxALIGN_LEFT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) 
    maxText->SetBackgroundColour(m_BackgroundColor);
  maxText->SetFont(m_Font);

  w_id_sli = GetWidgetId(id);
  sli = new mafGUIFloatSlider(this, w_id_sli,*var,min,max, dp, size);
  if(m_UseBackgroundColor) 
    sli->SetBackgroundColour(m_BackgroundColor);

  int w_id_text = GetWidgetId(id);
  text = new wxTextCtrl    (this, w_id_text, "", dp, wxSize(text_w,LH), m_EntryStyle/*|wxTE_READONLY*/);
  text->SetFont(m_Font);
  text->Enable(textBoxEnable);

  text->SetValidator(mafGUIValidator(this,w_id_text,text,var,sli,min,max));
  sli->SetValidator(mafGUIValidator(this,w_id_sli,sli,var,text));

  sizer->Add(minText,  0, wxRIGHT, LM);
  sizer->Add(sli,  0, 0, LM);
  sizer->Add(maxText,  0);

  Add(sizer,0,wxALL, M); 

  return sli;
}
//----------------------------------------------------------------------------
void mafGUI::Radio(int id,wxString label,int* var, int numchoices, const wxString choices[], int dim, wxString tooltip,int style)
//----------------------------------------------------------------------------
{
  // SIL: 24/04/03
	// wxWindows assign by itself the RadioButtons id's in a way 
	// that it break the correct tab-ordering - and there's is no workaround
	// workaround: - use combo instead
  // SIL. 23-may-2006 : -- fixed with wxWidgets 2.6.3

  wxRadioBox *radio = NULL;
  int w_id;

  if(!label.IsEmpty())
  {
    wxBoxSizer *sizer = NULL;
    sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,-1), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);

    w_id = GetWidgetId(id);
    radio = new wxRadioBox  (this, w_id, "",dp, wxSize(DW,-1), numchoices, choices,dim,style|m_EntryStyle|wxTAB_TRAVERSAL );

    sizer->Add( lab,  0, wxRIGHT, LM);
    sizer->Add( radio,0, wxRIGHT, HM);
    Add(sizer,0,wxALL, M); 
  }
  else
  {
    w_id = GetWidgetId(id);
    radio = new wxRadioBox  (this, w_id, "",dp, wxSize(FW,-1), numchoices, choices,dim,style|m_EntryStyle|wxTAB_TRAVERSAL );
    Add(radio,0, wxRIGHT, HM);
  }

  if(m_UseBackgroundColor) 
    radio->SetBackgroundColour(m_BackgroundColor);
  radio->SetValidator( mafGUIValidator(this,w_id,radio,var) );
  radio->SetFont(m_Font);
  if(tooltip != "") 
    radio->SetToolTip(tooltip);
}
//----------------------------------------------------------------------------
wxComboBox *mafGUI::Combo(int id,mafString label,int* var,int numchoices, const wxString choices[], mafString tooltip)
//----------------------------------------------------------------------------
{
	wxComboBox *combo = NULL;
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  int w_id;

  if(!label.IsEmpty())
  {
    wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW,-1), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
    w_id = GetWidgetId(id);
	  combo = new wxComboBox  (this, w_id, "", dp, wxSize(DW,-1), numchoices, choices,wxCB_READONLY);
	  combo->SetFont(m_Font);
    sizer->Add( lab,  0, wxRIGHT, LM);
	  sizer->Add( combo,0, wxRIGHT, HM);
  }
  else
  {
    w_id = GetWidgetId(id);
	  combo = new wxComboBox  (this, w_id, "", dp, wxSize(FW,-1), numchoices, choices,wxCB_READONLY);
	  combo->SetFont(m_Font);
    sizer->Add( combo,0, wxRIGHT, HM);
  }
  
  combo->SetValidator( mafGUIValidator(this,w_id,combo,var) );
	if(!tooltip.IsEmpty()) 
    combo->SetToolTip(tooltip.GetCStr());

	Add(sizer,0,wxALL, M);
  return combo;
}
//----------------------------------------------------------------------------
void mafGUI::FileOpen(int id,mafString label,mafString* var, const mafString wildcard, mafString tooltip)
//----------------------------------------------------------------------------
{
  int text_w = EW+HM+EW;
  int butt_w;
	wxStaticText *lab  = new wxStaticText(this, GetWidgetId(id), label.GetCStr(), dp, wxSize(LW,BH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) 
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
  mafGUIButton  *butt = new mafGUIButton(this, w_id, "open",dp, wxSize(butt_w,BH));
  wxTextCtrl *text = new wxTextCtrl  (this, GetWidgetId(id), ""   , dp, wxSize(text_w,BH),wxTE_READONLY|m_EntryStyle);
  text->SetFont(m_Font);
  butt->SetValidator( mafGUIValidator(this,w_id,butt,var,text,true,wildcard) );
  butt->SetFont(m_Font);
  if(!tooltip.IsEmpty())
	{
		text->SetToolTip(tooltip.GetCStr());
		butt->SetToolTip(tooltip.GetCStr());
	}
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	if (!label.IsEmpty())
	{
    sizer->Add( lab,  0, wxRIGHT, LM);
	}
	sizer->Add( butt, 0, wxRIGHT, HM);
	sizer->Add( text, 0);
  Add(sizer,0,wxALL, M); 
}
//----------------------------------------------------------------------------
void mafGUI::DirOpen(int id,mafString label,mafString *var, mafString tooltip)
//----------------------------------------------------------------------------
{
  int text_w = EW+HM+EW;
  int butt_w;
	wxStaticText *lab  = new wxStaticText(this, GetWidgetId(id), label.GetCStr(),   dp, wxSize(LW,BH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) 
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
  mafGUIButton    *butt = new mafGUIButton   (this, w_id, "browse",dp, wxSize(butt_w,BH));
  wxTextCtrl   *text = new wxTextCtrl  (this, GetWidgetId(id), "", dp, wxSize(text_w,BH),wxTE_READONLY|m_EntryStyle  );
  text->SetFont(m_Font);
	butt->SetValidator( mafGUIValidator(this,w_id,butt,var,text) );
  butt->SetFont(m_Font);
  if(!tooltip.IsEmpty())
	{
		text->SetToolTip(tooltip.GetCStr());
		butt->SetToolTip(tooltip.GetCStr());
	}
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  if (!label.IsEmpty())
  {
    sizer->Add( lab,  0, wxRIGHT, LM);
  }
	sizer->Add( butt, 0, wxRIGHT, HM);
	sizer->Add( text, 0);
  Add(sizer,0,wxALL, M); 
}
//----------------------------------------------------------------------------
void mafGUI::FileSave(int id,mafString label,mafString* var, const mafString wildcard, mafString tooltip, bool enableTextCtrl)
//----------------------------------------------------------------------------
{
  int text_w = EW+HM+EW;
  int butt_w;
	wxStaticText *lab  = new wxStaticText(this, GetWidgetId(id), label.GetCStr(),   dp, wxSize(LW,BH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) 
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
  mafGUIButton    *butt = new mafGUIButton   (this, w_id, "save",  dp, wxSize(butt_w,BH));
  wxTextCtrl   *text = new wxTextCtrl  (this, GetWidgetId(id), "",      dp, wxSize(text_w,BH),wxTE_READONLY|m_EntryStyle  );
  text->SetFont(m_Font);
	butt->SetValidator( mafGUIValidator(this,w_id,butt,var,text,false,wildcard) );
  butt->SetFont(m_Font);
  if(!tooltip.IsEmpty())
	{
		text->SetToolTip(tooltip.GetCStr());
		butt->SetToolTip(tooltip.GetCStr());
	}
  text->Enable(enableTextCtrl);
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	if (!label.IsEmpty())
	{
    sizer->Add( lab,  0, wxRIGHT, LM);
	}
	sizer->Add( butt, 0, wxRIGHT, HM);
	sizer->Add( text, 0);
  Add(sizer,0,wxALL, M); 
}
//----------------------------------------------------------------------------
void mafGUI::Color(int id,wxString label,wxColour* var, wxString tooltip)
//----------------------------------------------------------------------------
{
	wxStaticText	*lab  = new wxStaticText(this, GetWidgetId(id), label,dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) 
    lab->SetBackgroundColour(m_BackgroundColor);
  lab->SetFont(m_Font);
	wxTextCtrl		*text = new wxTextCtrl  (this, GetWidgetId(id), ""   ,dp, wxSize(EW,LH),wxTE_READONLY|m_EntryStyle );
  text->SetFont(m_Font);
  int w_id = GetWidgetId(id);
  mafGUIButton    *butt = new mafGUIButton   (this, w_id, "...",  dp, wxSize(BH,BH));
  butt->SetValidator( mafGUIValidator(this,w_id,butt,var,text) );
	butt->SetFont(m_Font);
  if(tooltip != "") 
    butt->SetToolTip(tooltip);

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add( lab,  0, wxRIGHT, LM);
	sizer->Add( text, 0, wxRIGHT, HM);
	sizer->Add( butt, 0);
  Add(sizer,0,wxALL, M); 
}
#ifdef MAF_USE_VTK //:::::::::::::::::::::::::::::::::
//----------------------------------------------------------------------------
mafGUILutSwatch *mafGUI::Lut(int id,wxString label,vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  wxStaticText	*lab  = new wxStaticText(this, GetWidgetId(id), label,dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  if(m_UseBackgroundColor) 
    lab->SetBackgroundColour(m_BackgroundColor);
  lab->SetFont(m_Font);

  mafGUILutSwatch *luts = new mafGUILutSwatch (this,GetWidgetId(id), dp, wxSize(DW,18), wxTAB_TRAVERSAL | wxSIMPLE_BORDER );
  luts->SetLut(lut);
  luts->SetEditable(true);
  luts->SetListener(this);

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add( lab,  0, wxRIGHT, LM);
  sizer->Add( luts, 0, wxEXPAND, HM);
  Add(sizer,0,wxALL, M); 
  return luts;
}
#endif             //:::::::::::::::::::::::::::::::::
//----------------------------------------------------------------------------
void mafGUI::TwoButtons(int firstID, int secondID, const char* label1, const char* label2, int alignment /* = wxALL */)
//----------------------------------------------------------------------------
{
  int w_idFirst = GetWidgetId(firstID);
  int w_idSecond = GetWidgetId(secondID);
  mafGUIButton    *b1 = new mafGUIButton(this, w_idFirst, label1,dp, wxSize(FW/2,BH) );
  b1->SetValidator( mafGUIValidator(this,w_idFirst,b1) );
  b1->SetFont(m_Font);
  mafGUIButton    *b2 = new mafGUIButton(this, w_idSecond, label2, dp, wxSize(FW/2,BH) );
  b2->SetValidator( mafGUIValidator(this,w_idSecond,b2) );
  b2->SetFont(m_Font);

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add( b1, 0);
  sizer->Add( b2, 0);
  Add(sizer,0,wxALL|alignment, M);
}
//----------------------------------------------------------------------------
void mafGUI::OkCancel(int alignment)
//----------------------------------------------------------------------------
{
  mafGUIButton    *b1 = new mafGUIButton(this, wxOK, "ok",dp, wxSize(FW/2,BH) );
  b1->SetValidator( mafGUIValidator(this,wxOK,b1) );
  b1->SetFont(m_Font);
  mafGUIButton    *b2 = new mafGUIButton(this, wxCANCEL, "cancel", dp, wxSize(FW/2,BH) );
  b2->SetValidator( mafGUIValidator(this,wxCANCEL,b2) );
  b2->SetFont(m_Font);

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add( b1, 0);
  sizer->Add( b2, 0);
	Add(sizer,0,wxALL|alignment, M);
}
//----------------------------------------------------------------------------
wxListBox *mafGUI::ListBox(int id,wxString label,int height, wxString tooltip, long lbox_style , int width) //<*> togliere direction
//----------------------------------------------------------------------------
{
	if(width<0)
		width = (label == "") ? FW : DW;
  int w_id = GetWidgetId(id);
	wxListBox *lb = new wxListBox(this, w_id,dp,wxSize(width ,height),0, NULL,lbox_style | m_EntryStyle);  // wxSUNKEN_BORDER non funzia - aggiunge anche il bordino nero
  lb->SetValidator( mafGUIValidator(this,w_id,lb) );
	lb->SetFont(m_Font);
  if(tooltip != "") lb->SetToolTip(tooltip);
  if(label == "")
	{
		Add(lb,0,wxALL, M); 
	}
	else
  {
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
		if(tooltip != "")	
      lab->SetToolTip(tooltip);
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add( lab,  0, wxRIGHT, LM);
		sizer->Add( lb	, 0, wxRIGHT, HM);
		Add(sizer,0,wxALL, M);
  }
	return lb;
}
//----------------------------------------------------------------------------
wxGrid *mafGUI::Grid(int id, wxString label,int height, int row, int cols, wxString tooltip) //<*> le griglie mettono le scrollbar alla vfc
//----------------------------------------------------------------------------//    inoltre ListBox,CheckListBoz e Grid possono avere le scrollbar   
{                                                                             //    che stanno male quando anche il GuiHolder ha la scrollbar
  int width = (label == "") ? FW : DW;
	wxGrid *grid = new wxGrid(this, GetWidgetId(id),dp, wxSize(width,height));
	grid->CreateGrid(row, cols);
	grid->SetFont(m_Font);
  if(tooltip != "") 
    grid->SetToolTip(tooltip);

	grid->EnableEditing(false);
	grid->SetColLabelSize(LH);
	grid->SetLabelSize(wxVERTICAL,LW);
	grid->SetColSize(0,EW);
	grid->SetColSize(1,EW);

	grid->SetColLabelValue(0, "");
	grid->SetColLabelValue(1, "");
	grid->SetRowLabelValue(0, "");
	grid->SetRowLabelValue(1, "");
	
  if(label == "")
	{
		Add(grid,0,wxALL, M); 
	}
	else
  {
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
		if(tooltip != "")	
      lab->SetToolTip(tooltip);
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add( lab,  0, wxRIGHT, LM);
		sizer->Add( grid	, 0, wxRIGHT, HM);
		Add(sizer,0,wxALL, M);
  }
	return grid;
}

//----------------------------------------------------------------------------
mafGUICheckListBox* mafGUI::CheckList(int id,wxString label,int height, wxString tooltip)
//----------------------------------------------------------------------------
{
  int width = (label == "") ? FW : DW;
	mafGUICheckListBox *clb =  new mafGUICheckListBox(this, GetWidgetId(id) ,dp,wxSize(width,height));
  
  clb->SetListener(this);
	if(tooltip != "") 
    clb->SetToolTip(tooltip);

  if(label == "")
	{
		Add(clb, 0, wxALL, LM);
	}
	else
  {
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
		if(tooltip != "")	
      lab->SetToolTip(tooltip);
		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add( lab,  0, wxRIGHT, LM);
		sizer->Add( clb	, 0, wxRIGHT, HM);
		Add(sizer,0,wxALL, M);
  }
	return clb;
}

//----------------------------------------------------------------------------
void mafGUI::VectorN(int id,wxString label, double *var,int num_elem,double min, double max, int decimal_digit, wxString tooltip)
//----------------------------------------------------------------------------
{
	// if the label is empty the available space for widget is FW (full width)
	// if label is non empty the available space for widget is DW = FW-LW (full width minus label width) 
  int space = (label == "") ? FW : DW;

	// the space available for widget is divided in num_elem pieces
	// and widgetS ARE placed in each piece 
  int w = space/num_elem - M;
	
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	if(label != "")
	{
		wxStaticText *lab  = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
  	sizer->Add(lab,0,wxRIGHT,LM);
	}
	for(int i=0;i<num_elem;i++)
	{
    int w_id = GetWidgetId(id);
		wxTextCtrl *tex = new wxTextCtrl  (this, w_id,"",dp,wxSize(w,LH), m_EntryStyle );
    tex->SetFont(m_Font);
		tex->SetValidator( mafGUIValidator(this,w_id,tex,&var[i],min,max,decimal_digit));
		if(tooltip != "") 
      tex->SetToolTip(tooltip);
  	sizer->Add(tex,0,wxRIGHT,HM);
	}
	Add(sizer,0,wxEXPAND,M);
}
//----------------------------------------------------------------------------
void mafGUI::VectorN(int id,wxString label, int *var,int num_elem,int min, int max, wxString tooltip)
//----------------------------------------------------------------------------
{
	// if the label is empty the available space for widget is FW (full width)
	// if label is non empty the available space for widget is DW = FW-LW (full width minus label width) 
  int space = (label == "") ? FW : DW;

	// the space available for widget is divided in num_elem pieces
	// and widgetS ARE placed in each piece 
  int w = space/num_elem - M; 
	
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	if(label != "")
	{
		wxStaticText *lab  = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW,LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
    if(m_UseBackgroundColor) 
      lab->SetBackgroundColour(m_BackgroundColor);
    lab->SetFont(m_Font);
  	sizer->Add(lab,0,wxRIGHT,LM);
	}
	for(int i=0;i<num_elem;i++)
	{
    int w_id = GetWidgetId(id);
		wxTextCtrl *tex = new wxTextCtrl  (this, w_id,"",dp,wxSize(w,LH), m_EntryStyle );
    tex->SetFont(m_Font);
		tex->SetValidator( mafGUIValidator(this,w_id,tex,&var[i],min,max));
		if(tooltip != "") 
      tex->SetToolTip(tooltip);
  	sizer->Add(tex,0,wxRIGHT,HM);
	}
	Add(sizer,0,wxEXPAND,M);
}
//----------------------------------------------------------------------------
mafGUIRollOut *mafGUI::RollOut(int id, mafString title, mafGUI *roll_gui, bool rollOutOpen)
//----------------------------------------------------------------------------
{
  int w_id = GetWidgetId(id);
  mafGUIRollOut *roll = new mafGUIRollOut(this, title, roll_gui, w_id, rollOutOpen);
  roll->SetListener(m_Listener);
  return roll;
}
//----------------------------------------------------------------------------
void mafGUI::OnListBox (wxCommandEvent &event)
//----------------------------------------------------------------------------
{
  wxListBox *lb = (wxListBox *)event.GetEventObject();
  ((mafGUIValidator *)lb->GetValidator())->TransferFromWindow();
  OnEvent(&mafEvent(this, event.GetId(),(long) event.GetInt()));
}
//----------------------------------------------------------------------------
void mafGUI::OnCheckListBox (wxCommandEvent &event)
//----------------------------------------------------------------------------
{
  OnEvent(&mafEvent(this, event.GetId(),(long) event.GetInt()));
}
//----------------------------------------------------------------------------
void mafGUI::Reparent(wxWindow *parent)
//----------------------------------------------------------------------------
{
  assert(parent);
  wxWindow::Reparent(parent);
  this->FitGui();
  this->Update();
  this->Show(true);
}
//----------------------------------------------------------------------------
void mafGUI::GetWidgetValue(long widget_id, WidgetDataType &widget_data)
//----------------------------------------------------------------------------
{
  mafGUIValidator *validator = NULL;
  wxWindow *widget = NULL;
  widget = this->FindWindow(widget_id);

  if(widget == NULL)
    return;

  validator = (mafGUIValidator *)widget->GetValidator();
  if(validator == NULL)
    return;

  validator->GetWidgetData(widget_data);
}
//----------------------------------------------------------------------------
void mafGUI::SetWidgetValue(int id, WidgetDataType &widget_data)
//----------------------------------------------------------------------------
{
  mafGUIValidator *validator = NULL;
  wxWindow *widget = NULL;
  widget = this->FindWindow(id);

  if(widget == NULL)
    return;

  validator = (mafGUIValidator *)widget->GetValidator();
  if(validator == NULL)
    return;

  validator->SetWidgetData(widget_data);

  if(id >= MINID && id <MAXID)
    id = GetModuleId(id);
  mafEventMacro(mafEvent(this, id));
}
