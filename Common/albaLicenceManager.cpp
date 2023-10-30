/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: albaLicenceManager.cpp,v $
Language:  C++
Date:      $Date: 2009-05-19 14:29:52 $
Version:   $Revision: 1.1 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) 2001/2005
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "albaDecl.h"

#include "albaCrypt.h"
#include "albaDefines.h"
#include "albaDefines.h"
#include "albaEventInteraction.h"
#include "albaGUIButton.h"
#include "albaGUIDialog.h"
#include "albaGUIPicButton.h"
#include "albaGUIValidator.h"
#include "albaGUIValidator.h"
#include "albaLicenceManager.h"

#include "wx/calctrl.h"
#include "wx/datetime.h"
#include "wx/datetime.h"
#include "wx/msw/registry.h"
#include "wx/window.h"
#include <wx/bitmap.h>
#include <wx/dir.h>
#include <wx/image.h>


//----------------------------------------------------------------------------
albaLicenceManager::albaLicenceManager(wxString appName)
{
	m_AppName = appName;

	m_RegistrationDialogIsOpened = false;

	m_RegistrationDialog = NULL;
	m_GenerateLicenceDialog = NULL;
	m_PreviewImage = NULL;
	m_LicModality = TIME_LICENCE;

	m_CryptKey = "";
	m_FirstKey = "";
	m_SecondKey = "";
	m_RegMail = "";
	m_RegImagePath = "";
	SetLicScope(licenceScope::USER_LICENCE);
}

//----------------------------------------------------------------------------
albaLicenceManager::~albaLicenceManager()
{
	delete m_PreviewImage;
}

//----------------------------------------------------------------------------
albaLicenceManager::licenceStatuses albaLicenceManager::GetCurrentMode()
{
	wxString regKeyStr = wxString(m_RegistryBaseKey + m_AppName + "-lic");
	wxRegKey RegKey(regKeyStr);
	if (!RegKey.Exists())
	{
		char tmp[255];

		//use first run time to obtain an value that bind licence to the current user
		unsigned long secs = SecondsInThisMillennium();

		sprintf(tmp, "%s-code:%lu", m_AppName.c_str(), secs);
					
		wxString cryptedCode = EncryptStr(tmp);
		
		//create a new regkey to store the encrypted string
		wxLog::EnableLogging(false);
		bool created = RegKey.Create();
		
		if(created)
			RegKey.SetValue("LocalKey", cryptedCode.c_str());
		wxLog::EnableLogging(true);
		
		
		return TRIAL_MODE;
	}
	else
	{
		if (m_LicModality == TIME_LICENCE)
		{
			wxDateTime currentDate, dateExpire;
			bool hasExpire = GetExpireDate(dateExpire);

			if (!hasExpire)
				return TRIAL_MODE;

			currentDate.SetToCurrent();

			if (dateExpire.IsEarlierThan(currentDate))
				return EXPIRED_MODE;
		}
		else //BINARY_LICENCE
		{
			if (!IsRegistred())
				return TRIAL_MODE;
		}
	}

	return LICENSED_MODE;
}

//----------------------------------------------------------------------------
bool albaLicenceManager::GetExpireDate( wxDateTime &dateExpire)
{
	wxString regKeyStr = wxString(m_RegistryBaseKey + m_AppName + "-lic");
	wxRegKey RegKey(regKeyStr);

	if (!RegKey.HasValue("Registered"))
		return false;

	wxString encriptedExpire;
	RegKey.QueryValue("Registered", encriptedExpire);
	wxString decriptedExpire = DecryptStr(encriptedExpire);

	//if decriptedExpire is empty there can be a rewriting tentative so we set an earlier date to
	//return expired licence status
	if (decriptedExpire.empty())
		dateExpire.SetToCurrent().Add(wxDateSpan(0, 0, -1));
	else
		dateExpire.ParseDate(decriptedExpire);

	return true;
}


//----------------------------------------------------------------------------
bool albaLicenceManager::IsRegistred()
{
	wxString regKeyStr = wxString(m_RegistryBaseKey + m_AppName + "-lic");
	wxRegKey RegKey(regKeyStr);

	if (!RegKey.HasValue("Registered"))
		return false;

	wxString encriptedRegistered;
	RegKey.QueryValue("Registered", encriptedRegistered);
	wxString decriptedRegistered = DecryptStr(encriptedRegistered);

	if (decriptedRegistered.empty())
		return false;

	return true;
}


//----------------------------------------------------------------------------
unsigned long albaLicenceManager::SecondsInThisMillennium()
{
	time_t timer;
	struct tm y2k = { 0 };

	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

	time(&timer);   //get current time; same as: timer = time(NULL)  
	return difftime(timer, mktime(&y2k));
}

//----------------------------------------------------------------------------
void albaLicenceManager::AddTimeLicence(wxDateTime expireDate)
{
	wxString cryptedDate = EncryptStr(expireDate.FormatISODate());
	
	wxString regKeyStr = wxString(m_RegistryBaseKey + m_AppName + "-lic");
	wxRegKey RegKey(regKeyStr);
	RegKey.Create();
	RegKey.SetValue("Registered", cryptedDate.c_str());
}

//----------------------------------------------------------------------------
void albaLicenceManager::AddBinaryLicence()
{
	wxString criptedReg = EncryptStr("REGISTERED");

	wxString regKeyStr = wxString(m_RegistryBaseKey + m_AppName + "-lic");
	wxRegKey RegKey(regKeyStr);
	RegKey.Create();
	RegKey.SetValue("Registered", criptedReg.c_str());
}

//----------------------------------------------------------------------------
albaLicenceManager::addLicenceStatuses albaLicenceManager::CheckCreateLicence(wxString registrationString)
{
	wxString decriptedExpire,decriptedKey;
	wxString regKeyStr = wxString(m_RegistryBaseKey + m_AppName + "-lic");
	wxString localKey;
	wxRegKey RegKey(regKeyStr);

	//if decrypt function return an empty string the registration string is wrong
	wxString decripRegString = DecryptStr(registrationString);
	if (decripRegString.empty())
	{
		return WRONG_LICENCE;
	}

	RegKey.QueryValue("LocalKey", localKey);

	//if the decrypted key is empty is possible to have a manumission and we must return wrong licence
	decriptedKey = DecryptStr(localKey.c_str());
	if (decriptedKey.empty())
	{
		return WRONG_LICENCE;
	}

	int locKeyLen = decriptedKey.length();
	int licLen = decripRegString.length();

	//check base licence if does not match the licence was created for another computer/user
	if (strncmp(decriptedKey.c_str(), decripRegString.c_str(), locKeyLen))
	{
		return WRONG_LICENCE;
	}

	//delimitator check
	if (decripRegString[locKeyLen] != '|')
	{
		return WRONG_LICENCE;
	}

	if (m_LicModality == TIME_LICENCE)
	{
		wxString dateString = decripRegString.substr(locKeyLen + 1, licLen);
		wxDateTime RegDate, currentExpDate, currentDate;

		RegDate.ParseDate(dateString.c_str());
		currentDate.SetToCurrent();

		//check dates 
		if (RegDate.IsEarlierThan(currentDate))
			return BAD_DATE;

		//check if the licence was already registered, current expire date >= licence expire date
		bool hasExpire = GetExpireDate(currentExpDate);
		if (hasExpire && (RegDate.IsEarlierThan(currentExpDate) || RegDate.IsEqualTo(currentExpDate)))
			return ALREADY_REGISTERED;
		AddTimeLicence(RegDate);
	}
	else //BINARY_LICENCE
	{
		wxString regString = decripRegString.substr(locKeyLen + 1, licLen);
		if (strcmp(regString.c_str(), "REGISTERED"))
			return WRONG_LICENCE;

		AddBinaryLicence();
	}
	return LICENCE_ADDED;
}

//----------------------------------------------------------------------------
wxString albaLicenceManager::EncryptStr(wxString plainStr)
{
	std::string encStr;

	if (m_CryptKey == "")
		albaDefaultEncryptFromMemory(plainStr.c_str(), encStr);
	else
		albaEncryptFromMemory(plainStr.c_str(), encStr, m_CryptKey.c_str());

	return encStr.c_str();
}

//----------------------------------------------------------------------------
wxString albaLicenceManager::DecryptStr(wxString plainStr)
{
	std::string decStr;

	if (m_CryptKey == "")
		albaDefaultDecryptInMemory(plainStr.c_str(), decStr);
	else
		albaDecryptInMemory(plainStr.c_str(), decStr, m_CryptKey.c_str());

	return decStr.c_str();
}


//----------------------------------------------------------------------------
albaLicenceManager::CreateNewLicenceStatuses albaLicenceManager::CreateNewTimeLicence(wxString RegCode, wxDateTime expirationDate, wxString &newLicence)
{
	wxString decriptedRegCode, encLicence;
	
	decriptedRegCode=DecryptStr(RegCode);
	if (decriptedRegCode.empty())
	{
		return WRONG_BASE_STRING;
	}
		
	char tmp[255];
	sprintf(tmp, "%s-code:", m_AppName.c_str());
	int baseRegLen = strlen(tmp);
	
	wxString baseRegCode = decriptedRegCode.substr(0, baseRegLen);
	wxString secsInMillennium = decriptedRegCode.substr(baseRegLen);

	if (strncmp(tmp, baseRegCode.c_str(), baseRegLen))
		return WRONG_BASE_STRING;

	unsigned long secsInMil;
	sscanf(secsInMillennium.c_str(), "%ud", &secsInMil);

	if (secsInMil < 0 || secsInMil > SecondsInThisMillennium())
		return WRONG_SECS_IN_MILLENNIUM;

	char fullCheckStr[255];
	sprintf(fullCheckStr, "%s%lu", tmp, secsInMil);

	if (strcmp(fullCheckStr, decriptedRegCode.c_str()))
		return WRONG_FULL_STRING;

	char licence[255];
	sprintf(licence, "%s|%s", decriptedRegCode.c_str(), expirationDate.FormatISODate().c_str());

	newLicence = EncryptStr(licence);

	return LICENCE_CREATED;
}

//----------------------------------------------------------------------------
albaLicenceManager::CreateNewLicenceStatuses albaLicenceManager::CreateNewBinaryLicence(wxString RegCode, wxString &newLicence)
{
	wxString decriptedRegCode, encLicence;

	decriptedRegCode = DecryptStr(RegCode);
	if (decriptedRegCode.empty())
	{
		return WRONG_BASE_STRING;
	}

	char tmp[255];
	sprintf(tmp, "%s-code:", m_AppName.c_str());
	int baseRegLen = strlen(tmp);

	wxString baseRegCode = decriptedRegCode.substr(0, baseRegLen);
	wxString secsInMillennium = decriptedRegCode.substr(baseRegLen);

	if (strncmp(tmp, baseRegCode.c_str(), baseRegLen))
		return WRONG_BASE_STRING;

	unsigned long secsInMil;
	sscanf(secsInMillennium.c_str(), "%ud", &secsInMil);

	if (secsInMil < 0 || secsInMil > SecondsInThisMillennium())
		return WRONG_SECS_IN_MILLENNIUM;

	char fullCheckStr[255];
	sprintf(fullCheckStr, "%s%lu", tmp, secsInMil);

	if (strcmp(fullCheckStr, decriptedRegCode.c_str()))
		return WRONG_FULL_STRING;

	char licence[255];
	sprintf(licence, "%s|REGISTERED", decriptedRegCode.c_str());

	newLicence = EncryptStr(licence);

	return LICENCE_CREATED;
}


//GUI - Show Licence Dialog
//----------------------------------------------------------------------------
void albaLicenceManager::ShowRegistrationDialog()
{
	if (m_RegistrationDialogIsOpened)
		HideRegistrationDialog();

	wxString regKeyStr = wxString(m_RegistryBaseKey + m_AppName + "-lic");
	wxRegKey RegKey(regKeyStr);
	if (RegKey.Exists())
	{
		RegKey.QueryValue("LocalKey", m_FirstKey);
	}

	wxString imgPath = m_RegImagePath;
	wxString title = "Register Product";

	// Create Dialog
	if (m_RegistrationDialog == NULL)
	{
		m_RegistrationDialog = new albaGUIDialog(title, albaCLOSEWINDOW);
		m_RegistrationDialog->SetListener(this);

		wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);

		// Add Image
		if (wxFileExists(imgPath))
		{
			wxBitmap *previewBitmap;

			// Load and show the image
			m_PreviewImage = new wxImage();
			m_PreviewImage->LoadFile(imgPath.c_str(), wxBITMAP_TYPE_ANY);

			previewBitmap = new wxBitmap(*m_PreviewImage);
			albaGUIPicButton *previewImageButton = new albaGUIPicButton(m_RegistrationDialog, previewBitmap, -1);

			mainSizer->Add(previewImageButton, 0, wxALL | wxALIGN_CENTER, 3);

			delete previewBitmap;
		}

		int vertPanelWidth = 500;
		wxBoxSizer *mainVertSizer = new wxBoxSizer(wxVERTICAL);
		mainVertSizer->SetMinSize(wxSize(vertPanelWidth, 200));

		//STEP 1 ////////////////////////////////////////////

		int borderSize = 0;

		// TEXT 1
		wxString text1 = "\n Step 1\n  This is your personal code.";

		if (!m_RegMail.IsEmpty())
			text1 += "Copy and send it to " + m_RegMail + " to receive the activation key.\n";

		wxStaticText* staticText1 = new wxStaticText(m_RegistrationDialog, -1, text1, wxPoint(-1, -1), wxSize(vertPanelWidth, 50), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
		mainVertSizer->Add(staticText1, 0, wxALL | wxALIGN_LEFT | wxEXPAND, borderSize);

		// TEXT CTRL - FIRST KEY
		m_FirstKey_textCtrl = new wxTextCtrl(m_RegistrationDialog, NULL, m_FirstKey, wxPoint(-1, -1), wxSize(vertPanelWidth, 50), wxALL | wxEXPAND);
		m_FirstKey_textCtrl->SetEditable(false);
		m_FirstKey_textCtrl->Enable(true);
		m_FirstKey_textCtrl->SelectAll();

		// Merging sizers into dialog
		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxVERTICAL, m_RegistrationDialog, "Your Personal Key");
		labelSizer1->Add(m_FirstKey_textCtrl, 0, wxALL | wxEXPAND, 0);
		mainVertSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		// COPY BUTTON
		albaGUIButton *copyButton = new albaGUIButton(m_RegistrationDialog, ID_COPY_KEY, "Copy", wxPoint(-1, -1)); //Copy to Clipboard
		copyButton->SetListener(this);
		mainVertSizer->Add(copyButton, 0, wxALIGN_RIGHT, 5);

		//STEP 2 ////////////////////////////////////////////

		// TEXT 2
		wxString text2 = " Step 2\n  Paste here the activation key and register.\n";
		wxStaticText* staticText2 = new wxStaticText(m_RegistrationDialog, -1, text2, wxPoint(-1, -1), wxSize(vertPanelWidth, 50), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
		mainVertSizer->Add(staticText2, 0, wxALL | wxALIGN_LEFT | wxEXPAND, borderSize);

		// TEXT CTRL - SECOND KEY
		wxTextCtrl *secondKey_textCtrl = new wxTextCtrl(m_RegistrationDialog, NULL, m_SecondKey, wxPoint(-1, -1), wxSize(vertPanelWidth, 100), wxALL | wxALIGN_LEFT | wxST_NO_AUTORESIZE);
		secondKey_textCtrl->SetValidator(albaGUIValidator(this, NULL, secondKey_textCtrl, &m_SecondKey));
		secondKey_textCtrl->SetEditable(true);
		secondKey_textCtrl->Enable(true);

		// Merging sizers into dialog
		wxStaticBoxSizer *labelSizer2 = new wxStaticBoxSizer(wxVERTICAL, m_RegistrationDialog, "Your Activation Key");
		labelSizer2->Add(secondKey_textCtrl, 0, wxALL | wxEXPAND, 0);
		mainVertSizer->Add(labelSizer2, 0, wxALL | wxEXPAND, 5);

		// VERIFY BUTTON
		albaGUIButton *registerButton = new albaGUIButton(m_RegistrationDialog, ID_VERIFY_KEY, "Register", wxPoint(-1, -1));
		registerButton->SetListener(this);
		mainVertSizer->Add(registerButton, 0, wxALIGN_RIGHT, 5);		

		mainSizer->Add(mainVertSizer, 0, wxTOP | wxLEFT | wxRIGHT | wxEXPAND);

		m_RegistrationDialog->Add(mainSizer, 0, wxALL, 0);
		m_RegistrationDialog->Fit();
	}

	// Show dialog
	wxSize s = albaGetFrame()->GetSize();
	wxPoint p = albaGetFrame()->GetPosition();
	int posX = p.x + s.GetWidth() * .5 - m_RegistrationDialog->GetSize().GetWidth() * .5;
	int posY = p.y + s.GetHeight() * .5 - m_RegistrationDialog->GetSize().GetHeight() * .5;
	m_RegistrationDialog->SetPosition(wxPoint(posX, posY));
	m_RegistrationDialog->ShowModal();

	m_RegistrationDialogIsOpened = true;
}

//----------------------------------------------------------------------------
void albaLicenceManager::HideRegistrationDialog()
{
	m_RegistrationDialog->Hide();
	//m_RegistrationDialog->DissociateHandle();
	//m_RegistrationDialog->Close();

	m_RegistrationDialogIsOpened = false;
}

//----------------------------------------------------------------------------
void albaLicenceManager::ShowGenerateLicenceDialog()
{
	wxString title = "Generate Licence";

	if (m_GenerateLicenceDialog == NULL)
	{
		int panelWidth = 500;

		m_GenerateLicenceDialog = new albaGUIDialog(title, albaCLOSEWINDOW);
		m_GenerateLicenceDialog->SetListener(this);

		wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->SetMinSize(wxSize(panelWidth, 200));

		int borderSize = 0;

		// TEXT CTRL - FIRST KEY
		wxTextCtrl *firstKey_textCtrl = new wxTextCtrl(m_GenerateLicenceDialog, NULL, m_FirstKey, wxPoint(-1, -1), wxSize(panelWidth, 100), wxALL | wxEXPAND);
		firstKey_textCtrl->SetValidator(albaGUIValidator(this, -1, firstKey_textCtrl, &m_FirstKey));
		firstKey_textCtrl->SetEditable(true);
		firstKey_textCtrl->Enable(true);

		// Merging sizers into dialog
		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxVERTICAL, m_GenerateLicenceDialog, "Personal Key");
		labelSizer1->Add(firstKey_textCtrl, 0, wxALL | wxEXPAND, 0);

		mainSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		if (m_LicModality == TIME_LICENCE)
		{
			// TEXT CTRL - DATE
			m_CalendarCtrl = new wxCalendarCtrl(m_GenerateLicenceDialog, -1, wxDateTime::Now());// , wxPoint(-1, -1), wxSize(-1, -1), wxALL | wxALIGN_LEFT | wxST_NO_AUTORESIZE);

			// Merging sizers into dialog		
			wxStaticBoxSizer *labelSizer2 = new wxStaticBoxSizer(wxVERTICAL, m_GenerateLicenceDialog, "Date");
			labelSizer2->Add(m_CalendarCtrl, 0, wxALL | wxEXPAND, 0);

			mainSizer->Add(labelSizer2, 0, wxALL | wxEXPAND, 5);
		}

		// VERIFY BUTTON
		albaGUIButton *generateButton = new albaGUIButton(m_GenerateLicenceDialog, ID_GENERATE_KEY, "Generate", wxPoint(-1, -1));
		generateButton->SetListener(this);
		mainSizer->Add(generateButton, 0, wxALIGN_RIGHT, 5);

		// TEXT CTRL - RESULT
		m_Result_textCtrl = new wxTextCtrl(m_GenerateLicenceDialog, NULL, "", wxPoint(-1, -1), wxSize(panelWidth, 100), wxALL | wxEXPAND);
		m_Result_textCtrl->SetEditable(false);
		m_Result_textCtrl->Enable(true);

		// Merging sizers into dialog
		wxStaticBoxSizer *labelSizer4 = new wxStaticBoxSizer(wxVERTICAL, m_GenerateLicenceDialog, "Result");
		labelSizer4->Add(m_Result_textCtrl, 0, wxALL | wxEXPAND, 0);

		mainSizer->Add(labelSizer4, 0, wxALL | wxEXPAND, 5);

		// CLOSE BUTTON
		albaGUIButton *closeButton = new albaGUIButton(m_GenerateLicenceDialog, wxOK, "Close", wxPoint(-1, -1));
		closeButton->SetListener(this);
		mainSizer->Add(closeButton, 0, wxALIGN_RIGHT, 5);

		m_GenerateLicenceDialog->Add(mainSizer, 0, wxALL, 0);
		m_GenerateLicenceDialog->Fit();
	}

	// Show dialog
	wxSize s = albaGetFrame()->GetSize();
	wxPoint p = albaGetFrame()->GetPosition();
	int posX = p.x + s.GetWidth() * .5 - m_GenerateLicenceDialog->GetSize().GetWidth() * .5;
	int posY = p.y + s.GetHeight() * .5 - m_GenerateLicenceDialog->GetSize().GetHeight() * .5;
	m_GenerateLicenceDialog->SetPosition(wxPoint(posX, posY));
	m_GenerateLicenceDialog->ShowModal();
}

//----------------------------------------------------------------------------
void albaLicenceManager::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case ID_COPY_KEY:
		{
			// Copy key to clipboard
			m_FirstKey_textCtrl->SelectAll();
			albaString value = m_FirstKey_textCtrl->GetValue();

			HWND hwnd = GetDesktopWindow();
			ToClipboard(hwnd, value.GetCStr());

		}break;
		case ID_VERIFY_KEY:
		{
			if (m_SecondKey.Length() > 0)
			{
				wxString message = "Warning";
				wxString title;
				long style = wxOK;

				switch (CheckCreateLicence(m_SecondKey))
				{
				case addLicenceStatuses::LICENCE_ADDED:
				{
					title = "Licence Added!";
					message = "Congratulation, you have successfully activated this product.\n";

					if (m_LicModality == TIME_LICENCE)
					{
						// Show Expiration Date and Calculate days left
						wxDateTime exDate;
						GetExpireDate(exDate);

						char temp[20];
						sprintf(temp, "%02d-%02d-%d", exDate.GetDay(), exDate.GetMonth() + 1, exDate.GetYear());
						wxString expirationDate = temp;

						wxString expirationDateStatus = m_AppName + " Licensed User Expiration date: " + expirationDate;

						message += "\n" + expirationDateStatus;
					}
					else
					{
						message += "\n";
					}
					
					message += "\n\nPlease restart the application to apply the update.";

					style = wxOK;
				}
				break;
				case addLicenceStatuses::ALREADY_REGISTERED:
				{
					title = "Warning";
					message = "Licence is already registered.\nIf you need further assistance please do not hesitate to contact us.";
					style = wxOK | wxICON_WARNING;
				}
				break;
				case addLicenceStatuses::BAD_DATE:
				{
					title = "Error";
					message = "Licence is already expired.\nIf you need further assistance please do not hesitate to contact us.";
					style = wxOK | wxICON_WARNING;
				}
				break;
				case addLicenceStatuses::WRONG_LICENCE:
				{
					title = "Error";
					message = "Wrong Licence!\nIf you need further assistance please do not hesitate to contact us.";
					style = wxOK | wxICON_ERROR;
				}
				break;
				}
				
				int res = wxMessageBox(message, title, style, NULL);

				if (res == wxOK)
					HideRegistrationDialog();
			}
		}
		break;
		case ID_GENERATE_KEY:
		{
			int result;
			if (m_LicModality == TIME_LICENCE)
				result = CreateNewTimeLicence(m_FirstKey, m_CalendarCtrl->GetDate(), m_SecondKey);
			else //BINARY_LICENCE
				result = CreateNewBinaryLicence(m_FirstKey, m_SecondKey);

			m_Result_textCtrl->SetValue(m_SecondKey);
			m_Result_textCtrl->SelectAll();

			wxString resutlMessage = "";
			if (result == CreateNewLicenceStatuses::LICENCE_CREATED) resutlMessage = "Licence Created";
			else if (result == CreateNewLicenceStatuses::WRONG_BASE_STRING) resutlMessage = "Wrong Personal key, check it\n cause: Wong Base String";
			else if (result == CreateNewLicenceStatuses::WRONG_FULL_STRING) resutlMessage = "Wrong Personal key, check it\n cause: Wrong Full String";
			else if (result == CreateNewLicenceStatuses::WRONG_SECS_IN_MILLENNIUM) resutlMessage = "Wrong Personal key, check it\n cause: Wrong Secs in Millennium";

			wxMessageBox(resutlMessage, "Result", wxOK, NULL);
		}
		break;
		default:
			albaEventMacro(*e);
			break;
		}
	}
}

//----------------------------------------------------------------------------
void albaLicenceManager::SetLicScope(licenceScope scope)
{
	if (scope == licenceScope::USER_LICENCE)
		m_RegistryBaseKey = "HKEY_CURRENT_USER\\Software\\";
	else
		m_RegistryBaseKey = "HKEY_LOCAL_MACHINE\\Software\\";
}

//----------------------------------------------------------------------------
void albaLicenceManager::ToClipboard(HWND hwnd, const std::string &s)
{
	OpenClipboard(hwnd);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}
