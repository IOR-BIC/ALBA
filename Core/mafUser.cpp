/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafUser.cpp,v $
Language:  C++
Date:      $Date: 2009-01-15 11:06:52 $
Version:   $Revision: 1.7.2.2 $
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

#include "mafUser.h"
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/ffile.h>

#include "mafDecl.h"
#include "mafGUIDialogLogin.h"
#include "mafCrypt.h"

#include <fstream>

//----------------------------------------------------------------------------
mafUser::mafUser()
//----------------------------------------------------------------------------
{
  m_Username = "";
  m_Password = "";
  m_UserHome = "";
  m_ProxyHost = "";
  m_ProxyPort = 0;
  m_ProxyFlag = 0;
  m_UserInfoFile = "/.usrInfo";
  m_Initialized = false;
  m_RememberCredentials = false;
}
//----------------------------------------------------------------------------
mafUser::~mafUser()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
int mafUser::ShowLoginDialog()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    InitializeUserInformations();
  }
  mafGUIDialogLogin login_dialog(_("User authentication"));
  login_dialog.SetUserCredentials(m_Username, m_Password, m_ProxyFlag, m_ProxyHost, m_ProxyPort, m_RememberCredentials);
  int result = login_dialog.ShowModal();
  if(result != wxID_OK) return wxID_CANCEL;
  m_Username = login_dialog.GetUser();
  m_Password = login_dialog.GetPwd();
  m_ProxyFlag = login_dialog.GetProxyFlag();
  m_ProxyHost = login_dialog.GetProxyHost();
  m_ProxyPort = login_dialog.GetProxyPort();
  m_RememberCredentials = login_dialog.GetRememberUserCredentials();
  UpdateUserCredentialsFile();
  return wxID_OK;
}
//----------------------------------------------------------------------------
bool mafUser::CheckUserCredentials()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    InitializeUserInformations();
  }
  bool res = true;
  if (m_Username.IsEmpty() && m_Password.IsEmpty())
  {
    res = ShowLoginDialog() != wxID_CANCEL;
  }
  // Check only username and not password.
  // This can be valid for anonymous (or guest) user without password
  res = !m_Username.IsEmpty() && res;
  return res;
}
//----------------------------------------------------------------------------
bool mafUser::IsAuthenticated()
//----------------------------------------------------------------------------
{
  return false;
}

//----------------------------------------------------------------------------
void mafUser::InitializeUserInformations()
//----------------------------------------------------------------------------
{
  // if not initialized, m_UserHome will be initialized to the LocalSettings user folder.
  InitUserInfoHome();

  wxString credentials = "";
  if (wxFileExists(m_UserInfoFile.GetCStr()))
  {
#ifdef MAF_USE_CRYPTO
    bool decrypt_success = false;
    std::string decrypt_credentials;
    decrypt_credentials.clear();
    decrypt_success = mafDefaultDecryptFileInMemory(m_UserInfoFile.GetCStr(), decrypt_credentials);
    if (!decrypt_success)

    {
      mafLogMessage(_("Error on Decryption!!"));
      return;
    }
    credentials = decrypt_credentials.c_str();
#else
    wxFFile f_in(m_UserInfoFile.GetCStr(), "r");
    f_in.ReadAll(&credentials);
    f_in.Close();
#endif
  }

  if (credentials.Length() > 0)
  {
    mafString usr;
    mafString pwd;
    mafString proxyHost;
    mafString proxyPort;
    mafString useProxy;
    
    wxStringTokenizer tkz(credentials, "\n");
    usr = tkz.GetNextToken().c_str();
    if (tkz.HasMoreTokens())
    {
      pwd = tkz.GetNextToken().c_str();
    }
    if (tkz.HasMoreTokens())
    {
      proxyHost = tkz.GetNextToken().c_str();
    }
    if (tkz.HasMoreTokens())
    {
      proxyPort = tkz.GetNextToken().c_str();
    }
    if (tkz.HasMoreTokens())
    {
      useProxy = tkz.GetNextToken().c_str();
    }

    m_Username = usr;
    m_Password = pwd;
    m_ProxyHost = proxyHost;
    m_ProxyPort = atoi(proxyPort.GetCStr());
    m_ProxyFlag = atoi(useProxy.GetCStr());
    m_RememberCredentials = 1;
  }
  else
  {
    m_Username = wxGetUserName().c_str();
    m_Password = "";
    m_RememberCredentials = 0;
  }

  m_Initialized = true;
}
//----------------------------------------------------------------------------
bool mafUser::SetCredentials(mafString &name, mafString &pwd, int &proxyFlag,  mafString &proxyHost, mafString &proxyPort, int &remember_me)
//----------------------------------------------------------------------------
{
  m_Username = name;
  m_Password = pwd;
  m_RememberCredentials = remember_me;
  m_ProxyFlag = proxyFlag;
  m_ProxyHost = proxyHost;
  m_ProxyPort = atoi(proxyPort);
  
  // empty username is not accepted!!
  m_Initialized = !m_Username.IsEmpty();
  if (m_Initialized) 
  {
    if (m_UserHome.IsEmpty())
    {
      InitUserInfoHome();
    }
    UpdateUserCredentialsFile();
  }
  return m_Initialized;
}
//----------------------------------------------------------------------------
mafString &mafUser::GetName()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    InitializeUserInformations();
  }
  return m_Username;
}
//----------------------------------------------------------------------------
mafString &mafUser::GetPwd()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    InitializeUserInformations();
  }
  return m_Password;
}

//----------------------------------------------------------------------------
mafString &mafUser::GetProxyHost()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    InitializeUserInformations();
  }
  return m_ProxyHost;
}

//----------------------------------------------------------------------------
int &mafUser::GetProxyPort()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    InitializeUserInformations();
  }
  return m_ProxyPort;
}

//----------------------------------------------------------------------------
int &mafUser::GetProxyFlag()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    InitializeUserInformations();
  }
  return m_ProxyFlag;
}

//----------------------------------------------------------------------------
int mafUser::GetRememberUserCredentials()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    InitializeUserInformations();
  }
  return m_RememberCredentials;
}
//----------------------------------------------------------------------------
void mafUser::UpdateUserCredentialsFile()
//----------------------------------------------------------------------------
{
  if (m_RememberCredentials != 0)
  {
    wxString credentials;
    credentials = m_Username;
    credentials << "\n";
    credentials << m_Password;
    credentials << "\n";
    credentials << m_ProxyHost;
    credentials << "\n";
    credentials << m_ProxyPort;
    credentials << "\n";
    credentials << m_ProxyFlag;
#ifdef MAF_USE_CRYPTO
    bool encrypt_success = false;
    encrypt_success = mafDefaultEncryptFileFromMemory(credentials.c_str(), credentials.Length(), m_UserInfoFile.GetCStr());
    if (!encrypt_success)
    {
      mafLogMessage(_("Error on Encryption!!"));
    }
#else
    wxFFile f_out(m_UserInfoFile.GetCStr(), "w");
    f_out.Write(credentials);
    f_out.Close();
#endif
  }
  else
  {
    wxRemoveFile(m_UserInfoFile.GetCStr());
  }
}
//----------------------------------------------------------------------------
void mafUser::InitUserInfoHome()
//----------------------------------------------------------------------------
{
  wxStandardPaths std_paths;
  m_UserHome = std_paths.GetUserLocalDataDir().c_str();
  if (!wxDirExists(m_UserHome.GetCStr()))
  {
    wxMkdir(m_UserHome.GetCStr());
  }
  m_UserInfoFile = m_UserHome;
  m_UserInfoFile << "/.usrInfo";
}
