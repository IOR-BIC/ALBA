/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLutEditor.cpp,v $
  Language:  C++
  Date:      $Date: 2006-09-12 15:59:01 $
  Version:   $Revision: 1.10 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <wx/image.h>
#include <math.h>

#include "mmgLutEditor.h"
#include "mmgValidator.h"
#include "mmgButton.h"
#include "mmgLutPreset.h"
#include "mmgDialog.h"

//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------
const int  M	= 1;											// margin all around a row
const int LM	= 5;											// label margin
const int HM	= 2*M;										// horizontal margin

const int LH	= 16;											// label/entry height
const int BH	= 20;											// button height

const int LW	= 128; 									  // label width
const int EW	= 50;											// entry width  - era 45
const int FW	= LW+LM+EW+HM+EW+HM+EW;		// full width
const int DW	= EW+HM+EW+HM+EW;					// Data Width - Full Width without the Label

static wxPoint dp = wxDefaultPosition; 
  
//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum LUT_EDITOR_WIDGET_ID
{
  ID_PRESET = MINID,
  ID_NUMENTRY,
  ID_VALUE_RANGE,
  ID_LW,
  ID_CW,
  ID_SHADE_ALPHA,
  ID_SHADE_RGB,
  ID_SHADE_HSV,
  ID_SHADE_ALL,
  ID_OK,
  ID_CANCEL,
  ID_APPLY,
};

//----------------------------------------------------------------------------
// mmgLutEditor
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgLutEditor,wxPanel)
  EVT_COMBOBOX(ID_PRESET, mmgLutEditor::OnComboSelection)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgLutEditor::mmgLutEditor(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)         
//----------------------------------------------------------------------------
{
	m_Listener     = NULL;
  m_ExternalLut = NULL;
  m_NumEntry = 256;
  m_ValueRange[0] = 0;
  m_ValueRange[1] = 100;
  m_Preset = 0;
  m_Info = "" ;

  wxFont bold_font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  bold_font.SetPointSize(9);
  bold_font.SetWeight(wxBOLD);
  
  wxBoxSizer *sizer =  new wxBoxSizer( wxVERTICAL );

  wxBoxSizer   *sz;
  wxStaticText *lab;
  wxTextCtrl   *text;
  wxTextCtrl   *text2;
  wxComboBox   *combo;
  mmgButton    *butt;
  
  lab = new wxStaticText(this, -1, "presets", dp, wxSize(LW,LH), wxALIGN_RIGHT );
	combo = new wxComboBox  (this, ID_PRESET, "", dp, wxSize(DW,-1), lutPresetNum, LutNames, wxCB_READONLY);
	sz = new wxBoxSizer(wxHORIZONTAL);
  sz->Add( lab,  1, wxRIGHT, LM);
	sz->Add( combo,0, wxRIGHT, HM);
  combo->SetValidator( mmgValidator(this,ID_PRESET,combo,&m_Preset) );
	sizer->Add(sz,0,wxALL, M); 

	lab = new wxStaticText (this, -1, "number of entries [1..256]", dp, wxSize(LW,LH), wxALIGN_RIGHT );
	text = new wxTextCtrl  (this, -1, ""   ,        dp, wxSize(DW,LH), wxNO_BORDER  );
	text->SetValidator( mmgValidator(this,ID_NUMENTRY,text,&m_NumEntry,1,256) );
	sz = new wxBoxSizer(wxHORIZONTAL);
	sz->Add( lab,  0, wxRIGHT, LM);
	sz->Add( text, 0, wxRIGHT, HM);
	sizer->Add(sz,0,wxALL, M);

	lab = new wxStaticText (this, -1, "mapped value range",  dp, wxSize(LW,LH), wxALIGN_RIGHT );
	text = new wxTextCtrl  (this, -1, ""   ,        dp, wxSize(DW/2-HM,LH), wxNO_BORDER  );
	text->SetValidator( mmgValidator(this,ID_VALUE_RANGE,text,&m_ValueRange[0], -1e200, 1e200, -1 ) ); //SIL: 17-01-06 - removed number of digits -- introduce errors on big numbers
	text2 = new wxTextCtrl  (this, -1, ""   ,       dp, wxSize(DW/2,LH), wxNO_BORDER);
	text2->SetValidator( mmgValidator(this,ID_VALUE_RANGE,text2,&m_ValueRange[1] ,-1e200, 1e200, -1 ) ); //SIL: 17-01-06 - removed number of digits -- introduce errors on big numbers
  sz = new wxBoxSizer(wxHORIZONTAL);
	sz->Add( lab,   0, wxRIGHT, LM);
	sz->Add( text,  0, wxRIGHT, HM);
	sz->Add( text2, 0, wxRIGHT, HM);
	sizer->Add(sz,0,wxALL, M);

	lab = new wxStaticText (this, -1, "Preview",dp,wxSize(150,-1));
  lab->SetFont(bold_font);
	sizer->Add( lab, 0, wxALL, M);
  m_LutSwatch = new mmgLutSwatch(this, -1, dp,wxSize(286,16));
	sizer->Add( m_LutSwatch, 0, wxALL, 4);

  lab = new wxStaticText (this, -1, "Lookup Table Entries",dp,wxSize(150,-1));
  lab->SetFont(bold_font);
	sizer->Add( lab, 0, wxALL, M);
  
  m_LutWidget = new mmgLutWidget(this,ID_LW);//,dp,wxSize(300,300));
  m_LutWidget->SetListener(this);

  sizer->Add(m_LutWidget, 0, wxALL, M);
  
	lab = new wxStaticText(this, -1, "selected indexes:", dp, wxSize(FW,LH));
	lab->SetValidator( mmgValidator(this,-1,lab,&m_Info) );
  sizer->Add(lab, 0, wxALL, M);
  
	lab = new wxStaticText (this, -1, "Interpolations",dp,wxSize(150,-1));
  lab->SetFont(bold_font);
	sizer->Add( lab, 0, wxALL, M);
	sz = new wxBoxSizer(wxHORIZONTAL);

  butt = new mmgButton(this, ID_SHADE_RGB, "shade in rgba space",dp, wxSize(143,BH) );
	butt->SetValidator( mmgValidator(this,ID_SHADE_RGB,butt) );
	sz->Add( butt, 0, wxRIGHT, 2);

  butt = new mmgButton(this, ID_SHADE_HSV, "shade in hsva space",dp, wxSize(143,BH) );
	butt->SetValidator( mmgValidator(this,ID_SHADE_HSV,butt) );
	sz->Add( butt, 0);
  sizer->Add(sz,0,wxALL, M);

  lab = new wxStaticText (this, -1, "Color Editor",dp,wxSize(150,-1));
  lab->SetFont(bold_font);
	sizer->Add( lab, 0, wxALL, M);

  m_ColorWidget = new mmgColorWidget(this,ID_CW);//,dp,wxSize(300,300));
  m_ColorWidget->SetListener(this);
  sizer->Add(m_ColorWidget, 0, wxALL, M);

  
  if(m_parent->IsKindOf(CLASSINFO(wxDialog)))
  {
	  sz = new wxBoxSizer(wxHORIZONTAL);
    butt = new mmgButton(this, ID_OK, "ok",dp, wxSize(96,BH) );
	  butt->SetValidator( mmgValidator(this,ID_OK,butt) );
	  sz->Add( butt, 0, wxRIGHT, 2);

    butt = new mmgButton(this, ID_CANCEL, "cancel",dp, wxSize(96,BH) );
	  butt->SetValidator( mmgValidator(this,ID_CANCEL,butt) );
	  sz->Add( butt, 0, wxRIGHT, 2);

    butt = new mmgButton(this, ID_APPLY, "apply",dp, wxSize(96,BH) );
	  butt->SetValidator( mmgValidator(this,ID_APPLY,butt) );
	  sz->Add( butt, 0);
    sizer->Add(sz,0,wxALL, M);
  }
  
  /*
  lab = new wxStaticText(this, -1, "", dp, wxSize(192,LH));
	sz = new wxBoxSizer(wxHORIZONTAL);
	sz->Add( lab,  0, wxRIGHT, LM);
  mmgButton *butt = new mmgButton(this, ID_SHADE, "shade range",dp, wxSize(94,BH) );
	butt->SetValidator( mmgValidator(this,ID_SHADE,butt) );
	butt->SetToolTip("shade the colors of the selected lut entries");
	sz->Add( butt,  0, wxRIGHT, LM);
  sizer->Add(sz, 0, wxALL, M);
  */

  this->SetAutoLayout( TRUE );
  this->SetSizer( sizer );
  sizer->Fit(this);
  sizer->SetSizeHints(this);

  m_Lut = vtkLookupTable::New();
  m_Lut->Build();
  m_LutWidget->SetLut(m_Lut);
  m_LutSwatch->SetLut(m_Lut);
  TransferDataToWindow();
}
//----------------------------------------------------------------------------
mmgLutEditor::~mmgLutEditor()
//----------------------------------------------------------------------------
{
  if(m_Lut) 
	{
		m_Lut->Delete();
		m_Lut = NULL;
	}
}
//----------------------------------------------------------------------------
void mmgLutEditor::OnComboSelection(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
	lutPreset( m_Preset, m_Lut);
	m_NumEntry      = m_Lut->GetNumberOfTableValues();
	m_LutWidget->SetLut(m_Lut);
	m_LutSwatch->SetLut(m_Lut);
  TransferDataToWindow();
}

//----------------------------------------------------------------------------
void mmgLutEditor::OnEvent( mafEventBase *event )
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(event))
  {
    switch(e->GetId())
	  {
      case ID_PRESET:
      {
				//Paolo 30/01/2006
				//code moved into OnComboSelection to avoid application stuck on preset selection
      }
      break; 
      case ID_NUMENTRY:
        m_LutWidget->SetNumEntry(m_NumEntry);
		  break; 
      case ID_VALUE_RANGE:
        UpdateInfo();
        m_Lut->SetRange(m_ValueRange[0],m_ValueRange[1]);
      break; 
      case ID_CW:  //color in the color widget has changed
        m_LutWidget->SetSelectionColor(m_ColorWidget->GetColor());
        m_LutSwatch->Refresh();
		  break; 
      case ID_LW:  //the selection in the lut widget has changed 
        UpdateInfo();
        m_ColorWidget->SetColor(m_LutWidget->GetSelectionColor());
		  break; 
      case ID_SHADE_RGB:
        m_LutWidget->ShadeSelectionInRGB();
        m_LutSwatch->Refresh();
      break; 
      case ID_SHADE_HSV:
        m_LutWidget->ShadeSelectionInHSV();
        m_LutSwatch->Refresh();
      break; 
      case ID_OK:
        CopyLut(m_Lut, m_ExternalLut);
        if(m_parent->IsKindOf(CLASSINFO(wxDialog)))
        {
          wxDialog *dlg = (wxDialog *)m_parent;
					wxCommandEvent cmd(0, wxID_OK);
          dlg->OnOK( cmd );
        }
      break; 
      case ID_CANCEL:
        if(m_parent->IsKindOf(CLASSINFO(wxDialog)))
        {
          wxDialog *dlg = (wxDialog *)m_parent;
					wxCommandEvent cmd(0, wxID_CANCEL);
          dlg->OnCancel( cmd );
        }
      break; 
      case ID_APPLY:
        CopyLut(m_Lut, m_ExternalLut);
        mafEventMacro(mafEvent(this,GetId()));
      break; 
      default:
			  e->Log();
		  break; 
    }
  }
}
//----------------------------------------------------------------------------
void mmgLutEditor::UpdateInfo()
//----------------------------------------------------------------------------
{
  int min, max;
  m_LutWidget->GetSelection(&min, &max);
  

  float v1 = m_ValueRange[0] + ( min * (m_ValueRange[1]-m_ValueRange[0]) ) / (m_NumEntry-1);
  float v2 = m_ValueRange[0] + ( max * (m_ValueRange[1]-m_ValueRange[0]) ) / (m_NumEntry-1);
  
  if( min == -1) // no selection
  {
    m_Info = "no entry selected";
  }
  else if (min == max) // single selection
  {
    m_Info = wxString::Format( " index= %d value= %g", min, v1 );
  }
  else // multiple selection
  {
    m_Info = wxString::Format( " index= %d value= %g ... index= %d value= %g", min, v1, max, v2 );
  }
  TransferDataToWindow();  
}
//----------------------------------------------------------------------------
void mmgLutEditor::SetLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  m_ExternalLut = lut;

  // copy the given lut on the internal one
  if(m_ExternalLut != NULL)
  {
    CopyLut(m_ExternalLut, m_Lut);
    m_ValueRange[0] = m_Lut->GetRange()[0];
    m_ValueRange[1] = m_Lut->GetRange()[1];
    m_NumEntry      = m_Lut->GetNumberOfTableValues();
  }
  else
  {
  }

  m_LutWidget->SetLut(m_Lut);
  m_LutSwatch->SetLut(m_Lut);
  TransferDataToWindow();
}
//----------------------------------------------------------------------------
void mmgLutEditor::CopyLut(vtkLookupTable *from, vtkLookupTable *to)
//----------------------------------------------------------------------------
{
  if(from==NULL || to==NULL ) return;
  
  int n = from->GetNumberOfTableValues();
  if(n>256) n=256;
  to->SetNumberOfTableValues(n);
  to->SetRange(from->GetRange());
  for(int i=0; i<n; i++)
  {
    double *rgba;
    rgba = from->GetTableValue(i);
    to->SetTableValue(i,rgba[0],rgba[1],rgba[2],rgba[3]);
  }
}
//----------------------------------------------------------------------------
void mmgLutEditor::ShowLutDialog(vtkLookupTable *lut, mafObserver *listener, int id)
//----------------------------------------------------------------------------
{
  long style = /*wxDIALOG_MODAL |*/ wxDEFAULT_DIALOG_STYLE | /*wxSTAY_ON_TOP |*/ wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN;
  mmgDialog dlg("LutEditor", mafCLOSEWINDOW);
  
  mmgLutEditor *led = new mmgLutEditor(&dlg, id);
  led->SetListener(listener);
  led->SetLut(lut);
  dlg.Add(led,1,wxEXPAND);
  dlg.ShowModal();
}
