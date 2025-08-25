/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIBusyInfo
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIBusyInfo_H__
#define __albaGUIBusyInfo_H__
//----------------------------------------------------------------------------
// Include:


//----------------------------------------------------------------------------
// Forwarded references:
class wxBusyInfo;

class ALBA_EXPORT albaGUIBusyInfo 
{
public:
	albaGUIBusyInfo(const wxString& message, bool textMode = false);
	~albaGUIBusyInfo();

	/** Hide the busy info message-box */
	void Show();

	/** Hide the busy info message-box */
	void Hide();

	void UpdateText(const wxString& message);

	/** Show all current busy info message-box */
	static void ShowAll();

	/** Hide all current busy info message-box */
	static void HideAll();
	
protected:
	wxBusyInfo* m_WxBusy;
	wxString m_Message;
	bool m_TextMode;
	static std::vector<albaGUIBusyInfo *> glo_BusyList;
};

#endif  // __albaGUIBusyInfo_H__
