/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDialogPreview
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIDialogPreview_H__
#define __albaGUIDialogPreview_H__

class albaRWI;

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/dialog.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaGUIDialog.h"
//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------
enum DIALOG_EXSTYLES
{
  albaUSEGUI = albaCLOSE*2,
  albaUSERWI = albaUSEGUI*2,
};

/** albaGUIDialogPreview - a dialog widget with a render window.
albaGUIDialogPreview can be used inside operations to make a preview window.
@sa albaGUIDialog
*/
class ALBA_EXPORT albaGUIDialogPreview : public albaGUIDialog
{
public:
	albaGUIDialogPreview (const wxString& title,	long style = albaCLOSEWINDOW | albaRESIZABLE | albaCLOSE | albaUSEGUI | albaUSERWI );
	virtual ~albaGUIDialogPreview (); 
   
  /** Return the dialog's render window.*/
  albaRWI *GetRWI() {return m_Rwi;};
  /** Return the dialog's gui.*/
  albaGUI *GetGui() {return m_Gui;};

  wxBoxSizer *m_RwiSizer; ///< Sizer used for the vtk render window and if you want to plug any gui on bottom of the RWI

protected:
  wxBoxSizer *m_PreviewSizer; ///< Sizer used for the preview render window
  albaRWI     *m_Rwi; ///< Render window
  albaGUI     *m_Gui; ///< Gui variable used to plug custom widgets
};
#endif
