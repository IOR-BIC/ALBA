/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaAbsHelpManager.h
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef _albaAbsHelpManager_H
#define _albaAbsHelpManager_H

#include "albaConfigure.h"
#include "albaString.h"

/** Services of the help manager, reached through albaServiceClient::GetHelpManager() */
class ALBA_EXPORT albaAbsHelpManager
{
public:
	/** Return true if the help has a page for the given type name (an operation, a VME or a view) */
	virtual bool HasHelpEntry(albaString typeName) = 0;

	/** Open the help page of the given type name */
	virtual void ShowHelpEntry(albaString typeName) = 0;
};
#endif
