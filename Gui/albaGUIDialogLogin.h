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
#ifndef __albaGUIDialogLogin_H__
#define __albaGUIDialogLogin_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIDialog.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** albaGUIDialogLogin - a dialog widget with a find VME GUI.
@sa albaGUIDialog
*/
class albaGUIDialogLogin : public albaGUIDialog
{
public:
	albaGUIDialogLogin(const wxString& title, long style = albaCLOSEWINDOW | albaOK | albaCANCEL);
	virtual ~albaGUIDialogLogin();

  void OnEvent(albaEventBase *alba_event);

  /** Set the user credentials.*/
  void SetUserCredentials(albaString &usename, albaString &pwd, int &proxyFlag, albaString &proxyHost, int &proxyPort, int &remember_me);
  
  /** Return the username information.*/
  albaString &GetUser();

  /** Return the password information.*/
  albaString &GetPwd();

  /** Select image type during saving of the views*/
  int GetProxyFlag(){return m_ProxyFlag;};

  /** Select image type during saving of the views*/
  albaString &GetProxyHost();

  /** Get Port in proxy connection*/
  int GetProxyPort();

  /** Set the flag to store user credentials.*/
  void SetRememberUserCredentials(int remember = 1) {m_RememberMe = remember;};

  /** Return information to allow saving or not users' information.*/
  int GetRememberUserCredentials() {return m_RememberMe;};

protected:

  /** Used to enable/disable items according to the current widgets state.*/
  void EnableItems();

  albaGUI *m_Gui; ///< Gui variable used to plug custom widgets.
  albaString m_Username; ///< Current username inserted into the dialog.
  albaString m_Pwd; ///< Current password inserted into the dialog.
  albaString m_UsernameOld; ///< Last username inserted.
  albaString m_PwdOld; ///< Last password inserted.
  albaString   m_ProxyHost;///< Proxy Host
  int         m_ProxyFlag;///< Flag used to for enable proxy. 
  int         m_ProxyPort;///< Proxy Host
  bool m_InformationsInserted; ///< Flag used to check if the user insert some information or not.
  int m_RememberMe;
};
#endif
