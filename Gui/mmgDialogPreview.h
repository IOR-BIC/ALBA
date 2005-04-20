/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDialogPreview.h,v $
  Language:  C++
  Date:      $Date: 2005-04-20 09:39:10 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgDialogPreview_H__
#define __mmgDialogPreview_H__

class mafRWI;

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/dialog.h"
#include "mafEvent.h"
#include "mmgGui.h"
#include "mmgDialog.h"
//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------
enum mmgDIALOG_EXSTYLES
{
  mafUSEGUI = mafCLOSE*2,
  mafUSERWI = mafUSEGUI*2,
};

/** mmgDialogPreview - a dialog widget with a render window.
mmgDialogPreview can be used inside operations to make a preview window.
*/
class mmgDialogPreview : public mmgDialog
{
public:
	mmgDialogPreview (const wxString& title,	long style = mafCLOSEWINDOW | mafRESIZABLE | mafCLOSE | mafUSEGUI | mafUSERWI );
	virtual ~mmgDialogPreview (); 

  /** Return the dialog's render window.*/
  mafRWI *GetRWI() {return m_rwi;};
  /** Return the dialog's gui.*/
  mmgGui *GetGui() {return m_gui;};

protected:
  wxBoxSizer *m_preview_sizer; ///< Sizer used for the preview render window
  mafRWI     *m_rwi; ///< Render window
  mmgGui     *m_gui; ///< Gui variable used to plug custom widgets

  /** Virtual functions called on Dialoag Close - these can be redefined witout providing the Event Table */
  virtual void OnCloseWindow(wxCloseEvent& event);
};
#endif