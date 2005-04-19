/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDialogPreview.h,v $
  Language:  C++
  Date:      $Date: 2005-04-19 15:57:05 $
  Version:   $Revision: 1.1 $
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

//----------------------------------------------------------------------------
// mmgDialogPreview :
//----------------------------------------------------------------------------
/**

*/
class mmgDialogPreview : public mmgDialog
{
public:
	mmgDialogPreview (const wxString& title,	long style = mafCLOSEWINDOW | mafRESIZABLE | mafCLOSE | mafUSEGUI | mafUSERWI );
	virtual ~mmgDialogPreview (); 

  mafRWI *GetRWI() {return m_rwi;};
  mmgGui *GetGui() {return m_gui;};

 // int ShowModal();

protected:

  /** sizer for user widgets */
  wxBoxSizer      *m_preview_sizer;
  mafRWI          *m_rwi;
  mmgGui          *m_gui;

  /** Virtual functions called on Dialoag Close - these can be redefined witout providing the Event Table */
  virtual void OnCloseWindow(wxCloseEvent& event);
};
#endif