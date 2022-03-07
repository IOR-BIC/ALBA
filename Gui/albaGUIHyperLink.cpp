/*=========================================================================
Program:   AssemblerPro
Module:     albaGUIHyperLink.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaGUIHyperLink.h"

#include "math.h"
#include "wx/event.h"
#include "albaLogicWithManagers.h"

BEGIN_EVENT_TABLE(albaGUIHyperLink, wxControl)
  EVT_LEFT_DOWN(albaGUIHyperLink::OnMouse)	
	EVT_LEFT_UP(albaGUIHyperLink::OnMouse)
	EVT_LEAVE_WINDOW(albaGUIHyperLink::OnMouse)
	EVT_ENTER_WINDOW(albaGUIHyperLink::OnMouse)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUIHyperLink::albaGUIHyperLink(wxWindow *parent, wxWindowID id, const wxString& label, const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/, long style /*= 0*/, const wxString& name /*= wxStaticTextNameStr*/)
{
	this->Initialize();

	m_Listener = NULL;
	m_ValueString = label;
	m_URL = "";

	this->SetForegroundColour(wxColor(0, 0, 255));

	Create(parent, id, label, pos, size, style, name);

	wxFont underlineFont = wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL, true);
	this->SetFont(underlineFont);
	
}

//----------------------------------------------------------------------------
void albaGUIHyperLink::Initialize()
{

}

//----------------------------------------------------------------------------
void albaGUIHyperLink::OnMouse(wxMouseEvent &event)
{
	if (event.Entering())
	{
		this->SetForegroundColour(wxColor(0, 0, 155));
		SetLabel(m_ValueString);
	}
	else if (event.Leaving())
	{
		this->SetForegroundColour(wxColor(0, 0, 255));
		SetLabel(m_ValueString);
	}
	else if (event.ButtonDown())
	{
		this->SetForegroundColour(wxColor(255, 255, 0));
		SetLabel(m_ValueString);
	}
	else if (event.ButtonUp())
	{
		this->SetForegroundColour(wxColor(0, 0, 255));
		SetLabel(m_ValueString);

		if (m_URL != "")
			albaLogicWithManagers::ShowWebSite(m_URL);
	}
}
