/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testDialogDlg
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testDialogDlg_H__
#define __testDialogDlg_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/dialog.h"
#include "albaGUIDialog.h"
//----------------------------------------------------------------------------
// testDialogDlg :
//----------------------------------------------------------------------------
/**
sample showing a class derived from albaGUIDialog
*/
class testDialogDlg : public albaGUIDialog
{
public:
	testDialogDlg (const wxString& title);
	virtual ~testDialogDlg (); 
  void OnEvent(albaEventBase *event);

protected:
	/** Close the dialog */
	virtual void OnCloseWindow(wxCloseEvent& event);
  virtual void OnOK(wxCommandEvent& event);
  virtual void OnCancel(wxCommandEvent& event);

};
#endif
