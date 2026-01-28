/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDialogAskAndRemember
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

#include "albaGUIDialogAskAndRemember.h"
#include "albaDecl.h"

//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum LOGIN_ID
{
  ID_CHOICE = MINID,
  ID_REMEMBER,
};
//----------------------------------------------------------------------------
albaGUIDialogAskAndRemember::albaGUIDialogAskAndRemember(const wxString& title, wxString& message, wxString choices[], int choicesNum, int* choice, int* remember)
: albaGUIDialog(title, albaOK)
{

  m_Gui = new albaGUI(this);
  m_Gui->Label(message, true, true);
  m_Gui->Divider();
  m_Gui->Combo(ID_CHOICE,"", choice, choicesNum,choices); 
  m_Gui->Label("");
  if(remember != NULL)
    m_Gui->Bool(ID_REMEMBER, "Remember choice", remember, 1);
  Add(m_Gui,1);
  if (remember != NULL)
  {
    wxStaticText *guiLabel = new wxStaticText(this, -1, " (You can change this options from Edit->Settings...)", wxPoint(-1, -1), wxSize(500, -1), wxST_NO_AUTORESIZE);
    Add(guiLabel);
  }

}
//----------------------------------------------------------------------------
albaGUIDialogAskAndRemember::~albaGUIDialogAskAndRemember()
{
}
