/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafUser.h,v $
Language:  C++
Date:      $Date: 2007-12-19 12:07:19 $
Version:   $Revision: 1.2 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafUser_H__
#define __mafUser_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafString.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** mafUser - Used to manage username and password; store them and give basic function to check user's credentials.
This is intended as starting point for user managing. Method for Check User Credentials can be redefined to be
customized at application level according to custom requests.
*/
class mafUser
{
public:
  mafUser();
  virtual ~mafUser();

  /** Allows to show login dialog and get credentials from the application's user.*/
  int ShowLoginDialog();

  /** Function to be customized at application level.
  By default open Login Dialog if the user did not inserted any information.*/
  virtual bool CheckUserCredentials();

  /** Set the user's credentials. 
  Return 'true' if the users' credentials are correctly initialized, otherwise 'flase' value is returned. */
  bool SetCredentials(mafString &name, mafString &pwd, int &remember_me);

  /** Return the information on user name.*/
  mafString &GetName();

  /** Return the information on users' password.*/
  mafString &GetPwd();

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
  mafString m_Username;
  mafString m_Password;
  mafString m_UserHome;
  mafString m_UserInfoFile;
  int m_RememberCredentials;
  bool m_Initialized; ///< Flag used to verify if users' information has been initialized.
};
#endif
