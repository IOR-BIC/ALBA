/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDialogWarnAndSkipOthers
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIDialogWarnAndSkipOthers_H__
#define __albaGUIDialogWarnAndSkipOthers_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIDialog.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** albaGUIDialogWarnAndSkipOthers - a dialog widget for warning and remember
@sa albaGUIDialog
*/
class albaGUIDialogWarnAndSkipOthers : public albaGUIDialog
{
public:
	albaGUIDialogWarnAndSkipOthers(const wxString title, const wxString message, int *remember);
	virtual ~albaGUIDialogWarnAndSkipOthers();
 
protected:

	albaGUI* m_Gui; ///< Gui variable used to plug custom widgets.

};
#endif
