/*=========================================================================

 Program: MAF2
 Module: testDialogDlg
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

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
#include "mafGUIDialog.h"
//----------------------------------------------------------------------------
// testDialogDlg :
//----------------------------------------------------------------------------
/**
sample showing a class derived from mafGUIDialog
*/
class testDialogDlg : public mafGUIDialog
{
public:
	testDialogDlg (const wxString& title);
	virtual ~testDialogDlg (); 
  void OnEvent(mafEventBase *event);

protected:
	/** Close the dialog */
	virtual void OnCloseWindow(wxCloseEvent& event);
  virtual void OnOK(wxCommandEvent& event);
  virtual void OnCancel(wxCommandEvent& event);

};
#endif
