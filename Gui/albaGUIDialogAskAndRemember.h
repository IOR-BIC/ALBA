/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDialogAskAndRemember
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIDialogAskAndRemember_H__
#define __albaGUIDialogAskAndRemember_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIDialog.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** albaGUIDialogAskAndRemember - a dialog widget with a find VME GUI.
@sa albaGUIDialog
*/
class albaGUIDialogAskAndRemember : public albaGUIDialog
{
public:
	albaGUIDialogAskAndRemember(const wxString& title, wxString& message, wxString choices[],int choicesNum, int *choice, int *remember);
	virtual ~albaGUIDialogAskAndRemember();
 
protected:

	albaGUI* m_Gui; ///< Gui variable used to plug custom widgets.

};
#endif
