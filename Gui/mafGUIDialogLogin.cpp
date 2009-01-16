/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDialogLogin.cpp,v $
  Language:  C++
  Date:      $Date: 2009-01-16 09:24:06 $
  Version:   $Revision: 1.2.2.2 $
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
  PROXY_FLAG_ID,
  PROXY_HOST_ID,
  PROXY_PORT_ID,
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
  m_ProxyFlag = 0;
  m_ProxyHost = "";
  m_ProxyPort = 0;
  m_InformationsInserted = false;

  m_Gui = new mafGUI(this);
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
  EnableItems();
}
//----------------------------------------------------------------------------
void mafGUIDialogLogin::EnableItems()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(PROXY_HOST_ID,m_ProxyFlag != 0);
  m_Gui->Enable(PROXY_PORT_ID,m_ProxyFlag != 0);
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
mafString &mafGUIDialogLogin::GetProxyHost()
//----------------------------------------------------------------------------
{
  return m_ProxyHost;
}
//----------------------------------------------------------------------------
int mafGUIDialogLogin::GetProxyPort()
//----------------------------------------------------------------------------
{ 
  return m_ProxyPort;
}
//----------------------------------------------------------------------------
void mafGUIDialogLogin::SetUserCredentials(mafString &usename, mafString &pwd, int &proxyFlag, mafString &proxyHost, int &proxyPort, int &remember_me)
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
