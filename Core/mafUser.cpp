/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafUser.cpp,v $
Language:  C++
Date:      $Date: 2007-12-19 12:07:19 $
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

#include "mafUser.h"
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>

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
  login_dialog.SetUserCredentials(m_Username, m_Password, m_RememberCredentials);
  int result = login_dialog.ShowModal();
  if(result != wxID_OK) return wxID_OK;
  m_Username = login_dialog.GetUser();
  m_Password = login_dialog.GetPwd();
  m_RememberCredentials = login_dialog.GetRememberUserCredentials();
  UpdateUserCredentialsFile();
  return wxID_CANCEL;
}
//----------------------------------------------------------------------------
bool mafUser::CheckUserCredentials()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    InitializeUserInformations();
  }
  bool res;
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
void mafUser::InitializeUserInformations()
//----------------------------------------------------------------------------
{
  // if not initialized, m_UserHome will be initialized to the LocalSettings user folder.
  InitUserInfoHome();

  mafString credentials = "";
  if (wxFileExists(m_UserInfoFile.GetCStr()))
  {
    char usr[256];
    std::ifstream f_in;
    f_in.open(m_UserInfoFile.GetCStr(), std::ifstream::in| std::ifstream::binary);
#ifdef MAF_USE_CRYPTO
    while(!f_in.eof())
    {
      f_in.getline(usr,256);
      credentials << usr;
    }
    std::string decrypted_input;
    mafDefaultDecryptInMemory(credentials.GetCStr(), decrypted_input);
    credentials = decrypted_input.c_str();
#else
    f_in.getline(usr,256);
    credentials << usr;
    while(!f_in.eof())
    {
      credentials << "\n";
      f_in.getline(usr, 256);
      credentials << usr;
    }
#endif
    f_in.close();
  }

  if (credentials.Length() > 0)
  {
    mafString usr;
    mafString pwd;

    wxStringTokenizer tkz(credentials.GetCStr(), "\n");
    usr = tkz.GetNextToken().c_str();
    if (tkz.HasMoreTokens())
    {
      pwd = tkz.GetNextToken().c_str();
    }
    m_Username = usr;
    m_Password = pwd;
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
bool mafUser::SetCredentials(mafString &name, mafString &pwd, int &remember_me)
//----------------------------------------------------------------------------
{
  m_Username = name;
  m_Password = pwd;
  m_RememberCredentials = remember_me;
  
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
    mafString credentials;
    credentials = m_Username;
    credentials << "\n";
    credentials << m_Password;
#ifdef MAF_USE_CRYPTO
    std::string encrypted_output;
    mafDefaultEncryptFromMemory(credentials.GetCStr(), credentials.Length(), encrypted_output);
    credentials = encrypted_output.c_str();
#endif
    std::ofstream f_out;
    f_out.open(m_UserInfoFile.GetCStr(), std::ofstream::out | std::ofstream::binary);
    f_out.write(credentials.GetCStr(), credentials.Length());
    f_out.close();
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
