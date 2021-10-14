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

#ifndef __albaGUIAboutDialog_H__
#define __albaGUIAboutDialog_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

class albaGUI;
class albaGUIHolder;
class albaGUIBitmaps;
class albaGUIDialog;
class wxImage;

/** 
  class name: albaGUIAboutDialog.
  Represent the About dialog. It shows AppImage, Build version, Info, Buttons to webSite and License.
 */  
class ALBA_EXPORT albaGUIAboutDialog : public albaObserver
{

public:
	/** constructor */
	albaGUIAboutDialog(wxString dialog_title = _("About Dialog"));

	void CreateDialog();

	/** destructor */
	~albaGUIAboutDialog();

	/** Answer to the messages coming from interface. */
	void OnEvent(albaEventBase *alba_event);

	void SetImagePath(wxString imagePath) { m_ImagePath = imagePath; };
	void SetTitle(wxString title) { m_Title = title; };
	void SetBuildNum(wxString revision);
	void SetBuildDate(wxString date) { m_BuildDate = date; };
	void SetVersion(wxString ver) { m_Version = ver; };
	void SetWebSite(wxString webSite);
	void SetLicenseURL(wxString licenseURL);
	void SetExtraMessage(wxString message);

	/** Show the Settings Dialog. */
	bool ShowModal();
	
protected:
	wxStaticText* AddText(albaGUIDialog * dialog, wxString &text, int Width, int align);

	wxString GetBuildDate();

	albaGUIDialog	*m_AboutDialog;
	wxImage				*m_PreviewImage;

	wxString			m_Title;
	wxString			m_BuildNum;
	wxString			m_BuildDate;
	wxString			m_Version;
	wxString			m_ImagePath;
	wxString			m_WebSiteURL;
	wxString			m_LicenseURL;
	wxString			m_ExtraMessage;
};
#endif // _albaGUIAboutDialog_H_
