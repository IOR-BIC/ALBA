/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLutEditor.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-19 15:24:34 $
  Version:   $Revision: 1.2 $
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
enum 
{
  ID_FOO = MINID,
  ID_PRESET,
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
mmgLutEditor::mmgLutEditor(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)         
//----------------------------------------------------------------------------
{
	m_Listener     = NULL;
  m_external_lut = NULL;
  m_numEntry = 256;
  m_valueRange[0] = 0;
  m_valueRange[1] = 100;
  m_preset = 0;
  m_info = "" ;

  wxFont bold_font = wxFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
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
  combo->SetValidator( mmgValidator(this,ID_PRESET,combo,&m_preset) );
	sizer->Add(sz,0,wxALL, M); 

	lab = new wxStaticText (this, -1, "number of entries [1..256]", dp, wxSize(LW,LH), wxALIGN_RIGHT );
	text = new wxTextCtrl  (this, -1, ""   ,        dp, wxSize(DW,LH), wxNO_BORDER  );
	text->SetValidator( mmgValidator(this,ID_NUMENTRY,text,&m_numEntry,1,256) );
	sz = new wxBoxSizer(wxHORIZONTAL);
	sz->Add( lab,  0, wxRIGHT, LM);
	sz->Add( text, 0, wxRIGHT, HM);
	sizer->Add(sz,0,wxALL, M);

	lab = new wxStaticText (this, -1, "mapped value range",  dp, wxSize(LW,LH), wxALIGN_RIGHT );
	text = new wxTextCtrl  (this, -1, ""   ,        dp, wxSize(DW/2-HM,LH), wxNO_BORDER  );
	text->SetValidator( mmgValidator(this,ID_VALUE_RANGE,text,&m_valueRange[0]) );
	text2 = new wxTextCtrl  (this, -1, ""   ,       dp, wxSize(DW/2,LH), wxNO_BORDER  );
	text2->SetValidator( mmgValidator(this,ID_VALUE_RANGE,text2,&m_valueRange[1]) );
  sz = new wxBoxSizer(wxHORIZONTAL);
	sz->Add( lab,   0, wxRIGHT, LM);
	sz->Add( text,  0, wxRIGHT, HM);
	sz->Add( text2, 0, wxRIGHT, HM);
	sizer->Add(sz,0,wxALL, M);

	lab = new wxStaticText (this, -1, "Preview",dp,wxSize(150,-1));
  lab->SetFont(bold_font);
	sizer->Add( lab, 0, wxALL, M);
  m_ls = new mmgLutSwatch(this, -1, dp,wxSize(286,16));
	sizer->Add( m_ls, 0, wxALL, 4);

  lab = new wxStaticText (this, -1, "Lookup Table Entries",dp,wxSize(150,-1));
  lab->SetFont(bold_font);
	sizer->Add( lab, 0, wxALL, M);
  
  m_lw = new mmgLutWidget(this,ID_LW);//,dp,wxSize(300,300));
  m_lw->SetListener(this);

  sizer->Add(m_lw, 0, wxALL, M);
  
	lab = new wxStaticText(this, -1, "selected indexes:", dp, wxSize(FW,LH));
	lab->SetValidator( mmgValidator(this,-1,lab,&m_info) );
  sizer->Add(lab, 0, wxALL, M);
  
	lab = new wxStaticText (this, -1, "Interpolations",dp,wxSize(150,-1));
  lab->SetFont(bold_font);
	sizer->Add( lab, 0, wxALL, M);
	sz = new wxBoxSizer(wxHORIZONTAL);

  butt = new mmgButton(this, ID_SHADE_RGB, "shade colors in rgba space",dp, wxSize(143,BH) );
	butt->SetValidator( mmgValidator(this,ID_SHADE_RGB,butt) );
	sz->Add( butt, 0, wxRIGHT, 2);

  butt = new mmgButton(this, ID_SHADE_HSV, "shade colors in hsva space",dp, wxSize(143,BH) );
	butt->SetValidator( mmgValidator(this,ID_SHADE_HSV,butt) );
	sz->Add( butt, 0);
  sizer->Add(sz,0,wxALL, M);

  lab = new wxStaticText (this, -1, "Color Editor",dp,wxSize(150,-1));
  lab->SetFont(bold_font);
	sizer->Add( lab, 0, wxALL, M);

  m_cw = new mmgColorWidget(this,ID_CW);//,dp,wxSize(300,300));
  m_cw->SetListener(this);
  sizer->Add(m_cw, 0, wxALL, M);

  
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

  m_lut = vtkLookupTable::New();
  m_lut->Build();
  m_lw->SetLut(m_lut);
  m_ls->SetLut(m_lut);
  TransferDataToWindow();
}
//----------------------------------------------------------------------------
mmgLutEditor::~mmgLutEditor()
//----------------------------------------------------------------------------
{
  if(m_lut) m_lut->Delete();
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
        lutPreset( m_preset, m_lut);
        m_numEntry      = m_lut->GetNumberOfTableValues();
        m_lw->SetLut(m_lut);
        m_ls->SetLut(m_lut);
        TransferDataToWindow();
      }
      break; 
      case ID_NUMENTRY:
        m_lw->SetNumEntry(m_numEntry);
		  break; 
      case ID_VALUE_RANGE:
        UpdateInfo();
        m_lut->SetRange(m_valueRange[0],m_valueRange[1]);
      break; 
      case ID_CW:  //color in the color widget has changed
        m_lw->SetSelectionColor(m_cw->GetColor());
        m_ls->Refresh();
		  break; 
      case ID_LW:  //the selection in the lut widget has changed 
        UpdateInfo();
        m_cw->SetColor(m_lw->GetSelectionColor());
		  break; 
      case ID_SHADE_RGB:
        m_lw->ShadeSelectionInRGB();
        m_ls->Refresh();
      break; 
      case ID_SHADE_HSV:
        m_lw->ShadeSelectionInHSV();
        m_ls->Refresh();
      break; 
      case ID_OK:
        CopyLut(m_lut, m_external_lut);
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
        CopyLut(m_lut, m_external_lut);
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
  m_lw->GetSelection(&min, &max);
  

  float v1 = m_valueRange[0] + ( min * (m_valueRange[1]-m_valueRange[0]) ) / (m_numEntry-1);
  float v2 = m_valueRange[0] + ( max * (m_valueRange[1]-m_valueRange[0]) ) / (m_numEntry-1);
  
  if( min == -1) // no selection
  {
    m_info = "no entry selected";
  }
  else if (min == max) // single selection
  {
    m_info = wxString::Format( " index= %d value= %g", min, v1 );
  }
  else // multiple selection
  {
    m_info = wxString::Format( " index= %d value= %g ... index= %d value= %g", min, v1, max, v2 );
  }
  TransferDataToWindow();  
}
//----------------------------------------------------------------------------
void mmgLutEditor::SetLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  m_external_lut = lut;

  // copy the given lut on the internal one
  if(m_external_lut != NULL)
  {
    CopyLut(m_external_lut, m_lut);
    m_valueRange[0] = m_lut->GetRange()[0];
    m_valueRange[1] = m_lut->GetRange()[1];
    m_numEntry      = m_lut->GetNumberOfTableValues();
  }
  else
  {
  }

  m_lw->SetLut(m_lut);
  m_ls->SetLut(m_lut);
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
void mmgLutEditor::ShowLutDialog(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  long style = wxDIALOG_MODAL | wxDEFAULT_DIALOG_STYLE /*| wxTHICK_FRAME*/ | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN;
  wxDialog *dlg = new wxDialog(NULL, -1, "LutEditor", wxDefaultPosition, wxSize(300,670),	style);
  
  mmgLutEditor *led = new mmgLutEditor(dlg,-1);
  led->SetLut(lut);
  dlg->ShowModal();
  delete dlg;
}
