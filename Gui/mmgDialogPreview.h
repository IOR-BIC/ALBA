/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDialogPreview.h,v $
  Language:  C++
  Date:      $Date: 2005-11-09 13:17:39 $
  Version:   $Revision: 1.8 $
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
enum DIALOG_EXSTYLES
{
  mafUSEGUI = mafCLOSE*2,
  mafUSERWI = mafUSEGUI*2,
};

/** mmgDialogPreview - a dialog widget with a render window.
mmgDialogPreview can be used inside operations to make a preview window.
@sa mmgDialog
*/
class mmgDialogPreview : public mmgDialog
{
public:
	mmgDialogPreview (const wxString& title,	long style = mafCLOSEWINDOW | mafRESIZABLE | mafCLOSE | mafUSEGUI | mafUSERWI );
	virtual ~mmgDialogPreview (); 
   
  /** Return the dialog's render window.*/
  mafRWI *GetRWI() {return m_Rwi;};
  /** Return the dialog's gui.*/
  mmgGui *GetGui() {return m_Gui;};

  wxBoxSizer *m_RwiSizer; ///< Sizer used for the vtk render window and if you want to plug any gui on bottom of the RWI

protected:
  wxBoxSizer *m_PreviewSizer; ///< Sizer used for the preview render window
  mafRWI     *m_Rwi; ///< Render window
  mmgGui     *m_Gui; ///< Gui variable used to plug custom widgets
};
#endif
