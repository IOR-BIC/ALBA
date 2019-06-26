/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaUser
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaUser_H__
#define __albaUser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaString.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** albaUser - Used to manage username and password; store them and give basic function to check user's credentials.
This is intended as starting point for user managing. Method for Check User Credentials can be redefined to be
customized at application level according to custom requests.
*/
class ALBA_EXPORT albaUser
{
public:
  albaUser();
  virtual ~albaUser();

  /** Allows to show login dialog and get credentials from the application's user.*/
  int ShowLoginDialog();

  /** Function to be customized at application level.
  By default open Login Dialog if the user did not inserted any information.*/
  virtual bool CheckUserCredentials();

  /** Function to be customized at application level.
  Must return true if user is authenticated*/
  virtual bool IsAuthenticated();

  /** Set the user's credentials. 
  Return 'true' if the users' credentials are correctly initialized, otherwise 'flase' value is returned. */
  bool SetCredentials(albaString &name, albaString &pwd, int &proxyFlag, albaString &proxyHost, albaString &proxyPort, int &remember_me);

  /** Return the information on user name.*/
  albaString &GetName();

  /** Return the information on users' password.*/
  albaString &GetPwd();

  /** Return the information on proxy host.*/
  albaString &GetProxyHost();

  /** Return the information on proxy port.*/
  int &GetProxyPort();

  /** Return the information on proxy flag.*/
  int &GetProxyFlag();

  /** Return information to allow saving or not users' information.*/
  int GetRememberUserCredentials();

protected:
  /** Retrieve user information saved into an encrypted file.*/
  void InitializeUserInformations();

  /** Update user credentials into an encrypted file or remove the file if
  the user don't want to save its credentials.*/
  void UpdateUserCredentialsFile();

  /** Set the folder in which store encrypted user credentials.*/
  void InitUserInfoHome();

  // User credentials
  albaString m_Username;
  albaString m_Password;
  albaString   m_ProxyHost;///< Proxy Host
  int         m_ProxyFlag;///< Flag used to for enable proxy. 
  int   m_ProxyPort;///< Proxy Host
  albaString m_UserHome;
  albaString m_UserInfoFile;
  int m_RememberCredentials;
  bool m_Initialized; ///< Flag used to verify if users' information has been initialized.
};
#endif
