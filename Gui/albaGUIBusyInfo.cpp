/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIButton
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

#include "albaGUIBusyInfo.h"
#include "wx/busyinfo.h"

std::vector<albaGUIBusyInfo*> albaGUIBusyInfo::glo_BusyList;

//----------------------------------------------------------------------------
albaGUIBusyInfo::albaGUIBusyInfo(const wxString& message, bool textMode)
{
	glo_BusyList.push_back(this); 
	m_Message = message;
	m_TextMode = textMode;
	m_WxBusy = NULL;
	Show();
}

//----------------------------------------------------------------------------
albaGUIBusyInfo::~albaGUIBusyInfo()
{
	//Remove the pointer to this object in the Busy List
	std::vector<albaGUIBusyInfo*>::iterator it = std::find(glo_BusyList.begin(), glo_BusyList.end(), this);
	if (it != glo_BusyList.end()) {
		glo_BusyList.erase(it);
	}

	Hide();
}

//----------------------------------------------------------------------------
void albaGUIBusyInfo::Show()
{
	if (m_TextMode)
		printf(m_Message.ToAscii());
	else if (m_WxBusy == NULL)
			m_WxBusy = new wxBusyInfo(m_Message);
}

//----------------------------------------------------------------------------
void albaGUIBusyInfo::Hide()
{
	cppDEL(m_WxBusy);
}

//----------------------------------------------------------------------------
void albaGUIBusyInfo::UpdateText(const wxString& message)
{
	m_Message = message;

	if (m_TextMode)
		printf(message.ToAscii());
	else if (m_WxBusy)
	{
		Hide();
		Show();
	}
}

//----------------------------------------------------------------------------
void albaGUIBusyInfo::ShowAll()
{
	for (int i = 0; i < glo_BusyList.size(); i++)
		glo_BusyList[i]->Show();
}

//----------------------------------------------------------------------------
void albaGUIBusyInfo::HideAll()
{
	for (int i = 0; i < glo_BusyList.size(); i++)
		glo_BusyList[i]->Hide();
}
