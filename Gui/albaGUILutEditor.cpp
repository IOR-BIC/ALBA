/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILutEditor
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
#include <wx/image.h>
#include <math.h>

#include "albaGUILutEditor.h"
#include "albaGUIValidator.h"
#include "albaGUIButton.h"
#include "albaGUILutPreset.h"
#include "albaGUIDialog.h"



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
  ID_USER_PRESET,
  ID_ADD_TO_ULIB,
  ID_REMOVE_FROM_ULIB,
  ID_NEW_USER_LUT_NAME,
};

//----------------------------------------------------------------------------
// albaGUILutEditor
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUILutEditor,wxPanel)
  //EVT_COMBOBOX(ID_PRESET, albaGUILutEditor::OnComboSelection)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUILutEditor::albaGUILutEditor(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
:wxPanel(parent,id,pos,size,style)         
//----------------------------------------------------------------------------
{
	m_Listener     = NULL;
  m_ExternalLut = NULL;
  m_NumEntry = 256;
  m_ValueRange[0] = 0;
  m_ValueRange[1] = 100;
  m_Preset = 0;
  m_UserPreset = 0;
  m_NewUserLutName = "New User Preset";
  m_Info = "" ;
  
  m_UserLutLibraryDir = (albaGetAppDataDirectory()+"\\UserDefinedLookupTables\\");

  if (wxDirExists(m_UserLutLibraryDir))
  {
    albaLogMessage("User lut library found! Loading...");
  } 
  else
  {
    albaLogMessage("User lut library not found! Creating new one...");
    wxMkdir(m_UserLutLibraryDir);
  }

  assert(wxDirExists(m_UserLutLibraryDir));

  m_UserLutLibrary = new albaLUTLibrary;
  m_UserLutLibrary->SetDir(m_UserLutLibraryDir);
  m_UserLutLibrary->Load();

	wxArrayString presetsLutNames;
	presetsLutNames.Add("Current Lut");

	for (int id = 0; id < lutPresetNum; id++) 
	{ 
		presetsLutNames.Add(LutNames[id]);
	}


  int userLutPresetNum = m_UserLutLibrary->GetNumberOfLuts();
  
  std::vector<std::string> lutNames;
  m_UserLutLibrary->GetLutNames(lutNames);

  wxArrayString userLutNames;

  userLutPresetNum++;
  userLutNames.Add("");//To avoid bug of wx , if there is only one lut in combo box it's impossible to choise it

  for (int id = 0; id < userLutPresetNum-1; id++) 
  { 
    userLutNames.Add(lutNames[id].c_str());
  }
  
  wxFont bold_font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  bold_font.SetPointSize(9);
  bold_font.SetWeight(wxBOLD);
  
  wxBoxSizer *sizer =  new wxBoxSizer( wxVERTICAL );

  wxBoxSizer   *sz;
  wxStaticText *lab;
  wxTextCtrl   *text;
  wxTextCtrl   *text2;
  albaGUIButton    *butt;
  
  lab = new wxStaticText(this, -1, _("Presets LUT"), dp, wxSize(LW,LH), wxALIGN_RIGHT );
	m_PresetCombo = new wxComboBox(this, ID_PRESET, "", dp, wxSize(DW,-1),  presetsLutNames, wxCB_READONLY);
  sz = new wxBoxSizer(wxHORIZONTAL);
  sz->Add( lab,  1, wxRIGHT, LM);
	sz->Add( m_PresetCombo,0, wxRIGHT, HM);
  m_PresetCombo->SetValidator( albaGUIValidator(this,ID_PRESET,m_PresetCombo,&m_Preset) );
	
	sizer->Add(sz,0,wxALL, M); 

	lab = new wxStaticText (this, -1, _("number of entries [1..256]"), dp, wxSize(LW,LH), wxALIGN_RIGHT );
	text = new wxTextCtrl  (this, -1, ""   ,        dp, wxSize(DW,LH), wxNO_BORDER  );
	text->SetValidator( albaGUIValidator(this,ID_NUMENTRY,text,&m_NumEntry,1,256) );
	sz = new wxBoxSizer(wxHORIZONTAL);
	sz->Add( lab,  0, wxRIGHT, LM);
	sz->Add( text, 0, wxRIGHT, HM);
	sizer->Add(sz,0,wxALL, M);

	lab = new wxStaticText (this, -1, _("mapped value range"),  dp, wxSize(LW,LH), wxALIGN_RIGHT );
	text = new wxTextCtrl  (this, -1, ""   ,        dp, wxSize(DW/2-HM,LH), wxNO_BORDER  );
	text->SetValidator( albaGUIValidator(this,ID_VALUE_RANGE,text,&m_ValueRange[0], -1e200, 1e200, -1 ) ); //SIL: 17-01-06 - removed number of digits -- introduce errors on big numbers
	text2 = new wxTextCtrl  (this, -1, ""   ,       dp, wxSize(DW/2,LH), wxNO_BORDER);
	text2->SetValidator( albaGUIValidator(this,ID_VALUE_RANGE,text2,&m_ValueRange[1] ,-1e200, 1e200, -1 ) ); //SIL: 17-01-06 - removed number of digits -- introduce errors on big numbers
  sz = new wxBoxSizer(wxHORIZONTAL);
	sz->Add( lab,   0, wxRIGHT, LM);
	sz->Add( text,  0, wxRIGHT, HM);
	sz->Add( text2, 0, wxRIGHT, HM);
	sizer->Add(sz,0,wxALL, M);

  //wxStaticText *labelUserPresets;
  //labelUserPresets = new wxStaticText (this, -1, _("User Presets"),dp,wxSize(150,-1));
  //labelUserPresets->SetFont(bold_font);
  // sizer->Add( labelUserPresets, 0, wxALL, M);

  albaGUIButton  *buttonSetUserLUT;
  buttonSetUserLUT = new albaGUIButton(this, ID_ADD_TO_ULIB, _("Set"),dp, wxSize(LW,BH) );
  buttonSetUserLUT->SetValidator( albaGUIValidator(this,ID_ADD_TO_ULIB,buttonSetUserLUT) );

  wxTextCtrl *textNewUserLUTName;
  textNewUserLUTName = new wxTextCtrl  (this, -1, ""   ,        dp, wxSize(DW,LH), wxNO_BORDER  );
  textNewUserLUTName->SetValidator( albaGUIValidator(this,ID_NEW_USER_LUT_NAME,textNewUserLUTName,&m_NewUserLutName) );


  wxBoxSizer   *setUserLutHSizer;
  setUserLutHSizer = new wxBoxSizer(wxHORIZONTAL);
  setUserLutHSizer->Add( buttonSetUserLUT,0, wxRIGHT, LM);
  setUserLutHSizer->Add( textNewUserLUTName,  1, wxRIGHT, HM);
  sizer->Add(setUserLutHSizer,0,wxALL, M);

  
  albaGUIButton  *buttonRemoveFromUserLUTS;
  buttonRemoveFromUserLUTS = new albaGUIButton(this, ID_REMOVE_FROM_ULIB, _("Remove"),dp, wxSize(DW,BH) );
  buttonRemoveFromUserLUTS->SetValidator( albaGUIValidator(this,ID_REMOVE_FROM_ULIB,buttonRemoveFromUserLUTS) );  

  wxStaticText *staticTextSelectUserLut = new wxStaticText (this, -1, _("select user LUT"), dp, wxSize(LW,LH), wxALIGN_RIGHT );
  m_UserPresetCombo = new wxComboBox(this, ID_USER_PRESET, "", dp, wxSize(DW,-1), userLutNames, wxCB_READONLY);

  wxBoxSizer   *selectUserLutsHSizer;

  selectUserLutsHSizer = new wxBoxSizer(wxHORIZONTAL);
  selectUserLutsHSizer->Add( staticTextSelectUserLut,  1, wxRIGHT, LM);
  selectUserLutsHSizer->Add( m_UserPresetCombo,0, wxRIGHT, HM);
  m_UserPresetCombo->SetValidator( albaGUIValidator(this,ID_USER_PRESET,m_UserPresetCombo,&m_UserPreset) );
  sizer->Add(selectUserLutsHSizer,0,wxRIGHT, HM);
  
  wxStaticText *staticTextRemoveUserLut = new wxStaticText (this, -1, _("remove selected LUT"), dp, wxSize(LW,BH), wxALIGN_RIGHT );

  wxSizer *removeFromUserLutsHSizer = new wxBoxSizer(wxHORIZONTAL);
  removeFromUserLutsHSizer->Add( staticTextRemoveUserLut,  1, wxRIGHT, LM);
  removeFromUserLutsHSizer->Add( buttonRemoveFromUserLUTS,0, wxRIGHT, HM);
  sizer->Add(removeFromUserLutsHSizer,0,wxRIGHT, HM);

  wxStaticText* div = new wxStaticText(this, -1, "",dp, wxSize(FW, 2), 0);
  sizer->Add(div,0,wxALL, M);


  //lab = new wxStaticText (this, -1, _("Preview"),dp,wxSize(150,-1));
  //lab->SetFont(bold_font);
  // 	sizer->Add( lab, 0, wxALL, M);
  m_LutSwatch = new albaGUILutSwatch(this, -1, dp,wxSize(286,16));
	sizer->Add( m_LutSwatch, 0, wxALL, 4);

//  lab = new wxStaticText (this, -1, _("Lookup Table Entries"),dp,wxSize(150,-1));
//  lab->SetFont(bold_font);
// 	sizer->Add( lab, 0, wxALL, M);
  
  m_LutWidget = new albaGUILutWidget(this,ID_LW);//,dp,wxSize(300,300));
  m_LutWidget->SetListener(this);

  sizer->Add(m_LutWidget, 0, wxALL, M);
  
	lab = new wxStaticText(this, -1, _("selected indexes:"), dp, wxSize(FW,LH));
	lab->SetValidator( albaGUIValidator(this,-1,lab,&m_Info) );
  sizer->Add(lab, 0, wxALL, M);
  
	//lab = new wxStaticText (this, -1, _("Interpolations"),dp,wxSize(150,-1));
 // lab->SetFont(bold_font);
// 	sizer->Add( lab, 0, wxALL, M);
	sz = new wxBoxSizer(wxHORIZONTAL);

  butt = new albaGUIButton(this, ID_SHADE_RGB, _("shade in rgba space"),dp, wxSize(143,BH) );
	butt->SetValidator( albaGUIValidator(this,ID_SHADE_RGB,butt) );
	sz->Add( butt, 0, wxRIGHT, 2);

  butt = new albaGUIButton(this, ID_SHADE_HSV, _("shade in hsva space"),dp, wxSize(143,BH) );
	butt->SetValidator( albaGUIValidator(this,ID_SHADE_HSV,butt) );
	sz->Add( butt, 0);
  sizer->Add(sz,0,wxALL, M);

  //lab = new wxStaticText (this, -1, _("Color Editor"),dp,wxSize(150,-1));
  //lab->SetFont(bold_font);
	  // sizer->Add( lab, 0, wxALL, M);

  m_ColorWidget = new albaGUIColorWidget(this,ID_CW);//,dp,wxSize(300,300));
  m_ColorWidget->SetListener(this);
  sizer->Add(m_ColorWidget, 0, wxALL, M);
  
  if(m_parent->IsKindOf(CLASSINFO(wxDialog)))
  {
	  sz = new wxBoxSizer(wxHORIZONTAL);
    butt = new albaGUIButton(this, ID_OK, _("ok"),dp, wxSize(96,BH) );
	  butt->SetValidator( albaGUIValidator(this,ID_OK,butt) );
	  sz->Add( butt, 0, wxRIGHT, 2);

    butt = new albaGUIButton(this, ID_CANCEL, _("cancel"),dp, wxSize(96,BH) );
	  butt->SetValidator( albaGUIValidator(this,ID_CANCEL,butt) );
	  sz->Add( butt, 0, wxRIGHT, 2);

    butt = new albaGUIButton(this, ID_APPLY, _("apply"),dp, wxSize(96,BH) );
	  butt->SetValidator( albaGUIValidator(this,ID_APPLY,butt) );
	  sz->Add( butt, 0);
    sizer->Add(sz,0,wxALL, M);
  }

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
albaGUILutEditor::~albaGUILutEditor()
//----------------------------------------------------------------------------
{
  cppDEL(m_UserLutLibrary);

  if(m_Lut) 
	{
		m_Lut->Delete();
		m_Lut = NULL;
	}
}
//----------------------------------------------------------------------------
/*void albaGUILutEditor::OnComboSelection(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
	UpdateLut();
  //TransferDataToWindow();
}*/
//----------------------------------------------------------------------------
void albaGUILutEditor::UpdateLut()
//----------------------------------------------------------------------------
{
	if(m_Preset>0)
	{
		lutPreset(m_Preset-1, m_Lut);
	}
	else
	{
		CopyLut(m_ExternalLut,m_Lut);
	}
	m_Lut->Build();
	UpdateWidgetsOnLutChange();
}
//----------------------------------------------------------------------------
void albaGUILutEditor::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
	  {

      case ID_NEW_USER_LUT_NAME:
      {
        albaLogMessage("ID_NEW_USER_LUT_NAME");
      }
      break;

      case ID_ADD_TO_ULIB:
      {
        albaLogMessage("ID_ADD_TO_ULIB");
        m_UserLutLibrary->Add(m_Lut, m_NewUserLutName.char_str());
        int id = m_UserPresetCombo->FindString(m_NewUserLutName.char_str());
        if (id != -1)
        {
          // the item  already exists and it will not be added to the combo
          return;
        }
        wxString sel = m_UserPresetCombo->GetString(id) ;
        m_UserPresetCombo->Append(m_NewUserLutName);
      }
      break;

      case ID_REMOVE_FROM_ULIB:
      {
        if (m_UserPresetCombo->GetSelection() == 0)
        {
          return;
        }

        albaLogMessage("ID_REMOVE_FROM_ULIB");
        m_UserLutLibrary->Add(m_Lut, m_NewUserLutName.char_str());
        int id = m_UserPresetCombo->GetSelection();
        wxString sel = m_UserPresetCombo->GetString(id) ;
        m_UserPresetCombo->Delete(id);
        m_UserLutLibrary->Remove(sel.char_str());

      }
      break;

      case ID_USER_PRESET:
      {
        if (m_UserPresetCombo->GetSelection() == 0)
        {
          return;
        }

        albaLogMessage("ID_USER_PRESET");
        int id = m_UserPresetCombo->GetSelection();
        wxString sel = m_UserPresetCombo->GetString(id) ;
        albaLogMessage(sel.char_str());
				double range[2];
				range[0]=m_Lut->GetRange()[0];
				range[1]=m_Lut->GetRange()[1];
				m_Lut->DeepCopy(m_UserLutLibrary->GetLutByName(sel.char_str()));
				m_Lut->SetRange(range);
				
        m_NewUserLutName = sel.char_str();
        UpdateWidgetsOnLutChange();
      }
      break;

      case ID_PRESET:
      {
        if (m_UserPresetCombo->GetSelection() != 0)
        {
          m_UserPreset = 0;
       	  m_UserPresetCombo->SetSelection(0);
          m_UserPresetCombo->Update();

          this->Update();
        }

        UpdateLut();
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
			    dlg->EndModal(wxID_OK);
        }
      break; 
      case ID_CANCEL:
        if(m_parent->IsKindOf(CLASSINFO(wxDialog)))
        {
          wxDialog *dlg = (wxDialog *)m_parent;
					dlg->EndModal(wxID_CANCEL);
        }
      break; 
      case ID_APPLY:
        CopyLut(m_Lut, m_ExternalLut);
        albaEventMacro(albaEvent(this,GetId()));
      break; 
      default:
			  e->Log();
		  break; 
    }
  }
}
//----------------------------------------------------------------------------
void albaGUILutEditor::UpdateInfo()
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
    m_Info = albaString::Format( " index= %d value= %g", min, v1 );
  }
  else // multiple selection
  {
    m_Info = albaString::Format( " index= %d value= %g ... index= %d value= %g", min, v1, max, v2 );
  }
  TransferDataToWindow();  
}
//----------------------------------------------------------------------------
void albaGUILutEditor::SetLut(vtkLookupTable *lut)
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

	int lutPos=presetsIdxByLut(m_Lut);
	m_Preset=lutPos+1;
	
  m_LutWidget->SetLut(m_Lut);
  m_LutSwatch->SetLut(m_Lut);
  TransferDataToWindow();
}
//----------------------------------------------------------------------------
void albaGUILutEditor::CopyLut(vtkLookupTable *from, vtkLookupTable *to)
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
void albaGUILutEditor::ShowLutDialog(vtkLookupTable *lut, albaObserver *listener, int id)
//----------------------------------------------------------------------------
{
  long style = wxDEFAULT_DIALOG_STYLE | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN;
  albaGUIDialog dlg(_("LutEditor"), albaCLOSEWINDOW);
  
  int x_init,y_init;
  x_init = albaGetFrame()->GetPosition().x;
  y_init = albaGetFrame()->GetPosition().y;

  albaGUILutEditor *led = new albaGUILutEditor(&dlg, id);
  led->SetListener(listener);
  led->SetLut(lut);
  dlg.Add(led,1,wxEXPAND);
  dlg.SetSize(x_init,y_init,-1,-1);
  dlg.ShowModal();
}

void albaGUILutEditor::UpdateWidgetsOnLutChange()
{
  m_NumEntry = m_Lut->GetNumberOfTableValues();
  m_ValueRange[0] = m_Lut->GetRange()[0];
  m_ValueRange[1] = m_Lut->GetRange()[1];
  m_LutWidget->SetLut(m_Lut);
  m_LutSwatch->SetLut(m_Lut);
  TransferDataToWindow();
}
