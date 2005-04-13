/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testDialogDlg.h,v $
  Language:  C++
  Date:      $Date: 2005-04-13 21:04:15 $
  Version:   $Revision: 1.1 $
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
  void OnEvent(mafEvent& e);

protected:
	/** Close the dialog */
	virtual void OnCloseWindow(wxCloseEvent& event);
  virtual void OnOK(wxCommandEvent& event);
  virtual void OnCancel(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()
};
#endif
