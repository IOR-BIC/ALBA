/*=========================================================================

 Program: MAF2
 Module: mafGUIDialogPreview
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class MAF_EXPORT mafGUIDialogPreview : public mafGUIDialog
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
