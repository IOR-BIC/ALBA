/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDialogWarnAndSkipOthers
 Authors: Gianluigi Crimi
 
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

#include "albaGUIDialogWarnAndSkipOthers.h"
#include "albaDecl.h"

//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum LOGIN_ID
{
  ID_REMEMBER = MINID,
};
//----------------------------------------------------------------------------
albaGUIDialogWarnAndSkipOthers::albaGUIDialogWarnAndSkipOthers(const wxString title,const wxString message, int* remember)
: albaGUIDialog(title, albaOK)
{
  m_Gui = new albaGUI(this);
	m_Gui->Label("");
  m_Gui->Label(message, true, true);
  m_Gui->Label("");
  m_Gui->Divider(1);
  m_Gui->Bool(ID_REMEMBER, "Don't show this message again", remember, 1);
  Add(m_Gui,1);

	SetMinSize(wxSize(500, 120));
	SetSize(wxSize(500, 120));
	m_DialogSizer->Fit(this);
 }
//----------------------------------------------------------------------------
albaGUIDialogWarnAndSkipOthers::~albaGUIDialogWarnAndSkipOthers()
{
}
