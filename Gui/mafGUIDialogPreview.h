/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDialogPreview.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIDialogPreview_H__
#define __mafGUIDialogPreview_H__

class mafRWI;

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/dialog.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafGUIDialog.h"
//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------
enum DIALOG_EXSTYLES
{
  mafUSEGUI = mafCLOSE*2,
  mafUSERWI = mafUSEGUI*2,
};

/** mafGUIDialogPreview - a dialog widget with a render window.
mafGUIDialogPreview can be used inside operations to make a preview window.
@sa mafGUIDialog
*/
class mafGUIDialogPreview : public mafGUIDialog
{
public:
	mafGUIDialogPreview (const wxString& title,	long style = mafCLOSEWINDOW | mafRESIZABLE | mafCLOSE | mafUSEGUI | mafUSERWI );
	virtual ~mafGUIDialogPreview (); 
   
  /** Return the dialog's render window.*/
  mafRWI *GetRWI() {return m_Rwi;};
  /** Return the dialog's gui.*/
  mafGUI *GetGui() {return m_Gui;};

  wxBoxSizer *m_RwiSizer; ///< Sizer used for the vtk render window and if you want to plug any gui on bottom of the RWI

protected:
  wxBoxSizer *m_PreviewSizer; ///< Sizer used for the preview render window
  mafRWI     *m_Rwi; ///< Render window
  mafGUI     *m_Gui; ///< Gui variable used to plug custom widgets
};
#endif
