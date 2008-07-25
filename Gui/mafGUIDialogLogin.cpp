/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDialogLogin.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:23 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGUIDialogLogin.h"
#include "mafDecl.h"

//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum LOGIN_ID
{
  USERNAME_ID = MINID,
  PWD_ID,
  REMEMBER_ME_ID,
};
//----------------------------------------------------------------------------
mafGUIDialogLogin::mafGUIDialogLogin(const wxString& title, long style)
: mafGUIDialog(title, style)
//----------------------------------------------------------------------------
{
  m_RememberMe = 0;
  m_Username = "";
  m_Pwd = "";
  m_UsernameOld = m_Username;
  m_PwdOld = m_Pwd;
  m_InformationsInserted = false;

  m_Gui = new mafGUI(this);
  m_Gui->String(USERNAME_ID,_("user: "), &m_Username);
  m_Gui->String(PWD_ID,_("pwd: "), &m_Pwd, "", false, true);
  m_Gui->Bool(REMEMBER_ME_ID, _("remember me"), &m_RememberMe, 1);
  m_Gui->Divider();

  Add(m_Gui,1);
}
//----------------------------------------------------------------------------
mafGUIDialogLogin::~mafGUIDialogLogin()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIDialogLogin::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case USERNAME_ID:
      m_InformationsInserted = true;
    break;
    case PWD_ID:
      m_InformationsInserted = true;
    break;
    case REMEMBER_ME_ID:
    break;
    case wxID_CANCEL:
    case wxCANCEL:
      if (m_InformationsInserted)
      {
        m_Username = m_UsernameOld;
        m_Pwd = m_PwdOld;
      }
      mafGUIDialog::OnEvent(maf_event);
    break;
    default:
      mafGUIDialog::OnEvent(maf_event);
  }
}
//----------------------------------------------------------------------------
mafString &mafGUIDialogLogin::GetUser()
//----------------------------------------------------------------------------
{
  return m_Username;
}
//----------------------------------------------------------------------------
mafString &mafGUIDialogLogin::GetPwd()
//----------------------------------------------------------------------------
{
  return m_Pwd;
}
//----------------------------------------------------------------------------
void mafGUIDialogLogin::SetUserCredentials(mafString &usename, mafString &pwd, int &remember_me)
//----------------------------------------------------------------------------
{
  if (!usename.IsEmpty())
  {
    if (m_Username != usename)
    {
      m_Username = usename;
    }
    if (m_Pwd != pwd)
    {
      m_Pwd = pwd;
    }
    m_InformationsInserted = true;
  }
  if (m_RememberMe != remember_me)
  {
    m_RememberMe = remember_me;
  }
  m_Gui->Update();
}
