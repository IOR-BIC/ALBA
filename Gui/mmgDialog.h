/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDialog.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:21 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgDialog_H__
#define __mmgDialog_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/dialog.h"
//----------------------------------------------------------------------------
// mmgDialog :
//----------------------------------------------------------------------------
class mmgDialog : public wxDialog  
{
public:
	mmgDialog (const wxString& title,	long style = wxDIALOG_MODAL | wxDEFAULT_DIALOG_STYLE | wxTHICK_FRAME);
	virtual ~mmgDialog (); 

	/** Set the flag that allow to close the dialog through close button. */
	void EnableCloseButton(bool enable);

protected:
	bool m_enable_close_button;
	
	/** Close the dialog if the m_enable_close_button flag is true. */
	void OnCloseWindow(wxCloseEvent& event);
	
	/** Resize the dialog. */
	void OnSize(wxSizeEvent &event);
	
	DECLARE_EVENT_TABLE()
};
#endif
