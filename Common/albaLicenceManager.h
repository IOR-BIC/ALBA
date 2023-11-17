/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: albaLicenceManager.h,v $
Language:  C++
Date:      $Date: 2009-05-19 14:29:52 $
Version:   $Revision: 1.1 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "albaDefines.h"
#include "albaObserver.h"

#ifndef __albaLicenceManager_H__
#define __albaLicenceManager_H__

class albaGUIButton;
class albaGUIDialog;
class albaGUIPicButton;
class albaGUIValidator;
class wxDateTime;
class wxImage;
class wxCalendarCtrl;

class ALBA_EXPORT albaLicenceManager : public albaObserver
{
public:

	albaLicenceManager(wxString appName);
	~albaLicenceManager();

	enum licenceStatuses {
		TRIAL_MODE,
		LICENSED_MODE,
		EXPIRED_MODE,
	};

	enum licenceModalities {
		TIME_LICENCE,
		BINARY_LICENCE,
	};

	enum licenceScope
	{
		USER_LICENCE,
		MACHINE_LICENCE,
	};

	enum addLicenceStatuses {
		LICENCE_ADDED,
		WRONG_LICENCE,
		BAD_DATE,
		ALREADY_REGISTERED,
	};

	enum CreateNewLicenceStatuses
	{
		WRONG_BASE_STRING,
		WRONG_FULL_STRING,
		WRONG_SECS_IN_MILLENNIUM,
		LICENCE_CREATED,
	};

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

	/*Hide Gui Dialog for Registration Licence*/
	void HideRegistrationDialog();

	/*Show Gui Dialog for Generation Licence */
	void ShowGenerateLicenceDialog();

	/** on event function */
	virtual void OnEvent(albaEventBase *alba_event);

	/** set expire date to the argument, return false if there is no expire date */
	bool albaLicenceManager::GetExpireDate(wxDateTime &dateExpire);

	/** set expire date to the argument, return false if there is no expire date */
	bool albaLicenceManager::IsRegistred();

	/** sets the listener */
	void SetListener(albaObserver *Listener) { m_Listener = Listener; };
		
	/** Sets CryptKey, if not set the alba default key will be used for enc/decryption */
	void SetCryptKey(wxString cryptKey) { m_CryptKey = cryptKey; }

	/*Set mail to send the registration key. This is showed in RegistrationDialog*/
	void SetRegistrationMail(wxString mail) { m_RegMail = mail; };

	/*Set the image path. This is showed in RegistrationDialog*/
	void SetImagePath(wxString path) { m_RegImagePath = path; };


	/** Returns LicModality */
	albaLicenceManager::licenceModalities GetLicModality() const { return m_LicModality; }

	/** Sets LicModality */
	void SetLicModality(albaLicenceManager::licenceModalities licModality) { m_LicModality = licModality; }

	void SetLicScope(albaLicenceManager::licenceScope scope);

protected:

	/** encrypts the input string */
	wxString EncryptStr(wxString plainStr);

	/** decrypts the input string, return empty string if the input is not correctly encrypted */
	wxString DecryptStr(wxString plainStr);



	/** creates a new licence string */
	CreateNewLicenceStatuses CreateNewTimeLicence(wxString RegCode, wxDateTime expirationDate, wxString &newLicence);

	/** creates a new licence string */
	CreateNewLicenceStatuses CreateNewBinaryLicence(wxString RegCode, wxString &newLicence);
	
	/** check a new licence and add is to the current user */
	addLicenceStatuses CheckCreateLicence(wxString registrationString);

	/** returns the number of seconds from 1/1/2000 00:00 to now*/
	unsigned long SecondsInThisMillennium();

	/** license the software to expireDate*/
	void AddTimeLicence(wxDateTime expireDate);

	/** license the software to expireDate*/
	void AddBinaryLicence();

	wxString m_AppName;
	wxString m_CryptKey;
	wxString m_RegistryBaseKey;
	wxString m_RegMail;
	wxString m_RegImagePath;

	albaObserver *m_Listener;

	//GUI

	bool m_RegistrationDialogIsOpened;

	/*Copy string to clipboard*/
	void ToClipboard(HWND hwnd, const std::string &s);

	wxString				m_FirstKey;
	wxString				m_SecondKey;

	albaGUIDialog		*m_RegistrationDialog;
	albaGUIDialog		*m_GenerateLicenceDialog;

	wxImage					*m_PreviewImage;

	wxTextCtrl			*m_FirstKey_textCtrl;
	wxTextCtrl			*m_Result_textCtrl;
	wxCalendarCtrl	*m_CalendarCtrl;

	licenceModalities m_LicModality;
};
#endif