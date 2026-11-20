/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaAbsOpManager.h
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef _albaAbsOpManager_H
#define _albaAbsOpManager_H

#include "albaConfigure.h"

/** Services of the operation manager, reached through albaServiceClient::GetOpManager() */
class ALBA_EXPORT albaAbsOpManager
{
public:
	/** Clear the undo stack and the clipboard; ignored while an operation is running */
	virtual void ClearUndoStack() = 0;

	/** Enable or disable the operations in menus and toolbar, checking which ones accept the selected VME */
	virtual void EnableOp(bool CanEnable = true) = 0;
};
#endif
