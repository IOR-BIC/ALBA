/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testDialogDlg.h,v $
  Language:  C++
  Date:      $Date: 2005-05-04 13:03:07 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __testDialogDlg_H__
#define __testDialogDlg_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/dialog.h"
#include "mmgDialog.h"
//----------------------------------------------------------------------------
// testDialogDlg :
//----------------------------------------------------------------------------
/**
sample showing a class derived from mmgDialog
*/
class testDialogDlg : public mmgDialog
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
