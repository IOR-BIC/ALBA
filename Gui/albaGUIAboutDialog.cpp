/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIAboutDialog
 Authors: Nicola Vanella
 
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


#include <wx/event.h>
#include "wx/busyinfo.h"
#include <wx/settings.h>
#include <wx/laywin.h>
#include <wx/imaglist.h>

#include "albaDecl.h"
#include "albaGUI.h"
#include "albaGUIAboutDialog.h"
#include "albaGUITree.h"
#include "albaGUI.h"
#include "albaGUIHolder.h"
#include "albaPics.h"
#include "albaGUIDialog.h"
#include "wx/msw/registry.h"
#include "albaGUIPicButton.h"
#include "albaGUIButton.h"
#include "albaGUIValidator.h"
#include "albaLogicWithManagers.h"
#include "wx/datetime.h"

//----------------------------------------------------------------------------
// constant :
//----------------------------------------------------------------------------
enum albaGUIAboutDialog_IDS
{
	ID_SHOW_WEBSITE = MINID,
  ID_SHOW_LICENSE,
};

//----------------------------------------------------------------------------
albaGUIAboutDialog::albaGUIAboutDialog(wxString dialog_title)
{  
	m_Title = "";
	m_BuildNum = "";
	m_BuildDate = GetBuildDate();
	m_Version = "";
	m_ImagePath = "";	
	m_WebSiteURL = "";
	m_LicenseURL = "";
	m_ExtraMessage = "";

	m_AboutDialog = NULL;
	m_PreviewImage = NULL;
}

//----------------------------------------------------------------------------
albaGUIAboutDialog::~albaGUIAboutDialog()
{ 
	delete m_PreviewImage;
}

//----------------------------------------------------------------------------
bool albaGUIAboutDialog::ShowModal()
{
	CreateDialog();

  return m_AboutDialog->ShowModal() != 0;
}

//----------------------------------------------------------------------------
void albaGUIAboutDialog::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_SHOW_WEBSITE:
      {
				albaLogicWithManagers::ShowWebSite(m_WebSiteURL);
			}
      break; 
			case ID_SHOW_LICENSE:
			{
				albaLogicWithManagers::ShowWebSite(m_LicenseURL);
			}
			break;
      default:
        e->Log();
      break; 
    }
  }
}

void albaGUIAboutDialog::SetBuildNum(wxString revision)
{
	albaString tmp = revision.c_str();
	tmp.Replace('_', ' ');
	m_BuildNum = tmp.GetCStr();
}
	
//----------------------------------------------------------------------------
void albaGUIAboutDialog::SetWebSite(wxString webSite)
{
	m_WebSiteURL = webSite;
}
//----------------------------------------------------------------------------
void albaGUIAboutDialog::SetLicenseURL(wxString licenseURL)
{
	m_LicenseURL = licenseURL;
}

//----------------------------------------------------------------------------
void albaGUIAboutDialog::SetExtraMessage(wxString message)
{
	m_ExtraMessage = "\n" + message;
}

//----------------------------------------------------------------------------
void albaGUIAboutDialog::CreateDialog()
{
	wxString title = "About ";
	title += m_Title;

	wxString description = m_Title;
	description += "\n";
	
	// Build number
	description += m_BuildNum;

	// Version
	if (!m_Version.IsEmpty())
		description += _(" - Version ") + m_Version;

	// Build Date
	description += m_BuildDate;

	// Copyright
	wxString copyright;
	copyright.Printf("\n\n© %d BIC - RIT - IOR", (1900 + wxDateTime::GetTmNow()->tm_year));
	description += copyright;

	// Extra
	description += m_ExtraMessage;

	//////////////////////////////////////////////////////////////////////////

	if (m_AboutDialog == NULL)
	{
		m_AboutDialog = new albaGUIDialog(title, albaCLOSEWINDOW);

		wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

		// Images
		albaGUIPicButton *previewImageButton;

		int panelWidth = 560;

		if (wxFileExists(m_ImagePath))
		{
			wxBitmap *previewBitmap;

			// Load and show the image
			m_PreviewImage = new wxImage();
			m_PreviewImage->LoadFile(m_ImagePath.c_str(), wxBITMAP_TYPE_ANY);

			previewBitmap = new wxBitmap(*m_PreviewImage);
			previewImageButton = new albaGUIPicButton(m_AboutDialog, previewBitmap, -1);

			panelWidth = m_PreviewImage->GetWidth();

			mainSizer->Add(previewImageButton, 0, wxALL | wxALIGN_CENTER, 0);

			delete previewBitmap;
		}

		// Creating the static text area

		wxBoxSizer *infoTextSizer = new wxBoxSizer(wxHORIZONTAL);

		int borderSize = 10;

		infoTextSizer->Add(AddText(m_AboutDialog, description, panelWidth - (borderSize * 2), wxALIGN_LEFT), 0, wxALL | wxALIGN_LEFT, borderSize);

		mainSizer->Add(infoTextSizer, 0, wxTOP | wxLEFT, 0);

		if (m_LicenseURL != "")
		{
			wxString copyright = "Distributed under";

			wxBoxSizer *licenseTextSizer = new wxBoxSizer(wxHORIZONTAL);

			licenseTextSizer->Add(AddText(m_AboutDialog, copyright, 85, wxALIGN_LEFT), 0, wxALL | wxALIGN_CENTER, 0);

			albaGUIButton *licenseButton = new albaGUIButton(m_AboutDialog, ID_SHOW_LICENSE, "license", wxPoint(-1, -1), wxSize(40, 20));
			licenseButton->SetBackgroundStyle(wxBG_STYLE_COLOUR);
			licenseButton->SetForegroundColour(wxColour(0, 0, 255));
			licenseTextSizer->Add(licenseButton, 0, wxEXPAND | wxLEFT, 0);
			licenseButton->SetListener(this);

			mainSizer->Add(licenseTextSizer, 0, wxTOP | wxLEFT | wxRIGHT | wxEXPAND, borderSize);
		}

		// Creating buttons
		wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);

		if (m_WebSiteURL != "")
		{
			albaGUIButton *siteButton = new albaGUIButton(m_AboutDialog, ID_SHOW_WEBSITE, "Web Site", wxPoint(-1, -1));
			siteButton->SetListener(this);
			buttonSizer->Add(siteButton, 0, wxALIGN_LEFT, 0);
		}

		int buttonWidth = 75;

		wxButton *okButton = new wxButton(m_AboutDialog, wxID_OK, "Ok");

		wxString spacing = " ";
		mainSizer->Add(AddText(m_AboutDialog, spacing, panelWidth - (buttonWidth * 2) - borderSize, wxALIGN_RIGHT), 0, wxALL | wxALIGN_LEFT, 0);
		buttonSizer->Add(AddText(m_AboutDialog, spacing, panelWidth - (buttonWidth * 2) - borderSize, wxALIGN_RIGHT), 0, wxALL | wxALIGN_LEFT, 0);

		buttonSizer->Add(okButton, 0, wxALIGN_RIGHT, 0);
		mainSizer->Add(buttonSizer, 0, wxALL, 5);

		m_AboutDialog->Add(mainSizer, 0, wxALL);
		m_AboutDialog->Fit();

		// Show dialog
		wxSize s = albaGetFrame()->GetSize();
		wxPoint p = albaGetFrame()->GetPosition();
		int posX = p.x + s.GetWidth() * .5 - m_AboutDialog->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - m_AboutDialog->GetSize().GetHeight() * .5;
		m_AboutDialog->SetPosition(wxPoint(posX, posY));
	}
}
//----------------------------------------------------------------------------
wxStaticText* albaGUIAboutDialog::AddText(albaGUIDialog * dialog, wxString &text, int Width, int align)
{
	// Creating the static text area
	wxStaticText* guiLabel = new wxStaticText(dialog, -1, text, wxPoint(-1, -1), wxSize(Width, -1), align | wxST_NO_AUTORESIZE);
	wxFont fixedFont = guiLabel->GetFont();

	// Setting font to fixed size to avoid wx non-sense 
	fixedFont.SetFamily(wxFONTFAMILY_DECORATIVE);
	guiLabel->SetFont(fixedFont);

	return guiLabel;
}

//----------------------------------------------------------------------------
wxString albaGUIAboutDialog::GetBuildDate()
{
	unsigned int month, day, year;

	char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
		"Sep", "Oct", "Nov", "Dec" };

	char temp[] = __DATE__;
	unsigned char i;

	year = atoi(temp + 9);
	*(temp + 6) = 0;
	day = atoi(temp + 4);
	*(temp + 3) = 0;

	for (i = 0; i < 12; i++)
	{
		if (!strcmp(temp, months[i]))
		{
			month = i + 1;
			wxString date = wxString::Format(" %d/%d/%d ", day, month, year);
			return date;
		}
	}

	return "";
}
