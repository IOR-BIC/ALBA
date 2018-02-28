/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafLicenceManager.h,v $
Language:  C++
Date:      $Date: 2009-05-19 14:29:52 $
Version:   $Revision: 1.1 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "mafObserver.h"

#ifndef __mafLicenceManager_H__
#define __mafLicenceManager_H__

class mafGUIButton;
class mafGUIDialog;
class mafGUIPicButton;
class mafGUIValidator;
class wxDateTime;
class wxImage;
class wxCalendarCtrl;

class MAF_EXPORT mafLicenceManager : public mafObserver
{
public:

	mafLicenceManager(wxString appName);
	~mafLicenceManager();

	enum licenceStatuses {
		TRIAL_MODE,
		LICENSED_MODE,
		EXPIRED_MODE,
	};

	enum addLicenceStatuses {
		LICENCE_ADDED,
		WRONG_LICENCE,
		BAD_DATE,
		ALREADY_REGISTERED,
	};

#ifdef _DEBUG
	enum CreateNewLicenceStatuses
	{
		WRONG_BASE_STRING,
		WRONG_FULL_STRING,
		WRONG_SECS_IN_MILLENNIUM,
		LICENCE_CREATED,
	};
#endif

	//----------------------------------------------------------------------------
	// Widgets ID's
	enum LicenceManagerEventId
	{
		ID_COPY_KEY = MINID,
		ID_VERIFY_KEY,
		ID_GENERATE_KEY,
	};
	
	/** returns current registration status */
	licenceStatuses GetCurrentMode();

	/*Show Gui Dialog for Registration Licence*/
	void ShowRegistrationDialog();

	/*Show Gui Dialog for Generation Licence (Debug Mode)*/
	void ShowGenerateLicenceDialog();

	/** on event function */
	virtual void OnEvent(mafEventBase *maf_event);

	/** set expire date to the argument, return false if there is no expire date */
	bool mafLicenceManager::GetExpireDate(wxDateTime &dateExpire);
	
	/** sets the listener */
	void SetListener(mafObserver *Listener) { m_Listener = Listener; };
		
	/** Sets CryptKey, if not set the maf default key will be used for enc/decryption */
	void SetCryptKey(wxString cryptKey) { m_CryptKey = cryptKey; }

	/*Set mail to send the registration key. This is showed in RegistrationDialog*/
	void SetRegistrationMail(wxString mail) { m_RegMail = mail; };

	/*Set the image path. This is showed in RegistrationDialog*/
	void SetImagePath(wxString path) { m_RegImagePath = path; };

protected:

	/** encrypts the input string */
	wxString EncryptStr(wxString plainStr);

	/** decrypts the input string, return empty string if the input is not correctly encrypted */
	wxString DecryptStr(wxString plainStr);

#ifdef _DEBUG

	/** creates a new licence string */
	CreateNewLicenceStatuses CreateNewLicence(wxString RegCode, wxDateTime expirationDate, wxString &newLicence);
#endif
	
	/** check a new licence and add is to the current user */
	addLicenceStatuses CheckCreateLicence(wxString registrationString);

	/** returns the number of seconds from 1/1/2000 00:00 to now*/
	unsigned long SecondsInThisMillennium();

	/** license the software to expireDate*/
	void AddLicence(wxDateTime expireDate);
	
	wxString m_AppName;
	wxString m_CryptKey;
	wxString m_RegMail;
	wxString m_RegImagePath;

	mafObserver *m_Listener;

	//GUI

	/*Copy string to clipboard*/
	void ToClipboard(HWND hwnd, const std::string &s);

	wxString				m_FirstKey;
	wxString				m_SecondKey;

	mafGUIDialog		*m_RegistrationDialog;
	mafGUIDialog		*m_GenerateLicenceDialog;

	wxImage					*m_PreviewImage;

	wxTextCtrl			*m_FirstKey_textCtrl;
	wxTextCtrl			*m_Result_textCtrl;
	wxCalendarCtrl	*m_CalendarCtrl;
};
#endif