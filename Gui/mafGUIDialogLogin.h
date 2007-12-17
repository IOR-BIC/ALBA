/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDialogLogin.h,v $
  Language:  C++
  Date:      $Date: 2007-12-17 11:47:28 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIDialogLogin_H__
#define __mafGUIDialogLogin_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mmgDialog.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** mafGUIDialogLogin - a dialog widget with a find VME GUI.
@sa mmgDialog
*/
class mafGUIDialogLogin : public mmgDialog
{
public:
	mafGUIDialogLogin(const wxString& title, long style = mafCLOSEWINDOW | mafOK | mafCANCEL);
	virtual ~mafGUIDialogLogin();

  void OnEvent(mafEventBase *maf_event);

  /** Set the user credentials.*/
  void SetUserCredentials(mafString &usename, mafString &pwd, int &remember_me);
  
  /** Return the username information.*/
  mafString &GetUser();

  /** Return the password information.*/
  mafString &GetPwd();

  /** Set the flag to store user credentials.*/
  void SetRememberUserCredentials(int remember = 1) {m_RememberMe = remember;};

  /** Return information to allow saving or not users' information.*/
  int GetRememberUserCredentials() {return m_RememberMe;};

protected:
  mmgGui *m_Gui; ///< Gui variable used to plug custom widgets.
  mafString m_Username; ///< Current username inserted into the dialog.
  mafString m_Pwd; ///< Current password inserted into the dialog.
  mafString m_UsernameOld; ///< Last username inserted.
  mafString m_PwdOld; ///< Last password inserted.
  bool m_InformationsInserted; ///< Flag used to check if the user insert some information or not.
  int m_RememberMe;
};
#endif
