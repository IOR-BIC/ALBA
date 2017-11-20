/*=========================================================================

 Program: MAF2
 Module: mafGUIAboutDialog
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIAboutDialog_H__
#define __mafGUIAboutDialog_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

class mafGUI;
class mafGUIHolder;
class mafGUIBitmaps;
class mafGUIDialog;

/** 
  class name: mafGUIAboutDialog.
  Represent the About dialog. It shows AppImage, Build version, Info, Buttons to webSite and License.
 */  
class MAF_EXPORT mafGUIAboutDialog : public mafObserver
{

public:
	/** constructor */
	mafGUIAboutDialog(wxString dialog_title = _("Application Settings"));

	void CreateDialog();

	/** destructor */
	~mafGUIAboutDialog();

	/** Answer to the messages coming from interface. */
	void OnEvent(mafEventBase *maf_event);

	void SetImagePath(wxString imagePath) { m_ImagePath = imagePath; };
	void SetTitle(wxString title) { m_Title = title; };
	void SetRevision(wxString revision) { m_Revision = revision; };
	void SetWebSite(wxString webSite);
	void SetLicenseURL(wxString licenseURL);

	/** Show the Settings Dialog. */
	bool ShowModal();

protected:
	wxStaticText* AddText(mafGUIDialog * dialog, wxString &text, int Width, int align);

	mafGUIDialog *m_AboutDialog;

	wxString			m_Title;
	wxString			m_Revision;
	wxString			m_ImagePath;
	wxString			m_WebSiteURL;
	wxString			m_LicenseURL;

};
#endif // _mafGUIAboutDialog_H_
