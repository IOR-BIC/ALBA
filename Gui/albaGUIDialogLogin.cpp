/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDialogLogin
 Authors: Paolo Quadrani
 
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

#include "albaGUIDialogLogin.h"
#include "albaDecl.h"

//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum LOGIN_ID
{
  USERNAME_ID = MINID,
  PWD_ID,
  REMEMBER_ME_ID,
  PROXY_FLAG_ID,
  PROXY_HOST_ID,
  PROXY_PORT_ID,
};
//----------------------------------------------------------------------------
albaGUIDialogLogin::albaGUIDialogLogin(const wxString& title, long style)
: albaGUIDialog(title, style)
//----------------------------------------------------------------------------
{
  m_RememberMe = 0;
  m_Username = "";
  m_Pwd = "";
  m_UsernameOld = m_Username;
  m_PwdOld = m_Pwd;
  m_ProxyFlag = 0;
  m_ProxyHost = "";
  m_ProxyPort = 0;
  m_InformationsInserted = false;

  m_Gui = new albaGUI(this);
  m_Gui->String(USERNAME_ID,_("user: "), &m_Username);
  m_Gui->String(PWD_ID,_("pwd: "), &m_Pwd, "", false, true);
  m_Gui->Bool(PROXY_FLAG_ID,_("using proxy"),&m_ProxyFlag,1);
  m_Gui->String(PROXY_HOST_ID,_("Host"),&m_ProxyHost,"");
  m_Gui->Integer(PROXY_PORT_ID,_("Port"),&m_ProxyPort,1);
  m_Gui->Bool(REMEMBER_ME_ID, _("remember me"), &m_RememberMe, 1);
  m_Gui->Divider();

  EnableItems();
  Add(m_Gui,1);
}
//----------------------------------------------------------------------------
albaGUIDialogLogin::~albaGUIDialogLogin()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIDialogLogin::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
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
      albaGUIDialog::OnEvent(alba_event);
    break;
    default:
      albaGUIDialog::OnEvent(alba_event);
  }
  EnableItems();
}
//----------------------------------------------------------------------------
void albaGUIDialogLogin::EnableItems()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(PROXY_HOST_ID,m_ProxyFlag != 0);
  m_Gui->Enable(PROXY_PORT_ID,m_ProxyFlag != 0);
}
//----------------------------------------------------------------------------
albaString &albaGUIDialogLogin::GetUser()
//----------------------------------------------------------------------------
{
  return m_Username;
}
//----------------------------------------------------------------------------
albaString &albaGUIDialogLogin::GetPwd()
//----------------------------------------------------------------------------
{
  return m_Pwd;
}
//----------------------------------------------------------------------------
albaString &albaGUIDialogLogin::GetProxyHost()
//----------------------------------------------------------------------------
{
  return m_ProxyHost;
}
//----------------------------------------------------------------------------
int albaGUIDialogLogin::GetProxyPort()
//----------------------------------------------------------------------------
{ 
  return m_ProxyPort;
}
//----------------------------------------------------------------------------
void albaGUIDialogLogin::SetUserCredentials(albaString &usename, albaString &pwd, int &proxyFlag, albaString &proxyHost, int &proxyPort, int &remember_me)
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
    if (!proxyHost.IsEmpty() && m_ProxyHost != proxyHost)
    {
      m_ProxyHost = proxyHost; 
    }
    if (m_ProxyPort != proxyPort)
    {
      m_ProxyPort = proxyPort; 
    }
    if (m_ProxyFlag != proxyFlag)
    {
      m_ProxyFlag = proxyFlag; 
    }
    m_InformationsInserted = true;
  }
  if (m_RememberMe != remember_me)
  {
    m_RememberMe = remember_me;
  }
  EnableItems();
  m_Gui->Update();
}
